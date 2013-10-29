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
	Bullet(
		const Zeni::Point3f &corner_ = Zeni::Point3f(0.0f, 0.0f, 0.0f),
		const Zeni::Vector3f &scale_ = Zeni::Vector3f(5.0f, 5.0f, 5.0f)
	);
	Bullet(const Bullet &rhs);
	Bullet & operator=(const Bullet &rhs);
	~Bullet();

	void render();

	void collide();

	const Zeni::Collision::Sphere & get_body() const;

private:
	void create_body();

	// Level 1
	static Zeni::Model * model;
	static unsigned long instance_count;

	// Level 2
	Zeni::Point3f center;
	Zeni::Vector3f scale;

	// Level 3
	Zeni::Collision::Sphere body; // not motion so much as collision

	// Level 4
	// A stationary Bullet has no controls
};


#endif
