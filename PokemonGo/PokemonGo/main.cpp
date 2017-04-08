#include <stdlib.h>
#include <glut.h>
#include <time.h>
#include <math.h>
#include <windows.h>

#define PI 3.141592

#define D_MAX_POKEMON 3
#define D_BALL_RADIUS 50
#define D_BALL_SPEED 20
#define D_ARROW_ROTATION 5

double g_cpi = PI / 180;

//화면 사이즈 값
GLsizei g_nHeight = 500, g_nWidth = 500;
//사정거리 값
GLfloat g_fMyRadius = 100;

//View씬의 활성화 상태
bool g_bViewScene = true;
//Select씬의 활성화 상태
bool g_bSelectScene = false;
//씬 전환 시간 카운트
DWORD g_TransScene = 0;


//주인공의 X좌표
float g_Trainer_X = g_nWidth / 2;
//주인공의 Y좌표
float g_Trainer_Y = g_nHeight / 2;

//포켓몬 도형 구조체
typedef struct Shape {
	int nX;
	int nY;
	int nWidth;
	int nHeight;
}stShape;

enum {
	E_SHAPE_TRIANGLE,
	E_SHAPE_RECTANGLE,
	E_SHAPE_PENTAGON
};

//포켓몬 객체 리스트
stShape g_ShapeList[3];

//선택된 포켓몬 객체 ID
int g_selectedPokemonInd = 0;

//공 발사시간 카운트
DWORD g_ShootTime = 0;
//공 발사 각도 저장
float g_shootAngle = 0;
//공 발사 회전시 방향 전환
float g_shootAngleAddSign = 1.0f;
//공 발사 상태
bool g_shootStart = false;
//포켓몬 객체 충돌 여부
bool g_IsGetPokemon = false;
//포켓몬 공의 X좌표
float g_Ball_X = g_nWidth / 2;
//포켓몬 공의 Y좌표
float g_Ball_Y = g_nHeight / 4;
//포켓몬 공의 너비
float g_Pokemon_Width = 100;
//포켓몬 공의 높이
float g_Pokemon_Height = 100;
//포켓몬의 X 좌표
float g_Pokemon_X = g_nWidth / 2 - g_Pokemon_Width / 2;
//포켓몬의 Y 좌표
float g_Pokemon_Y = g_nHeight - g_Pokemon_Height - 10;


//삼각형 도형 그리기
void DrawTriangle(GLfloat _x, GLfloat _y, GLfloat _width, GLfloat _height)
{
	glBegin(GL_POLYGON);

	glVertex2f(_x, _y);
	glVertex2f(_x + _width / 2, _y + _height);
	glVertex2f(_x + _width, _y);

	glEnd();
}

//사각형 도형 그리기
void DrawRectangle(GLfloat _x, GLfloat _y, GLfloat _width, GLfloat _height)
{
	glBegin(GL_POLYGON);

	glVertex2f(_x, _y + _height);
	glVertex2f(_x, _y);
	glVertex2f(_x + _width, _y);
	glVertex2f(_x + _width, _y + _height);

	glEnd();
}

//오각형 도형 그리기
void DrawPentagon(GLfloat _x, GLfloat _y, GLfloat _width, GLfloat _height)
{
	glBegin(GL_POLYGON);

	glVertex2f(_x + _width / 4, _y);
	glVertex2f(_x, _y + _height / 2);
	glVertex2f(_x + _width / 2, _y + _height);
	glVertex2f(_x + _width, _y + _height / 2);
	glVertex2f(_x + (_width - _width / 4), _y);

	/*glVertex2f(_x - _width / 2, _y);
	glVertex2f(_x, _y + _height / 2);
	glVertex2f(_x + _width / 2, _y);
	glVertex2f(_x + _width / 4, _y - _height / 2);
	glVertex2f(_x - _width / 4, _y - _height / 2);*/

	glEnd();
}

