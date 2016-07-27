#include<thread>
#include<chrono>
#include "SFML/Graphics.hpp"

#include "mem.h"
#include "gpu.h"

using namespace std;

int tiles[384][8][8];

void GenerateTiles(){
	int j = 0;
	for (int i = 0x8000; i < 0x9800; i += 16){
		unsigned char tile[16];
		for (int k = 0; k < 16; k++){
			tile[k] = mem[i + k];
		}
		for (int k = 0; k < 8; k++){
			for (int t = 0; t < 8; t++){
				char bit1 = (tile[k] >> t)&1;
				char bit2 = (tile[k + 1] >> t)&1;
				unsigned char color = bit1 + (bit2 << 1);
				tiles[i - 0x8000][k][t] = color;
			}
		}
		j++;
	}
}

void GPU()
{
	sf::RenderWindow window(sf::VideoMode(256, 256, 32), "Test");
	sf::Image background;
	sf::Texture texture;
	sf::Sprite sprite;

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		unsigned short current = 0x9800;
		unsigned char image[262144];
		int index = 0;

		for (int i = 0; i < 32; i++){
			for (int j = 0; i < 32; j++){
				for (int x = 0; x < 8; x++){
					for (int z = 0; z < 8; z++){
						switch (tiles[mem[current]][x][z]){
						case 0:
							image[index] = image[index + 1] = image[index + 2] = 255;
							image[index + 3] = 1;
							index += 4;
							break;
						case 1:
							image[index] = image[index + 1] = image[index + 2] = 176;
							image[index + 3] = 1;
							index += 4;
							break;
						case 2:
							image[index] = image[index + 1] = image[index + 2] = 92;
							image[index + 3] = 1;
							index += 4;
							break;
						case 3:
							image[index] = image[index + 1] = image[index + 2] = 0;
							image[index + 3] = 1;
							index += 4;
							break;
						}
					}
				}

				current++;
			}
		}



		window.clear();
		background.create(256,256, image);
		texture.loadFromImage(background);
		sprite.setTexture(texture);
		window.draw(sprite);
		window.display();

		this_thread::sleep_for(chrono::nanoseconds(1674207600));
	}
}