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
		, 15.0f
	  )
	, firing(false)
	, eating(false)
{
	gameover = false;
	gameover_since = 0.0f;

	crates.push_back(new Crate(Point3f(100.0f, 100.0f, 0.0f), Crate::POWERFUL, Crate::SMALL));
	crates.push_back(new Crate(Point3f(100.0f, 200.0f, 0.0f), Crate::SUPER, Crate::BIG));
	crates.push_back(new Crate(Point3f(300.0f, 500.0f, 0.0f), Crate::RANGE, Crate::HUGE));

	set_pausable(true);

	// get_Video().set_ambient_lighting(Color(1.0f, 0.0f, 0.0f, 0.0f));
	get_Video().set_ambient_lighting(Color(1.0f, 1.0f, 1.0f, 1.0f));

	get_Video().set_Material(m);

	get_Video().set_Light(0, light0);
}

Play_State::~Play_State() {
	while (!crates.empty()) delete crates.back(), crates.pop_back();
}

void Play_State::on_push() {
	get_Window().set_mouse_state(Window::MOUSE_RELATIVE);

	// get_Video().set_clear_Color(get_Colors()["white"]);
}

void Play_State::on_key(const SDL_KeyboardEvent &event) {
	if (gameover) {
		return;
	}

	if (event.repeat) {
		return;
	}

	if (m_player.on_key(event)) {
		return;
	}

	Gamestate_Base::on_key(event);
}

void Play_State::on_mouse_button(const SDL_MouseButtonEvent &event) {
	if (gameover) {
		firing = false;
		return;
	}

	if (event.button == SDL_BUTTON_LEFT) {
		firing = event.state == SDL_PRESSED;
	}
	else if (event.button == SDL_BUTTON_RIGHT) {
		eating = event.state == SDL_PRESSED;
	}
}

void Play_State::on_mouse_motion(const SDL_MouseMotionEvent &event) {
	m_player.on_mouse_motion(float(event.xrel), float(event.yrel));
}

Vector3f ORIGIN_SPRAY(float mag) {
	int big(mag * 2 * 10);

	return Vector3f(
		(float(rand() % big)/10.0f) - mag,
		(float(rand() % big)/10.0f) - mag,
		(float(rand() % big)/10.0f) - mag
	);
}

