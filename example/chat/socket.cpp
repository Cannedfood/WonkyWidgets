#include "socket.hpp"

extern "C" {
#include <fcntl.h>
#include <netinet/in.h> // struct sockaddr_in
#include <sys/socket.h>
#include <unistd.h> // close

#include <netdb.h> // getaddrinfo

}

#include <cstring>
#include <string>

namespace stx {

// =============================================================
// == Detail =============================================
// =============================================================

namespace detail {

// TODO: only create new references pointer when the shared_socket is copied

shared_socket::shared_socket() :
	handle(-1),
	references(nullptr)
{}

shared_socket::shared_socket(shared_socket const& s) : shared_socket() {
	reset(s);
}

shared_socket::shared_socket(shared_socket&& s) : shared_socket() {
	reset(std::move(s));
}

shared_socket::shared_socket(int i) : shared_socket() {
	reset(i);
}

shared_socket& shared_socket::operator=(shared_socket const& s) {
	reset(s);
	return *this;
}
shared_socket& shared_socket::operator=(shared_socket&& s) {
	reset(std::move(s));
	return *this;
}

shared_socket::~shared_socket() {
	reset();
}

shared_socket& shared_socket::reset() {
	if(references) {
		if(--(*references) == 0) {
			close(handle);
			delete references;
		}
	}
	handle = -1;
	references = nullptr;
	return *this;
}

shared_socket& shared_socket::reset(int i) {
	reset();
	handle = i;
	if(i != -1) {
		references = new std::atomic<int>(1);
	}
	return *this;
}

shared_socket& shared_socket::reset(shared_socket&& other) {
	reset();
	handle = other.handle;
	references = other.references;
	other.handle = -1;
	other.references = nullptr;
	return *this;
}

shared_socket& shared_socket::reset(shared_socket const& other) {
	if(other.references) {
		++(*other.references);
	}

	reset();
	handle = other.handle;
	references = other.references;
	return *this;
}

shared_socket::operator bool() const noexcept {
	return handle != -1;
}

} // namespace detail

// =============================================================
// == Url =============================================
// =============================================================

std::string url::port_of   (std::string const& url) {
	size_t pos = url.find_last_of(':');
	if(pos == std::string::npos) return "";
	return url.substr(pos);
}
std::string url::protocol_of(std::string const& url) {
	size_t pos = url.find("://");
	if(pos == std::string::npos) return "";
	return url.substr(0, pos);
}
std::string url::host_of   (std::string const& url) {
	size_t beg = url.find("://");
	if(beg == std::string::npos) beg = 0;

	size_t end = url.find("/", beg);
	if(end == std::string::npos) end = url.size() - beg;

	return url.substr(beg, end);
}
std::string url::path_of   (std::string const& url) {
	// TODO
	return "Unimplemented function";
}

// =============================================================
// == TCP =============================================
// =============================================================

// ** Connection *******************************************************

tcp_connection::tcp_connection() {}

bool tcp_connection::connect(char const* url, char const* service, bool force, bool allow_ipv6) {
	addrinfo* info = nullptr;

	::getaddrinfo(url, service, nullptr, &info);

	if(!info) {
		throw socket_error::failed_operation("Failed resolving '" + std::string(url) + "': " + strerror(errno));
	}

	addrinfo* used_info = info;

	if(!allow_ipv6) {
		while(used_info && info->ai_family == AF_INET6) {
			used_info = used_info->ai_next;
		}
		if(!used_info) {
			::freeaddrinfo(info);
			throw socket_error::failed_operation("Server " + std::string(url) + " requires ipv6");
		}
	}

	m_socket.reset(::socket(used_info->ai_family, SOCK_STREAM, 0));
	if(!m_socket) {
		::freeaddrinfo(info);
		throw socket_error::failed_operation("Failed creating socket: " + std::string(strerror(errno)));
	}

	if(force) {
		int yes = 1;
		setsockopt(m_socket.handle, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
	}
	if(::connect(m_socket.handle, used_info->ai_addr, used_info->ai_addrlen)) {
		m_socket.reset();
		freeaddrinfo(info);
		// throw socket_error::failed_operation("Failed to connect: " + std::string(strerror(errno)));
		return false;
	}

	freeaddrinfo(info);

	return true;
}

void tcp_connection::close() {
	m_socket.reset();
	m_server_socket.reset();
}

void tcp_connection::send_raw(void const* data, std::size_t len) {
	::send(m_socket.handle, data, len, 0);
}

std::size_t tcp_connection::recv_raw(void* to, std::size_t max_len) {
	return ::recv(m_socket.handle, to, max_len, 0);
}

// ** Server *******************************************************

tcp_server::tcp_server() {}

tcp_server::tcp_server(uint16_t port, bool blocking) {
	bind(port, blocking);
}

bool tcp_server::bind(uint16_t port, bool blocking) {
	int type = SOCK_STREAM;

	if(!blocking) type |= SOCK_NONBLOCK;

	m_socket.reset(::socket(AF_INET, type, 0));
	if(!m_socket) {
		throw socket_error::failed_operation("Failed creating socket: " + std::string(::strerror(errno)));
	}

	::sockaddr_in addr   = {};
	addr.sin_family      = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port        = htons(port);

	if(::bind(m_socket.handle, (sockaddr*) &addr, sizeof(addr))) {
		m_socket.reset();
		throw socket_error::failed_operation("Failed binding socket to port " + std::to_string(port) + ": " + std::string(::strerror(errno)));
	}

	return m_socket;
}

tcp_connection tcp_server::listen(std::size_t max_connections) {
	if(!m_socket) {
		throw socket_error::invalid_operation("Socket was not bound to a port!");
	}

	tcp_connection connection;

	if(::listen(m_socket.handle, ((int) max_connections) - 1)) {
		throw socket_error::failed_operation("Failed listening for connections: " + std::string(::strerror(errno)));
	}

	sockaddr_in addr;
	unsigned int sz = sizeof(addr);
	connection.m_socket.reset(::accept(m_socket.handle, (sockaddr*) &addr, &sz));
	if(connection.m_socket) {
		connection.m_server_socket.reset(m_socket);
	}
	else throw socket_error::failed_operation("Failed creating connection: " + std::string(::strerror(errno)));

	printf("New connection %i: %i\n", connection.m_server_socket.handle, connection.m_socket.handle);

	return connection;
}

void tcp_server::close() {
	m_socket.reset();
}

// =============================================================
// == Udp =============================================
// =============================================================

udp_server::udp_server() :
	m_socket(::socket(AF_INET, SOCK_DGRAM, 0))
{}

udp_server::udp_server(uint16_t port) :
	udp_server()
{
	bind(port);
}

udp_server& udp_server::bind(uint16_t port) {
	sockaddr_in addr;
	addr.sin_family      = AF_INET;
	addr.sin_port        = htons(port);
	addr.sin_addr.s_addr = INADDR_ANY;

	int yes = 1;
	setsockopt(m_socket.handle, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

	if(::bind(m_socket.handle, (sockaddr const*) &addr, sizeof(addr))) {
		throw socket_error::failed_operation("Failed binding socket to port " + std::to_string(port) + ": " + strerror(errno));
	}

	return *this;
}

} // namespace stx
