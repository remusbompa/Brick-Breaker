#include "Tema1.h"

#include <vector>
#include<set>
#include <iostream>
#include <time.h>
#include <Core/Engine.h>
#include "Transform2D.h"
#include "Tema1Object2D.h"

using namespace std;

// numarul de caramizi distruse
int Tema1::nrDestroyed = 0;
//cosinusul unghiului theta daca bila loveste platforma pe margine
float Tema1::plExtr = cos(RADIANS(10));

Tema1::Tema1()
{
}

Tema1::~Tema1()
{
}

void Tema1::Init()
{
	glm::ivec2 resolution = window->GetResolution();
	auto camera = GetSceneCamera();
	camera->SetOrthographic(0, (float)resolution.x, 0, (float)resolution.y, 0.01f, 400);
	camera->SetPosition(glm::vec3(0, 0, 50));
	camera->SetRotation(glm::vec3(0, 0, 0));
	camera->Update();
	GetCameraInput()->SetActive(false);

	resx = (float)resolution.x;
	resy = (float)resolution.y;

	plX = rplx * resx;
	radiusPl = plX / 2;

	float plY = rply * resy;

	float pozbileY = rpozbile * resy;
	float pozbilaY = rpozbila * resy;

	float pereteV = rpex * resx;
	float difBileX = rdifb * resx;

	float diamBila = rbx * resx;

	radius = diamBila / 2;

	glm::vec3 corner = { 0,0,0 };

	platformaPoz = { resx / 2 - plX / 2, pozbileY - plY, 0 };
	bilePoz[0] = glm::vec3(pereteV, pozbileY, 0);
	bilePoz[1] = glm::vec3(pereteV + difBileX, pozbileY, 0);
	bilePoz[2] = glm::vec3(pereteV + 2 * difBileX, pozbileY, 0);
	bilaPoz = glm::vec3(resx / 2, pozbilaY, 0);

	//creare platforma
	Mesh* platforma = Tema1Object2D::CreateRect("platforma", platformaPoz, plX, plY, glm::vec3(0, 1, 0));
	AddMeshToList(platforma);

	//creare cele 3 vieti
	Mesh* bila0 = Tema1Object2D::CreateCircle("bila0", bilePoz[0], diamBila /2 , glm::vec3(1, 1, 1));
	AddMeshToList(bila0);
	Mesh* bila1 = Tema1Object2D::CreateCircle("bila1", bilePoz[1], diamBila / 2, glm::vec3(1, 1, 1));
	AddMeshToList(bila1);
	Mesh* bila2 = Tema1Object2D::CreateCircle("bila2", bilePoz[2], diamBila / 2, glm::vec3(1, 1, 1));
	AddMeshToList(bila2);

	//creare bila
	Mesh* bila = Tema1Object2D::CreateCircle("bila", bilaPoz, diamBila / 2, glm::vec3(1, 1, 1));
	AddMeshToList(bila);

	//creare cei 3 pereti
	float pozPerV = rpozperVy * resy;
	float pozPerO = rpozperOy * resy;

	Mesh* pereteSt = Tema1Object2D::CreateRect("pereteSt", glm::vec3(0.0f,pozPerV,0.0f), pereteV, resy - pozPerV, glm::vec3(1, 0, 0));
	AddMeshToList(pereteSt);

	Mesh* pereteDr = Tema1Object2D::CreateRect("pereteDr", glm::vec3(resx - pereteV, pozPerV, 0.0f), pereteV, resx - pozPerV, glm::vec3(1, 0, 0));
	AddMeshToList(pereteDr);

	Mesh* pereteSus = Tema1Object2D::CreateRect("pereteSus", glm::vec3(0.0f, resy - pozPerO, 0.0f), resx * 1.0f, pozPerO, glm::vec3(1, 0, 0));
	AddMeshToList(pereteSus);

	//creare primul bonus -> peretele de Jos
	Mesh* pereteJos = Tema1Object2D::CreateRect("pereteJos", glm::vec3(0.0f, 0.0f, 0.0f), resx * 1.0f, platformaPoz.y, glm::vec3(1, 0, 0));
	AddMeshToList(pereteJos);

	//creare matrice bricks de caramizi
	float w = rcx * resx;
	float h = rcy * resy;
	x = w / (nrCx + rix * (nrCx - 1));
	y = h / (nrCy + rjy * (nrCy - 1));
	float interX = rix * x;
	float interY = rjy * y;
	glm::vec3 bricksPoz = {resx / 2 - w /2, (resy - 2 * pozPerV) / 2 - h /2 + 2 * pozPerV, 0};

	bricks = Tema1Object2D::CreateBricks(bricksPoz, x, y, interX ,interY, nrCx, nrCy,  glm::vec3(1, 0, 0));
	
	// matrice cu scalarea caramizilor
	scaleBricks = new float*[nrCx];
	for (int i = 0; i < nrCx; i++) {
		scaleBricks[i] = new float[nrCy];
		for (int j = 0;j < nrCy; j++)
			scaleBricks[i][j] = 1.0f;
	}

	//matrice cu caramizile distruse
	destroyed = new bool*[nrCx];
	for (int i = 0; i < nrCx; i++) {
		destroyed[i] = new bool[nrCy];
		for (int j = 0;j < nrCy; j++)
			destroyed[i][j] = false;
	}

	//creare matrice powerups
	nr = (int)(r_powerUps * nrCx * nrCy);
	powerups = new PowerUp**[nrCx];
	for (int i = 0; i < nrCx; i++) {
		powerups[i] = new PowerUp*[nrCy];
		for (int j = 0; j < nrCy;j++) {
			powerups[i][j] = NULL;
		}
	}
	CreatePowerUps();
}

