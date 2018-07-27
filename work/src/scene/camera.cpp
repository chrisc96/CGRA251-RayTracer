
#include <iostream>

// glm
#include <gtc/matrix_transform.hpp>

// project
#include "camera.hpp"
#include "opengl.hpp"

using namespace std;


void Camera::setPositionOrientation(const glm::vec3 &pos, float yaw, float pitch) {
	m_position = pos;
	m_yaw = yaw;
	m_pitch = pitch;

	// update rotation matrix (based on yaw and pitch)
	m_rotation = glm::rotate(glm::mat4(1), m_yaw, glm::vec3(0, 1, 0)) * glm::rotate(glm::mat4(1), m_pitch, glm::vec3(1, 0, 0));
}

Ray Camera::generateRay(const glm::vec2 &pixel) {
	//-------------------------------------------------------------
	// [Assignment 4] :
	// Generate a ray in the scene using the camera position,
	// rotation field of view on the y axis (fovy) and the image
	// size. The pixel is given in image coordinates [0, imagesize]
	// This COULD be done by first creating the ray in ViewSpace
	// then transforming it by the position and rotation to get
	// it into worldspace.
	//-------------------------------------------------------------
	
	float m_fovx = (m_image_size.x / m_image_size.y) * m_fovy;
	float view_height = glm::tan(m_fovy / 2);
	float view_width  = glm::tan(m_fovx / 2);

	// Calculate pixel coordinates in ndc space
	glm::vec3 ndcPixelCoords(((pixel.x * 2.0f / m_image_size.x) - 1) * view_width, ((pixel.y * 2.0f / m_image_size.y) - 1) * view_height, -1);
	glm::vec3 ndcNorm(glm::normalize(ndcPixelCoords));
	glm::vec4 viewDirVec4(ndcNorm, 0.0f);
	glm::vec4 worldDirVec4 = m_rotation * viewDirVec4;
	glm::vec3 worldDir(worldDirVec4);
	
	Ray ray(m_position, worldDir);
	return ray;
}