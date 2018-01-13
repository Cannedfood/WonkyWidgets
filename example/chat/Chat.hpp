#pragma once

#include "socket.hpp"
#include <functional>
#include <mutex>

class Chat {
	stx::tcp_connection mSend;

	stx::tcp_server mServer;
	std::thread mListenThread;
	std::vector<std::thread> mRecvThreads;

	uint32_t    mId;
	std::string mUsername;

	std::mutex mOnReceiveMutex;
	std::function<void(const char* user, const char* msg)> mOnReceive;

	void recv(stx::tcp_connection on);
public:
	constexpr static char DefaultPort[] = "4442";

	Chat();
	~Chat();

	void connect(std::string const& url, char const* port = DefaultPort);
	void disconnect();

	void listen(uint16_t port = 4442);
	void stopListening();

	void executeRequest(const char* cmd);
	void executeCommand(const char* cmd);

	void send(std::string const& msg);

	void username(std::string name) { mUsername = std::move(name); }
	auto username() -> std::string const& { return mUsername; }

	auto onReceive() -> std::function<void(const char* user, const char* msg)> const& {
		return mOnReceive;
	}

	void onReceive(std::function<void(const char* user, const char* msg)> fn);
};
