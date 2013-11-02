#include "Player.h"

using namespace Zeni;
using namespace Zeni::Collision;

#include "iostream"
using namespace std;

Player::Player(const Camera &camera_, const Vector3f &end_point_b_, const float radius_)
	: camera(camera_)
	, m_end_point_b(end_point_b_)
	, m_radius(radius_)
	, m_is_on_ground(false)
	, shield("models/player-shield.3ds")
	, model("models/player.3ds")
{
	camera.fov_rad = Zeni::Global::pi / 3.0f;

	wasdSpeed = 40.0f;
	updownSpeed = 30.0f;

	last_shot_fired = 0.0f;
	shooting_interval = 0.15f;

	shield_start = 0.0f;
	shield_show_length = 0.05f;

	damage = 50;
	health = 10000;
	bullet_range = 200.0f;

	on_mouse_motion(0.0f, 0.0f);
	apply_camera();
	create_body();

	camera_xy = camera_xz = camera.orientation.normalized();

	reset_movement_vectors = false;
}

const Vector3f ORIGIN_VECTOR = Vector3f(0.0, 0.0, 0.0);
const Vector3f FORWARD_AXIS  = Vector3f(1.0, 0.0, 0.0);
const Vector3f LEFT_AXIS     = Vector3f(0.0, 1.0, 0.0);
const Vector3f BACK_AXIS     = Vector3f(-1.0, 0.0, 0.0);
const Vector3f RIGHT_AXIS    = Vector3f(0.0, -1.0, 0.0);
const Vector3f UP_AXIS       = Vector3f(0.0, 0.0, 1.0);
const Vector3f DOWN_AXIS     = Vector3f(0.0, 0.0, -1.0);

bool Vector3f_equals_Vector3f(const Vector3f &lhs, const Vector3f &rhs) {
	if (lhs.x != rhs.x)
		return false;
	if (lhs.y != rhs.y)
		return false;
	if (lhs.z != rhs.z)
		return false;

	return true;
}

bool playerOnKey(bool is_keydown, Vector3f &vector, const Vector3f &axis, Quaternion orientation) {
	if (is_keydown) {
		if (axis.z == 0.0f) {
			vector = orientation * axis;
			vector.z = 0.0f;
		}
		else {
			vector.z = axis.z;
		}
	}
	else {
		vector.x = 0.0f;
		vector.y = 0.0f;
		vector.z = 0.0f;
	}
	return true;
}

bool Player::on_key(const SDL_KeyboardEvent &event) {
	if      (event.keysym.sym == SDLK_w) {
		return playerOnKey(event.type == SDL_KEYDOWN, forwardVector, FORWARD_AXIS, camera.orientation);
	}

	else if (event.keysym.sym == SDLK_a) {
		return playerOnKey(event.type == SDL_KEYDOWN, leftVector, LEFT_AXIS, camera.orientation);
	}

	else if (event.keysym.sym == SDLK_s) {
		return playerOnKey(event.type == SDL_KEYDOWN, backVector, BACK_AXIS, camera.orientation);
	}

	else if (event.keysym.sym == SDLK_d) {
		return playerOnKey(event.type == SDL_KEYDOWN, rightVector, RIGHT_AXIS, camera.orientation);
	}

	else if (event.keysym.sym == SDLK_q) {
		return playerOnKey(event.type == SDL_KEYDOWN, upVector, UP_AXIS, camera.orientation);
	}

	else if (event.keysym.sym == SDLK_e) {
		return playerOnKey(event.type == SDL_KEYDOWN, downVector, DOWN_AXIS, camera.orientation);
	}

	else if (event.keysym.sym == SDLK_SPACE) {
		reset_movement_vectors = event.type == SDL_KEYDOWN;
		return true;
	}

	return false;
}