/*
	Creaza matricea de powerUps -uri, alegand aleator nr numere din intervalul [0,nrC-1], reprezentand
	caramizile ce vor contine powerUps - uri. Daca caramida (i,j) nu contine powerUps, powerups[i][j] = NULL.
	Culorile powerUps-urilor sunt alese tot aleator.
*/
void Tema1::CreatePowerUps() 
{
	srand((unsigned int)time(NULL));
	std::set<int> nrAlese = {};

	int nrC = nrCx * nrCy;
	for (int it = 0; it < nr; it++) {
		int ran;
		do {
			ran = rand() % nrC;
		} while (nrAlese.find(ran) != nrAlese.end());

		nrAlese.insert(ran);
		int j = ran / nrCx;
		int i = ran % nrCx;
		char name[30];
		sprintf(name, "powerup%d", ran);
		powerups[i][j] = new PowerUp();
		Mesh* mesh = bricks[i][j];

		std::vector<VertexFormat> vertices = mesh->vertices;
		glm::vec3 center = { (vertices[0].position.x + vertices[1].position.x) / 2,
							(vertices[0].position.y + vertices[2].position.y) / 2,
							vertices[0].position.z };
		powerups[i][j]->centru = center;


		float dim = r_dimPowerUps * min(x, y);
		glm::vec3 leftBottomCorner = center - glm::vec3{ dim / 2, dim / 2, 0 };

		int r = rand() % 1000, g = rand() % 1000, b = rand() % 1000;
		powerups[i][j]->mesh = Tema1Object2D::CreateRect(name, leftBottomCorner, dim, dim, glm::vec3(r / 1000.0f, g / 1000.0f, b / 1000.0f));
	}
}

void Tema1::FrameStart()
{
	// clears the color buffer (using the previously set color) and depth buffer
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::ivec2 resolution = window->GetResolution();
	// sets the screen area where to draw
	glViewport(0, 0, resolution.x, resolution.y);
}

