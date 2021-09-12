#include "PPU466.hpp"
#include "Mode.hpp"
#include "AssetLoad.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <deque>
#include "CollisionManager.hpp"

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

	// ----- asset importing -----
	AssetAtlas atlas;

	//functions called by main loop:
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;

	// ----- collisions -----
	CollisionManager collision_manager;

	//----- game state -----
	//player position:
	glm::vec2 player_at = glm::vec2(0.0f);
	unsigned player_tile_index = 33;
	glm::vec2 goal_at = glm::vec2(200, 200);
	unsigned goal_tile_index = 34;
	int health; //the player dies when health is less than zero
	std::array<unsigned short, 64> light_levels; // the light levels of each block
	void level_complete(); // TODO
	void subtract_health(int amt) {health -= amt; if (health < 0) player_died(); }
	void player_died(); // TODO

	//input tracking:
	struct Button {
		uint8_t downs = 0;
		uint8_t pressed = 0;
	} left, right, down, up;

	//some weird background animation:
	float background_fade = 0.0f;


	//----- drawing handled by PPU466 -----
	PPU466 ppu;
	//current background
	std::array< TileRef, PPU466::BackgroundWidth* PPU466::BackgroundHeight > curr_bg;
};
