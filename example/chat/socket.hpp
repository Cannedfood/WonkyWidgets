#pragma once

#if __cplusplus < 201103L
#error This file requires compiler and library support \
for the ISO C++ 2011 standard. This support must be enabled \
with the -std=c++11 or -std=gnu++11 compiler options.
#endif

#include <atomic>
#include <thread>
#include <vector>
#include <exception>

namespace stx {

namespace detail {

struct shared_socket {
	int               handle;
	std::atomic<int>* references;

	shared_socket();
	explicit shared_socket(int i);
	~shared_socket();

	// -- Copy & Move -------------------------------------------------------
	shared_socket(shared_socket const& s);
	shared_socket(shared_socket&& s);
	shared_socket& operator=(shared_socket const& s);
	shared_socket& operator=(shared_socket&& s);

	// -- Reset -------------------------------------------------------
	shared_socket& reset();
	shared_socket& reset(int i);
	shared_socket& reset(shared_socket const& s);
	shared_socket& reset(shared_socket&& other);

	operator bool() const noexcept; //<! Returns whether the socket is valid
};

} // namespace detail

// =============================================================
// == Errors =============================================
// =============================================================

namespace socket_error {

class invalid_operation : public std::runtime_error {
public: invalid_operation(std::string const& what) : std::runtime_error(what) {}
};

class failed_operation : public std::runtime_error {
public: failed_operation(std::string const& what) : std::runtime_error(what) {}
};

} // namespace socket_error

// =============================================================
// == Url =============================================
// =============================================================

class url {
	std::string m_port, m_protocol, m_host, m_path;

public:
	std::string port();
	std::string protocol();
	std::string host();
	std::string path();
	std::string value();

	// -- Static utilities -------------------------------------------------------
	static std::string port_of    (std::string const& url);
	static std::string protocol_of(std::string const& url);
	static std::string host_of    (std::string const& url);
	static std::string path_of    (std::string const& url);
};

// =============================================================
// == TCP =============================================
// =============================================================

class tcp_connection {
	friend class tcp_server;

	detail::shared_socket m_server_socket;
	detail::shared_socket m_socket;

public:
	tcp_connection();

	bool connect(const char* url, char const* service_or_port, bool force = false, bool allow_ipv6 = true);

	void close();

	std::size_t       recv_raw(void* to, std::size_t max_len);
	std::vector<char> recv_all();
	void              send_raw(void const* data, std::size_t len);

	template<typename T>
	std::size_t recv(T* to, std::size_t max_len);
	template<typename T>
	bool recv(T& to) {
		return recv_raw(&to, sizeof(to)) == sizeof(to);
	}

	template<typename T>
	void send(T const* to, std::size_t len);

	template<typename T>
	void send(T const& t) { send_raw(&t, sizeof(T)); }

	bool connected() const noexcept { return m_socket; }
	operator bool()  const noexcept { return connected(); }
};

class tcp_server {
	detail::shared_socket m_socket;

public:
	tcp_server();
	tcp_server(uint16_t port, bool blocking = true);

	bool bind(uint16_t port, bool blocking = true);

	tcp_connection listen(std::size_t max_connections = 0);

	template<typename C>
	void listen(std::size_t max_connections, C&& callback);
	template<typename C>
	void listen(C&& callback);

	template<typename C>
	std::thread listenAsync(std::size_t max_connections, C&& callback);
	template<typename C>
	std::thread listenAsync(C&& callback);

	void close();

	operator bool() const noexcept { return m_socket; }
};

// =============================================================
// == Udp =============================================
// =============================================================

class udp_server;

class udp_address {};

struct udp_packet {
	udp_server*       server;
	udp_address       address;
	std::vector<char> data;

	void respond(void* data, std::size_t len);
};

class udp_server {
	detail::shared_socket m_socket;

public:
	udp_server();
	udp_server(uint16_t port);

	udp_server& bind(uint16_t port);
	udp_packet listen();

	void send_raw(const char* url, void const* data, size_t len);
	void send_raw(udp_address const& to, void const* data, size_t len);
};

} // namespace stx


// =============================================================
// == Inline implementation ===================================
// =============================================================

namespace stx {

// ** Tcp connection *******************************************************

template<typename T>
void tcp_connection::send(T const* to, std::size_t len) { send_raw((void const*)to, len * sizeof(T)); }

template<typename T>
std::size_t tcp_connection::recv(T* to, std::size_t max_len) { return recv_raw((void*)to, max_len * sizeof(T)); }

// ** Tcp server *******************************************************

template<typename C>
void tcp_server::listen(std::size_t max_connections, C&& callback) {
	while(m_socket.handle != -1) {
		tcp_connection con = listen(max_connections);
		if(con) {
			callback(std::move(con));
		}
	}
}

template<typename C>
void tcp_server::listen(C&& callback) {
	while(m_socket.handle != -1) {
		tcp_connection con = listen();
		if(con) {
			callback(std::move(con));
		}
	}
}

template<typename C>
std::thread tcp_server::listenAsync(std::size_t max_connections, C&& callback) {
	return std::thread([=]() { listen(max_connections, std::forward<C>(callback)); });
}

template<typename C>
std::thread tcp_server::listenAsync(C&& callback) {
	return std::thread([=]() { listen(callback); });
}

// ** Udp server *******************************************************


} // namespace stx
