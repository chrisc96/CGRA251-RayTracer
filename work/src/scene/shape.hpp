
#pragma once

// glm
#include <glm.hpp>

// project
#include "ray.hpp"
#include "scene.hpp"


class Shape {
public:
	virtual RayIntersection intersect(const Ray &ray) = 0;
};


class AABB : public Shape {
private:
	glm::vec3 m_center;
	glm::vec3 m_halfsize;

public:
	AABB(const glm::vec3 &c, float hs) : m_center(c), m_halfsize(hs) { }
	AABB(const glm::vec3 &c, const glm::vec3 &hs) : m_center(c), m_halfsize(hs) { }
	virtual RayIntersection intersect(const Ray &ray) override;
};


class Sphere : public Shape {
private:
	glm::vec3 m_center;
	float m_radius;

public:
	Sphere(const glm::vec3 &c, float radius) : m_center(c), m_radius(radius) { }
	virtual RayIntersection intersect(const Ray &ray) override;
};

class Plane : public Shape {
private:
	glm::vec3 m_position; // Any position on the plane
	glm::vec3 m_normal; // A vector that represents the direction the plane faces

public:
	Plane(const glm::vec3 &pos, glm::vec3 &norm) : m_position(pos), m_normal(norm) { }
	virtual RayIntersection intersect(const Ray &ray) override;
};

class Disk : public Shape {
private:
	glm::vec3 m_position; // Any position on the disc
	glm::vec3 m_normal; // A vector that represents the direction the disk faces
	float m_radius;
public:
	Disk(const glm::vec3 &pos, glm::vec3 &norm, float r) : m_position(pos), m_normal(norm), m_radius(r) { }
	virtual RayIntersection intersect(const Ray &ray) override;
};

class Triangle : public Shape {
private:
	glm::vec3 m_v1;
	glm::vec3 m_v2;
	glm::vec3 m_v3;
public:
	Triangle(const glm::vec3 &v1, const glm::vec3 &v2, const glm::vec3 &v3) : m_v1(v1), m_v2(v2), m_v3(v3) { }
	virtual RayIntersection intersect(const Ray &ray) override;
};

//-------------------------------------------------------------
// [Assignment 4] :
// Implement the following additional Shapes :
// - Plane
// - Disk
// - Triangle
// Follow the pattern shown by AABB and Sphere for implementing
// a class that subclasses Shape making sure that you implement
// the intersect method for each new Shape.
//-------------------------------------------------------------

// YOUR CODE GOES HERE
// ...

