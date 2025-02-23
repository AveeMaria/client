#pragma once 

#include "Utils.hpp"
#include <SDL_net.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <memory>
#include <bitset>
#include <type_traits>
#include <variant>
#include <string>

struct entity {
    int id;
    Coords c;

    void update(Coords coord) {
        c.x = coord.x; c.y = coord.y;
    }
    void print() const {
        std::cout << "ID: " << id << " x: " << c.x << " y: " << c.y << "\n";
    }
};

class Comms {
private:
	const char* host;
	Uint16 port;

	IPaddress ip;
	UDPsocket sock;
public:
	Comms();
	Comms(const char* h, Uint16 p);
	~Comms();

    //to se klice v konstruktorjih
	bool resolveHost();
	bool openSocket();

    //part za sendat
    template <typename T>
    bool send(T);
	bool stupidestSend(const char* data) const;
	bool stupidSend(Uint8* data, size_t size) const;
	bool allocPacket(UDPpacket** packet, const Uint8* data, int size) const;


    //part za recivevat
    bool allocEmptyPacket(UDPpacket** packet, int size) const;
    bool recieve(UDPpacket** p);

    bool recieve();
};

///////////////////////////
// POMEBNE STVARI KLELE  //
///////////////////////////

//pazi kako to sranje includas!!!!! ful sketchy error je ce narobe
enum class PacketType : Uint8 {
    INT,
    FLOAT,
    COORDS,
    STRING,
    ENTITYPOS,
    UNDEFINED = 255//ubi se picka
};

static void printBytes(char data[], size_t size) {
    for (size_t i = 0; i < size; ++i) {
        std::cout << std::bitset<8>(data[i]) << " ";
    }
    std::cout << "\n";
}

template<typename T>
Uint8 checkType(const T& data) {
    if (std::is_same<T, int>::value) {
        std::cout << "The type is int.\n";
        return static_cast<Uint8>(PacketType::INT);
    }
    else if (std::is_same<T, float>::value) {
        std::cout << "The type is float.\n";
        return static_cast<Uint8>(PacketType::FLOAT);
    }
    else if (std::is_same<T, Coords>::value) {
        std::cout << "The type is Coords.\n";
        return static_cast<Uint8>(PacketType::COORDS);
    }
    else if (std::is_same<T, std::string>::value) {
        std::cout << "The type is std::string.\n";
        return static_cast<Uint8>(PacketType::STRING);
    }
    else if (std::is_same<T, entity>::value) {
        std::cout << "The type is std::string.\n";
        return static_cast<Uint8>(PacketType::ENTITYPOS);
    }
    else {
        std::cout << "The type is undefined.\n";
        return static_cast<Uint8>(PacketType::UNDEFINED);
    }
}

template<typename T>
Uint8* prepareData(T data) {
    size_t packetSize = 1 + sizeof(data);
    std::cout << "data size: " << packetSize << "\n";

    Uint8* buffer = new Uint8[1 + packetSize];
    Uint8 type = checkType(data);

    if (type == 255) {
        return nullptr;
    }
    else {
        buffer[0] = type;
    }

    std::memcpy(&buffer[1], &data, packetSize);

    printBytes(reinterpret_cast<char*>(buffer), packetSize);

    return buffer;
}

template<typename T>
bool Comms::send(T data)
{
	Uint8* packet = prepareData(data);
	if (packet == nullptr) {
		delete[] packet;
		return false;
	}
	stupidSend(packet, sizeof(data) + 1);
	delete[] packet;
	return true;
}

//to je sexy af
using ReturnType = std::variant<int, float, std::string, Coords, entity>;
//ReturnType myFunction(Uint32 option);

template <typename T>
ReturnType typeReturner(T data);

///////////////////////
// recieve del kode? //
///////////////////////

template<typename T>
ReturnType typeReturner(T data)
{
    PacketType type = checkType(data);
    switch (type) {
    case PacketType::INT:
        std::cout << "error\n";
    case PacketType::ENTITYPOS:
        entity tmp;
        memcpy(&tmp, data[1], sizeof(tmp));
        return tmp;
    }


    /*
    if (option == 1) {
        return 42;
    }
    else if (option == 2) {
        return 3.14f;
    }
    else if (option == 3) {
        return std::string("Hello, World!");
    }
    else {
        return Coords{ 1, 2 };
    }*/
}
