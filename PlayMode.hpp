#include "PPU466.hpp"
#include "Mode.hpp"
#include "AssetLoad.h"
#include "AssetImporter.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <deque>
#include "ObjectTypes.hpp"

/*
 *
 * PlayMode needs to know several things about the game state:
 * 1) the light level of each sprite. 0 = no light, 7 = max light
 * 2) collisions of each sprite. Can the player collide with the sprite? Y or N?
 * 
 */
struct PlayMode : Mode {
	PlayMode();
	virtual ~PlayMode();

	enum GameState
	{
		GAMEPLAY,
		WIN,
		DEAD
	};

	GameState curr_state;
	GameState next_state;

	// ----- asset importing/loading -----
	AssetAtlas atlas;
	AssetImporter importer;

	//functions called by main loop:
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;

	//----- game state -----
	//player position:
	int SPRITES_MAX = 64;
	glm::vec2 player_at = glm::vec2(0, 232);
	std::vector<glm::vec2> walls_at = std::vector<glm::vec2>(SPRITES_MAX);
	std::vector<glm::vec2> lights_at = std::vector<glm::vec2>(SPRITES_MAX);
	std::vector<std::string> lights_type = std::vector<std::string>(SPRITES_MAX); //Fill in with light type strings we want to use
	
	//Can we also have a vector the same size as lights_at which indicates the light type? Could be a Light object reference

	//player velocity
	glm::vec2 player_velocity = glm::vec2(0.0f);
	unsigned player_tile_index = 33;
	unsigned player_sprite_index = 0;
	glm::vec2 goal_at = glm::vec2(248, 0);
	unsigned goal_sprite_index = 34;

	int health; //the player dies when health is less than zero
	
	// the index into the light_levels array must be the same index into the ppu.sprites array
	std::array<unsigned short, 64> light_levels; // the light levels of each block

	void level_complete(); // TODO
	void subtract_health(int amt) {health -= amt; if (health < 0) player_died(); }
	void player_died(); // TODO

	float gravity = -120.0f;
	float max_jump_time = 0.4f;
	float jump_speed = 60.0f;
	bool jumping = false;
	bool grounded = false;

	//input tracking:
	struct Button {
		uint8_t downs = 0;
		uint8_t pressed = 0;
		float held = 0.0f; //Only used for spacebar!
	} left, right, down, up, space;

	//some weird background animation:
	float background_fade = 0.0f;

	//Lighting
	ObjectTypes LightTypeInd = ObjectTypes();
	std::array<std::array< uint8_t, PPU466::BackgroundWidth* PPU466::BackgroundHeight>
		, 3> backgroundColors; //0 = dark, 1 = partially lit, 2 = lit, each an array of pallets
	//Each object should have 3 pallets stored with it, which are swapped out depending on its lighting value.
	// uint8_t whichLight(glm::vec2 lightPos, glm::vec2 objPos, float innerTheta, float outerTheta);
	// void updatePallet();
	// uint8_t updateLightLevel(size_t objInd);


	//----- drawing handled by PPU466 -----
	PPU466 ppu;
	//current background
	std::array< TileAssetData, PPU466::BackgroundWidth* PPU466::BackgroundHeight > curr_bg;
	std::array< uint8_t, PPU466::BackgroundWidth* PPU466::BackgroundHeight > curr_bg_palletes;
	std::array< TileRef, PPU466::BackgroundWidth* PPU466::BackgroundHeight > curr_level;
	unsigned default_wall_tile = 35;
	unsigned default_light_tile = 36;
	unsigned default_wall_pallete = 8;
	void draw_gameplay();
	void draw_win();
	void draw_dead();
};
