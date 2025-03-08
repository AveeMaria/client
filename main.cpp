#include "include/Comms.hpp"
#include "include/Utils.hpp"
#include "include/PacketType.hpp"

SDL_Renderer* renderer = nullptr;

void draw(SDL_Rect* r) {
	SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, r);
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    SDL_RenderPresent(renderer);
}

int main() {
	SDL_Window* window;

	if (SDLNet_Init() < 0) {
		std::cerr << "ERROR: SDLNet_Init failed: " << SDLNet_GetError() << "\n";
		SDL_Quit();
	}

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER) == 0)
	{
		window = SDL_CreateWindow("xd", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1000, 600, NULL);
		renderer = SDL_CreateRenderer(window, -1, 0);
		if (renderer) {
			SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

			//simuliramo slow load time
			std::this_thread::sleep_for(std::chrono::milliseconds(300));
		}
	}

	
	Comms comms("127.0.0.1", (Uint16)123456);
	//Comms comms("84.20.249.112", (Uint16)12345);
	//Comms comms("192.168.0.24", (Uint16)12345);


	
	
	if (comms.send(SYN { SDL_GetTicks()})) {
		std::cout << "connecting to server..\n";
	}
	
	while (true) {
		UDPpacket* recvPacket;
		if (comms.recieve(&recvPacket)) {
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
			case 10:
				std::cout << "type: SYN\n";//TEGA CLIENT NE SPREJEMA KER POSLJE
				break;
			case 15:
				std::cout << "type: SYN_ACK\n";

				if (!comms.stack_send( ACK{ SDL_GetTicks() }, recvPacket->address)) {
					std::cerr << "ERROR: ACK not sent.\n";
				}

				break;

			case 20:
				std::cout << "type: ACK\n";//TEGA CLIENT NE SPREJEMA KER POSLJE
				break;
				///////
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