#include "PlayMode.hpp"

//for the GL_ERRORS() macro:
#include "gl_errors.hpp"

#include "AssetImporter.hpp"

//for glm::value_ptr() :
#include <glm/gtc/type_ptr.hpp>

#include <random>

PlayMode::PlayMode() {

	AssetImporter importer;

	importer.LoadPNGS();

	//Also, *don't* use these tiles in your game:
	player_at = initPos;

	{ //walls
		for (int i = 0; i < 30; i++)
		{
			float x = (i * 17.f);
			if (int(x) % 2 == 0) x = -x;
			float y = (float)240 * (float)i / 30;
			walls_at.push_back(glm::vec2(uint8_t(x) % 256, uint8_t(y) % 240));
		}
	}

	{//Spikes
		spikes_at.push_back(glm::vec2(4 * 8, 16));
	}

	importer.loadLevel("level1", &walls_at, &spikes_at);

	{ //use tiles 0-16 as some weird dot pattern thing:
		std::array< uint8_t, 8 * 8 > distance;
		for (uint32_t y = 0; y < 8; ++y) {
			for (uint32_t x = 0; x < 8; ++x) {
				float d = glm::length(glm::vec2((x + 0.5f) - 4.0f, (y + 0.5f) - 4.0f));
				d /= glm::length(glm::vec2(4.0f, 4.0f));
				distance[x + 8 * y] = std::max(0, std::min(255, int32_t(255.0f * d)));
			}
		}
		for (uint32_t index = 0; index < 16; ++index) {
			PPU466::Tile tile;
			uint8_t t = (255 * index) / 16;
			for (uint32_t y = 0; y < 8; ++y) {
				uint8_t bit0 = 0;
				uint8_t bit1 = 0;
				for (uint32_t x = 0; x < 8; ++x) {
					uint8_t d = distance[x + 8 * y];
					if (d > t) {
						bit0 |= (1 << x);
					}
					else {
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
		0b00011000,
		0b00100100,
		0b00000000,
		0b00100100,
		0b00000000,
		0b00000000,
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

	//Spike pallete
	ppu.palette_table[2] = {
		glm::u8vec4(0xFF, 0x00, 0x00, 0xFF),
		glm::u8vec4(0x98,0x96,0x98,0xff),
		glm::u8vec4(0x3c,0x3c,0x3c,0xff),
		glm::u8vec4(0xec,0xee,0xec,0xff),
	};

	//Wall
	ppu.palette_table[3] = {
		glm::u8vec4(0x3c,0x3c,0x3c,0xff),
		glm::u8vec4(0x54,0x54,0x45,0xff),
		glm::u8vec4(0x28,0x72,0x00,0xff),
		glm::u8vec4(0x98, 0x96, 0x98, 0xff),
	};

	//used for the player:
	ppu.palette_table[7] = {
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
		glm::u8vec4(0x98, 0x4b, 0x00, 0xff),
		glm::u8vec4(0xe0, 0xac, 0x69, 0xff),
		glm::u8vec4(0x00, 0x00, 0x00, 0xff),
	};

	//used for the misc other sprites:
	ppu.palette_table[6] = {
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
		glm::u8vec4(0x88, 0x88, 0xff, 0xff),
		glm::u8vec4(0x00, 0x00, 0x00, 0xff),
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
	};

	importer.writeToPPU(&ppu, 0);

	for (uint32_t y = 0; y < PPU466::BackgroundHeight; ++y) {
		for (uint32_t x = 0; x < PPU466::BackgroundWidth; ++x) {
			//TODO: make weird plasma thing
			ppu.background[x + PPU466::BackgroundWidth * y] = ((x + y) % 2);
		}
	}
}

PlayMode::~PlayMode() {
}

void PlayMode::resetPlayer() {
	player_at = initPos;
	grounded = true;
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

	{ //handle player collisions with walls
		for (auto w = walls_at.begin(); w < walls_at.end(); w++)
		{
			unsigned player_at_floor_x = (int)floor(player_at.x);
			unsigned player_at_floor_y = (int)floor(player_at.y);
			if (player_at_floor_x + 8 == (*w).x && (*w).y - 7 <= player_at_floor_y && player_at_floor_y <= (*w).y + 7)
			{ //collides to the left of (*w)
				player_at.x = (*w).x - 8;
			}
			else if (player_at_floor_x - 8 + 1 == (*w).x && (*w).y - 7 <= player_at_floor_y && player_at_floor_y <= (*w).y + 7)
			{ //collides to the right
				player_at.x = (*w).x + 8;
			}
			else if (player_at_floor_y - 8 + 1 == (*w).y && (*w).x - 7 <= player_at_floor_x && player_at_floor_x <= (*w).x + 7)
			{ //collides above
				grounded = true;
				player_at.y = (*w).y + 8;
				player_velocity.y = 0;
			}
			else if (player_at_floor_y + 8 == (*w).y && (*w).x - 7 <= player_at_floor_x && player_at_floor_x <= (*w).x + 7)
			{ //collides below
				player_at.y = (*w).y - 8;
			}
		}
		//Handle collision detection for spikes
		for (auto s = spikes_at.begin(); s < spikes_at.end(); s++)
		{
			unsigned player_at_floor_x = (int)floor(player_at.x);
			unsigned player_at_floor_y = (int)floor(player_at.y);
			if (player_at_floor_y - 8 + 1 == (*s).y && (*s).x - 7 <= player_at_floor_x && player_at_floor_x <= (*s).x + 7)
			{ //collides above
				resetPlayer();
				player_velocity.y = 0;
			}
		}
	}

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
	player_at += player_velocity * elapsed;

	//Bounds check!
	if (player_at.y <= 0)
	{
		player_at.y = 0;
		player_velocity.y = 0;
		grounded = true;
	}
	
}

void PlayMode::draw(glm::uvec2 const &drawable_size) {
	//--- set ppu state based on game state ---

	//background color will be some hsv-like fade:
	ppu.background_color = glm::u8vec4(
		0x44,
		0x44,
		0xff,
		0xff
	);

	//background scroll:
	ppu.background_position.x = int32_t(.6f * player_at.x);
	ppu.background_position.y = 0;// int32_t(player_at.y);

	//player sprite:
	ppu.sprites[0].x = int32_t(player_at.x);
	ppu.sprites[0].y = int32_t(player_at.y);
	ppu.sprites[0].index = 4;
	ppu.sprites[0].attributes = 7;

	size_t offset1 = 1;
	for (size_t k = 0; k < walls_at.size(); k++)
	{
		ppu.sprites[k + offset1].x = int32_t(walls_at[k].x);
		ppu.sprites[k + offset1].y = int32_t(walls_at[k].y);
		ppu.sprites[k + offset1].index = 3;
		ppu.sprites[k + offset1].attributes = 3;
	}

	size_t offset2 = offset1 + walls_at.size();
	//some other misc sprites:
	/*for (size_t i = offset2; i < 63; ++i) {
		float amt = (i + 2.0f * background_fade) / 62.0f;
		ppu.sprites[i].x = int32_t(0.5f * PPU466::ScreenWidth + std::cos( 2.0f * M_PI * amt * 5.0f + 0.01f * player_at.x) * 0.4f * PPU466::ScreenWidth);
		ppu.sprites[i].y = int32_t(0.5f * PPU466::ScreenHeight + std::sin( 2.0f * M_PI * amt * 3.0f + 0.01f * player_at.y) * 0.4f * PPU466::ScreenWidth);
		ppu.sprites[i].index = 32;
		ppu.sprites[i].attributes = 6;
		if (i % 2) ppu.sprites[i].attributes |= 0x80; //'behind' bit
	}*/
	size_t offset3 = offset2;
	assert(spikes_at.size() > 0);
	for (size_t s = 0; s < spikes_at.size(); s++) {
		ppu.sprites[s + offset3].x = int32_t(spikes_at[s].x);
		ppu.sprites[s + offset3].y = int32_t(spikes_at[s].y);
		ppu.sprites[s + offset3].index = 2;
		ppu.sprites[s + offset3].attributes = 2;
	}
	//--- actually draw ---
	ppu.draw(drawable_size);
}