/* This file is part of the Zenipex Library.
* Copyleft (C) 2011 Mitchell Keith Bloch a.k.a. bazald
*
* This source file is simply under the public domain.
*/

#ifndef TUTORIAL_STATE_H
#define TUTORIAL_STATE_H

#include <Zeni/Camera.h>
#include <Zeni/Game.h>
#include <Zeni/Timer.h>
#include <Zeni/Video.h>
#include <Zeni/Vector3f.h>

class Tutorial_State : public Zeni::Gamestate_Base {
	struct Space_Rectangle {
		Space_Rectangle()
			: position(100.0f, 40.0f, 20.0f), 
			vec0(0.0f, -80.0f, 0.0f), 
			vec1(0.0f, 0.0f, -40.0f)
		{
		}

		Zeni::Point3f position;
		Zeni::Vector3f vec0;
		Zeni::Vector3f vec1;
	} m_rect;

	struct Player {
		Player()
			: max_velocity(20.0f, 20.0f),
			mouse_scale(128.0f, 128.0f),
			controller_scale(32.0f, 32.0f)
		{
		}

		Zeni::Camera camera;
		Zeni::Vector2f max_velocity;
		Zeni::Vector2f mouse_scale;
		Zeni::Vector2f controller_scale;
	} m_player;

	struct Controls {
		Controls() : left(false), right(false), up(false), down(false),
			lright(0.0f), lup(0.0f), rright(0.0f), rrup(0.0f) {}
		
		bool left;
		bool right;
		bool up;
		bool down;
		float lright;
		float lup;
		float rright;
		float rrup;
	} m_controls;

public:
	Tutorial_State();
	~Tutorial_State();

	void on_push();
	void on_pop();
	void on_cover();

	void on_controller_axis(const SDL_ControllerAxisEvent &event);
	void on_controller_button(const SDL_ControllerButtonEvent &event);

	void on_key(const SDL_KeyboardEvent &event);

	void on_mouse_motion(const SDL_MouseMotionEvent &event);

	void perform_logic();

private:
	void turn_camera(const Zeni::Point2f &amount);
	void step(const float &time_step);

	void render();

	Zeni::Time m_current_time;
	float m_time_passed;
	float m_max_time_step;  //< Optional
	float m_max_time_steps; //< Optional
};

#endif
