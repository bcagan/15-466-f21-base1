#include "PlayMode.hpp"
#include "AssetImporter.hpp"

//for the GL_ERRORS() macro:
#include "gl_errors.hpp"

//for glm::value_ptr() :
#include <glm/gtc/type_ptr.hpp>

#include <random>

#define ERROR_FLOAT 0.000005f

PlayMode::PlayMode() {

	//Load in new files, and clean out the old script!
	AssetImporter importer;

	//Regenerate all of our files!
	importer.WritePngsToFile();

	//Create a new atlas, load all saved pngs into it.
	importer.LoadTiles(atlas);

	//atlas = AssetAtlas();
	walls_at.push_back(glm::vec2(PPU466::ScreenWidth / 2, 1)); // test wall
\
	// step 1) read the tiles form the asset atlas
	importer.LoadBackground(atlas);
	importer.LoadLevel(atlas, "levels/dummy_level/png");
	/*
	importer.writePngToSave("TestLevel");
	*/
	//Remove and instead individually load tiles AND load backgrounds

	// step 2) populate the ppu tiles, sprite locations, and background
	for (int i = 0; i < atlas.tiles.size(); i++)
	{
		ppu.tile_table[i].bit0 = atlas.tiles[i].bit0;
		ppu.tile_table[i].bit1 = atlas.tiles[i].bit1;
	}

	LevelRetType level = atlas.getLevel("DefaultLevel");
	curr_level = level.tiles;
	for (int i = 0; i < curr_level.size(); i++)
	{
		// TileRef curr_tile = curr_level[i];

		// if the curr tile is a light, add to lights_at

		// if the curr tile is a wall, add to walls_at
	}
	

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

	//wall sprite:
	ppu.tile_table[default_wall_tile].bit0 = {
		0b11111111,
		0b11111111,
		0b11111111,
		0b11111111,
		0b11111111,
		0b11111111,
		0b11111111,
		0b11111111,
	};

	// wall sprite:
	ppu.tile_table[default_wall_tile].bit1 = {
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
	};
	//light sprite:
	ppu.tile_table[default_wall_tile].bit0 = {
		0b11111111,
		0b11111111,
		0b11111111,
		0b11111111,
		0b11111111,
		0b11111111,
		0b11111111,
		0b11111111,
	};

	// light sprite:
	ppu.tile_table[default_wall_tile].bit1 = {
		0b11111111,
		0b11111111,
		0b11111111,
		0b11111111,
		0b11111111,
		0b11111111,
		0b11111111,
		0b11111111,
	};

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
	ppu.tile_table[player_tile_index].bit0 = {
		0b11111111,
		0b11111111,
		0b11111111,
		0b11111111,
		0b11111111,
		0b11111111,
		0b11111111,
		0b11111111,
	};
	ppu.tile_table[player_tile_index].bit1 = {
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

	//wall:
	ppu.palette_table[default_wall_pallete] = {
		glm::u8vec4(0xff, 0x00, 0x00, 0x00),
		glm::u8vec4(0xff, 0xff, 0x00, 0x00),
		glm::u8vec4(0x00, 0x00, 0xff, 0x00),
		glm::u8vec4(0xff, 0xff, 0x00, 0x00),
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

	{ //handle player collisions with walls
		for (auto w = walls_at.begin(); w < walls_at.end(); w++)
		{
			unsigned player_at_floor_x = (int)floor(player_at.x);
			unsigned player_at_floor_y = (int)floor(player_at.y);
			if (player_at_floor_x + 8 == (*w).x && (*w).y - 7 <= player_at_floor_y && player_at_floor_y <= (*w).y + 7)
			{ //collides to the left of (*w)
				player_at.x = (*w).x - 8;
			} else if (player_at_floor_x - 8  + 1 == (*w).x && (*w).y - 7 <= player_at_floor_y && player_at_floor_y <= (*w).y + 7)
			{ //collides to the right
				player_at.x = (*w).x + 8;
			} else if (player_at_floor_y - 8 + 1 == (*w).y && (*w).x - 7 <= player_at_floor_x && player_at_floor_x <= (*w).x + 7)
			{ //collides above
				grounded = true;
				player_at.y = (*w).y + 8;
				player_velocity.y = 0;
			} else if (player_at_floor_y + 8 == (*w).y && (*w).x - 7 <= player_at_floor_x && player_at_floor_x <= (*w).x + 7)
			{ //collides below
				player_at.y = (*w).y - 8;
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
	if (false) // TODO: replace with goal collision
	{
		level_complete();
		next_state = WIN;
	}
	
	//TODO: if player collides with a light, subtract health
	//TODO: if the player is in shadow, add health
}

void PlayMode::draw(glm::uvec2 const &drawable_size) {
	
	switch(curr_state)
	{
		case GAMEPLAY:
			draw_gameplay();
			break;
		case WIN:
			draw_win();
			break;
		case DEAD:
			draw_dead();
			break;
	}
	//--- actually draw ---
	ppu.draw(drawable_size);
}

//Lighting

//Checks if object is with in outer or inner range of spotlight, and gives lighting value accordingly
/*uint8_t PlayMode::whichLight(glm::vec2 lightPos, glm::vec2 objPos, float innerTheta, float outerTheta) {
	if (objPos.y + 4.f > lightPos.y) return 0;
	assert(innerTheta != 0.0f && innerTheta != 90.0f && outerTheta != 0.0f && outerTheta != 90.0f);

	//Derived from https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-box-intersection

	auto getVal = [this](glm::vec2 lightPos, glm::vec2 objPos, float theta){
		
		float minX = objPos.x + 4.f;
		float maxX = objPos.x - 4.f;
		if (abs(lightPos.x - (objPos.x - 4.f)) < abs(lightPos.x - (objPos.x + 4.f))) {
			minX = objPos.x - 4.f; maxX = objPos.x + 4.f;
		}
		glm::vec2 b0 = glm::vec2(minX, objPos.y + 4.f); //close
		glm::vec2 b1 = glm::vec2(maxX, objPos.y - 4.f); //far	


		bool doesLeft = true;
		bool doesRight = true;
		float tx = (b0.x - lightPos.x) / -cos(theta);//? is this dX?
		float ty = (b0.y - lightPos.y) / -sin(theta);
		float tMinInLeft = (tx > ty) ? tx : ty;
		float t1x = (b1.x - lightPos.x) / -cos(theta);//? is this dX?
		float t1y = (b1.y - lightPos.y) / -sin(theta);
		float tMaxInLeft = (tx < ty) ? tx : ty;
		//intersection check
		if (tx > t1y || ty > t1x) doesLeft = false;

		tx = (b0.x - lightPos.x) / cos(theta);//? is this dX?
		ty = (b0.y - lightPos.y) / -sin(theta);
		float tMinInRight = (tx > ty) ? tx : ty;
		t1x = (b1.x - lightPos.x) / cos(theta);//? is this dX?
		t1y = (b1.y - lightPos.y) / -sin(theta);
		float tMaxInRight = (tx < ty) ? tx : ty;
		//intersection check
		if (tx > t1y || ty > t1x) doesRight = false;
		//Inner val
		float retVal = 0.0f;
		if (doesLeft && doesRight) { //If both left and right intersect box
			glm::vec2 minIntLeft = glm::vec2(-tMinInLeft * cos(innerTheta) + lightPos.x,
			-tMinInLeft * sin(innerTheta) + lightPos.y);
			glm::vec2 maxIntLeft = glm::vec2(-tMaxInLeft * cos(innerTheta) + lightPos.x,
			-tMaxInLeft * sin(innerTheta) + lightPos.y);
			glm::vec2 minIntRight = glm::vec2(tMinInRight * cos(innerTheta) + lightPos.x,
			-tMinInRight * sin(innerTheta) + lightPos.y);
			glm::vec2 maxIntRight = glm::vec2(tMaxInRight * cos(innerTheta) + lightPos.x,
			-tMaxInRight * sin(innerTheta) + lightPos.y);

			//Due to location of light,we know min hits are on top, and max are on sides
			retVal = 64.f;
			retVal -= (minIntLeft.x - objPos.x + 4.f) * (maxIntLeft.y - objPos.y + 4.f) / 2;
			retVal -= (objPos.x + 4.f - minIntRight.x) * (objPos.y + 4.f - maxIntRight.y) / 2;
			retVal /= 64.f;

		}
		else if (doesLeft) { //If only left does
			if(minIntLeft.y >= objPos.x + 4.f - ERROR_FLOAT){
				retVal = 64.f;
				retVal -= (minIntLeft.x - objPos.x + 4.f) * (maxIntLeft.y - objPos.y + 4.f) / 2;
				retVal /= 64.f;
			}
			else{
				retVal = 64.f;
				retVal -= (manIntLeft.x - objPos.x + 4.f) * (minIntLeft.y - objPos.y + 4.f) / 2;
				retVal/= 64.fl
			}
		}
		else if (doesRight) { //If only right does, consider left the left side of box
			if(minIntRight.y >= objPos.x + 4.f - ERROR_FLOAT){
				retVal = 64.f;
				retVal -= (objPos.x + 4.f - minIntRight.x) * (objPos.y + 4.f - maxIntRight.y) / 2;
				retVal /= 64.f;
			}
			else{
				retVal = 64.f;
				retVal -= (objPos.x + 4.f - maxIntRight.x) * (objPos.y + 4.f - minIntRight.y) / 2;
				retVal/= 64.f;
			}
		}
		return retVal;
	};

	float innerVal = 2.0f*getVal(lightPos, objPos, innerTheta);
	float outerVal = getVal(lightPos, objPos, outerTheta) - innerVal/2;
	return roundf(innerVal + outerVal);
}




//x,y, in [256,240]
void PlayMode::updatePallet() {
	for (size_t x = 0; x < PPU466::BackgroundWidth; x++) {
		for (size_t y = 0; y < PPU466::BackgroundHeight; y++) {
			glm::vec2 objPos = glm::vec2((float)(x * 8 + 4), (float)(y * 8 + 4));
			uint8_t lightVal = 0;
			for (size_t whichLight = 0; whichLight < lights_at.size(); whichLight++) {
				uint8_t tempVal = whichLight(lights_at[whichLight], objPos, lights_object[whichLight].inner, lights_object[whichLight].outer);
				if (tempVal > lightVal) lightVal = tempVal;
			}
			size_t ind = y * PPU466::BackgroundWidth + x;
			curr_bg.pallets[ind] = backgroundColors[lightVal][ind];
		}
	}
}
*/

//x,y, in [256,240]
/*void PlayMode::updateLightLevels() {
	for (size_t objInd = 0; objInd < light_levels.size(); objInd++) {
		glm::vec2 objPos = glm::vec2((sprits[objInd]).x + 4.f, (sprites[objInd]).y + 4.f);
		uint8_t lightVal = 0;
		for (size_t whichLight = 0; whichLight < lights.size(); whichLight++) {
			uint8_t tempVal = whichLight(lights[whichLight].pos, objPos, lights[whichLight].inner, lights[whichLight].outer);
			if (tempVal > lightVal) lightVal = tempVal;
		}
		lightLevels[ind] = lightVal;
	}
}
*/

void PlayMode::level_complete()
{
	std::cout << "The level has been completed!" << std::endl;
}

void PlayMode::player_died()
{
	std::cout << "The player has died! :(" << std::endl;
}

void PlayMode::draw_gameplay()
{
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
	ppu.sprites[player_sprite_index].x = int32_t(player_at.x);
	ppu.sprites[player_sprite_index].y = int32_t(player_at.y);
	ppu.sprites[player_sprite_index].index = player_tile_index;
	ppu.sprites[player_sprite_index].attributes = 7;

	//goal sprite
	ppu.sprites[1].x = int32_t(goal_at.x);
	ppu.sprites[1].y = int32_t(goal_at.y);
	ppu.sprites[1].index = goal_sprite_index;
	ppu.sprites[1].attributes = 7;

	//platforms and walls
	int i;
	for (i = 2; i < walls_at.size() + 2; i++)
	{
		ppu.sprites[i].x = walls_at[i - 2].x;
		ppu.sprites[i].y = walls_at[i - 2].y;
		ppu.sprites[i].index = default_wall_tile;
		ppu.sprites[i].attributes = default_wall_pallete;
	}

	//lights
	for (int j = i; j < i + lights_at.size(); j++)
	{
		ppu.sprites[j].x = lights_at[j - i].x;
		ppu.sprites[j].y = walls_at[j - i].y;
		ppu.sprites[j].index = default_light_tile;
		ppu.sprites[j].attributes = default_wall_pallete;
	}
	
	//some other misc sprites:
	for (uint32_t i = 3; i < 63; ++i) {
		float amt = (i + 2.0f * background_fade) / 62.0f;
		ppu.sprites[i].x = int32_t(0.5f * PPU466::ScreenWidth + std::cos( 2.0f * M_PI * amt * 5.0f + 0.01f * player_at.x) * 0.4f * PPU466::ScreenWidth);
		ppu.sprites[i].y = int32_t(0.5f * PPU466::ScreenHeight + std::sin( 2.0f * M_PI * amt * 3.0f + 0.01f * player_at.y) * 0.4f * PPU466::ScreenWidth);
		ppu.sprites[i].index = 32;
		ppu.sprites[i].attributes = 6;
		if (i % 2) ppu.sprites[i].attributes |= 0x80; //'behind' bit
	}

}

void PlayMode::draw_win()
{

}

void PlayMode::draw_dead()
{

}