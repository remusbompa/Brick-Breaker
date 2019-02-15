#include "Tema1Object2D.h"
#include <Core/Engine.h>
#include "Tema1.h"

/*
	Functia creaza un obiect 2D dreptungiular de dimensiune lengthX X lengthY, culoare color, avand coltul din
	stanga jos leftBottomCorner
*/
Mesh* Tema1Object2D::CreateRect(std::string name, glm::vec3 leftBottomCorner, float lengthX, float lengthY, glm::vec3 color)
{
	glm::vec3 corner = leftBottomCorner;

	std::vector<VertexFormat> vertices =
	{
		VertexFormat(corner, color),
		VertexFormat(corner + glm::vec3(lengthX, 0, 0), color),
		VertexFormat(corner + glm::vec3(lengthX, lengthY, 0), color),
		VertexFormat(corner + glm::vec3(0, lengthY, 0), color)
	};

	Mesh* rectangle = new Mesh(name);
	std::vector<unsigned short> indices = { 0, 1, 2, 3 };

	
	// draw 2 triangles. Add the remaining 2 indices
	indices.push_back(0);
	indices.push_back(2);

	rectangle->InitFromData(vertices, indices);
	return rectangle;
}

/*
	Functia creaza un cerc de raza radius, centrul in center si culoare color 
*/
Mesh* Tema1Object2D::CreateCircle(std::string name, glm::vec3 center, float radius, glm::vec3 color)
{

	int nrFragments = 100;
	std::vector<VertexFormat> vertices ={ VertexFormat(center, color) };
	for (int i = 0; i < nrFragments; i++) {
		float theta = RADIANS((float)(360 * i) / nrFragments);
		float x = radius * cos(theta), y = radius * sin(theta);
		vertices.push_back(VertexFormat(glm::vec3(center.x + x, center.y + y,center.z), color));
	}

	Mesh* circle = new Mesh(name);
	std::vector<unsigned short> indices = {};
	for (int i = 1; i < nrFragments; i++) {
		indices.push_back(0);
		indices.push_back(i);
		indices.push_back(i+1);
	}

	indices.push_back(0);
	indices.push_back(nrFragments);
	indices.push_back(1);

	circle->InitFromData(vertices, indices);
	return circle;
}

/*
	Functia creaza o matrice de pointeri Mesh*, care contine mesele caramizilor, fiecare avand lungimile 
	lengthX si lengthY, culoarea color, un spatiu intre ele de interX pe Ox si interY pe Oy si fiind in numar
	de nrCx pe Ox si nrCy pe Oy. Coltul din stanga jos a caramizii din stanga jos este leftBottomCorner.
*/
Mesh*** Tema1Object2D::CreateBricks(glm::vec3 leftBottomCorner, float lengthX, float lengthY, float interX, float interY, int nrCx, int  nrCy, glm::vec3 color) {
	glm::vec3 corner = leftBottomCorner;

	std::vector<VertexFormat> vertices = {};
	std::vector<unsigned short> indices = {};
	
	Mesh*** bricks = new Mesh**[nrCx];
	for (int i = 0; i < nrCx; i++) {
		bricks[i] = new Mesh*[nrCy];
		for (int j = 0; j < nrCy; j++) {
			glm::vec3 difCorner = {(lengthX+interX)*i,(lengthY+interY)*j,0};
			char name[30];
			sprintf(name,"brick%d%d",i,j);
			Mesh* brick = Tema1Object2D::CreateRect(std::string(name), corner + difCorner, lengthX, lengthY, color);
			bricks[i][j] = brick;
			
		}
	}
	return bricks;
}

