#pragma once

#include <memory>
#include <thread>
#include <mainframe/networking/socket.h>
#include <mainframe/utils/event.hpp>

namespace mainframe {
	namespace ptalk {
		class Client;

		class Server {
			networking::Socket sock4;
			networking::Socket sock6;
			std::thread* threadListener4 = nullptr;
			std::thread* threadListener6 = nullptr;
			bool listening = false;

			bool listenClient(networking::Socket& sock, std::thread*& thread, int port);

		public:
			utils::Event<std::shared_ptr<Client>> onClient;

			Server();
			~Server();

			bool listen(int port);
			void stop();
		};
	}
}