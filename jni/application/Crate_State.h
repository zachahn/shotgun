#ifndef CRATE_STATE_H
#define CRATE_STATE_H

#include "Crate.h"
#include "Player.h"
#include <Zeni/Gamestate.h>
#include <Zeni/Timer.h>
#include <vector>

using namespace std;

class Crate_State : public Zeni::Gamestate_Base {
public:
	Crate_State();
	~Crate_State();

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

	Projector3D projector;
};

#endif
