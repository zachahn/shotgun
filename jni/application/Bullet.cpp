#include <zenilib.h>

#include "Bullet.h"

using namespace Zeni;
using namespace Zeni::Collision;

#include <iostream>
using namespace std;


Bullet::Bullet(const Point3f &corner_, const Vector3f &scale_)
	: center(corner_)
	, scale(scale_)
{
	if (! instance_count) {
		model = new Model("models/Bullet.3ds");
	}
	++instance_count;

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

const Sphere & Bullet::get_body() const {
	return body;
}

void Bullet::create_body() {
	body = Sphere(center, scale.x);
}

Model * Bullet::model = 0;
unsigned long Bullet::instance_count = 0lu;