/*
	Functia trateaza coliziunea dintre bila si un dreptunghi. Pozitia actuala a bilei este posBilaOld iar cea viitoare
	(in cazul in care nu se modifica unghiul theta) este posBilaNew, raza e radius. Dreptunghiul este dat ca parametru
	prin varfurile sale: vertices. Mai intai se verifica daca centrul bilei va fi in exteriorul dreptunghiului, in coliziune,
	dar la stanga, la dreapta, deasupra sau sub el. Apoi se verifica daca va fi in interior, existand 2 posibilitati: este
	deja in interior (si Old si New sunt in dreptunghi), caz in care se considera coliziunea cu cel mai apropiat perete, 
	sau Old este in exterior, caz in care se considera coliziunea cu latura ce intersecteaza segmentul Old - New. In final
	sunt tratate cazurile cand centrul bilei se va afla in exteriorul dreptunghiului, in coliziune, in colturile din:
	dreaptta sus, stanga sus, dreapta jos, stanga jos.
	Functia intoarce:	0 - ptr nicio coliziune
						1 - coliziune cu un perete orizontal
						2 - coliziune cu un perete vertical
*/
int Tema1Object2D::ColisionRect(std::vector<VertexFormat> vertices, glm::vec3 posBilaOld, glm::vec3 posBilaNew, float radius)
{
	float l = vertices[0].position.x, r = vertices[1].position.x;
	float b = vertices[0].position.y, t = vertices[2].position.y;

	float x1 = posBilaOld.x, y1 = posBilaOld.y;
	float x2 = posBilaNew.x, y2 = posBilaNew.y;

	if ((x2 >= l - radius && x2 <= l) && (y2 >= b && y2 <= t)) {
		//std::cout << "SUNT MARGINEA DE STANGA !\n";
		return 2;
	}

	if ((x2 >= r && x2 <= r + radius) && (y2 >= b && y2 <= t)) {
		//std::cout << "SUNT MARGINEA DE DREAPTA !\n";
		return 2;
	}

	if ((y2 >= b - radius && y2 <= b) && (x2 >= l && x2 <= r)) {
		//std::cout << "SUNT MARGINEA DE JOS !\n";
		return 1;
	}

	if ((y2 >= t && y2 <= t + radius) && (x2 >= l && x2 <= r)) {
		//std::cout << "SUNT MARGINEA DE SUS !\n";
		return 1;
	}

	//daca bila se va afla in interiorul dreptunghiului
	float x,y;
	if (x2 > l && x2 < r && y2 > b && y2 < t) {

		//daca si Old si New sunt in interiorul dreptunghiului
		if (x1 > l && x1 < r && y1 > b && y1 < t) {

			//std::cout << "SUNT INAUNTRU !\n";
			float dxl = x1 - l, dxr = r - x1;
			float dyt = t - y1, dyb = y1 - b;

			float min = min(min(min(dxl, dxr), dyt), dyb);
			if (min == dyb || min == dyt) return 1;
			else return 2;
		}

		//daca New e in interior si Old e in exterior
		else {
			//std::cout << "SUNT LA INTERSECTIE !\n";
			x = x1 + (b - y1) * (x2 - x1) / (y2 - y1);
			if ((x <= r && x >= l) && (x1 <= x && x <= x2)) {
				return 1; //ciocnire de orizontala
			}

			x = x1 + (t - y1) * (x2 - x1) / (y2 - y1);
			if ((x <= r && x >= l) && (x1 <= x && x <= x2)) {
				return 1; //ciocnire de orizontala
			}

			y = y1 + (l - x1) * (y2 - y1) / (x2 - x1);
			if ((y <= t && y >= b) && (y1 <= y && y <= y2)) {
				return 2; //ciocnire de verticala
			}

			y = y1 + (r - x1) * (y2 - y1) / (x2 - x1);
			if ((y <= t && y >= b) && (y1 <= y && y <= y2)) {
				return 2; //ciocnire de verticala
			}
		}
	}

	// daca New se afla in coloziune, in exteriorul dreptunghiului 

	// in clotul stanga sus
	if ((x2 - l) * (x2 - l) + (y2 - t) * (y2 - t) <= radius * radius) {
		//std::cout << "SUNT COLT ST SUS !\n";
		float dx = abs(posBilaNew.x - l);
		float dy = abs(posBilaNew.y - t);
		if (dx < dy)
			return 1;
		else
			return 2;
		
	}

	//in coltul dreapta sus
	if ((x2 - r) * (x2 - r) + (y2 - t) * (y2 - t) <= radius * radius) {
		//std::cout << "SUNT COLT DR SUS !\n";
		float dx = abs(posBilaNew.x - r);
		float dy = abs(posBilaNew.y - t);
		if (dx < dy)
			return 1;
		else
			return 2;
	}

	//in coltul stanga jos
	if ((x2 - l) * (x2 - l) + (y2 - b) * (y2 - b) <= radius * radius) {
		//std::cout << "SUNT COLT ST JOS !\n";
		float dx = abs(posBilaNew.x - l);
		float dy = abs(posBilaNew.y - b);
		if (dx < dy)
			return 1;
		else
			return 2;
	}

	//in coltul dreapta jos
	if ((x2 - r) * (x2 - r) + (y2 - b) * (y2 - b) <= radius * radius) {
		//std::cout << "SUNT COLT DR JOS !\n";
		float dx = abs(posBilaNew.x - r);
		float dy = abs(posBilaNew.y - b);
		if (dx < dy)
			return 1;
		else
			return 2;
	}

	return 0;
}

