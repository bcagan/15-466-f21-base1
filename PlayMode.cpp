#include "PlayMode.hpp"
#include "AssetImporter.hpp"

//for the GL_ERRORS() macro:
#include "gl_errors.hpp"

//for glm::value_ptr() :
#include <glm/gtc/type_ptr.hpp>

#include <random>

#include "CollisionManager.hpp"

PlayMode::PlayMode() {

	//Load in new files, and clean out the old script!
	AssetImporter importer;

	atlas = AssetAtlas();

	collision_manager = CollisionManager(&(ppu.sprites));

	// step 1) read the tiles form the asset atlas
	//std::string assets_path = "./"; // TODO: change this
	//atlas.loadAssets(assets_path); 

	//Remove and instead individually load tiles AND load backgrounds

	// step 2) load the current background and level
	//TODO:
	curr_bg = atlas.getBG("DefaultBackground").tiles;

	// you *must* use an asset pipeline of some sort to generate tiles.
	// don't hardcode them like this!
	// or, at least, if you do hardcode them like this,
	//  make yourself a script that spits out the code that you paste in here
	//   and check that script into your repository.

	//Also, *don't* use these tiles in your game:

	{ //use tiles 0-16 as some weird dot pattern thing:
		std::array< uint8_t, 8*8 > distance;
		for (uint32_t y = 0; y < 8; ++y) {
			for (uint32_t x = 0; x < 8; ++x) {
				float d = glm::length(glm::vec2((x + 0.5f) - 4.0f, (y + 0.5f) - 4.0f));
				d /= glm::length(glm::vec2(4.0f, 4.0f));
				distance[x+8*y] = std::max(0,std::min(255,int32_t( 255.0f * d )));
			}
		}
		for (uint32_t index = 0; index < 16; ++index) {
			PPU466::Tile tile;
			uint8_t t = (255 * index) / 16;
			for (uint32_t y = 0; y < 8; ++y) {
				uint8_t bit0 = 0;
				uint8_t bit1 = 0;
				for (uint32_t x = 0; x < 8; ++x) {
					uint8_t d = distance[x+8*y];
					if (d > t) {
						bit0 |= (1 << x);
					} else {
						bit1 |= (1 << x);
					}
				}
				tile.bit0[y] = bit0;
				tile.bit1[y] = bit1;
			}
			ppu.tile_table[index] = tile;
		}
	}

	//use sprite 32 as a "player":
	ppu.tile_table[32].bit0 = {
		0b01111110,
		0b11111111,
		0b11111111,
		0b11111111,
		0b11111111,
		0b11111111,
		0b11111111,
		0b01111110,
	};
	ppu.tile_table[32].bit1 = {
		0b00000000,
		0b00000000,
		0b00100100,
		0b00011000,
		0b00000000,
		0b01000010,
		0b00100100,
		0b01000010,
	};

	//use sprite 32 as a "player true":
	ppu.tile_table[33].bit0 = {
		0b11111111,
		0b11111111,
		0b11111111,
		0b11111111,
		0b11111111,
		0b11111111,
		0b11111111,
		0b11111111,
	};
	ppu.tile_table[33].bit1 = {
		0b00000000,
		0b01111110,
		0b10111101,
		0b10111101,
		0b11011011,
		0b11011011,
		0b11100111,
		0b11100111,
	};

	//makes the outside of tiles 0-16 solid:
	ppu.palette_table[0] = {
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
		glm::u8vec4(0x00, 0x00, 0x00, 0xff),
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
		glm::u8vec4(0x00, 0x00, 0x00, 0xff),
	};

	//makes the center of tiles 0-16 solid:
	ppu.palette_table[1] = {
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
		glm::u8vec4(0x00, 0x00, 0x00, 0xff),
		glm::u8vec4(0x00, 0x00, 0x00, 0xff),
	};

	//used for the player:
	ppu.palette_table[7] = {
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
		glm::u8vec4(0xff, 0x00, 0xff, 0xff),
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
	};

	//used for the misc other sprites:
	ppu.palette_table[6] = {
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
		glm::u8vec4(0xff, 0x00, 0x00, 0xff),
		glm::u8vec4(0x00, 0x00, 0x00, 0xff),
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
	};

}

PlayMode::~PlayMode() {
}

bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {

	if (evt.type == SDL_KEYDOWN) {
		if (evt.key.keysym.sym == SDLK_LEFT) {
			left.downs += 1;
			left.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_RIGHT) {
			right.downs += 1;
			right.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_UP) {
			up.downs += 1;
			up.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_DOWN) {
			down.downs += 1;
			down.pressed = true;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_SPACE) {
			space.downs += 1;
			space.pressed = true;
			return true;
		}
	} else if (evt.type == SDL_KEYUP) {
		if (evt.key.keysym.sym == SDLK_LEFT) {
			left.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_RIGHT) {
			right.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_UP) {
			up.pressed = false;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_DOWN) {
			down.pressed = false;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_SPACE)
		{
			space.pressed = false;
			return true;
		}
	}

	return false;
}