void Tema1::Update(float deltaTimeSeconds)
{
	//desenare platforma
	modelMatrixPl = Transform2D::Translate(translatePl, 0.0f);
	RenderMesh2D(meshes["platforma"], shaders["VertexColor"], modelMatrixPl);

	// desenare vieti in functie de tura (vieti pierdute)
	if (tura == 0) {
		RenderMesh2D(meshes["bila0"], shaders["VertexColor"], glm::mat3(1));
		RenderMesh2D(meshes["bila1"], shaders["VertexColor"], glm::mat3(1));;
		RenderMesh2D(meshes["bila2"], shaders["VertexColor"], glm::mat3(1));
	}
	else if (tura == 1) {
		RenderMesh2D(meshes["bila0"], shaders["VertexColor"], glm::mat3(1));
		RenderMesh2D(meshes["bila1"], shaders["VertexColor"], glm::mat3(1));;
	}
	else if (tura == 2) {
		RenderMesh2D(meshes["bila0"], shaders["VertexColor"], glm::mat3(1));
	}

	//desenare bila
	glm::mat3 modelMatrixB = glm::mat3(1);
	modelMatrixB *= Transform2D::Translate(translateBx, translateBy);
	//daca bonusul SuperBall este activat
	if (marireBila) {
		if (elapsedTimeBila > 0) {
			modelMatrixB *= Transform2D::Translate(bilaPoz.x, bilaPoz.y);
			modelMatrixB *= Transform2D::Scale(scaleFactor, scaleFactor);
			modelMatrixB *= Transform2D::Translate(-bilaPoz.x,-bilaPoz.y);
			elapsedTimeBila -= deltaTimeSeconds;
			
		}
		else {
			marireBila = false;
			elapsedTimeBila = timeBila;
			radius /= scaleFactor;
		}
	}

	RenderMesh2D(meshes["bila"], shaders["VertexColor"], modelMatrixB);

	//desenare pereti

	//perete stanga
	glm::mat3 modelMatrixSt = glm::mat3(1);
	RenderMesh2D(meshes["pereteSt"], shaders["VertexColor"], modelMatrixSt);

	//perete dreapta
	glm::mat3 modelMatrixDr = glm::mat3(1);
	RenderMesh2D(meshes["pereteDr"], shaders["VertexColor"], modelMatrixDr);

	//perete sus
	glm::mat3 modelMatrixSus = glm::mat3(1);
	RenderMesh2D(meshes["pereteSus"], shaders["VertexColor"], modelMatrixSus);

	//daca bonusul Perete jos este activat
	if (pereteJos) {
		if (elapsedTimePereteJos > 0) {
			elapsedTimePereteJos -= deltaTimeSeconds;

			glm::mat3 modelJos = glm::mat3(1.0f);
			RenderMesh2D(meshes["pereteJos"], shaders["VertexColor"], modelJos);
		}
		else {
			pereteJos = false;
			elapsedTimePereteJos = timePereteJos;
		}
	}

	/*
		Se itereaza prin caramizi iar daca caramida a fost distrusa, se modifica scalarea si se deseneaza 
		powerUps-ul (daca exista).
	*/
	for (int i = 0; i < nrCx; i++) {
		for (int j = 0;j < nrCy; j++) {
			Mesh* mesh = bricks[i][j];
			glm::mat3 modelMatrix = glm::mat3(1);
			// daca caramida a fost distrusa, se modifica scalarea si se deseneaza powerUps-ul (daca exista)
			if (destroyed[i][j]) {
				std::vector<VertexFormat> vertices = mesh->vertices;

				glm::vec2 centru = { (vertices[0].position.x + vertices[1].position.x) / 2,
									(vertices[0].position.y + vertices[3].position.y) / 2 };

				//scalare caramida (i,j)
				modelMatrix *= Transform2D::Translate(centru.x, centru.y);
				modelMatrix *= Transform2D::Scale(scaleBricks[i][j], scaleBricks[i][j]);
				modelMatrix *= Transform2D::Translate(-centru.x, -centru.y);

				//se micsoreaza scalare pana ajunge <=0
				if (scaleBricks[i][j] <= deltaTimeSeconds) scaleBricks[i][j] = 0;
				else scaleBricks[i][j] -= deltaTimeSeconds;

				//daca caramida are powerUp
				if (powerups[i][j] != NULL) {
					glm::vec3 centru = powerups[i][j]->centru;

					// matrice de modelare ptr rotire si translatare in jos cu viteza velocity / 2 a powerUp ului
					glm::mat3 modelMatrixP;
					modelMatrixP *= Transform2D::Translate(0, (velocity / 2) * powerups[i][j]->translateY);
					modelMatrixP *= Transform2D::Translate(centru.x, centru.y);
					modelMatrixP *= Transform2D::Rotate(powerups[i][j]->theta);
					modelMatrixP *= Transform2D::Translate(-centru.x, -centru.y);

					//desenare powerUp
					Mesh* mesh = powerups[i][j]->mesh;
					RenderMesh2D(mesh, shaders["VertexColor"], modelMatrixP);

					// modificare translatare si unghi de rotatie pentru urmatorul frame
					powerups[i][j]->translateY -= deltaTimeSeconds;
					powerups[i][j]->theta += deltaTimeSeconds;

					

					//se obtin noile varfuri ale PowerUp ului, dupa rotatie si translatie (dupa aplicarea modelMatrixP)
					glm::mat4 modelMatrixP4 = Transform2D::To4D(modelMatrixP);
					std::vector<VertexFormat> newVerticesPo = {  
						VertexFormat(Transform2D::To3D(glm::vec4(mesh->vertices[0].position,1) * glm::transpose(modelMatrixP4)), mesh->vertices[0].color),
						VertexFormat(Transform2D::To3D(glm::vec4(mesh->vertices[1].position,1) * glm::transpose(modelMatrixP4)), mesh->vertices[1].color),
						VertexFormat(Transform2D::To3D(glm::vec4(mesh->vertices[2].position,1) * glm::transpose(modelMatrixP4)), mesh->vertices[2].color),
						VertexFormat(Transform2D::To3D(glm::vec4(mesh->vertices[3].position,1) * glm::transpose(modelMatrixP4)), mesh->vertices[3].color) };

					//se obtin noile varfuri ale platformei, dupa translatie (dupa aplicare modelMatrixPl)
					Mesh* platforma = meshes["platforma"];
					glm::mat4 modelMatrixPl4 = Transform2D::To4D(modelMatrixPl);
					std::vector<VertexFormat> newVerticesPl = { 
						VertexFormat(Transform2D::To3D(glm::vec4(platforma->vertices[0].position,1) * glm::transpose(modelMatrixPl4)), platforma->vertices[0].color),
						VertexFormat(Transform2D::To3D(glm::vec4(platforma->vertices[1].position,1) * glm::transpose(modelMatrixPl4)), platforma->vertices[1].color),
						VertexFormat(Transform2D::To3D(glm::vec4(platforma->vertices[2].position,1) * glm::transpose(modelMatrixPl4)), platforma->vertices[2].color),
						VertexFormat(Transform2D::To3D(glm::vec4(platforma->vertices[3].position,1) * glm::transpose(modelMatrixPl4)), platforma->vertices[3].color) };
					
					//in cazul in care se detecteaza o viitoare coliziune
					if (Tema1Object2D::ColisionPlPowerUp(newVerticesPl, newVerticesPo)) {
						//creste numarul de PowerUps acumulate
						nrPowerUps++;
						cout << "Nr catched power ups: " << nrPowerUps << "\n";
						/*
						daca al doilea bonus e activat si s-au acumulat destule PowerUps pentru activarea 
						celui de-al treilea bonus: Platforma Lipicioasa
						*/
						if (marireBila && nrPowerUps >= nrForThirdBonus) {
							lipicios = true;
							cout << "Bonus: Platforma lipicioasa!\n";
							nrPowerUps -= nrForThirdBonus;
						}
						/*
						daca primul bonus e activat si s-au acumulat suficiente powerUps pentru activarea 
						celui de al doilea bonus: Superball
						*/
						else if (pereteJos && nrPowerUps >= nrForSecondBonus) {
							//daca e deja activ, se resteaza timpul
							if (marireBila) {
								elapsedTimeBila = timeBila;
							}
							else {
								marireBila = true;
								radius *= scaleFactor;
							}

							cout << "Bonus: Superball!\n";
							nrPowerUps -= nrForSecondBonus;
						}
						/*
						daca nu mai e activat niciun bonus si s-au acumulat destule powerUps pentru activarea
						primului bonus: Perete Jos
						*/
						else if (nrPowerUps >= nrForFirstBonus) {
							//daca e deja activ, se resteaza timpul
							if (pereteJos) {
								elapsedTimePereteJos = timePereteJos;
							}
							else {
								pereteJos = true;
							}
							cout << "Bonus: Perete Jos!\n";
							nrPowerUps -= nrForFirstBonus;
						}
						
						//in urma coliziunii powerUps nu va mai fi afisat
						powerups[i][j] = NULL;
					}
				}
			}

			//desenare caramida de pe pozitia (i,j);
			RenderMesh2D(mesh, shaders["VertexColor"], modelMatrix);
		}
	}
	
	

	glm::vec3 oldBilaPoz = bilaPoz + glm::vec3(translateBx, translateBy, 0);

	if(clicked){
		// noua pozitie daca bila nu e legata de platforma
		translateBy += velocity * deltaTimeSeconds * sin(RADIANS(theta) ); 
		translateBx += velocity * deltaTimeSeconds * cos(RADIANS(theta) );
	}
	glm::vec3 newBilaPoz = bilaPoz + glm::vec3(translateBx, translateBy, 0);

	//verificare coliziune cu cei 3 pereti, platforma si caramizile

	//theta se poate modifica in urma coliziunilor
	translateBy -= velocity * deltaTimeSeconds * sin(RADIANS(theta));
	translateBx -= velocity * deltaTimeSeconds * cos(RADIANS(theta));

	//tratare coliziuni
	vector<string> numeMese = { "pereteSt", "pereteDr", "pereteSus", "platforma", "bricks", "pereteJos" };
	bool forward = true;
	for (int i = 0; i <= 5; i++) {
		int res;
		if (i == 5 && !pereteJos) break;
		if(i < 3 || i==5) res = Tema1Object2D::ColisionRect(meshes[numeMese[i]]->vertices, oldBilaPoz, newBilaPoz, radius);
		else if(i == 3) res = Tema1Object2D::ColisionPl(meshes[numeMese[i]]->vertices, oldBilaPoz, newBilaPoz, translatePl, &theta, radius, radiusPl);
		else if (i == 4) res = Tema1Object2D::ColisionBricks(&bricks, nrCx, nrCy, oldBilaPoz, newBilaPoz, radius,destroyed);

		if (res) {
			//coliziune cu plan orizontal
			if (res == 1 && i != 3) {
				theta = 360.0f - theta;
			}
			//coliziune cu plan vertical
			else if (res == 2) {
				theta = 180.0f - theta;
				if (theta < 0) theta += 360.0f;
			}
			//coliziune cu platforma lipicioasa => bila sta pe loc (forward = false) si ramane legata de platforma (clicked = false)
			if (i == 3 && lipicios) {
				clicked = false;
				forward = false;
				freezeTranslate = translateBx - translatePl;
			}
			break;
		}
	}

	if (forward) {
		//noua pozitie daca theta se schimba
		translateBy += velocity * deltaTimeSeconds * sin(RADIANS(theta));
		translateBx += velocity * deltaTimeSeconds * cos(RADIANS(theta));
	}

	//noua pozitie a bilei
	newBilaPoz = bilaPoz + glm::vec3(translateBx, translateBy, 0);
	//tratare cazuri in care bila iese din scena
	if (newBilaPoz.x < 0 ||newBilaPoz.x > resx || newBilaPoz.y < 0 || newBilaPoz.y > resy) {
		//tratare pierderi vieti
		clicked = false;
		tura = (tura + 1) % 3;
		translateBx = 0, translateBy = 0;
		translatePl = 0;
		theta = 90.0f;
		lipicios = false;
		nrPowerUps = 0;
		if (pereteJos){
			pereteJos = false;
			elapsedTimePereteJos = timePereteJos;
			
		}

		if (marireBila) {
			radius /= scaleFactor;
			marireBila = false;
			elapsedTimeBila = timeBila;
		}

		//tratare pierdere
		if (tura == 0) {
			cout << "LOST!\n";
			Tema1::nrDestroyed = 0;
			for (int i = 0;i < nrCx; i++) {
				for (int j = 0; j < nrCy; j++) {
					destroyed[i][j] = false;
					powerups[i][j] = NULL;
					scaleBricks[i][j] = 1;
				}
			}
			
			CreatePowerUps();
		}
	}

	//numarul de caramizi
	int nrC = nrCx * nrCy;
	//tratare caz victorie
	if (Tema1::nrDestroyed == nrC) {
		cout << "WIN!\n";

		clicked = false;
		tura = 0;
		translateBx = 0, translateBy = 0;
		translatePl = 0;
		theta = 90.0f;

		nrPowerUps = 0;
		lipicios = false;
		if (pereteJos) {
			pereteJos = false;
			elapsedTimePereteJos = timePereteJos;

		}

		if (marireBila) {
			radius /= scaleFactor;
			marireBila = false;
			elapsedTimeBila = timeBila;
		}

		Tema1::nrDestroyed = 0;
		for (int i = 0;i < nrCx; i++) {
			for (int j = 0; j < nrCy; j++) {
				destroyed[i][j] = false;
				powerups[i][j] = NULL;
				scaleBricks[i][j] = 1;
			}
		}

		CreatePowerUps();
	}

	
}

