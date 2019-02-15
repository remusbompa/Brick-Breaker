#pragma once

#include <Component/SimpleScene.h>
#include <string>
#include <Core/Engine.h>

//clasa ce modeleaza un PowerUp
class PowerUp {

public:
	Mesh* mesh;
	float translateY = 0;
	float theta = 0;
	glm::vec3 centru;
};

class Tema1 : public SimpleScene
{
public:
	Tema1();
	~Tema1();

	void Init() override;
	void CreatePowerUps();

	//pozitia initiala a platformei
	glm::vec3 platformaPoz;
	//pozitia initiala a celor 3 vieti
	glm::vec3 bilePoz[3];
	//pozitia initiala a bilei
	glm::vec3 bilaPoz;

	//translatie pe Ox a platformei fata de poz initiala
	float translatePl = 0;
	//translatiile bilei fata de poz initiala
	float translateBx = 0.0f, translateBy = 0.0f;

	//cosinusul unghiului theta daca bila loveste platforma pe margine
	static float plExtr; 
	// numarul de caramizi distruse
	static int nrDestroyed;

private:
	void FrameStart() override;
	void Update(float deltaTimeSeconds) override;
	void FrameEnd() override;

	void OnInputUpdate(float deltaTime, int mods) override;
	void OnKeyPress(int key, int mods) override;
	void OnKeyRelease(int key, int mods) override;
	void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
	void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
	void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
	void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
	void OnWindowResize(int width, int height) override;

protected:

	//matricea de modelare a platformei
	glm::mat3 modelMatrixPl = glm::mat3(1);

	//daca bila este legata sau nu de platforma
	bool clicked = false;

	// raporturi ptr imagine
	float rplx = 0.167164f;
	float rcx = 0.644f;
	float rpex = 0.020895f;
	float rbx = 0.01591044f;
	float rdifb = 0.02388059f;

	float rcy = 0.54255319f;
	float rply = 0.015957446f;
	float rpozperVy = 0.10638f;
	float rpozperOy = 0.037234f;
	float rpozbila = 0.08574468f;
	float rpozbile = 0.037234042f;

	float r_powerUps = 0.5f;
	float r_dimPowerUps = 2.0f / 3.0f;

	float rix = 0.4f;
	float rjy = 0.3f;

	//numarul de powerUps
	int nr;
	// dimensiunea unei caramizi pe Ox si Oy
	float x, y;

	//nr de caramizi pe Ox si pe Oy
	int nrCx = 12;
	int nrCy = 10;

	//lungimea platformei pe Ox
	float plX;
	
	//unghiul vitezei bilei fata de sensul pozitiv al axei Ox, luat in sens trigonometric
	float theta = 90.0f;
	//viteza bilei
	float velocity = 400.0f;

	//raza bilei
	float radius;
	//jumat din lungimea platformei pe Ox
	float radiusPl;

	//rezolutia pe Ox si Oy
	float resx, resy;
	//numarul de vieti pierdute
	int tura = 0;

	//matricea caramizilor distruse
	bool** destroyed;
	//matricea scalarii caramizilor
	float** scaleBricks;
	//matricea caramizilor
	Mesh*** bricks;
	//matricea powerUps -urilor
	PowerUp*** powerups;

	//nr de powerUps acumulate de la ultimul bonus
	int nrPowerUps = 0;
	//nr de powerUps necesare pentru primul bonus
	int nrForFirstBonus = 10;
	//nr de powerUps necesare pentru al doilea bonus, daca primul bonus e activ
	int nrForSecondBonus = 5;
	//nr de powerUps necesare pentru al treilea bonus, daca al doilea bonus e activ
	int nrForThirdBonus = 5;

	//folosite pentru primul bonus
	bool pereteJos = false;
	float elapsedTimePereteJos = 30.0f;
	float timePereteJos = 30.0f;

	//folosite pentru cel de-al doile bonus
	bool marireBila = false;
	float scaleFactor = 3.0f;
	float elapsedTimeBila = 30.0f;
	float timeBila = 30.0f;

	//folosite pentru cel de-al treilea bonus
	bool lipicios = false;
	bool holding = false;
	float freezeTranslate;
};