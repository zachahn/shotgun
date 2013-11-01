#include <zenilib.h>

#include "Bullet.h"

using namespace Zeni;
using namespace Zeni::Collision;

#include <iostream>
using namespace std;


Bullet::Bullet(const Vector3f &direction_, const Point3f &corner_, const int &damage_, const Vector3f &scale_)
	: center(corner_)
	, scale(scale_)
	, direction(direction_)
	, damage(damage_)
{
	if (! instance_count) {
		model = new Model("models/bullet-log.3ds");
	}
	++instance_count;

	direction.normalized();

	speed = 20.0f;

	distance_travelled = 0.0f;

	create_body();
}

Bullet::Bullet(const Bullet &rhs)
	: center(rhs.center)
	, scale(rhs.scale)
{
	++instance_count;

	create_body();
}

Bullet & Bullet::operator=(const Bullet &rhs) {
	center = rhs.center;
	scale = rhs.scale;

	create_body();

	return *this;
}

Bullet::~Bullet() {
	if (!--instance_count) {
		delete model;
		model = 0lu;
	}
}

void Bullet::render() {
	model->set_translate(center);
	model->set_scale(scale);
	model->render();
}

void Bullet::collide() {
}

void Bullet::fly(float time_step) {
	distance_travelled += speed * time_step;
	center += direction * speed * time_step;

	create_body();
}

const Sphere & Bullet::get_body() const {
	return body;
}

void Bullet::create_body() {
	body = Sphere(center, scale.x);
}

Model * Bullet::model = 0;
unsigned long Bullet::instance_count = 0lu;
