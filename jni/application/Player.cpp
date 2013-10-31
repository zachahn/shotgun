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

	wasdSpeed = 50.0f;
	updownSpeed = 30.0f;

	last_shot_fired = 0.0f;
	shooting_interval = 0.2f;

	shield_start = 0.0f;
	shield_show_length = 0.1f;

	damage = 500;
	health = 10000;

	on_mouse_motion(0.0f, 0.0f);
	apply_camera();
	create_body();

	camera_xy = camera_xz = camera.orientation.normalized();
}

bool playerOnKey(const SDL_KeyboardEvent &event, Vector3f &vector, Vector3f &axis, Quaternion orientation) {
	if (event.type == SDL_KEYDOWN) {
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
		Vector3f forward = Vector3f(1.0, 0.0, 0.0);
		return playerOnKey(event, forwardVector, forward, camera.orientation);
	}

	else if (event.keysym.sym == SDLK_a) {
		Vector3f left = Vector3f(0.0, 1.0, 0.0);
		return playerOnKey(event, leftVector, left, camera.orientation);
	}

	else if (event.keysym.sym == SDLK_s) {
		Vector3f back = Vector3f(-1.0, 0.0, 0.0);
		return playerOnKey(event, backVector, back, camera.orientation);
	}

	else if (event.keysym.sym == SDLK_d) {
		Vector3f right = Vector3f(0.0, -1.0, 0.0);
		return playerOnKey(event, rightVector, right, camera.orientation);
	}

	else if (event.keysym.sym == SDLK_q) {
		Vector3f up = Vector3f(0.0, 0.0, 1.0);
		return playerOnKey(event, upVector, up, camera.orientation);
	}

	else if (event.keysym.sym == SDLK_e) {
		Vector3f down = Vector3f(0.0, 0.0, -1.0);
		return playerOnKey(event, downVector, down, camera.orientation);
	}

	return false;
}

Vector3f Player::get_next_velocity() {
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
	// const Quaternion backup = camera.orientation;
	// const Vector3f backup_up = camera.get_up();

	// camera.adjust_pitch(phi);

	// if (camera.get_up().k < 0.0f && backup_up.k >= 0.0f)
	// 	camera.orientation = backup;

	// camera.position += camera.get_left().get_ij().normalized() * distance;
	// camera.position.z += phi;
	// camera.look_at(center, Vector3f(0.0f, 1.0f, 0.0f));


	camera_xz = Quaternion::Axis_Angle(Vector3f(0.0f, 1.0f, 0.0f), phi/1000.0f) * camera_xz;
	// Quaternion yz_ = Quaternion::Axis_Angle(Vector3f(0.0f, 1.0f, 0.0f), phi/1000.0f) * camera.orientation;

	// Vector3f offset(-100.0f, 0.0f, 0.0);
	// camera.position = center + (yz_ * offset);
	// camera.look_at(center);
}

void Player::turn_left_xy(const float &theta) {
	// camera.turn_left_xy(theta);
	// camera.position.x += theta;
	// camera.move_left_xy(theta);
	// camera.look_at(center);


	// camera.turn_left_xy(theta / 100.0f);
	camera_xy = Quaternion::Axis_Angle(Vector3f(0.0f, 0.0f, -1.0f), theta/1000.0f) * camera_xy;// * camera.orientation;

	// Vector3f offset(-100.0f, 0.0f, 0.0);
	// camera.position = center + (camera_xy * offset);
	// camera.look_at(center);
}

void Player::on_mouse_motion(const float &x, const float &y) {
	// Quaternion originalOrientation = camera.orientation;
	// Vector3f originalPosition = camera.position;

	adjust_pitch(y);
	turn_left_xy(x);

	// camera_look_orientation = camera.orientation;
	// camera_look_position = camera.position - center;

	// camera.orientation = originalOrientation;
	// camera.position = originalPosition;
	Vector3f offset(-100.0f, 0.0f, 0.0);
	// camera_look_position = center + (camera_xy * camera_xz * offset);
	// camera.look_at(center);
}

void Player::apply_camera() {
	// camera.orientation = camera_look_orientation;
	// camera.position    = center + camera_look_position;
	Vector3f offset(-100.0f, 0.0f, 0.0);
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
}

void Player::create_body() {
	Sound &sr = get_Sound();

	body = Sphere(
		  center
		, m_radius
	);

	sr.set_listener_position(camera.position);
	sr.set_listener_forward_and_up(camera.get_forward(), camera.get_up());
	sr.set_listener_velocity(m_velocity);
}