//타원 도형 그리기
void DrawCircle(GLfloat _x, GLfloat _y, GLfloat _radius)
{
	glBegin(GL_POLYGON);

	int angle = 360 / 12;

	for (int i = 0; i < 360; i += angle) {
		float cir = g_cpi * i;
		glVertex2f(_x + _radius * sin(cir), _y + _radius * cos(cir));
	}

	glEnd();
}

//별모양 도형 그리기
void DrawStar(GLfloat _x, GLfloat _y, GLfloat _radius)
{
	glBegin(GL_POLYGON);

	int angle = 360 / 12;

	int j = 0;
	for (int i = 0; i < 360; i += angle) {
		float cir = g_cpi * i;

		//중간 중간 들어간 정점 처리
		if(j%2 != 0)
			glVertex2f(_x + _radius * sin(cir), _y + _radius * cos(cir));
		else
			glVertex2f(_x + _radius/2 * sin(cir), _y + _radius/2 * cos(cir));

		j++;
	}

	glEnd();
}

//화살표 도형 그리기
void DrawArrow(GLfloat _x, GLfloat _y, GLfloat _width, GLfloat _height)
{
	int add = 1;

	glMatrixMode(GL_MODELVIEW);

	glPushMatrix();
		
		//화살표의 위치로 이동
		glTranslatef(_x + _width / 2, _y - (_height + (-_height) / 4), 0.0f);
		//Z축 회전
		glRotatef(g_shootAngle, 0.0f, 0.0f, 1.0f);
		//원점을 기준으로 회전
		glBegin(GL_POLYGON);
			glVertex2f(
				0 + _width / 4 - _width / 2,
				0 - _height / 4 + _height);

			glVertex2f(
				0 - _width / 2,
				0 - _height / 4 + _height);

			glVertex2f(
				0 + _width / 2 - _width / 2,
				0 + _height);

			glVertex2f(
				0 + _width - _width / 2,
				0 - _height / 4 + _height);

			glVertex2f(
				0 + _width - _width / 4 - _width / 2,
				0 - _height / 4 + _height);

			glVertex2f(
				0 + _width - _width / 4 - _width / 2,
				0 - _height + _height);

			glVertex2f(
				0 + _width / 4 - _width / 2,
				0 - _height + _height);
		glEnd();


	glPopMatrix();


	//g_shootAngle += add * D_ARROW_ROTATION;

	if (g_shootAngle > 90 || g_shootAngle < - 90) {
		add *= -1;
	}
}

void myinit()
{
	glViewport(0, 0, g_nWidth, g_nHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, (GLdouble)g_nWidth, 0.0, (GLdouble)g_nHeight);
	glMatrixMode(GL_MODELVIEW);
	glColor3f(1.0, 0.0, 0.0);

	srand(time(0));

	g_TransScene = GetTickCount() / 1000;
}

void ViewScene()
{
	int width = 70;
	int height = 70;

	for (static int i = 0; i < D_MAX_POKEMON; i++) {
		//X
		g_ShapeList[i].nX = rand() % (g_nWidth - width * 2) + width;
		
		//Y
		g_ShapeList[i].nY = rand() % (g_nHeight - height * 2) + height;

		//Width
		g_ShapeList[i].nWidth = width;

		//Height
		g_ShapeList[i].nHeight = height;
	}

	glColor3f(0.5, 1.0, 0.5);
	DrawCircle(g_Trainer_X, g_Trainer_Y, g_fMyRadius);

	glColor3f(1, 0.5, 0.5);
	DrawTriangle(g_ShapeList[E_SHAPE_TRIANGLE].nX, g_ShapeList[E_SHAPE_TRIANGLE].nY, width, height);
	DrawRectangle(g_ShapeList[E_SHAPE_RECTANGLE].nX, g_ShapeList[E_SHAPE_RECTANGLE].nY, width, height);
	DrawPentagon(g_ShapeList[E_SHAPE_PENTAGON].nX, g_ShapeList[E_SHAPE_PENTAGON].nY, width, height);

	glColor3f(0.5, 0.5, 1.0);
	DrawStar(g_Trainer_X, g_Trainer_Y, D_BALL_RADIUS);
}

