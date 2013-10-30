#ifndef CRATE_H
#define CRATE_H

#include <zenilib.h>

using namespace Zeni;

class Crate {
public:
	Crate(const Point3f &corner_      = Point3f(0.0f, 0.0f, 0.0f),
	      const Vector3f &scale_      = Vector3f(30.0f, 30.0f, 30.0f),
	      const Quaternion &rotation_ = Quaternion::Axis_Angle(Vector3f(0.0f, 0.0f, 1.0f), 0.0f));
	Crate(const Crate &rhs);
	Crate & operator=(const Crate &rhs);
	~Crate();

	void render();

	void collide();

	void look_at(Vector3f);

	const Collision::Sphere & get_body() const {return m_body;}

private:
	void create_body();

	// Level 1
	static Model* powerful_model;
	static Model* range_model;
	static Model* regular_model;
	static Model* super_model;
	static unsigned long m_instance_count;

	Sound_Source * m_source;

	Model* model;

	// Level 2
	Point3f m_corner;
	Vector3f m_scale;
	Quaternion m_rotation;

	// Level 3
	Collision::Sphere m_body; // not motion so much as collision

	// Level 4
	// A stationary Crate has no controls
};


#endif
