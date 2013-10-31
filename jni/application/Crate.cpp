#include <zenilib.h>

#include "Crate.h"

using namespace Zeni;
using namespace Zeni::Collision;

#include <iostream>
using namespace std;

Crate::Crate(const Point3f &center_, const int &type_, const int &size_)
	: m_source(new Sound_Source(get_Sounds()["collide"]))
	, center(center_)
	, type(type_)
	, health(size_)
{
	init();
	++m_instance_count;
	create_body();
}

Crate::Crate(const Crate &rhs)
	: m_source(new Sound_Source(get_Sounds()["collide"]))
	, center(rhs.center)
	, radius(rhs.radius)
	, type(rhs.type)
	, health(rhs.health)
{
	init();
	++m_instance_count;

	create_body();
}

void Crate::init() {
	calculate_radius();

	if (! m_instance_count) {
		regular_model  = new Model("models/planet-regular.3ds");
		powerful_model = new Model("models/planet-powerful.3ds");
		range_model    = new Model("models/planet-range.3ds");
		super_model    = new Model("models/planet-super.3ds");
	}

	if      (POWERFUL == type) {
		model = powerful_model;
		range = 150.0f;
		shooting_interval = 0.25f;
		damage = 200;
	}
	else if (RANGE == type) {
		model = range_model;
		range = 250.0f;
		shooting_interval = 0.25f;
		damage = 100;
	}
	else if (SUPER == type) {
		model = super_model;
		range = 220.0f;
		shooting_interval = 0.25f;
		damage = 200;
	}
	else {
		model = regular_model;
		range = 150.0f;
		type = REGULAR;
		shooting_interval = 0.2f;
		damage = 100;
	}
}

Crate & Crate::operator=(const Crate &rhs) {
	center = rhs.center;
	radius = rhs.radius;
	type   = rhs.type;

	init();
	create_body();

	return *this;
}

Crate::~Crate() {
	delete m_source;

	if (!--m_instance_count) {
		delete powerful_model;
		delete range_model;
		delete regular_model;
		delete super_model;

		powerful_model = 0lu;
		range_model    = 0lu;
		regular_model  = 0lu;
		super_model    = 0lu;
	}
}

void Crate::calculate_radius() {
	health = health < 0 ? 0 : health;

	radius = powf(float(health), 0.3333f) + 10.0f;

	scale = Vector3f(radius, radius, radius);

	cout << radius<< endl;
}

void Crate::render() {
	model->set_translate(center);
	model->set_scale(scale);
	model->render();
}

void Crate::hit(int damage) {
	health -= damage;

	calculate_radius();

	create_body();

	collide();
}

void Crate::collide() {
	if (! m_source->is_playing())
		m_source->play();
}

bool Crate::fire(float time) {
	if (health <= 0) {
		return false;
	}

	if (time >= last_shot_fired + shooting_interval) {
		last_shot_fired = time;
		return true;
	}

	return false;
}

const Collision::Sphere & Crate::get_body() const {
	return body;
}

// void Crate::look_at(Vector3f pos) {
// 	Vector3f cxy;
// 	Vector3f pxy;
// 	Quaternion dq;

// 	cxy.x = center.x;
// 	cxy.y = center.y;
// 	pxy.x = pos.x;
// 	pxy.y = pos.y;

// 	dq = dq.Vector3f_to_Vector3f(pxy, cxy);
// 	m_rotation = dq;
// }

void Crate::create_body() {
	body = Sphere(center, radius);

	m_source->set_position(center);
}

Model* Crate::powerful_model = 0;
Model* Crate::range_model    = 0;
Model* Crate::regular_model  = 0;
Model* Crate::super_model    = 0;

unsigned long Crate::m_instance_count = 0lu;

const int Crate::HUGE  = 100000;
const int Crate::BIG   =  10000;
const int Crate::SMALL =   5000;
