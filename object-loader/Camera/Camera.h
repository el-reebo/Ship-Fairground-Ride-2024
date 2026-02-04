#ifndef _CAMERA_H
#define _CAMERA_H

#include "..\gl\glew.h"

#include "..\glm\glm.hpp"



class Camera
{
public:
	enum class CamType {
			FREECAM,
			GROUNDCAM,
			RIDECAM
		};
private:
	glm::vec3 position;
	glm::vec3 viewDirection;
	glm::vec3 strafeDirection;
	glm::vec3 upDirection;
	CamType type;
	//___mouse variables___
	glm::vec2 oldMousePos;
	float mouseSens;

	float movementSpeed = 0.05f;


public:
	Camera();

	glm::mat4 getViewMatrix() const;
	void updateMousePos(const glm::vec2& newMousePos); //record position of mouse on screen
	void updateMouseDelta(float dx, float dy);
	void updateCameraPos(const glm::vec3& newPosition);
	void setCamType(CamType type);
	void setUpDirection(const glm::vec3& newUp);
	CamType getCamType() const;
	bool isInsideRide(const glm::vec3& position, const glm::vec3& rideMin, const glm::vec3& rideMax);
	void moveForward();
	void moveBack();
	void moveLeft();
	void moveRight();
	void moveUp();
	void moveDown();
};

#endif _CAMERA_H