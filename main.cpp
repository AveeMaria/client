#include "include/Comms.hpp"
#include "include/Utils.hpp"

#include <vector>
SDL_Renderer* renderer = nullptr;
SDL_Window* window;
SDL_Rect player{ 0,0,100,100 };
std::vector<entity> entities;

void draw(SDL_Rect* r) {
	SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, r);
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderPresent(renderer);
}

int main() {
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

	
	//Comms comms("127.0.0.1", (Uint16)12345);
	Comms comms("192.168.0.24", (Uint16)12345);
	
	int x = 19;
	float y = 5.6f;
	Coords c = { 100, 50 };
	std::string s;
	entities.push_back({ 15, {0,0} });

	UDPpacket* r;
	r = SDLNet_AllocPacket(512);
	while (true) {
		for (auto& e : entities) {
			draw(&player);
			e.print();
		}
		
		if (comms.recieve(&r)) {
			std::cout << "OK: recieved\n";

			if (static_cast<PacketType>(r->data[0]) == PacketType::ENTITYPOS) {
				std::cout << "recieved entity pos\n";

				int id;
				memcpy(&id, r->data + 1, sizeof(id));
				Coords c;

				//offsetamo za 1(type) + 4 (id)
				memcpy(&c, r->data + 1 + sizeof(id), sizeof(c));

				std::cout << "\nID: " << id<<"\n";

				for (auto& e : entities) {
                    if (e.id == id) {
						e.update(c);//da entityju te coordse
						player.x = e.c.x;
						player.y = e.c.y;
						//e.c = c;
						break;
					}
				}
				//break;
			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(16));
	}
	SDLNet_FreePacket(r);
	
	for (auto& e : entities) {
		e.print();
	}
	

	/*
	if (comms.send(c)) {
		std::cout << "OK: sent\n";
	}
	else {
		std::cout << "ERROR: not sent\n";
	}*/

	/*
	if (comms.recieve()) {
		std::cout << "OK: recieved\n";
	}
	else {
		std::cout << "OK: not recieved\n";
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(1000));


	if (comms.send(y)) {
		std::cout << "OK: sent\n";
	}
	else {
		std::cout << "ERROR: not sent\n";
	}


	if (comms.recieve()) {
		std::cout << "OK: recieved\n";
	}
	else {
		std::cout << "OK: not recieved\n";
	}
	*/

	std::this_thread::sleep_for(std::chrono::milliseconds(25000));
	return 0;
}