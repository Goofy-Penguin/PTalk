#include <mainframe/ptalk/server.h>
#include <mainframe/ptalk/client.h>
#include <mainframe/ptalk/message.h>
#include <fmt/printf.h>
#include <chrono>

#ifndef _MSC_VER
#include <arpa/inet.h>
#endif

using namespace mainframe::ptalk;
using namespace mainframe::networking;

const int SERVER_PORT = 5028;

Server serv;
std::shared_ptr<Client> client;
std::shared_ptr<Client> connectedClient;
bool gotPong = false;
time_t timeoutTimer = 0;

bool setupServer() {
	if (!serv.listen(SERVER_PORT)) return false;

	serv.onClient += [](std::shared_ptr<Client> newclient) {
		std::string ip = inet_ntoa(newclient->getSocket().addr.sin_addr);
		fmt::print("[server] client connected [{}]\n", ip);

		connectedClient = newclient;

		newclient->onMessage += [newclient, ip](MessageIncoming& msg) {
			fmt::print("[server] [{}] recieved '{}'\n", ip, msg.getName());
		};

		newclient->addMethod("ping", [newclient, ip](MessageIncoming& msg) {
			auto& data = msg.getData();
			auto timestamp = data["time"].get<time_t>();

			fmt::print("[server] [{}] sending pong back with time {}\n", ip, timestamp);
			newclient->send({"pong", {{"time", timestamp}}});
		});
	};

	return true;
}

bool connectToServer() {
	client = std::make_shared<Client>();
	client->setRef(client);

	if (client->connect("127.0.0.1", SERVER_PORT) != SocketError::success) {
		fmt::print("[client] failed to connect to '127.0.0.1:{}'\n", SERVER_PORT);
		return false;
	}

	fmt::print("[client] connected to server\n");

	client->onMessage += [](Message& msg) {
		fmt::print("[client] recieved '{}'\n", msg.getName());
	};

	client->addMethod("pong", [](Message& msg) {
		fmt::print("[client] recieved pong back\n");
		gotPong = true;
	});

	return true;
}

void cleanup() {
	client->disconnect();
	serv.stop();
}

bool waitFor(bool& var) {
	while (!var) {
		auto curtime = time(nullptr);
		if (curtime >= timeoutTimer) {
			fmt::print("Test failed at receive timeout :(\n");
			return false;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	return true;
}

int main() {
	if (!setupServer()) {
		fmt::print("Test failed at server setup :(\n");
		return 1;
	}

	if (!connectToServer()) {
		fmt::print("Test failed at connect :(\n");
		return 2;
	}

	timeoutTimer = time(nullptr) + 5;

	fmt::print("[client] sending ping\n");
	client->send({"ping", {{"time", timeoutTimer}}});
	if (!waitFor(gotPong)) return 3;

	cleanup();

	fmt::print("Test successfull :)\n");
	return 0;
}