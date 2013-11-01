#ifndef BULLET_H
#define BULLET_H

#include <Zeni/Collision.h>
#include <Zeni/Model.h>
#include <Zeni/Quaternion.h>
#include <Zeni/Sound.h>
#include <Zeni/Vector3f.h>

using namespace Zeni;

class Bullet {
public:
	const static int ENEMY  = 0;
	const static int ATTACK = 1;
	const static int EAT    = 2;

	Bullet(
		const Zeni::Vector3f &direction_,
		const Zeni::Point3f &corner_,
		const int &type_,
		const int &damage_,
		const float &range_ = 150.0f,
		const Zeni::Vector3f &scale_ = Zeni::Vector3f(1.0f, 1.0f, 1.0f)
	);
	Bullet(const Bullet &rhs);
	Bullet & operator=(const Bullet &rhs);
	~Bullet();

	void render();

	void collide();

	void fly(float);

	const Zeni::Collision::Sphere & get_body() const;

	// Level 2 (or whatever)
	Zeni::Point3f center;
	Zeni::Vector3f direction;
	Quaternion rotation;
	float speed;

	int damage;

	float range;

	float distance_travelled;

private:
	void create_body();

	// Level 1
	static Model* enemy;
	static Model* attack;
	static Model* eat;
	Model* model;

	static unsigned long instance_count;

	// Level 2
	Zeni::Vector3f scale;

	// Level 3
	Zeni::Collision::Sphere body; // not motion so much as collision

	// Level 4
	// A stationary Bullet has no controls
};


#endif
