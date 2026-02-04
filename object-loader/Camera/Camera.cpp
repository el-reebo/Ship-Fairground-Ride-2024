#include "Camera.h"
#include "..\glm\gtc\matrix_transform.hpp"
#include "..\glm\gtc\matrix_inverse.hpp"
#include "..\glm\gtc\type_ptr.hpp"

glm::vec3 rideMin(-3.0f, 0.0f, -3.9f);
glm::vec3 rideMax(3.0f, 5.0f, 3.9f);

Camera::Camera() :
	viewDirection(-1.0f, 0.0f, 0.0f),
	strafeDirection(glm::cross(viewDirection, upDirection)),
	upDirection(0.0f, 1.0f, 0.0f), 
	position(10.0f, 0.5f, 0.0f),
	mouseSens(0.5f)
{

}

glm::mat4 Camera::getViewMatrix() const
{
	return glm::lookAt(position, position + viewDirection,  upDirection);
}

void Camera::updateMousePos(const glm::vec2& newMousePos)
{
	//vector that travels from old mouse position on screen to new mouse position on screen
	glm::vec2 mouseVector = oldMousePos - newMousePos;

	if (glm::length(mouseVector) > 15.0f) //Prevent disoriating jumps when cursor leaves window 
	{ 
		oldMousePos = newMousePos;
	}
	else {
		//Rotates view Direction depending on displacement in x axis (horizontal axis of the actual screen)
		viewDirection = glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(mouseVector.x * mouseSens), upDirection) *
			glm::vec4(viewDirection, 1.0f));

		strafeDirection = glm::cross(viewDirection, upDirection); //Used as axis for up and down rotation
		glm::vec3 verticalViewDirection = glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(mouseVector.y * mouseSens), strafeDirection) *
			glm::vec4(viewDirection, 1.0f));
		
		//Check ensuring you can't look up or down close to beyond the y axis
		if (glm::abs(glm::dot(verticalViewDirection, glm::vec3(0.0f, 1.0f, 0.0f))) < 0.99f)
		{
			viewDirection = verticalViewDirection;
		}

		oldMousePos = newMousePos;
	}
}

void Camera::updateMouseDelta(float dx, float dy)
{
	// Horizontal rotation
	viewDirection = glm::vec3(
		glm::rotate(glm::mat4(1.0f),
			glm::radians(dx * mouseSens),
			upDirection) *
		glm::vec4(viewDirection, 1.0f));

	// Vertical rotation
	strafeDirection = glm::cross(viewDirection, upDirection); //strafe direction vector

	glm::vec3 verticalView = glm::vec3(
		glm::rotate(glm::mat4(1.0f),
			glm::radians(dy * mouseSens),
			strafeDirection) *
		glm::vec4(viewDirection, 1.0f));

	// Prevent camera flip
	if (glm::abs(glm::dot(verticalView, glm::vec3(0.0f, 1.0f, 0.0f))) < 0.99f)
	{
		viewDirection = verticalView;
	}
}

void Camera::updateCameraPos(const glm::vec3& newPosition)
{
	position = newPosition;
}

void Camera::setCamType(CamType newType) {
	type = newType;
}

void Camera::setUpDirection(const glm::vec3& newUp)
{
	upDirection = newUp;
}

Camera::CamType Camera::getCamType() const
{
	return type;
}

bool Camera::isInsideRide(const glm::vec3& position, const glm::vec3& rideMin, const glm::vec3& rideMax)
{
	return (position.x >= rideMin.x && position.x <= rideMax.x &&
		position.y >= rideMin.y && position.y <= rideMax.y &&
		position.z >= rideMin.z && position.z <= rideMax.z);
}

void Camera::moveForward()
{
	glm::vec3 newPosition;
	glm::vec3 movementIncrement = viewDirection * movementSpeed;

	if (type == Camera::CamType::GROUNDCAM) {
		//removes y component of view direction allowing movement in the same y axis
		newPosition = position + (normalize(glm::vec3(viewDirection.x, 0, viewDirection.z)) * movementSpeed);
	} else {
		newPosition = position + movementIncrement;
	}

	//ride collision
	if (!isInsideRide(newPosition, rideMin, rideMax))
	{
		position = newPosition;
	}

	//ground collision
	if (position.y < 0.5)
	{
		position -= (position.y - 0.5);//Add on however much they've gone below y = 0
	}

	
}
void Camera::moveBack()
{
	glm::vec3 newPosition;
	glm::vec3 movementIncrement = viewDirection * movementSpeed;

	if (type == Camera::CamType::GROUNDCAM) {
		//removes y component of view direction allowing movement in the same y axis
		newPosition = position - (normalize(glm::vec3(viewDirection.x, 0, viewDirection.z)) * movementSpeed);
	}
	else {
		newPosition = position - movementIncrement;
	}

	if (!isInsideRide(newPosition, rideMin, rideMax))
	{
		position = newPosition;
	}

	if (position.y < 0.5)
	{
		position -= (position.y - 0.5);
	}
}
void Camera::moveLeft()
{
	glm::vec3 newPosition;
	newPosition = position - (normalize(strafeDirection) * movementSpeed);

	//ride collision
	if (!isInsideRide(newPosition, rideMin, rideMax))
	{
		position = newPosition;
	}
}
void Camera::moveRight()
{
	glm::vec3 newPosition;
	newPosition = position + (normalize(strafeDirection) * movementSpeed);

	if (!isInsideRide(newPosition, rideMin, rideMax))
	{
		position = newPosition;
	}
}
void Camera::moveUp()
{
	glm::vec3 newPosition;
	newPosition = position + (upDirection * movementSpeed);

	if (!isInsideRide(newPosition, rideMin, rideMax))
	{
		position = newPosition;
	}
}
void Camera::moveDown()
{
	glm::vec3 newPosition;
	newPosition = position - (upDirection * movementSpeed);

	if (!isInsideRide(newPosition, rideMin, rideMax))
	{
		position = newPosition;
	}

	if (position.y < 0.5)
	{
		position -= (position.y - 0.5);
	}
}