void Tema1::FrameEnd()
{

}

void Tema1::OnInputUpdate(float deltaTime, int mods)
{

}

void Tema1::OnKeyPress(int key, int mods)
{
	// add key press event
}

void Tema1::OnKeyRelease(int key, int mods)
{
	// add key release event
}

void Tema1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	//daca platforma nu e lipicioasa sau e lipicioasa dar nu s-a dat primul click (sa se activeze holding)
	if (!holding) {
		//translatare centru platforma pe coord x a mouse -ului
		translatePl = (float)mouseX - platformaPoz.x - plX / 2;

		if (!clicked) {
			//daca a inceput o noua viata / un nou joc
			if (!lipicios) {
				//translatare bila pe coord x a mouse-ului
				translateBx = (float)mouseX - bilaPoz.x;
			}
			//daca platforma e lipicioasa, se pastraza distanta fata de centrul platformei (freezeTranslate)
			else {
				translateBx = (float)mouseX - bilaPoz.x + freezeTranslate ;
			}
		}
	}
}

void Tema1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
	if (IS_BIT_SET(button, GLFW_MOUSE_BUTTON_LEFT))
	{
		/*
		daca s-a dat click si platforma e lipicioasa, se tine apasat si se alege 
		punctul spre care sa mearga bila (se activeaza holding)
		*/
		if (lipicios) {
			holding = true;
			cout << "Click for direction!\n";
		}
		/*
		daca s-a inceput cu o noua viata / un alt joc la primul cick, se elibereaza
		bila de platforma
		*/
		else {
			clicked = true;
		}
	}
}

void Tema1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
	if (IS_BIT_SET(button, GLFW_MOUSE_BUTTON_LEFT))
	{
		/*
		daca holding este activat si se elibereaza click stanga, bila se indreapta spre
		punctul spre care indica mouse -ul, dupa care platforma nu mai e lipicioasa
		*/
		if (holding) {
			float dx = (float)mouseX - (bilaPoz.x + translateBx);
			float dy = (resy - (float)mouseY) - (bilaPoz.y + translateBy);
			if (dx == 0) theta = 90;
			else theta = DEGREES(atan(dy / dx));

			if (dx < 0 && dy >= 0) {
				theta = 180.0f + theta;
			}
			else if (dx < 0 && dy < 0) {
				theta = 180.0f + theta;
			}
			else if (dx > 0 && dy >= 0) {
				theta = theta;
			}
			else if (dx > 0 && dy < 0) {
				theta = theta;
			}

			lipicios = false;
			clicked = true;
			holding = false;
		}
	}
}

void Tema1::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}

void Tema1::OnWindowResize(int width, int height)
{
}

