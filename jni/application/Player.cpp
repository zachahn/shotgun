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
{
	camera.fov_rad = Zeni::Global::pi / 3.0f;

	wasdSpeed = 200.0f;
	updownSpeed = 100.0f;

	create_body();
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

	cout << "W " << wasd.x << " " << wasd.y << " " << wasd.z << endl;
	cout << "U " << updown.x << " " << updown.y << " " << updown.z << endl;
	cout << "N " << next_velocity.x << " " << next_velocity.y << " " << next_velocity.z << endl;

	return next_velocity;
}

// Level 2
void Player::set_position(const Point3f &position) {
	camera.position = position;
	create_body();
}

void Player::adjust_pitch(const float &phi) {
	const Quaternion backup = camera.orientation;
	const Vector3f backup_up = camera.get_up();

	camera.adjust_pitch(phi);

	if (camera.get_up().k < 0.0f && backup_up.k >= 0.0f)
		camera.orientation = backup;
}

void Player::turn_left_xy(const float &theta) {
	camera.turn_left_xy(theta);
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
	camera.position += time_step * m_velocity;
	create_body();
}

void Player::create_body() {
	Sound &sr = get_Sound();

	m_body = Capsule(
		  camera.position
		, camera.position + m_end_point_b
		, m_radius
	);

	sr.set_listener_position(camera.position);
	sr.set_listener_forward_and_up(camera.get_forward(), camera.get_up());
	sr.set_listener_velocity(m_velocity);
}
