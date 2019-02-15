#pragma once

#include <include/glm.h>

/*
	Fisierul defineste functii de contruire a matricilor de modelare 2D in coord omogene:
	Translatie, Rotatie si Scalare, precum si 2 functii de conversie.
*/
namespace Transform2D
{
	// Translate matrix
	inline glm::mat3 Translate(float translateX, float translateY)
	{
		return glm::transpose(
			glm::mat3(1, 0, translateX,
				0, 1, translateY,
				0, 0, 1)
		);
	}

	// Scale matrix
	inline glm::mat3 Scale(float scaleX, float scaleY)
	{
		return glm::transpose(
			glm::mat3(scaleX, 0, 0,
				0, scaleY, 0,
				0, 0, 1)
		);

	}

	// Rotate matrix
	inline glm::mat3 Rotate(float radians)
	{
		float c = cos(radians), s = sin(radians);
		return glm::transpose(
			glm::mat3(c, -s, 0,
				s, c, 0,
				0, 0, 1)
		);

	}

	//functia transforma o matrice 2D in coord omogene, intr-una 3D in coord omogene.
	inline glm::mat4 To4D(glm::mat3 model3)
	{
		glm::mat4 model4 = {
			model3[0][0], model3[0][1], 0, model3[0][2],
			model3[1][0], model3[1][1], 0, model3[1][2],
			0, 0, 1, 0,
			model3[2][0], model3[2][1], 0, model3[2][2]
			
		};
		return model4;
	}

	//functia transforma un vector 3D in coord omogene, intr-unul 3D in coord carteziene. 
	inline glm::vec3 To3D(glm::vec4 model4)
	{
		glm::vec3 model3 = {
			model4[0],model4[1],model4[2]
		};
		return model3;
	}
}
