#pragma once

class Shader;

#include "..\glm\glm.hpp"
#include "..\glm\gtc\matrix_transform.hpp"

class Square
{
private:
	unsigned int m_vaoID;		// id for Vertex Array Object
	unsigned int m_vboID[2];	// ids for Vertex Buffer Objects

	Shader* m_Shader;  ///shader object 

	float m_SideSize;

public:
	Square();					//default constructor
	void Init(float colour[3]);
	void SetSideSize(float size);
	void Render(glm::mat4& ModelViewMatrix, glm::mat4& ProjectionMatrix);
};