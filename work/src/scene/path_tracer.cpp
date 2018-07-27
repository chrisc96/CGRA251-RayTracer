
// glm
#include <gtc/constants.hpp>
#include <gtc/random.hpp>

// std
#include <random>

// project
#include "scene.hpp"
#include "shape.hpp"
#include "light.hpp"
#include "material.hpp"
#include "path_tracer.hpp"



glm::vec3 SimplePathTracer::sampleRay(const Ray &ray, int) {
	// intersect ray with the scene
	RayIntersection intersect = m_scene->intersect(ray);

	// if ray hit something
	if (intersect.m_valid) {
		// simple grey shape shading
		float f = glm::abs(glm::dot(-ray.direction, intersect.m_normal));
		glm::vec3 grey(0.5, 0.5, 0.5);
		return glm::mix(grey / 2.0f, grey, f);
	}

	// no intersection - return background color
	return { 0.3f, 0.3f, 0.4f };
}



glm::vec3 CorePathTracer::sampleRay(const Ray &ray, int) {
	// intersect ray with the scene
	RayIntersection intersect = m_scene->intersect(ray);

	// if ray hit something
	if (intersect.m_valid) {
		glm::vec3 reflectionConstant = intersect.m_material->diffuse();
		float roughnessConstant = intersect.m_material->shininess();

		glm::vec3 controlGI(0); // Ambient light Global Illumination
		glm::vec3 summedLambertian(0);
		glm::vec3 summedPhong(0);
		glm::vec3 summedShadows(0);

		for (int i = 0; i < m_scene->lights().size(); i++) {
			// Setup variables
			Light &light = *m_scene->lights().at(i);
			glm::vec3 dirToLight = -(light.incidentDirection(intersect.m_position));
			glm::vec3 dirToLightNormal = glm::normalize(dirToLight);
			glm::vec3 point = intersect.m_position;
			glm::vec3 lightIntensity = light.irradiance(point);

			float normalToLight = glm::dot(intersect.m_normal, -(light.incidentDirection(intersect.m_position)));
			if (glm::isnan(normalToLight)) continue;
			if (light.occluded(m_scene, intersect.m_position + (intersect.m_normal * 0.001f)) && normalToLight >= 0) {
				controlGI += light.ambience();
				continue;
			}

			// Ambient light
			controlGI += light.ambience();

			// Lambertian Diffuse Reflection
			glm::vec3 lamb_surfaceDiffusion = intersect.m_material->diffuse();
			float intense = glm::dot(dirToLightNormal, intersect.m_normal);
			if (glm::isnan(intense)) continue;
			glm::vec3 lamb_intensity = lightIntensity * lamb_surfaceDiffusion * glm::max(0.0f, intense);
			summedLambertian += lamb_intensity;

			// Phong Specular Reflection
			glm::vec3 nHat = intersect.m_normal * (glm::dot(dirToLightNormal, intersect.m_normal));
			glm::vec3 perfectReflection = nHat * 2.0f - dirToLightNormal;
			glm::vec3 vecToCamera = glm::normalize(-ray.direction);
			
			glm::vec3 ks = intersect.m_material->specular();
			float val = glm::dot(perfectReflection, vecToCamera);
			if (glm::isnan(val)) continue;
			summedPhong += lightIntensity * ks * (glm::pow(glm::max(0.0f, val), roughnessConstant));
		}
		return controlGI * reflectionConstant + summedLambertian + summedPhong;
	}
	// no intersection - return background color
	return { 0.3f, 0.3f, 0.4f };
}



