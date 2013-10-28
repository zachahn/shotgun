#include <zenilib.h>

#include "Crate_State.h"

using namespace Zeni;
using namespace Zeni::Collision;
#include <iostream>
using namespace std;

Crate_State::Crate_State()
	: m_crate(Point3f(12.0f, 12.0f, 0.0f)
	, Vector3f(30.0f, 30.0f, 30.0f))
	, m_player(Camera(Point3f(0.0f, 0.0f, 50.0f)
	, Quaternion()
	, 1.0f, 10000.0f)
	, Vector3f(0.0f, 0.0f, -39.0f)
	, 11.0f)
{
	set_pausable(true);
}

void Crate_State::on_push() {
	get_Window().set_mouse_state(Window::MOUSE_RELATIVE);
}

void Crate_State::on_key(const SDL_KeyboardEvent &event) {
	if (event.repeat) {
		return;
	}

	if (m_player.on_key(event)) {
		return;
	}

	Gamestate_Base::on_key(event);
}

void Crate_State::on_mouse_motion(const SDL_MouseMotionEvent &event) {
	m_player.adjust_pitch(event.yrel / 500.0f);
	m_player.turn_left_xy(-event.xrel / 500.0f);
}

void Crate_State::perform_logic() {
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

	/** Get forward and left vectors in the XY-plane **/
	const Vector3f forward = m_player.get_camera().get_forward().get_ij().normalized();
	const Vector3f left = m_player.get_camera().get_left().get_ij().normalized();

	/** Get velocity vector split into a number of axes **/
	// const Vector3f velocity = 50.0f * m_player.forwardVector;(m_controls.forward - m_controls.back) * 50.0f * forward
	// 												+ (m_controls.left - m_controls.right) * 50.0f * left;
	const Vector3f velocity = m_player.get_next_velocity() * 50.0f;
	const Vector3f x_vel = velocity.get_i();
	const Vector3f y_vel = velocity.get_j();
	// Vector3f z_vel = m_player.get_velocity().get_k();
	Vector3f z_vel = velocity.get_k();

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

		/** Gravity has its effect **/
		z_vel -= Vector3f(0.0f, 0.0f, 50.0f * time_step);

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
}

void Crate_State::render() {
	// const pair<Point2i, Point2i> proj_res = make_pair(
	// 	  Point2i(0, 0)
	// 	, Point2i(854, 480)
	// );

	// get_Video().set_3d_view(
	// 	  m_player.get_camera()
	// 	, proj_res
	// );
	get_Video().set_3d(m_player.get_camera());

	m_crate.render();

	render_image(
		  "CRATE.PNG"
		, Point2f(0.0f, 0.0f)
		, Point2f(1000.0f, 1000.0f)
	);
}

void Crate_State::partial_step(const float &time_step, const Vector3f &velocity) {
	m_player.set_velocity(velocity);
	const Point3f backup_position = m_player.get_camera().position;

	m_player.step(time_step);

	/** If collision with the crate has occurred, roll things back **/
	if (m_crate.get_body().intersects(m_player.get_body())) {
		if (m_moved) {
			/** Play a sound if possible **/
			m_crate.collide();
			m_moved = false;
		}

		m_player.set_position(backup_position);

		/** Bookkeeping for jumping controls **/
		if (velocity.k < 0.0f)
			m_player.set_on_ground(true);
	}
}
