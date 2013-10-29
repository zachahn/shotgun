#include <zenilib.h>

#include "Crate.h"

using namespace Zeni;
using namespace Zeni::Collision;

#include <iostream>
using namespace std;


Crate::Crate(const Point3f &corner_, const Vector3f &scale_, const Quaternion &rotation_)
	: m_source(new Sound_Source(get_Sounds()["collide"]))
	, m_corner(corner_)
	, m_scale(scale_)
	, m_rotation(rotation_)
{
	if (! m_instance_count) {
		box_model = new Model("models/crate.3ds");
		shield_model = new Model("models/eshield.3ds");
	}
	++m_instance_count;

	create_body();
}

Crate::Crate(const Crate &rhs)
	: m_source(new Sound_Source(get_Sounds()["collide"]))
	, m_corner(rhs.m_corner)
	, m_scale(rhs.m_scale)
	, m_rotation(rhs.m_rotation)
{
	++m_instance_count;

	create_body();
}

Crate & Crate::operator=(const Crate &rhs) {
	m_corner = rhs.m_corner;
	m_scale = rhs.m_scale;
	m_rotation = rhs.m_rotation;

	create_body();

	return *this;
}

Crate::~Crate() {
	delete m_source;

	if (!--m_instance_count) {
		delete box_model;
		delete shield_model;
		box_model = 0lu;
		shield_model = 0lu;
	}
}

void Crate::render() {
	const std::pair<Vector3f, float> rotation = m_rotation.get_rotation();

	box_model->set_translate(m_corner);
	box_model->set_scale(m_scale);
	box_model->set_rotate(rotation.second, rotation.first);

	box_model->render();

	shield_model->set_translate(m_corner);
	shield_model->set_scale(m_scale);
	shield_model->render();
}

void Crate::collide() {
	if (! m_source->is_playing())
		m_source->play();
}

void Crate::look_at(Vector3f pos) {
	Vector3f cxy;
	Vector3f pxy;
	Quaternion dq;

	cxy.x = m_corner.x;
	cxy.y = m_corner.y;
	pxy.x = pos.x;
	pxy.y = pos.y;

	dq = dq.Vector3f_to_Vector3f(pxy, cxy);
	m_rotation = dq;
}

void Crate::create_body() {
	// m_body = Parallelepiped(
	// 	  m_corner
	// 	, m_rotation * m_scale.get_i()
	// 	, m_rotation * m_scale.get_j()
	// 	, m_rotation * m_scale.get_k()
	// );

	// m_source->set_position(m_corner + m_rotation * m_scale / 2.0f);

	m_body = Sphere(m_corner, m_scale.x);

	m_source->set_position(m_corner + m_rotation * m_scale / 2.0f);
}

Model * Crate::box_model = 0;
Model * Crate::shield_model = 0;
unsigned long Crate::m_instance_count = 0lu;
