#include <zenilib.h>

#include "Play_State.h"

using namespace Zeni;
using namespace Zeni::Collision;
#include <iostream>
using namespace std;

Play_State::Play_State()
	: m_player(Camera(Point3f(0.0f, 0.0f, 50.0f)
	, Quaternion()
	, 1.0f, 10000.0f)
	, Vector3f(0.0f, 0.0f, -39.0f)
	, 11.0f)
{
	crates.push_back(new Crate(Point3f(100.0f, 100.0f, 0.0f)));
	crates.push_back(new Crate(Point3f(12.0f, 12.0f, 0.0f)));

	set_pausable(true);
}

Play_State::~Play_State() {
	while(! crates.empty()) delete crates.back(), crates.pop_back();
}

void Play_State::on_push() {
	get_Window().set_mouse_state(Window::MOUSE_RELATIVE);
}

void Play_State::on_key(const SDL_KeyboardEvent &event) {
	if (event.repeat) {
		return;
	}

	if (m_player.on_key(event)) {
		return;
	}

	Gamestate_Base::on_key(event);
}

void Play_State::on_mouse_motion(const SDL_MouseMotionEvent &event) {
	m_player.adjust_pitch(event.yrel / 500.0f);
	m_player.turn_left_xy(-event.xrel / 500.0f);
}

void Play_State::perform_logic() {
	// const pair<Point2i, Point2i> proj_res = make_pair(
	// 	  Point2i(0, 0)
	// 	, Point2i(854, 480)
	// );
	// projector = Projector3D(
	// 	  m_player.get_camera()
	// 	, proj_res
	// );

	const Time_HQ current_time = get_Timer_HQ().get_time();
	float processing_time = float(current_time.get_seconds_since(time_passed));
	time_passed = current_time;

	const Vector3f velocity = m_player.get_next_velocity();
	const Vector3f x_vel = velocity.get_i();
	const Vector3f y_vel = velocity.get_j();
	const Vector3f z_vel = velocity.get_k();

	/** Bookkeeping for sound effects **/
	if (velocity.magnitude() != 0.0f)
		m_moved = true;

	/** Keep delays under control (if the program hangs for some time, we don't want to lose responsiveness) **/
	if (processing_time > 0.1f)
		processing_time = 0.1f;

	/** Physics processing loop**/
	for (float time_step = 0.05f;
			processing_time > 0.0f;
			processing_time -= time_step)
	{
		if (time_step > processing_time)
			time_step = processing_time;

		/** Try to move on each axis **/
		partial_step(time_step, x_vel);
		partial_step(time_step, y_vel);
		partial_step(time_step, z_vel);

		/** Keep player above ground; Bookkeeping for jumping controls **/
		const Point3f &position = m_player.get_camera().position;
		if (position.z < 50.0f) {
			m_player.set_position(Point3f(position.x, position.y, 50.0f));
			m_player.set_on_ground(true);
		}
	}

	const Point3f &position = m_player.get_camera().position;
	for (std::vector<Crate*>::iterator c = crates.begin(); c != crates.end(); ++c) {
		(*c)->look_at(position);
	}
}

void Play_State::render() {
	// const pair<Point2i, Point2i> proj_res = make_pair(
	// 	  Point2i(0, 0)
	// 	, Point2i(854, 480)
	// );

	// get_Video().set_3d_view(
	// 	  m_player.get_camera()
	// 	, proj_res
	// );
	get_Video().set_3d(m_player.get_camera());

	for (vector<Crate*>::iterator c = crates.begin(); c != crates.end(); ++c) {
		(*c)->render();
	}

	render_image(
		  "CRATE.PNG"
		, Point2f(0.0f, 0.0f)
		, Point2f(1000.0f, 1000.0f)
	);
}

void Play_State::partial_step(const float &time_step, const Vector3f &velocity) {
	m_player.set_velocity(velocity);
	const Point3f backup_position = m_player.get_camera().position;

	m_player.step(time_step);

	for (vector<Crate*>::iterator c = crates.begin(); c != crates.end(); ++c) {
		/** If collision with the crate has occurred, roll things back **/
		if ((*c)->get_body().intersects(m_player.get_body())) {
			if (m_moved) {
				/** Play a sound if possible **/
				(*c)->collide();
				m_moved = false;
			}

			m_player.set_position(backup_position);

			/** Bookkeeping for jumping controls **/
			if (velocity.k < 0.0f)
				m_player.set_on_ground(true);
		}
	}
}