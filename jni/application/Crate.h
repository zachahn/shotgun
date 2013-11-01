#ifndef CRATE_H
#define CRATE_H

#include <zenilib.h>

using namespace Zeni;

class Crate {
public:
	const static int REGULAR  = 0;
	const static int POWERFUL = 1;
	const static int RANGE    = 2;
	const static int SUPER    = 3;

	const static int HUGE;
	const static int BIG;
	const static int SMALL;

	float last_shot_fired;
	float shooting_interval;

	Point3f center;
	float range;

	int health;

	int damage;

	float hit_start;
	float hit_show_length;

	Crate(const Point3f &center_, const int &type_ = REGULAR, const int &size_ = SMALL);

	Crate(const Crate &rhs);
	Crate & operator=(const Crate &rhs);
	~Crate();

	void init();

	void render();

	void collide();

	void hit(int damage);

	bool fire(float time);

	void calculate_radius();

	const Collision::Sphere & get_body() const;

private:
	void create_body();

	// Level 1
	static Model* regular_model;
	static Model* range_model;
	static Model* powerful_model;
	static Model* super_model;
	static Model* dead_model;
	static Model* regular_hurt_model;
	static Model* range_hurt_model;
	static Model* powerful_hurt_model;
	static Model* super_hurt_model;
	static unsigned long m_instance_count;

	Sound_Source * m_source;

	Model* model;

	// Level 2
	float radius;
	Vector3f scale;
	int type;

	// Level 3
	Collision::Sphere body; // not motion so much as collision

	// Level 4
	// A stationary Crate has no controls
};


#endif