/*
	Functia detecteaza o coliziune a bilei cu o caramida. Se itereaza prin toate caramizile, iar daca caramida nu e distrusa,
	se detecteaza coliziunea cu bila, cu ajutorul functiei ColisionRect. In cazul mai multor coliziuni simulane, noul theta
	va fi dat de ultima coliziune (r = res).
*/
int Tema1Object2D::ColisionBricks(Mesh **** meshes, int nrCx, int nrCy, glm::vec3 posBilaOld, glm::vec3 posBilaNew, float radius, bool** destroyed) {
	Mesh *** bricks = *meshes;
	int r = 0;
	for (int i = 0; i < nrCx; i++) {
		for (int j = 0; j < nrCy; j++) {
			if (!destroyed[i][j]) {
				std::vector<VertexFormat> verticesB = bricks[i][j]->vertices;
				int res = ColisionRect(verticesB, posBilaOld, posBilaNew, radius);
				if (res) {
					destroyed[i][j] = true;
					Tema1::nrDestroyed++;
					r = res;
				}
			}
		}
	}
	return r;
}

/*
	Functia detecteaza coliziunea cu platforma si modifica unghiul theta in functie de pozitia bilei pe platforma. Parametrii: 
	vertices, translatePl sunt folositi ptr a afla noile coord ale varfurilor platformei: verticesNew, care impreuna cu posBilaOld,
	posBilaNew si radius sunt folosite pentru a apela functia ColisionRect, care detecteaza coliziunea cu platforma. Daca se
	obtine o coliziune (res != 0), se va modifica theta, acesta luand valori de la: acos(Tema1::plExtr) = RADIANS(10), pentru cazul
	in care bila loveste platforma pe margine, pana la 90, daca bila atinge centrul platformei. radiusPl e raza platformei.
	Functia intoarce:	- 0 -daca nu are loc o coliziune cu platforma
						- 1 -daca are loc o coliziune cu platforma
*/
int Tema1Object2D::ColisionPl(std::vector<VertexFormat> vertices, glm::vec3 posBilaOld, glm::vec3 posBilaNew, float translatePl, float* theta, float radius, float radiusPl) {
	glm::vec3 pos0 = vertices[0].position + glm::vec3(translatePl, 0, 0);
	glm::vec3 pos1 = vertices[1].position + glm::vec3(translatePl, 0, 0);
	glm::vec3 pos2 = vertices[2].position + glm::vec3(translatePl, 0, 0);
	glm::vec3 pos3 = vertices[3].position + glm::vec3(translatePl, 0, 0);

	std::vector<VertexFormat> verticesNew = {
		VertexFormat(pos0, vertices[0].color),
		VertexFormat(pos1, vertices[1].color),
		VertexFormat(pos2, vertices[2].color),
		VertexFormat(pos3, vertices[3].color)
	};

	int res = ColisionRect(verticesNew, posBilaOld, posBilaNew, radius);
	if (res) {
		float dx = posBilaOld.x - (pos0.x + radiusPl);
		if (dx > radiusPl) {
			dx = radiusPl;
		}

		if (dx < -radiusPl) {
			dx = -radiusPl;
		}

		*theta = DEGREES(acos(Tema1::plExtr * dx / radiusPl));
		return 1;
	}
	return res;
}