Vector3f Player::get_next_velocity() {
	if (reset_movement_vectors) {
		if (! Vector3f_equals_Vector3f(forwardVector, ORIGIN_VECTOR)) {
			playerOnKey(true, forwardVector, FORWARD_AXIS, camera.orientation);
		}
		if (! Vector3f_equals_Vector3f(leftVector, ORIGIN_VECTOR)) {
			playerOnKey(true, leftVector, LEFT_AXIS, camera.orientation);
		}
		if (! Vector3f_equals_Vector3f(backVector, ORIGIN_VECTOR)) {
			playerOnKey(true, backVector, BACK_AXIS, camera.orientation);
		}
		if (! Vector3f_equals_Vector3f(rightVector, ORIGIN_VECTOR)) {
			playerOnKey(true, rightVector, RIGHT_AXIS, camera.orientation);
		}
		if (! Vector3f_equals_Vector3f(upVector, ORIGIN_VECTOR)) {
			playerOnKey(true, upVector, UP_AXIS, camera.orientation);
		}
		if (! Vector3f_equals_Vector3f(downVector, ORIGIN_VECTOR)) {
			playerOnKey(true, downVector, DOWN_AXIS, camera.orientation);
		}
	}

	Vector3f wasd = forwardVector + leftVector + backVector + rightVector;
	Vector3f updown = upVector + downVector;

	Vector3f next_velocity = (wasd.normalized() * wasdSpeed) + (updown * updownSpeed);

	// cout << "W " << wasd.x << " " << wasd.y << " " << wasd.z << endl;
	// cout << "U " << updown.x << " " << updown.y << " " << updown.z << endl;
	// cout << "N " << next_velocity.x << " " << next_velocity.y << " " << next_velocity.z << endl;

	return next_velocity;
}

void Player::render() {
	model.set_translate(center);
	model.set_scale(Vector3f(m_radius, m_radius, m_radius));
	model.render();

	if (get_Timer().get_seconds() <= shield_start + shield_show_length) {
		shield.set_translate(center);
		shield.set_scale(Vector3f(m_radius, m_radius, m_radius));
		shield.render();
	}
}

const Zeni::Camera & Player::get_camera() const {
	return camera;
}

// Level 2
void Player::set_position(const Point3f &position_) {
	center = position_;
	create_body();
}

void Player::adjust_pitch(const float &phi) {
	camera_xz = Quaternion::Axis_Angle(Vector3f(0.0f, 1.0f, 0.0f), phi/1000.0f) * camera_xz;
}

void Player::turn_left_xy(const float &theta) {
	camera_xy = Quaternion::Axis_Angle(Vector3f(0.0f, 0.0f, -1.0f), theta/1000.0f) * camera_xy;
}

void Player::on_mouse_motion(const float &x, const float &y) {
	adjust_pitch(y);
	turn_left_xy(x);
}

void Player::apply_camera() {
	Vector3f offset(-m_radius - 100.0f, 0.0f, 0.0);
	camera.position = center + (camera_xy * camera_xz * offset);
	camera.look_at(center);
}

const Zeni::Collision::Sphere & Player::get_body() const {
	return body;
}

bool Player::is_on_ground() const {
	return m_is_on_ground;
}

const Zeni::Vector3f & Player::get_velocity() const {
	return m_velocity;
}

void Player::set_velocity(const Zeni::Vector3f &velocity_) {
	m_velocity = velocity_;
}

void Player::set_on_ground(const bool &is_on_ground_) {
	m_is_on_ground = is_on_ground_;
	if (m_is_on_ground)
		m_velocity.k = 0.0f;
}

void Player::jump() {
	if (m_is_on_ground) {
		m_velocity.k += 60.0f;
		m_is_on_ground = false;
	}
}

void Player::step(const float &time_step) {
	center += time_step * m_velocity;
	create_body();
}

bool Player::fire(float time) {
	if (time >= last_shot_fired + shooting_interval) {
		last_shot_fired = time;
		return true;
	}

	return false;
}

void Player::hit(int damage) {
	health -= damage;
	shield_start = get_Timer().get_seconds();

	create_body();
}

void Player::plusHealth(int additionalHealth) {
	health += additionalHealth;

	create_body();
}

void Player::create_body() {
	Sound &sr = get_Sound();

	health = health < 0 ? 0 : health;

	m_radius = powf(float(health), 0.3333f) + 10.0f;

	body = Sphere(center, m_radius);

	sr.set_listener_position(camera.position);
	sr.set_listener_forward_and_up(camera.get_forward(), camera.get_up());
	sr.set_listener_velocity(m_velocity);
}