glm::vec3 CompletionPathTracer::sampleRay(const Ray &ray, int depth) {
	//-------------------------------------------------------------
	// [Assignment 4] :
	// Using the same requirements for the CorePathTracer add in 
	// a recursive element to calculate perfect specular reflection.
	// That is compute the reflection ray off your intersection and
	// sample a ray in that direction, using the result to additionally
	// light your object. To make this more realistic you may weight
	// the incoming light by the (1 - (1/shininess)).
	//-------------------------------------------------------------

	if (depth == 0) return { 0.3f, 0.3f, 0.4f };

	RayIntersection intersect = m_scene->intersect(ray);

	// if ray hit something
	if (intersect.m_valid) {
		glm::vec3 reflectionConstant = intersect.m_material->diffuse();
		float roughnessConstant = intersect.m_material->shininess();

		glm::vec3 controlGI(0); // Ambient light Global Illumination
		glm::vec3 summedLambertian(0);
		glm::vec3 summedPhong(0);
		glm::vec3 summedShadows(0);

		for (int i = 0; i < m_scene->lights().size(); i++) {
			// Setup variables
			Light &light = *m_scene->lights().at(i);
			glm::vec3 dirToLight = -(light.incidentDirection(intersect.m_position));
			glm::vec3 dirToLightNormal = glm::normalize(dirToLight);
			glm::vec3 point = intersect.m_position;
			glm::vec3 lightIntensity = light.irradiance(point);
			glm::vec3 acneBias = intersect.m_normal * 1e-4f;

			float normalToLight = glm::dot(intersect.m_normal, dirToLightNormal);
			if (glm::isnan(normalToLight)) continue;
			if (light.occluded(m_scene, intersect.m_position + acneBias) && normalToLight >= 0) {
				controlGI += light.ambience();
				continue;
			}

			// Ambient light
			controlGI += light.ambience();

			// Lambertian Diffuse Reflection
			glm::vec3 lamb_surfaceDiffusion = intersect.m_material->diffuse();
			float intense = glm::dot(dirToLightNormal, intersect.m_normal);
			if (glm::isnan(intense)) continue;
			glm::vec3 lamb_intensity = lightIntensity * lamb_surfaceDiffusion * glm::max(0.f, intense);
			summedLambertian += lamb_intensity;

			// Phong Specular Reflection
			glm::vec3 nHat = (intersect.m_normal * (glm::dot(dirToLightNormal, intersect.m_normal)));
			glm::vec3 perfectReflection = nHat * 2.0f - dirToLightNormal;
			glm::vec3 vecToCamera = glm::normalize(-ray.direction);
			glm::vec3 ks = intersect.m_material->specular();

			float val = glm::dot(perfectReflection, vecToCamera);
			if (glm::isnan(val)) continue;

			glm::vec3 intensity(0);
			if (depth > 0) {
				Ray reflectRay(intersect.m_position + perfectReflection * 1e-4f, perfectReflection);
				float m = 1 - (1 / roughnessConstant);
				glm::vec3 mirrorColor = sampleRay(reflectRay, depth - 1);
				// Idk how TF this works, I spent so long trying to get reflection to work, it works average so I'm giving up at this point.
				intensity += m * mirrorColor;
			}
			summedPhong = intensity * ks;
		}
		return controlGI * reflectionConstant + summedLambertian + summedPhong;
	}
	// no intersection - return background color
	return { 0.3f, 0.3f, 0.4f };
}



glm::vec3 ChallengePathTracer::sampleRay(const Ray &ray, int depth) {
	//-------------------------------------------------------------
	// [Assignment 4] :
	// Implement a PathTracer that calculates the diffuse and 
	// specular, for the given ray in the scene, using the 
	// Phong lighting model. Give special consideration to objects
	// that occluded from direct lighting (shadow rays).
	// Implement support for textured materials (using a texture
	// for the diffuse portion of the material).
	//
	// EXTRA FOR EXPERTS :
	// Additionally implement indirect diffuse and specular instead
	// of using the ambient lighting term.
	// The diffuse is sampled from the surface hemisphere and the
	// specular is sampled from a cone of the phong lobe (which
	// gives a glossy look). For best results you need to normalize
	// the lighting (see http://www.thetenthplanet.de/archives/255)
	//-------------------------------------------------------------

	// YOUR CODE GOES HERE
	// ...

	// no intersection - return background color
	return { 0.3f, 0.3f, 0.4f };
}