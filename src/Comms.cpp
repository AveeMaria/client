#include "../include/Comms.hpp"

Comms::Comms()
{
    host = "127.0.0.1";
    port = 12345;
    resolveHost();
    openSocket();
}

Comms::Comms(const char* h, Uint16 p)
{
    host = h;
    port = p;
    resolveHost();
    openSocket();
}

Comms::~Comms()
{
    if (sock) {
        SDLNet_UDP_Close(sock);
    }
}

bool Comms::resolveHost() {
    if (SDLNet_ResolveHost(&ip, host, 12345) == -1) {
        std::cerr << "ERROR: SDLNet_ResolveHost: " << SDLNet_GetError() << "\n";
        return false;
    }

    std::cout << "OK: Host " << host << " resolved.\n";
    return true;
}

bool Comms::openSocket() {
    sock = SDLNet_UDP_Open(port);//0 al pa 12345
    if (!sock) {
        std::cerr << "ERROR: SDLNet_UDP_Open: " << SDLNet_GetError() << "\n";
        return false;
    }

    if (sock == nullptr) {
        std::cerr << "ERROR: Socket null.\n";
        return false;
    }

    std::cout << "OK: Socket opened on " << port << ".\n";
    return true;
}

bool Comms::stupidestSend(const char* data) const {
    UDPpacket* sendPacket;
    if (!allocPacket(&sendPacket, reinterpret_cast<const Uint8*>(data), (int)std::strlen(data) + 1)) {
        std::cerr << "ERROR: cant allocate packet.\n";
        return false;
    }

    sendPacket->address.host = ip.host;
    sendPacket->address.port = ip.port;

    if (SDLNet_UDP_Send(sock, -1, sendPacket) < 1) {
        std::cerr << "ERROR: SDLNet_UDP_Send error: " << SDLNet_GetError() << "\n";
        return false;
    }

    std::cout << "OK: poslan paket.\n";
    return true;
}

bool Comms::stupidSend(Uint8* data, size_t size) const {
    UDPpacket* sendPacket;
    if (!allocPacket(&sendPacket, data, (int)size)) {
        std::cerr << "ERROR: cant allocate packet.\n";
        return false;
    }

    sendPacket->address.host = ip.host;
    sendPacket->address.port = ip.port;

    if (SDLNet_UDP_Send(sock, -1, sendPacket) < 1) {
        std::cerr << "ERROR: SDLNet_UDP_Send error: " << SDLNet_GetError() << "\n";
        return false;
    }

    std::cout << "OK: poslan paket.\n";
    return true;
}

bool Comms::allocPacket(UDPpacket** packet, const Uint8* data, int size) const {
    *packet = SDLNet_AllocPacket(size + 512);

    if (!*packet) {
        std::cerr << "ERROR: SDLNet_AllocPacket: " << SDLNet_GetError() << "\n";
        return false;
    }

    std::memcpy((*packet)->data, data, size);
    (*packet)->len = size;

    std::cout << "OK: Packet allocated.\n";
    return true;
}

bool Comms::allocEmptyPacket(UDPpacket** packet, int size) const {
    *packet = SDLNet_AllocPacket(size + 256);//udp overhead?

    if (!*packet) {
        std::cerr << "ERROR: SDLNet_AllocPacket: " << SDLNet_GetError() << "\n";
        return false;
    }

    std::cout << "OK: Packet allocated.\n";
    return true;
}

//sta reci?
/*
using ReturnType = std::variant<int, float, std::string, Coords>;
ReturnType myFunction(Uint32 option) {
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
    }
}
*/

/////////////////
//  recive del //
/////////////////

bool Comms::recieve(UDPpacket** p)
{
    int packetsReceived = 0;

    // Keep receiving packets while available
    while (SDLNet_UDP_Recv(sock, *p)) {
        size_t size = (*p)->len;
        printBytes(reinterpret_cast<char*>((*p)->data), size);
        packetsReceived++;
    }

    return packetsReceived > 0;
}

bool Comms::recieve()
{
    UDPpacket* recvPacket;
    if (!allocEmptyPacket(&recvPacket, 256)) {
        SDLNet_FreePacket(recvPacket);
        return false;
    }

    int packetsReceived = 0;

    // Keep receiving packets while available
    while (SDLNet_UDP_Recv(sock, recvPacket)) {
        size_t size = recvPacket->len;
        printBytes(reinterpret_cast<char*>(recvPacket->data), size);
        packetsReceived++;
    }


    PacketType type = static_cast<PacketType>(recvPacket->data[0]);
    switch (type) {
    case PacketType::ENTITYPOS:

        std::cout << "enttiy pos recieved";
    }

    memset(recvPacket->data, 0, recvPacket->len);
    SDLNet_FreePacket(recvPacket);
    return packetsReceived > 0;
}