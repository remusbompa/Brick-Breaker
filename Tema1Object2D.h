#pragma once

#include <string>

#include <include/glm.h>
#include <Core/GPU/Mesh.h>
#include "Tema1.h"

/*
	Fisierul defineste functiile de construire a obiectelor si de detectare a coliziunilor.
*/
namespace Tema1Object2D
{
	
	Mesh* CreateRect(std::string name, glm::vec3 leftBottomCorner, float lengthX, float lengthY, glm::vec3 color);
	Mesh* CreateCircle(std::string name, glm::vec3 center, float radius, glm::vec3 color);
	Mesh*** CreateBricks(glm::vec3 leftBottomCorner, float lengthX, float lengthY, float interX, float interY, int nrCx, int  nrCy, glm::vec3 color);
	
	int ColisionRect(std::vector<VertexFormat> vertices, glm::vec3 posBilaOld, glm::vec3 posBilaNew, float radius);
	int ColisionBricks(Mesh **** meshes, int nrCx, int nrCy, glm::vec3 posBilaOld, glm::vec3 posBilaNew, float radius, bool** destroyed);
	int ColisionPl(std::vector<VertexFormat> vertices, glm::vec3 posBilaOld, glm::vec3 posBilaNew, float translatePl, float* theta, float radius, float radiusPl);
	bool ColisionPlPowerUp(std::vector<VertexFormat> verticesPl, std::vector<VertexFormat> verticesPo);
}

