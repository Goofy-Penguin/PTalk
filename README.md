# PTalk

An library that allows for 2-way communication between a client server.

## Table of contents
1. [Dependencies](#Dependecies)
2. [Examples](#Exambples)
	1. [Server](#Server)
	2. [Client](#Client)
3. [Technical details](#Technical-details)

## Dependencies

- [nlohmann::json](https://github.com/nlohmann/json)

## Examples

#### Server

```C++
// includes for this example
#include <mainframe/ptalk/server.h>
#include <mainframe/ptalk/client.h>
#include <mainframe/ptalk/messageincomming.h>
#include <fmt/printf.h>

// create the object
mainframe::ptalk::Server server;

// listen on port 5028
if (!server.listen(5028)) {
	return false;
}

// add an event to handle when a client connects
server.onClient += [](std::shared_ptr<Client> client) {
	// register a global message hook that gets fires before methods are called
	client->onMessage += [client](MessageIncomming& msg) {
		std::string ip = inet_ntoa(client->getSocket().addr.sin_addr);
		fmt::print("recieved '{}' from '{}'\n", msg.getName(), ip);
	};

	// register methods
	client->addMethod("methodName", [](MessageIncomming& msg) {
		nlohmann::json data = msg.getData()

		// reply to the message with a `nlohmann::json` object
		// a callback to enable replying is optional
		msg->respond("hello world");
	});

	// send a message with a `nlohmann::json` object
	// a callback to enable replying is optional
	client->send("methodName", "hello world");
}
```

#### Client

```C++
// includes for this example
#include <mainframe/ptalk/client.h>
#include <mainframe/ptalk/messageincomming.h>
#include <fmt/printf.h>

// create the object
auto client = std::make_shared<mainframe::ptalk::Client>();

// set the internal ref so that we can use conversations
client->setRef(client);

// connect to 5028
if (client->connect("127.0.0.1", 5028) != SocketError::success) {
	return false;
}

// register a global message hook that gets fires before methods are called
client->onMessage += [](MessageIncomming& msg) {
	fmt::print("recieved '{}' from server\n", msg.getName());
};

// register methods
client->addMethod("methodName", [](MessageIncomming& msg) {
	nlohmann::json data = msg.getData()

	// reply to the message with a `nlohmann::json` object
	// callbacks are optional
	msg->respond("hello world");
});

// send a message with a `nlohmann::json` object
// a callback to enable replying is optional
client->send("methodName", "hello world");
```

## Technical details

The protocol sends messages 2-way containing a `method name` for calling an method on the other end, and an `id` to respond and keep track of an conversation.

A packet is a composed of 4 components.

- `uint32_t` packet size excluding itself
- `uint32_t` id of the message used for replying
- `uint16_t:string` method name it wants to invoke
- `uint32_t:bson` payload of the message generated by `nlohmann::json::to_bson` from a `json` object

When the `method name` is empty, the message is an response.
If the user provided a callback at the `client->send("method", {json value}, callback)` the respond callback is called.

#### Note

When using `message::respond` more than once as a conversation, on the client side where you connect to the server you will have to create it as `std::shared_ptr<Client> client` and do `client->setRef(client)` on it.
This is so that the `client` can keep track of itself and doesn't start calling unallocated memory.
In the future I'd like to see this solved inside of the library itself.