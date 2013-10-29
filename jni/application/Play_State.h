#ifndef PLAY_STATE_H
#define PLAY_STATE_H

#include "Crate.h"
#include "Player.h"
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
	void on_mouse_motion(const SDL_MouseMotionEvent &event);

	void perform_logic();

	void render();

private:
	void partial_step(const float &time_step, const Zeni::Vector3f &velocity);

	Zeni::Time_HQ time_passed;

	vector<Crate*> crates;

	Crate m_crate;
	Player m_player;

	bool m_moved;

	Projector3D projector3;
	Projector2D projector2;
};

#endif