void Play_State::perform_logic() {
	projector2 = Projector2D(resolution2, get_Video().get_viewport());
	const pair<Point2i, Point2i> proj_res = make_pair(tr3, bl3);
	projector3 = Projector3D(m_player.get_camera(), proj_res);

	const Time_HQ current_time = get_Timer_HQ().get_time();
	float processing_time = float(current_time.get_seconds_since(time_passed));
	time_passed = current_time;

	if (gameover && get_Timer().get_seconds() >= gameover_since + 2.0f) {
		get_Game().pop_state();
	}

	const Vector3f velocity = m_player.get_next_velocity();
	const Vector3f x_vel = velocity.get_i();
	const Vector3f y_vel = velocity.get_j();
	const Vector3f z_vel = velocity.get_k();

	/** Bookkeeping for sound effects **/
	if (velocity.magnitude() != 0.0f)
		m_moved = true;

	// player attacks
	if (firing) {
		if (m_player.fire(get_Timer().get_seconds())) {
			player_bullets.push_back(
				new Bullet(
					m_player.get_camera().get_forward(),
					m_player.center + ORIGIN_SPRAY(5.0f),
					Bullet::ATTACK,
					m_player.damage
				)
			);
		}
	}

	else if (eating) {
		if (m_player.fire(get_Timer().get_seconds())) {
			player_eaters.push_back(
				new Bullet(
					m_player.get_camera().get_forward(),
					m_player.center + ORIGIN_SPRAY(5.0f),
					Bullet::EAT,
					m_player.damage
				)
			);
		}
	}


	// enemy attack and movement
	const Vector3f &position = m_player.center;
	for (vector<Crate*>::iterator c = crates.begin(); c != crates.end(); ++c) {
		Vector3f v = position - (*c)->center;
		if (v.magnitude() <= (*c)->range) {
			if ((*c)->fire(get_Timer().get_seconds())) {
				v += ORIGIN_SPRAY(100.0f);
				enemy_bullets.push_back(
					new Bullet(
						v.normalized(),
						(*c)->center + ORIGIN_SPRAY(7.0f),
						Bullet::ENEMY,
						(*c)->damage
					)
				);
			}
		}

		if      ((*c)->follow == 1 && (*c)->health > 0 ) {
			if (v.magnitude() >= 100.0f) {
				Vector3f a = v;
				a.z = 0.0f;
				(*c)->set_position( (*c)->center + (a.normalized() * 20.0f *  processing_time) );
			}
		}
		else if ((*c)->follow == 2 && (*c)->health > 0) {
			if (v.magnitude() <= 500.0f) {
				Vector3f a = -v;
				a.z = 0.0f;
				(*c)->set_position( (*c)->center + (a.normalized() * 25.0f *  processing_time) );
			}
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
		const Point3f &position = m_player.center;
		if (position.z < 50.0f) {
			m_player.set_position(Point3f(position.x, position.y, 50.0f));
			m_player.set_on_ground(true);
		}
	}
}

void Play_State::render() {
	// 3D STUFF
	const pair<Point2i, Point2i> proj_res = make_pair(tr3, get_Video().get_render_target_size());

	m_player.apply_camera();

	get_Video().set_lighting(true);
	get_Video().set_3d_view(m_player.get_camera(), proj_res);

	for (vector<Crate*>::iterator c = crates.begin(); c != crates.end(); ++c) {
		(*c)->render();
	}

	for (vector<Bullet*>::iterator b = player_bullets.begin(); b != player_bullets.end(); ++b) {
		(*b)->render();
	}

	for (vector<Bullet*>::iterator b = player_eaters.begin(); b != player_eaters.end(); ++b) {
		(*b)->render();
	}

	for (vector<Bullet*>::iterator b = enemy_bullets.begin(); b != enemy_bullets.end(); ++b) {
		(*b)->render();
	}

	m_player.render();

	// 2D STUFF
	get_Video().set_lighting(false);
	get_Video().set_2d(resolution2, true);

	render_image(
		  "crosshair"
		, bl2/2.0f - Point2f(10.0f, 10.0f)
		, bl2/2.0f + Point2f(10.0f, 10.0f)
	);

	get_Fonts()["onscreen"].render_text("Health: " + ulltoa(m_player.health), Point2f(), Color());
	get_Fonts()["onscreen"].render_text("Weapon Damage: " + ulltoa(m_player.damage), Point2f(0.0f, 30.0f), Color());
	get_Fonts()["onscreen"].render_text("Weapon Rabge: " + ulltoa(m_player.bullet_range), Point2f(0.0f, 60.0f), Color());
	if (gameover) {
		get_Fonts()["onscreen"].render_text("GAME OVER", Point2f(0.0f, 90.0f), get_Colors()["red"]);
	}
}

float MAX_BULLET_DISTANCE = 1000.0f;

void Play_State::partial_step(const float &time_step, const Vector3f &velocity) {
	m_player.set_velocity(velocity);
	const Point3f backup_position = m_player.center;

	m_player.step(time_step);

	// player bullets hit enemy
	for (vector<Bullet*>::iterator b = player_bullets.begin(); b != player_bullets.end();) {
		(*b)->fly(time_step);

		bool collided = false;

		for (vector<Crate*>::iterator c = crates.begin(); c != crates.end(); ++c) {
			if ((*c)->get_body().intersects((*b)->get_body())) {
				(*c)->hit((*b)->damage);
				m_player.health += (*b)->damage;

				collided = true;
				break;
			}
		}

		// remove collided bullets
		if (collided) {
			delete (*b);
			b = player_bullets.erase(b);
		}
		else if ((*b)->distance_travelled >= m_player.bullet_range) {
			delete (*b);
			b = player_bullets.erase(b);
		}
		else {
			++b;
		}
	}

	// player bullets hit enemy
	for (vector<Bullet*>::iterator b = player_eaters.begin(); b != player_eaters.end();) {
		(*b)->fly(time_step);

		bool collided = false;

		for (vector<Crate*>::iterator c = crates.begin(); c != crates.end(); ++c) {
			if ((*c)->get_body().intersects((*b)->get_body())) {
				(*c)->hit((*b)->damage);
				m_player.health += (*b)->damage;

				collided = true;
				break;
			}
		}

		// remove collided bullets
		if (collided) {
			delete (*b);
			b = player_eaters.erase(b);
		}
		else if ((*b)->distance_travelled >= m_player.bullet_range) {
			delete (*b);
			b = player_eaters.erase(b);
		}
		else {
			++b;
		}
	}

	// enemy bullets hit player
	for (vector<Bullet*>::iterator b = enemy_bullets.begin(); b != enemy_bullets.end();) {
		(*b)->fly(time_step);

		if ((*b)->get_body().intersects(m_player.get_body())) {
			m_player.hit((*b)->damage);
			if (m_player.health <= 0) {
				gameover = true;
				gameover_since = get_Timer().get_seconds();
			}
			delete (*b);
			b = enemy_bullets.erase(b);
		}
		else if ((*b)->distance_travelled >= MAX_BULLET_DISTANCE) {
			delete (*b);
			b = enemy_bullets.erase(b);
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

			// m_player.hit(get_Timer().get_seconds());

			m_player.set_position(backup_position);

			/** Bookkeeping for jumping controls **/
			if (velocity.k < 0.0f)
				m_player.set_on_ground(true);
		}
	}
}
