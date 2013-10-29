#include <zenilib.h>

#include "Play_State.h"

using namespace Zeni;
using namespace Zeni::Collision;
#include <iostream>
using namespace std;

Vector2f tr2(0.0f, 0.0f);
Vector2f bl2(854.0f, 480.0f);
Point2i tr3(0, 0);
Point2i bl3(854, 480);
pair<Point2f, Point2f> resolution2 = make_pair(tr2, bl2);

Play_State::Play_State()
	: m_player(
		  Camera(Point3f(0.0f, 0.0f, 50.0f), Quaternion(), 1.0f, 10000.0f)
		, Vector3f(0.0f, 0.0f, -39.0f)
		, 11.0f
	  )
	, firing(false)
{
	crates.push_back(new Crate(Point3f(100.0f, 100.0f, 0.0f)));
	crates.push_back(new Crate(Point3f(100.0f, 200.0f, 0.0f)));

	set_pausable(true);
}

Play_State::~Play_State() {
	while (!crates.empty()) delete crates.back(), crates.pop_back();
}

void Play_State::on_push() {
	get_Window().set_mouse_state(Window::MOUSE_RELATIVE);

	get_Video().set_clear_Color(get_Colors()["white"]);
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

void Play_State::on_mouse_button(const SDL_MouseButtonEvent &event) {
	firing = event.state == SDL_PRESSED;
}

void Play_State::on_mouse_motion(const SDL_MouseMotionEvent &event) {
	m_player.adjust_pitch(event.yrel / 500.0f);
	m_player.turn_left_xy(-event.xrel / 500.0f);
}

void Play_State::perform_logic() {
	projector2 = Projector2D(resolution2, get_Video().get_viewport());
	const pair<Point2i, Point2i> proj_res = make_pair(tr3, bl3);
	projector3 = Projector3D(m_player.get_camera(), proj_res);

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

	if (firing) {
		if (m_player.fire(get_Timer().get_seconds())) {
			player_bullets.push_back(new Bullet(m_player.get_camera().get_forward(), m_player.get_camera().position));
		}
	}

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
		// (*c)->look_at(position);
	}
}

void Play_State::render() {
	// 3D STUFF

	const pair<Point2i, Point2i> proj_res = make_pair(tr3, get_Video().get_render_target_size());

	get_Video().set_3d_view(m_player.get_camera(), proj_res);

	for (vector<Crate*>::iterator c = crates.begin(); c != crates.end(); ++c) {
		(*c)->render();
	}

	for (std::vector<Bullet*>::iterator b = player_bullets.begin(); b != player_bullets.end(); ++b) {
		(*b)->render();
	}

	// 2D STUFF

	get_Video().set_2d(resolution2, true);

	render_image(
		  "CRATE.PNG"
		, bl2/2.0f - Point2f(10.0f, 10.0f)
		, bl2/2.0f + Point2f(10.0f, 10.0f)
	);
}

void Play_State::partial_step(const float &time_step, const Vector3f &velocity) {
	m_player.set_velocity(velocity);
	const Point3f backup_position = m_player.get_camera().position;

	m_player.step(time_step);

	for (vector<Bullet*>::iterator b = player_bullets.begin(); b != player_bullets.end();) {
		(*b)->fly(time_step);

		bool collided = false;

		for (vector<Crate*>::iterator c = crates.begin(); c != crates.end(); ++c) {
			if ((*c)->get_body().intersects((*b)->get_body())) {
				(*c)->collide();
				collided = true;
				break;
			}
		}

		if (collided) {
			delete (*b);
			b = player_bullets.erase(b);
		}
		else {
			++b;
		}
	}

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
