#include <mainframe/ptalk/server.h>
#include <mainframe/ptalk/client.h>

namespace mainframe {
	namespace ptalk {
		Server::Server() : sock4(false), sock6(true) {}

		Server::~Server() {
			stop();
		}

		bool Server::listenClient(networking::Socket& sock, std::thread*& thread, int port) {
			if (!sock.create()) return false;
			if (!sock.bind(port)) return false;
			if (!sock.listen()) return false;

			thread = new std::thread([this, &sock]() {
				while (listening) {
					auto client = std::make_shared<Client>(false);
					if (!sock.accept(&client->getSocket())) continue;

					client->setRef(client);
					client->startThreads();

					onClient(client);
				}
			});

			return true;
		}

		bool Server::listen(int port) {
			if (listening) return false;

			int hostingCount = 0;
			if (listenClient(sock4, threadListener4, port)) hostingCount++;
			if (listenClient(sock6, threadListener6, port)) hostingCount++;

			if (hostingCount == 0) {
				stop();
				return false;
			}

			listening = true;
			return true;
		}

		void Server::stop() {
			listening = false;
			sock4.close();
			sock6.close();

			if (threadListener4 != nullptr) {
				if (threadListener4->joinable()) threadListener4->join();

				delete threadListener4;
				threadListener4 = nullptr;
			}

			if (threadListener6 != nullptr) {
				if (threadListener6->joinable()) threadListener6->join();

				delete threadListener6;
				threadListener6 = nullptr;
			}
		}
	}
}