void PlayMode::update(float elapsed) {

	//slowly rotates through [0,1):
	// (will be used to set background color)
	background_fade += elapsed / 10.0f;
	background_fade -= std::floor(background_fade);

	constexpr float PlayerSpeed = 30.0f;
	player_velocity.x = 0;
	if (left.pressed) player_velocity.x -= PlayerSpeed;
	if (right.pressed) player_velocity.x += PlayerSpeed;

	if (space.pressed && space.held < max_jump_time && grounded)
	{
		jumping = true;
		space.held += elapsed;
		if (space.held < max_jump_time)
		{
			player_velocity.y = jump_speed;
		}
	}
	else
	{
		grounded = false;
		if (space.held > 0.0f && jumping)
		{
			player_velocity.y = 0.0f;
			jumping = false;
		}
		if (!space.pressed)
		{
			space.held = 0.0f;
		}
	}

	player_velocity.y += gravity * elapsed;

	//if (down.pressed) player_at.y -= PlayerSpeed * elapsed;
	//if (up.pressed) player_at.y += PlayerSpeed * elapsed;

	player_at += player_velocity * elapsed;

	//Bounds check!
	if (player_at.y <= 0)
	{
		player_at.y = 0;
		player_velocity.y = 0;
		grounded = true;
	}

	//reset button press counters:
	left.downs = 0;
	right.downs = 0;
	up.downs = 0;
	down.downs = 0;
	space.downs = 0;


	//if player collides with goal, win the game
	if (collision_manager.Collides(player_tile_index, goal_tile_index))
	{
		level_complete();
		next_state = WIN;
	}

	//TODO: if player collides with a light, subtract health
	//TODO: if the player is in shadow, add health
}

void PlayMode::draw(glm::uvec2 const &drawable_size) {
	//--- set ppu state based on game state ---
	//curr_bg = atlas.getBG("Default").tiles;

	//background color will be some hsv-like fade:
	ppu.background_color = glm::u8vec4(
		std::min(255,std::max(0,int32_t(255 * 0.5f * (0.5f + std::sin( 2.0f * M_PI * (background_fade + 0.0f / 3.0f) ) ) ))),
		std::min(255,std::max(0,int32_t(255 * 0.5f * (0.5f + std::sin( 2.0f * M_PI * (background_fade + 1.0f / 3.0f) ) ) ))),
		std::min(255,std::max(0,int32_t(255 * 0.5f * (0.5f + std::sin( 2.0f * M_PI * (background_fade + 2.0f / 3.0f) ) ) ))),
		0xff
	);

	//tilemap gets recomputed every frame as some weird plasma thing:
	//NOTE: don't do this in your game! actually make a map or something :-)
	for (uint32_t y = 0; y < PPU466::BackgroundHeight; ++y) {
		for (uint32_t x = 0; x < PPU466::BackgroundWidth; ++x) {
			//TODO: make weird plasma thing
			ppu.background[x+PPU466::BackgroundWidth*y] = ((x+y)%16);
		}
	}

	//background scroll:
	ppu.background_position.x = int32_t(-0.5f * player_at.x);
	ppu.background_position.y = int32_t(-0.5f * player_at.y);

	//player sprite:
	ppu.sprites[0].x = int32_t(player_at.x);
	ppu.sprites[0].y = int32_t(player_at.y);
	ppu.sprites[0].index = player_tile_index;
	ppu.sprites[0].attributes = 7;

	//goal sprite
	/*
	 * ppu.sprites[1].x = int32_t(goal_at.x);
	 * ppu.sprites[1].y = int32_t(goal_at.y);
	 * ppu.sprites[1].index = <GOAL SPRITE TILE TABLE INDEX>;
	 * ppu.sprites[1].attributes = <GOAL ATTRIBUTES>;
	*/

	//some other misc sprites:
	for (uint32_t i = 2; i < 63; ++i) {
		float amt = (i + 2.0f * background_fade) / 62.0f;
		ppu.sprites[i].x = int32_t(0.5f * PPU466::ScreenWidth + std::cos( 2.0f * M_PI * amt * 5.0f + 0.01f * player_at.x) * 0.4f * PPU466::ScreenWidth);
		ppu.sprites[i].y = int32_t(0.5f * PPU466::ScreenHeight + std::sin( 2.0f * M_PI * amt * 3.0f + 0.01f * player_at.y) * 0.4f * PPU466::ScreenWidth);
		ppu.sprites[i].index = 32;
		ppu.sprites[i].attributes = 6;
		if (i % 2) ppu.sprites[i].attributes |= 0x80; //'behind' bit
	}

	//--- actually draw ---
	ppu.draw(drawable_size);
}

void PlayMode::level_complete()
{
	std::cout << "The level has been completed!" << std::endl;
}

void PlayMode::player_died()
{
	std::cout << "The player has died! :(" << std::endl;
}