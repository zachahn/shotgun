#ifndef PLAYER_H
#define PLAYER_H

#include <zenilib.h>

using namespace Zeni;

class Player {
public:
	Vector3f forwardVector;
	Vector3f leftVector;
	Vector3f backVector;
	Vector3f rightVector;
	Vector3f upVector;
	Vector3f downVector;

	float wasdSpeed;
	float updownSpeed;

	float last_shot_fired;
	float shooting_interval;

	float shield_start;
	float shield_show_length;

	int damage;
	int health;

	Model shield;
	Model model;
	Point3f center;
	Quaternion direction;

	Quaternion camera_xy;
	Quaternion camera_xz;

	Quaternion camera_look_orientation;
	Vector3f camera_look_position;

	Player(const Zeni::Camera &camera_,
		   const Zeni::Vector3f &end_point_b_,
		   const float radius_);

	bool on_key(const SDL_KeyboardEvent &event);

	Vector3f get_next_velocity();

	void render();

	void apply_camera();

	// Level 1
	const Zeni::Camera & get_camera() const;

	// Level 2
	void set_position(const Zeni::Point3f &position);

	void adjust_pitch(const float &phi);
	void turn_left_xy(const float &theta);

	void on_mouse_motion(const float &x, const float &y);

	// Level 3
	const Zeni::Collision::Sphere & get_body() const;
	bool is_on_ground() const;

	const Zeni::Vector3f & get_velocity() const;
	void set_velocity(const Zeni::Vector3f &velocity_);

	void set_on_ground(const bool &is_on_ground_);
	void jump();

	void step(const float &time_step);

	bool fire(float time);

	void hit(int damage);

private:
	void create_body();

	// Level 1/2
	Zeni::Camera camera;

	// Level 2
	Zeni::Vector3f m_end_point_b;
	float m_radius;

	// Level 3
	Zeni::Collision::Sphere body; // collision
	Zeni::Vector3f m_velocity;
	bool m_is_on_ground;

	// Level 4
	// Controls are external to Player
};

#endif
