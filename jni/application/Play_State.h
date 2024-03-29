#ifndef PLAY_STATE_H
#define PLAY_STATE_H

#include "Crate.h"
#include "Player.h"
#include "Bullet.h"
#include <Zeni/Gamestate.h>
#include <Zeni/Timer.h>
#include <vector>

using namespace std;

class Play_State : public Zeni::Gamestate_Base {
public:
	Play_State();
	~Play_State();

	void on_push();

	void on_key(const SDL_KeyboardEvent &event);
	void on_mouse_button(const SDL_MouseButtonEvent &event);
	void on_mouse_motion(const SDL_MouseMotionEvent &event);

	void perform_logic();

	void render();

private:
	void partial_step(const float &time_step, const Zeni::Vector3f &velocity);

	Zeni::Time_HQ time_passed;

	vector<Crate*> crates;
	vector<Bullet*> player_bullets;
	vector<Bullet*> player_eaters;
	vector<Bullet*> enemy_bullets;

	Player m_player;

	bool m_moved;
	bool firing;
	bool eating;

	Light light0;
	Material m;

	Projector3D projector3;
	Projector2D projector2;

	bool gameover;
	float gameover_since;

	bool win;
	float win_since;
};

#endif
