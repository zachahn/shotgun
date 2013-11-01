#include <zenilib.h>

#include "Bullet.h"

using namespace Zeni;
using namespace Zeni::Collision;

#include <iostream>
using namespace std;


Bullet::Bullet(const Vector3f &direction_, const Point3f &corner_, const int &type_, const int &damage_, const float &range_, const Vector3f &scale_)
	: center(corner_)
	, scale(scale_)
	, direction(direction_)
	, damage(damage_)
	, range(range_)
{
	if (! instance_count) {
		enemy  = new Model("models/bullet-enemy.3ds");
		attack = new Model("models/bullet-attack.3ds");
		eat    = new Model("models/bullet-eat.3ds");
	}
	++instance_count;

	direction.normalized();

	rotation = Quaternion::Axis_Angle(direction, 0.0f);

	speed = 20.0f;

	distance_travelled = 0.0f;

	if (type_ == ENEMY) {
		model = enemy;
	}
	else if (type_ == ATTACK) {
		model = attack;
	}
	else if (type_ == EAT) {
		model = eat;
	}
	else {
		model = eat;
	}

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
	model->set_rotate(rotation);
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

Model* Bullet::enemy  = 0;
Model* Bullet::attack = 0;
Model* Bullet::eat    = 0;

unsigned long Bullet::instance_count = 0lu;