/*
	Functia detecteaza coliziunea platformei cu un powerUp, si primeste ca parametri: varfurile platformei, verticesPl
	si varfurile powerUp -ului, verticesPo. Prima data, se verfifica daca un varf al powerUp ului e continut in
	platforma, caz in care avem o coliziune. In cazul in care niciun varf nu e continut in platforma, verific daca 
	o latura a powerUp-ului intersecteaza una a platformei.
	Functia intoarcea true daca s-a detectat o coliziune si false in caz contrar.
*/
bool Tema1Object2D::ColisionPlPowerUp(std::vector<VertexFormat> verticesPl, std::vector<VertexFormat> verticesPo) {
	float xp1 = verticesPl[0].position.x, xp2 = verticesPl[1].position.x;
	float yp1 = verticesPl[0].position.y, yp2 = verticesPl[2].position.y;

	float x0 = verticesPo[0].position.x, y0 = verticesPo[0].position.y;
	float x1 = verticesPo[1].position.x, y1 = verticesPo[1].position.y;
	float x2 = verticesPo[2].position.x, y2 = verticesPo[2].position.y;
	float x3 = verticesPo[3].position.x, y3 = verticesPo[3].position.y;

	float xmin, xmax, ymin, ymax;
	//verific daca un vf a powerUp-ului e continut in platforma
	if (x0 >= xp1 && x0 <= xp2 && y0 >= yp1 && y0 <= yp2) return true;
	if (x1 >= xp1 && x1 <= xp2 && y1 >= yp1 && y1 <= yp2) return true;
	if (x2 >= xp1 && x2 <= xp2 && y2 >= yp1 && y2 <= yp2) return true;
	if (x3 >= xp1 && x3 <= xp2 && y3 >= yp1 && y3 <= yp2) return true;

	//verific daca o latura a powerUp ului intersecteaza una a platformei
	float x, y;
	//latura 2->3
	xmin = min(x2, x3); xmax = max(x2, x3);
	ymin = min(y2, y3), ymax = max(y2, y3);
		//cu lat 0->1
	x = x3 + (yp1 - y3) * (x2 - x3) / (y2 - y3);
	y = yp1;
	if (x >= xp1 && x <= xp2 && x>= xmin && x <= xmax && y >= ymin && y <= ymax) return true;
		//cu lat 1->2
	y = y3 + (xp2 - x3) * (y2 - y3) / (x2 - x3);
	x = xp2;
	if (y >= yp1 && y <= yp2 && y >= ymin && y <= ymax && x >= xmin && x <= xmax) return true;
		//cu lat 2->3
	x = x3 + (yp2 - y3) * (x2 - x3) / (y2 - y3);
	y = yp2;
	if (x >= xp1 && x <= xp2 && x >= xmin && x <= xmax && y >= ymin && y <= ymax) return true;
		//cu lat 3->0
	y = y3 + (xp1 - x3) * (y2 - y3) / (x2 - x3);
	x = xp1;
	if (y >= yp1 && y <= yp2 && y >= ymin && y <= ymax && x >= xmin && x <= xmax) return true;

	//latura 0->1
	xmin = min(x0, x1); xmax = max(x0, x1);
	ymin = min(y0, y1), ymax = max(y0, y1);
		//cu lat 0->1
	x = x0 + (yp1 - y0) * (x1 - x0) / (y1 - y0);
	y = yp1;
	if (x >= xp1 && x <= xp2 && x >= xmin && x <= xmax && y >= ymin && y <= ymax) return true;
	//cu lat 1->2
	y = y0 + (xp2 - x0) * (y1 - y0) / (x1 - x0);
	x = xp2;
	if (y >= yp1 && y <= yp2 && y >= ymin && y <= ymax && x >= xmin && x <= xmax) return true;
	//cu lat 2->3
	x = x0 + (yp2 - y0) * (x1 - x0) / (y1 - y0);
	y = yp2;
	if (x >= xp1 && x <= xp2 && x >= xmin && x <= xmax && y >= ymin && y <= ymax) return true;
	//cu lat 3->0
	y = y0 + (xp1 - x0) * (y1 - y0) / (x1 - x0);
	x = xp1;
	if (y >= yp1 && y <= yp2 && y >= ymin && y <= ymax && x >= xmin && x <= xmax) return true;

	//latura 1->2
	xmin = min(x1, x2); xmax = max(x1, x2);
	ymin = min(y1, y2), ymax = max(y1, y2);
		//cu lat 0->1
	x = x1 + (yp1 - y1) * (x2 - x1) / (y2 - y1);
	y = yp1;
	if (x >= xp1 && x <= xp2 && x >= xmin && x <= xmax && y >= ymin && y <= ymax) return true;
	//cu lat 1->2
	y = y1 + (xp2 - x1) * (y2 - y1) / (x2 - x1);
	x = xp2;
	if (y >= yp1 && y <= yp2 && y >= ymin && y <= ymax && x >= xmin && x <= xmax) return true;
	//cu lat 2->3
	x = x1 + (yp2 - y1) * (x2 - x1) / (y2 - y1);
	y = yp2;
	if (x >= xp1 && x <= xp2 && x >= xmin && x <= xmax && y >= ymin && y <= ymax) return true;
	//cu lat 3->0
	y = y1 + (xp1 - x1) * (y2 - y1) / (x2 - x1);
	x = xp1;
	if (y >= yp1 && y <= yp2 && y >= ymin && y <= ymax && x >= xmin && x <= xmax) return true;

	//latura 3->0
	xmin = min(x0, x3); xmax = max(x0, x3);
	ymin = min(y0, y3), ymax = max(y0, y3);
	//cu lat 0->1
	x = x3 + (yp1 - y3) * (x0 - x3) / (y0 - y3);
	y = yp1;
	if (x >= xp1 && x <= xp2 && x >= xmin && x <= xmax && y >= ymin && y <= ymax) return true;
	//cu lat 1->2
	y = y3 + (xp2 - x3) * (y0 - y3) / (x0 - x3);
	x = xp2;
	if (y >= yp1 && y <= yp2 && y >= ymin && y <= ymax && x >= xmin && x <= xmax) return true;
	//cu lat 2->3
	x = x3 + (yp2 - y3) * (x0 - x3) / (y0 - y3);
	y = yp2;
	if (x >= xp1 && x <= xp2 && x >= xmin && x <= xmax && y >= ymin && y <= ymax) return true;
	//cu lat 3->0
	y = y3 + (xp1 - x3) * (y0 - y3) / (x0 - x3);
	x = xp1;
	if (y >= yp1 && y <= yp2 && y >= ymin && y <= ymax && x >= xmin && x <= xmax) return true;

	return false;
}