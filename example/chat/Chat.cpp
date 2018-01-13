#include "Chat.hpp"

#include <future>

Chat::Chat() {
	srand(clock());
	mId       = rand();
	mUsername = std::to_string(mId);
}

Chat::~Chat() {
	disconnect();
	stopListening();
}

void Chat::listen(uint16_t port) {
	stopListening();

	if(!mServer.bind(port)) {
		throw std::runtime_error("Couldn't open port " + std::to_string(port));
	}

	mListenThread = mServer.listenAsync([=](stx::tcp_connection con) {
		mRecvThreads.emplace_back([this, con = std::move(con)]() {
			puts("New connection");
			recv(std::move(con));
		});
	});
}

void Chat::stopListening() {
	mServer.close();
	mListenThread = std::thread();
	mRecvThreads.clear();
}

void Chat::executeCommand(const char* cmd) {
	std::string ip;
	std::string port;
	std::string cmd_s = cmd;
	ip = cmd_s.substr(0, cmd_s.find(' '));
	port = cmd_s.substr(0, cmd_s.find_last_of(' '));
	connect(ip, port.c_str());
}
void Chat::executeRequest(const char* cmd) {

}

void Chat::connect(std::string const& url, char const* port) {
	disconnect();

	if(!mSend.connect(url.c_str(), port)) {
		disconnect();
		throw std::runtime_error("Couldn't connect to " + url + ":" + port);
	}

	send("<joined>");
}

void Chat::disconnect() {
	if(mSend) {
		send("<left>");
		mSend.close();
	}
}

void Chat::recv(stx::tcp_connection connection) {
	uint32_t id;
	uint32_t name_len;
	uint32_t msg_len;
	std::vector<char> name;
	std::vector<char> msg;

	while(connection) {
		id = 0;
		connection.recv(id);
		if(id == 0) {
			puts("Received invalid message: id = 0");
			break; // NOPE
		}

		name_len = 0;
		connection.recv(name_len);
		if(name_len > 512) {// NOPE
			puts("Received invalid message: name_len > 512");
			break;
		}
		name.resize(name_len + 1, '\0');
		connection.recv(name.data(), name_len);

		msg_len = 0;
		connection.recv(msg_len);
		if(msg_len > 512) {// NOPE
			puts("Received invalid message: name_len > 512");
			break;
		}
		msg.resize(msg_len + 1, '\0');
		connection.recv(msg.data(), msg_len);

		if(msg_len) {
			if(id != mId) {
				if(msg[0] == '@') {
					executeRequest(msg.data() + 1);
				}
				else {
					// TODO: Forward message
					mOnReceiveMutex.lock();
					if(mOnReceive) {
						mOnReceive(name.data(), msg.data());
					}
					mOnReceiveMutex.unlock();
				}
			}
		}

		name.clear();
		msg.clear();
	}
}

void Chat::send(std::string const& msg) {
	if(msg.empty()) return;

	if(msg[0] == '$') {
		executeCommand(msg.c_str() + 1);
		return;
	}

	mSend.send(mId);
	mSend.send((uint32_t) mUsername.size());
	mSend.send(mUsername.c_str(), mUsername.size());
	mSend.send((uint32_t) msg.size());
	mSend.send(msg.c_str(), msg.size());

	if(!mSend) throw std::runtime_error("Connection closed unexpectedly");
}

void Chat::onReceive(std::function<void(const char* user, const char* msg)> fn) {
	mOnReceiveMutex.lock();
	mOnReceive = std::move(fn);
	mOnReceiveMutex.unlock();
}