void SelectScene()
{
	if (g_shootStart == false) {
		glColor3f(0.0, 0.0, 0.0);
		DrawArrow(
			g_nWidth / 2 - D_BALL_RADIUS / 2,
			g_nHeight / 4 + D_BALL_RADIUS + D_BALL_RADIUS / 2,
			D_BALL_RADIUS,
			D_BALL_RADIUS + 50
		);
	}

	//포켓몬을 잡지 못하였을때는 포켓몬 객체를 그린다.
	if (g_IsGetPokemon == false) {

		glColor3f(1, 0.5, 0.5);

		switch (g_selectedPokemonInd) {
		case E_SHAPE_TRIANGLE:
			DrawTriangle(g_Pokemon_X, g_Pokemon_Y, g_Pokemon_Width, g_Pokemon_Height);
			break;

		case E_SHAPE_RECTANGLE:
			DrawRectangle(g_Pokemon_X, g_Pokemon_Y, g_Pokemon_Width, g_Pokemon_Height);
			break;

		case E_SHAPE_PENTAGON:
			DrawPentagon(g_Pokemon_X, g_Pokemon_Y, g_Pokemon_Width, g_Pokemon_Height);
			break;
		}

	}

	glColor3f(0.8, 0.3, 0.3);
	DrawCircle(g_Ball_X, g_Ball_Y, D_BALL_RADIUS);
}

//원 vs 원 충돌
bool Collision_CircleVSCircle(float _x1, float _y1, float _radius1, float _x2, float _y2, float _radius2) 
{
	float fDistance = sqrt(pow(_x2 - _x1, 2) + pow(_y2 - _y1, 2));

	if (_radius1 + _radius2 > fDistance)
	{
		return true;
	}

	return false;
}

//점 vs 사각형 충돌 (사각형은 왼쪽 아래)
bool Collision_PointVSRect(float _x1, float _y1, float _width1, float _height1, float _x2, float _y2)
{
	if ( _x2 < _x1 + _width1 && _x2 > _x1
		&&
		_y2 < _y1 + _height1 && _y2 > _y1 )
	{
		return true;
	}

	return false;
}

void DrawScene() {
	//보여줄 씬 제어
	if (g_bViewScene) {
		ViewScene();
	}else if(g_bSelectScene){
		SelectScene();
	}
}

void myDisplay()
{
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	DrawScene();

	glFlush();

	Sleep(50);
}

