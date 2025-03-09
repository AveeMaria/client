#include "include/Comms.hpp"
#include "include/Utils.hpp"
#include "include/PacketType.hpp"


int main() {
	std::cout << "CLIENTSIDE\n";
	if (SDLNet_Init() < 0) {
		std::cerr << "ERROR: SDLNet_Init failed: " << SDLNet_GetError() << "\n";
		SDL_Quit();
	}
	
	Comms comms("127.0.0.1", (Uint16)123456);
	//Comms comms("84.20.249.112", (Uint16)12345);
	//Comms comms("192.168.0.24", (Uint16)12345);


	if (comms.stack_send( SYN { SDL_GetTicks()} )) {
		std::cout << "SYN SENT\n";
	}
	
	while (true) {
		UDPpacket* recvPacket;
		if (comms.recieve(&recvPacket)) {
			if (recvPacket->len == 0) {
				std::cout << "ERROR: EMPTY PACKET";
				continue;
			}
			printBytes(reinterpret_cast<char*>(recvPacket->data), recvPacket->len);

			///PREVER KER PACKET JE PO PRVEM BYTU

			switch ((Uint8)recvPacket->data[0]) {
			case 0:
				std::cout << "type: PING\n";
				break;
			case 5:
				std::cout << "type: PONG\n";
				break;
				///////
			case (int)PacketType::SYN:
				std::cout << "ERROR: type: SYN\n";//TEGA CLIENT NE SPREJEMA KER POSLJE
				break;
			case (int)PacketType::SYN_ACK:
				std::cout << "type: SYN_ACK\n";

				if (!comms.stack_send( ACK{ SDL_GetTicks() }, recvPacket->address)) {
					std::cerr << "ERROR: ACK not sent.\n";
				}

				break;

			case (int)PacketType::ACK:
				std::cout << "ERROR: type: ACK\n";//TEGA CLIENT NE SPREJEMA KER POSLJE
				break;
				//////
			case 40:
				std::cout << "type: ENTITY_POS\n";
				break;
			default:
				std::cout << "Unknown packet type.\n";
				break;
			}
		}
	}
}