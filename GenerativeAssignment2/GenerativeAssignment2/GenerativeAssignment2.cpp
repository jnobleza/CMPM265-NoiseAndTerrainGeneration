// GenerativeAssignment2.cpp : Defines the entry point for the console application.

#include "stdafx.h"
#include "SFML\Graphics.hpp"
#include <iostream>
#include <noise/noise.h>
#include "noiseutils.h"

using namespace noise;

class TileMap : public sf::Drawable, public sf::Transformable
{
public:

	bool load(const std::string& tileset, sf::Vector2u tileSize, const int* tiles, unsigned int width, unsigned int height)
	{
		// load the tileset texture
		if (!m_tileset.loadFromFile(tileset))
			return false;

		// resize the vertex array to fit the level size
		m_vertices.setPrimitiveType(sf::Quads);
		m_vertices.resize(width * height * 4);

		// populate the vertex array, with one quad per tile
		for (unsigned int i = 0; i < width; ++i)
			for (unsigned int j = 0; j < height; ++j)
			{
				// get the current tile number
				int tileNumber = tiles[i + j * width];

				// find its position in the tileset texture
				int tu = tileNumber % (m_tileset.getSize().x / tileSize.x);
				int tv = tileNumber / (m_tileset.getSize().x / tileSize.x);

				// get a pointer to the current tile's quad
				sf::Vertex* quad = &m_vertices[(i + j * width) * 4];

				// define its 4 corners
				quad[0].position = sf::Vector2f(i * tileSize.x, j * tileSize.y);
				quad[1].position = sf::Vector2f((i + 1) * tileSize.x, j * tileSize.y);
				quad[2].position = sf::Vector2f((i + 1) * tileSize.x, (j + 1) * tileSize.y);
				quad[3].position = sf::Vector2f(i * tileSize.x, (j + 1) * tileSize.y);

				// define its 4 texture coordinates
				quad[0].texCoords = sf::Vector2f(tu * tileSize.x, tv * tileSize.y);
				quad[1].texCoords = sf::Vector2f((tu + 1) * tileSize.x, tv * tileSize.y);
				quad[2].texCoords = sf::Vector2f((tu + 1) * tileSize.x, (tv + 1) * tileSize.y);
				quad[3].texCoords = sf::Vector2f(tu * tileSize.x, (tv + 1) * tileSize.y);
			}

		return true;
	}

private:

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		// apply the transform
		states.transform *= getTransform();

		// apply the tileset texture
		states.texture = &m_tileset;

		// draw the vertex array
		target.draw(m_vertices, states);
	}

	sf::VertexArray m_vertices;
	sf::Texture m_tileset;
};

// Randomize a number.
int randomize() {
	srand(time(NULL));
	unsigned int r = rand() % 256 + 1;
	return r;
}

int main(int argc, char** argv)
{
	// create the window
	noise::module::Perlin myModule;
	sf::RenderWindow window(sf::VideoMode(512, 256), "Tilemap");
	
	// Set srand so that the seed generated looks a bit more randomized.
	srand(time(NULL));
	unsigned int seedNum = rand() % 256 + 1;
	myModule.SetSeed(seedNum);
	utils::NoiseMap heightMap;
	utils::NoiseMapBuilderPlane heightMapBuilder;
	heightMapBuilder.SetSourceModule(myModule);
	heightMapBuilder.SetDestNoiseMap(heightMap);
	heightMapBuilder.SetDestSize(256, 256);
	heightMapBuilder.SetBounds(0.0, 100.0, 0.0, 100.0);
	heightMapBuilder.Build();

	int h = 0;
	
	int level[128];

	// Translate the noise map values into tiles
	for (int i = 0; i < heightMap.GetHeight(); i++) {
		for (int j = 0; j < heightMap.GetWidth(); j++) {
			float value = heightMap.GetValue(i,j);
			if (value < 0) {
				if (value < -0.25) {
					level[h] = 1;
				}
				else {
					level[h] = 0;
				}
			}
			else {
				if (value > 0.75) {
					level[h] = 3;
				}
				else {
					level[h] = 2;
				}
			}
			h++;
		}
	}
	// define the level with an array of tile indices
	//const int level[] =
	//{
	//	0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	//	0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 2, 0, 0, 0, 0,
	//	1, 1, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3,
	//	0, 1, 0, 0, 2, 0, 3, 3, 3, 0, 1, 1, 1, 0, 0, 0,
	//	0, 1, 1, 0, 3, 3, 3, 0, 0, 0, 1, 1, 1, 2, 0, 0,
	//	0, 0, 1, 0, 3, 0, 2, 2, 0, 0, 1, 1, 1, 1, 2, 0,
	//	2, 0, 1, 0, 3, 0, 2, 2, 2, 0, 1, 1, 1, 1, 1, 1,
	//	0, 0, 1, 0, 3, 2, 2, 2, 0, 0, 0, 0, 1, 1, 1, 1,
	//};

	// create the tilemap from the level definition
	TileMap map;
	if (!map.load("tileset.png", sf::Vector2u(32, 32), level, 16, 8))
		return -1;

	// run the main loop
	while (window.isOpen())
	{
		// handle events
		sf::Event event;
		while (window.pollEvent(event))
		{
			// If 1 is pressed on the keyboard, randomize map with new seed.
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1))
			{
				// Reset Noise Map with new, randomized seed.
				myModule.SetSeed(randomize());
				utils::NoiseMap heightMap;
				utils::NoiseMapBuilderPlane heightMapBuilder;
				heightMapBuilder.SetSourceModule(myModule);
				heightMapBuilder.SetDestNoiseMap(heightMap);
				heightMapBuilder.SetDestSize(256, 256);
				heightMapBuilder.SetBounds(0.0, 100.0, 0.0, 100.0);
				heightMapBuilder.Build();

				h = 0;

				// Translate the noise map values into tiles
				for (int i = 0; i < heightMap.GetHeight(); i++) {
					for (int j = 0; j < heightMap.GetWidth(); j++) {
						float value = heightMap.GetValue(i, j);
						if (value < 0) {
							if (value < -0.25) {
								level[h] = 1;
							}
							else {
								level[h] = 0;
							}
						}
						else {
							if (value > 0.75) {
								level[h] = 3;
							}
							else {
								level[h] = 2;
							}
						}
						h++;
					}
				}

				if (!map.load("tileset.png", sf::Vector2u(32, 32), level, 16, 8))
					return -1;
			}

			if (event.type == sf::Event::Closed)
				window.close();
			
		}

		// draw the map
		window.clear();
		window.draw(map);
		window.display();

	}

	return 0;
}