void myPlay() {
	//View
	if (g_bViewScene) {

		//부호를 갖는 값을 계속해서 더함
		g_fMyRadius += g_shootAngleAddSign * 2.0f;

		//좌우 범위를 넘지않게하고 방향을 바꿈
		if (g_fMyRadius > g_nWidth / 4) {
			g_shootAngleAddSign *= -1;
		}
		else if (g_fMyRadius < g_nWidth / 6) {
			g_shootAngleAddSign *= -1;
		}
	}
	//Select
	else {
		static DWORD randomShootTime = rand() % 3 + 2;
		static float selectedAngle = 0.0f;
		

		if (g_ShootTime == 0) {
			g_ShootTime = GetTickCount() / 1000;
		}

		//포켓몬을 잡지 못했고 발사 시간이 되면 값을 초기화한다.
		/*if (g_IsGetPokemon == false && GetTickCount() / 1000 - g_ShootTime > randomShootTime)
		{
			g_ShootTime = GetTickCount() / 1000;
			randomShootTime = rand() % 3 + 2;

			g_shootStart = true;
			selectedAngle = g_shootAngle;
		}*/

		//발사 상태 활성화 시 연산
		if (g_shootStart) {
			float angle = g_shootAngle * -1;

			//각도에 따른 x와 y값을 더하여 이동
			float xSin = sin(angle / 180 * PI) * D_BALL_SPEED;
			float yCos = cos(angle / 180 * PI) * D_BALL_SPEED;
			g_Ball_X += xSin;
			g_Ball_Y += yCos;

			//화면 밖으로 나갔을 시에 공을 제자리에 놓는다.
			if (g_Ball_X < 0 - D_BALL_RADIUS || g_Ball_X > g_nWidth + D_BALL_RADIUS
				||
				g_Ball_Y < 0 - D_BALL_RADIUS || g_Ball_Y > g_nHeight + D_BALL_RADIUS) {
				g_shootStart = false;
				g_Ball_X = g_nWidth / 2;
				g_Ball_Y = g_nHeight / 4;
			}

			//도형을 그릴때 X, Y가 왼쪽 아래가 기준으로 했으므로
			if (g_IsGetPokemon == false && Collision_CircleVSCircle(g_Ball_X, g_Ball_Y, D_BALL_RADIUS,
				g_Pokemon_X + g_Pokemon_Width / 2, g_Pokemon_Y + g_Pokemon_Height / 2, g_Pokemon_Width/2)) {
				g_IsGetPokemon = true;
				g_shootStart = false;

				g_Ball_X = g_nWidth / 2;
				g_Ball_Y = g_nHeight / 4;
			}
		}
	}

	glutPostRedisplay();
}

void mySpecialKeyInput(int key, int x, int y)
{
	//방향키에 따른 주인공 객체 이동 및 좌우 방향키에 따라 화살표 회전
	switch (key) {

	case GLUT_KEY_UP:
		if (g_bViewScene) { g_Trainer_Y += 5.0f; }
		break;

	case GLUT_KEY_LEFT:
		if (g_bViewScene) { g_Trainer_X -= 5.0f; }
		else if (g_bSelectScene) { g_shootAngle += D_ARROW_ROTATION; }
		break;

	case GLUT_KEY_RIGHT:
		if (g_bViewScene) { g_Trainer_X += 5.0f; }
		else if (g_bSelectScene) { g_shootAngle -= D_ARROW_ROTATION; }
		break;

	case GLUT_KEY_DOWN:
		if (g_bViewScene) { g_Trainer_Y -= 5.0f; }
		break;

	default:

		break;
	}
}

void myKeyInput(unsigned char key, int x, int y) {
	//SpaceBar 입력(포켓몬 볼 발사)
	if (key == 32) {
		g_shootStart = true;
	}
}

void myMouse(int btn, int state, int x, int y)
{
	switch (btn) {
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN) {

			//마우스 좌표와 포켓몬 객체들과의 충돌 검사
			for (int i = 0; i < D_MAX_POKEMON; i++) {
				if (g_bViewScene && 
					Collision_PointVSRect
					(g_ShapeList[i].nX, g_ShapeList[i].nY, g_ShapeList[i].nWidth, g_ShapeList[i].nHeight, x, g_nHeight-y))
				{
					g_bViewScene = false;
					g_bSelectScene = true;
					g_selectedPokemonInd = i;
				}
			}

			
		}
		break;
	}
}

void myReshape(GLsizei w, GLsizei h)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, (GLdouble)w, 0.0, (GLdouble)h);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glViewport(0, 0, w, h);

	g_nWidth = w;
	g_nHeight = h;
}

void DoMenu(int value) {
	if (g_IsGetPokemon == true)
		return;

	g_bViewScene = true;
	g_bSelectScene = false;
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutCreateWindow("Pokemon Go");
	myinit();
	glutReshapeFunc(myReshape);
	glutDisplayFunc(myDisplay);
	glutSpecialFunc(mySpecialKeyInput);
	glutKeyboardFunc(myKeyInput);
	glutMouseFunc(myMouse);
	glutIdleFunc(myPlay);

	glutCreateMenu(DoMenu);
	glutAddMenuEntry("Go to Menu", 1);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	glutMainLoop();
}