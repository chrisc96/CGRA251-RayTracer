
// glm
#include <gtc/constants.hpp>

// project
#include "light.hpp"

#include <iostream>

bool DirectionalLight::occluded(Scene *scene, const glm::vec3 &point) const {
	//-------------------------------------------------------------
	// [Assignment 4] :
	// Determine whether the given point is being occluded from
	// this directional light by an object in the scene.
	// Remember that directional lights are "infinitely" far away
	// so any object in the way would cause an occlusion.
	//-------------------------------------------------------------
	Ray r(point, -incidentDirection(point)); // Ray from object to light
	RayIntersection intersect = scene->intersect(r);
	if (intersect.m_valid) {
		return true;
	}
	else if (!intersect.m_valid || intersect.m_distance < 0) {
		return false;
	}
}

glm::vec3 DirectionalLight::incidentDirection(const glm::vec3 &) const {
	return m_direction;
}


glm::vec3 DirectionalLight::irradiance(const glm::vec3 &) const {
	return m_irradiance;
}


bool PointLight::occluded(Scene *scene, const glm::vec3 &point) const {
	//std::cout << "HERE" << std::endl;
	//-------------------------------------------------------------
	// [Assignment 4] :
	// Determine whether the given point is being occluded from
	// this directional light by an object in the scene.
	// Remember that point lights are somewhere in the scene and
	// an occulsion has to occur somewhere between the light and 
	// the given point.
	//-------------------------------------------------------------
	Ray r(point, -incidentDirection(point));
	RayIntersection intersect = scene->intersect(r);
	if (intersect.m_valid) {
		float dot = glm::dot(-incidentDirection(point), m_position - intersect.m_position);
		return dot >= 0;
	}
	else if (!intersect.m_valid || intersect.m_distance < 0) {
		return false;
	}
}


glm::vec3 PointLight::incidentDirection(const glm::vec3 &point) const {
	//-------------------------------------------------------------
	// [Assignment 4] :
	// Return the direction of the incoming light (light to point)
	//-------------------------------------------------------------
	return glm::normalize(point - m_position);
}


glm::vec3 PointLight::irradiance(const glm::vec3 &point) const {
	//-------------------------------------------------------------
	// [Assignment 4] :
	// Return the total irradiance on the given point.
	// This requires you to convert the flux of the light into
	// irradiance by dividing it by the surface of the sphere
	// it illuminates. Remember that the surface area increases
	// as the sphere gets bigger, ie. the point is further away.
	//-------------------------------------------------------------
	float distToLight = glm::abs(glm::distance(m_position, point));
	glm::vec3 intensity = m_flux / (4.0f * glm::pi<float>() * (distToLight * distToLight));
	return intensity;
}