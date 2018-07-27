
// std
#include <algorithm>
#include <utility>

// glm
#include <glm.hpp>
#include <gtc/constants.hpp>

// project
#include "shape.hpp"
#include <iostream>

RayIntersection AABB::intersect(const Ray &ray) {
	RayIntersection intersect;
	glm::vec3 rel_origin = ray.origin - m_center;

	// start magic
	// x
	float rdx_inv = 1 / ray.direction.x;
	float tx1 = (-m_halfsize.x - rel_origin.x) * rdx_inv;
	float tx2 = (m_halfsize.x - rel_origin.x) * rdx_inv;

	float tmin = std::min(tx1, tx2);
	float tmax = std::max(tx1, tx2);

	// y
	float rdy_inv = 1 / ray.direction.y;
	float ty1 = (-m_halfsize.y - rel_origin.y) * rdy_inv;
	float ty2 = (m_halfsize.y - rel_origin.y) * rdy_inv;

	tmin = std::max(tmin, std::min(ty1, ty2));
	tmax = std::min(tmax, std::max(ty1, ty2));

	// z
	float rdz_inv = 1 / ray.direction.z;
	float tz1 = (-m_halfsize.z - rel_origin.z) * rdz_inv;
	float tz2 = (m_halfsize.z - rel_origin.z) * rdz_inv;

	tmin = std::max(tmin, std::min(tz1, tz2));
	tmax = std::min(tmax, std::max(tz1, tz2));

	if (tmax < tmin) return intersect;
	// end magic

	intersect.m_distance = tmin < 0 ? tmax : tmin;
	intersect.m_position = ray.origin + intersect.m_distance * ray.direction;
	intersect.m_valid = tmax >= 0;
	glm::vec3 work_out_a_name_for_it_later = glm::abs((intersect.m_position - m_center) / m_halfsize);
	float max_v = std::max(work_out_a_name_for_it_later[0], std::max(work_out_a_name_for_it_later[1], work_out_a_name_for_it_later[2]));
	intersect.m_normal = glm::normalize(glm::mix(intersect.m_position - m_center, glm::vec3(0), glm::lessThan(work_out_a_name_for_it_later, glm::vec3(max_v))));
	intersect.m_uv_coord = (glm::abs(intersect.m_normal.x) > 0) ?
		glm::vec2(intersect.m_position.y, intersect.m_position.z) :
		glm::vec2(intersect.m_position.x, intersect.m_position.y + intersect.m_position.z);
	intersect.m_shape = this;

	return intersect;
}


RayIntersection Sphere::intersect(const Ray &ray) {
	RayIntersection intersect;

	glm::vec3 L = ray.origin - m_center;
	float a = glm::dot(ray.direction, ray.direction);
	float b = glm::dot(ray.direction, L) * 2.0f;
	float c = glm::dot(L, L) - (m_radius * m_radius);

	if (glm::isnan(a) || glm::isnan(b) || glm::isnan(c)) {
		intersect.m_valid = false;
		return intersect;
	}

	bool invalid = false;
	float t;
	float t0 = 0;
	float t1 = 0;
	// Solve the quadratic
	float discrim = b * b - 4 * a * c;
	if (discrim < 0) invalid = true;
	else if (discrim == 0) {
		t0 = t1 = -0.5f * b / a;
	}
	else {
		float q = (b > 0) ? -0.5f * (b + glm::sqrt(discrim)) : -0.5f * (b - glm::sqrt(discrim));
		t0 = q / a;
		t1 = c / q;
	}

	if (t0 > t1) std::swap(t0, t1);

	if (t0 < 0) {
		t0 = t1;
		if (t0 < 0) invalid = true;
	}
	
	t = t0;

	if (invalid) {
		intersect.m_valid = false;
	}
	else {
		intersect.m_valid = true;
		intersect.m_distance = t;
		intersect.m_position = glm::vec3(ray.origin + ray.direction*t);
		intersect.m_normal = glm::normalize(intersect.m_position - m_center);
	}
	return intersect;
}

RayIntersection Plane::intersect(const Ray & ray) {
	RayIntersection intersect;

	float t = -1;
	float denominator = glm::dot(m_normal, ray.direction);
	if (denominator == 0) {
		intersect.m_valid = false;
		return intersect;
	}
	if (glm::abs(denominator) > 1e-6) {
		float numerator = glm::dot((m_position - ray.origin), m_normal);
		t = numerator / denominator;
	}

	intersect.m_valid = t >= 0;
	intersect.m_distance = t;
	intersect.m_normal = glm::normalize(m_normal);
	if (denominator > 0) intersect.m_normal *= -1;
	intersect.m_position = glm::vec3(ray.origin + t*ray.direction);

	return intersect;
}

RayIntersection Disk::intersect(const Ray & ray)
{
	RayIntersection intersect;

	float t = -1;
	float denominator = glm::dot(m_normal, ray.direction);
	if (denominator == 0) {
		intersect.m_valid = false;
		return intersect;
	}
	if (glm::abs(denominator) > 1e-6) {
		float numerator = glm::dot((m_position - ray.origin), m_normal);
		t = numerator / denominator;
		glm::vec3 pos = ray.origin + ray.direction*t;
		if (glm::distance(pos, m_position) >= m_radius) {
			intersect.m_valid = false;
			return intersect;
		}
	}

	intersect.m_valid = t >= 0;
	intersect.m_distance = t;
	intersect.m_normal = glm::normalize(m_normal);
	if (denominator > 0) intersect.m_normal *= -1;
	intersect.m_position = glm::vec3(ray.origin + t*ray.direction);

	return intersect;
}

RayIntersection Triangle::intersect(const Ray & ray)
{
	RayIntersection intersect;

	glm::vec3 D = ray.direction;
	glm::vec3 O = ray.origin;
	glm::vec3 Q = m_v1;
	glm::vec3 N = glm::cross(m_v2 - m_v1, m_v3 - m_v1);

	glm::vec3 t1 = (Q - O);
	float t2 = glm::dot(D, N);
	float t3 = glm::dot(t1, N);

	if (glm::abs(t2) < 1e-6) {
		intersect.m_valid = false;
		return intersect;
	}
	float tPrime = t3 / t2;
	if (tPrime < 0) {
		intersect.m_valid = false;
		return intersect;
	}

	glm::vec3 pt(O + tPrime*D);
	glm::vec3 F0 = glm::cross((m_v2 - m_v1), (pt - m_v1));
	glm::vec3 F1 = glm::cross((m_v3 - m_v2), (pt - m_v2));
	glm::vec3 F2 = glm::cross((m_v1 - m_v3), (pt - m_v3));

	float NF0 = glm::dot(N, F0);
	float NF1 = glm::dot(N, F1);
	float NF2 = glm::dot(N, F2);

	if (NF0 < 0 || NF1 < 0 || NF2 < 0) {
		intersect.m_valid = false;
		return intersect;
	}

	intersect.m_valid = true;
	intersect.m_distance = tPrime;
	intersect.m_normal = glm::normalize(N);
	if (t2 > 0) intersect.m_normal *= -1;
	intersect.m_position = pt;

	return intersect;
}