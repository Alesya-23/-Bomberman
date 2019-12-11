// Bomberman.cpp : Определяет точку входа для приложения.
//
#define _CRT_SECURE_NO_WARNINGS
#include "framework.h"
#include "Bomberman.h"
#include<stdio.h>
#define MAX_LOADSTRING 100

#define MAX_LOADSTRING 100

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна



#define N_BRUSH 7
HBRUSH hbrush_fild[N_BRUSH] = {
	CreateSolidBrush(RGB(0,43,8)),//стены нельзя уничтожать
	CreateSolidBrush(RGB(0,138,9)), // поле- трава
	CreateSolidBrush(RGB(130, 100, 34)), //гриб хороший - герой
	CreateSolidBrush(RGB(255,255,255)), //ножки грибов
	CreateSolidBrush(RGB(209, 21, 21)),//враг шляпка
	CreateSolidBrush(RGB(57, 143, 96)), //cтены которые моэно уничтожить
	CreateSolidBrush(RGB(67, 3,66))
};

#define N_PEN 5
HPEN hpen_fild[N_PEN] = {

	CreatePen(PS_SOLID, 2, RGB(56, 39, 3)), //гриб хороший - герой
	CreatePen(PS_SOLID, 3, RGB(0,0,0)),// глаза 
	CreatePen(PS_SOLID, 2, RGB(219, 21, 21)), // контур плохого гриба - врага
	CreatePen(PS_SOLID, 2, RGB(0,0,0)),
	CreatePen(PS_SOLID, 2, RGB(255,255,255))

};
struct Position {
	int i, j;
};


struct Fild {
	int m;
	int n;
	int map[13][17];
	// 0 - стены которые нельзя уничтожать
	// 1 - трава
	// 2 -стены которые можно уничтожать
	// 3 - персонаж
	// 4- враги
	// 5 - стены уничтожить можно
	struct Position entry_hero;//вход
	struct Position entry_angry1;
	struct Position entry_angry2;
};
struct Fild fild = {

	13, 17,
	{
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 1, 1, 1, 5, 1, 1, 1, 1, 1, 1, 5, 1, 1, 5, 1, 0 },
		{ 0, 1, 0, 1, 0, 1, 0, 5, 0, 1, 0, 1, 0, 1, 0, 1, 0 },
		{ 0, 1, 1, 1, 1, 1, 1, 1, 1, 5, 1, 5, 1, 1, 1, 1, 0 },
		{ 0, 5, 0, 1, 0, 1, 0, 1, 0, 5, 0, 1, 0, 1, 0, 1, 0 },
		{ 0, 1, 1, 1, 1, 5, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0 },
		{ 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 5, 0, 1, 0, 1, 0 },
		{ 0, 1, 1, 1, 5, 1, 1, 1, 1, 1, 1, 5, 1, 1, 1, 1, 0 },
		{ 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0 },
		{ 0, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0 },
		{ 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0 },
		{ 0, 1, 1, 5, 1, 1, 1, 1, 5, 1, 1, 1, 1, 1, 5, 1, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
	},

		1,1,
		5,5,
		4,9

};

char filename[] = "C:\\проекты\\1.txt";

int loadFild(struct Fild* fild) {
	FILE* fin = fopen(filename, "rt");
	if (fin == NULL) {
		printf("File is not opened", filename);
		return 0;
	}
	fscanf(fin, "%d", &fild->m);
	fscanf(fin, "%d", &fild->n);

	for (int i = 0; i < fild->m; i++) {
		for (int j = 0; j < fild->n; j++) {
			fscanf(fin, "%d", &fild->map[i][j]);
		}
	}


	fscanf(fin, "%d", &fild->entry_hero.i);
	fscanf(fin, "%d", &fild->entry_hero.j);
	fscanf(fin, "%d", &fild->entry_angry1.i);
	fscanf(fin, "%d", &fild->entry_angry1.j);
	fscanf(fin, "%d", &fild->entry_angry2.i);
	fscanf(fin, "%d", &fild->entry_angry2.j);

	fclose(fin);
	return 1;

}

struct Object {
	POINT pos;
	POINT arrPos;
	POINT v;
};



struct Object hero;
struct Object angry1;
struct Object angry2;
struct Object bomb;



int map_x = 680;
int map_y = 520;
int size_map_x = 40; //масштаб
int size_map_y = 40;



int hero_state = 0; //стоим - 0 , 1 - вправо, 2 -вниз, 3 - влево, 4 вверх , -1 убит
int angry_1state = 0;// -1 уничтожен
int angry_2state = 0;// - 1 уничтожен

int bomb_state = 0; //1 - поставлена, 2 взрыв, 3 уничтожена

int time = 200; // время
int k_wall = 15; //количество врагов
int count = 0; //количество очков - уничтоженные стены + еда



void Draw_Fild(HDC hdc) {
	int i = 0;
	int j = 0;
	for (i = 0; i < fild.m; i++) {
		for (j = 0; j < fild.n; j++) {
			RECT rect = {
				j * size_map_x,i * size_map_y + 20 ,
				(j + 1) * size_map_x,	(i + 1) * size_map_y + 20
			};
			if (fild.map[i][j] == 0) {
				FillRect(hdc, &rect, hbrush_fild[0]);
			}
			else if (fild.map[i][j] == 1 || fild.map[i][j] == 3) {
				FillRect(hdc, &rect, hbrush_fild[1]);
			}
			else if (fild.map[i][j] == 5) {

				FillRect(hdc, &rect, hbrush_fild[5]);



			}
		}

	}
}


void DrawHero(HDC hdc, struct Object* obj) {
	obj->pos.x = obj->arrPos.x * size_map_x + size_map_x / 2;
	obj->pos.y = obj->arrPos.y * size_map_y + size_map_y / 2 + 20;
	SelectObject(hdc, hbrush_fild[2]);
	SelectObject(hdc, hpen_fild[0]);
	Chord(hdc, obj->pos.x - size_map_x / 2,
		obj->pos.y - size_map_y / 2, obj->pos.x + size_map_x / 2, obj->pos.y + size_map_y / 2,
		obj->pos.x + size_map_x / 2, obj->pos.y, obj->pos.x - size_map_x / 2,
		obj->pos.y);
	POINT pt[6] = {
		{obj->pos.x + 5, obj->pos.y },
		{obj->pos.x + 10, obj->pos.y + 10},
		{obj->pos.x + 5, obj->pos.y + 15},
		{obj->pos.x - 10, obj->pos.y + 15},
		{obj->pos.x - 15 , obj->pos.y + 10},
		{obj->pos.x - 10, obj->pos.y},
	};
	SelectObject(hdc, hbrush_fild[3]);
	Polygon(hdc, pt, 6);
	SelectObject(hdc, hpen_fild[1]);
	MoveToEx(hdc, obj->pos.x + 1, obj->pos.y + 5, NULL);
	LineTo(hdc, obj->pos.x - 2, obj->pos.y + 5);
	MoveToEx(hdc, obj->pos.x - 5, obj->pos.y + 5, NULL);
	LineTo(hdc, obj->pos.x - 7, obj->pos.y + 5);
	MoveToEx(hdc, obj->pos.x - 8, obj->pos.y + 10, NULL);
	LineTo(hdc, obj->pos.x + 2, obj->pos.y + 10);

}


void DrawAngry(HDC hdc, struct Object* obj) {

	POINT pt[3] = {
		{obj->pos.x , obj->pos.y },
		{obj->pos.x - 20, obj->pos.y + 20},
		{obj->pos.x + 20, obj->pos.y + 20}
	};
	SelectObject(hdc, hbrush_fild[4]);
	Polygon(hdc, pt, 3);
	SelectObject(hdc, hbrush_fild[3]);
	Rectangle(hdc, obj->pos.x - 5, obj->pos.y + 20, obj->pos.x + 5, obj->pos.y + 30);
	Ellipse(hdc, obj->pos.x - 5, obj->pos.y + 5, obj->pos.x, obj->pos.y + 10);
	Ellipse(hdc, obj->pos.x - 10, obj->pos.y + 13, obj->pos.x - 5, obj->pos.y + 18);
	Ellipse(hdc, obj->pos.x + 5, obj->pos.y + 8, obj->pos.x + 10, obj->pos.y + 13);
}

//скопипастить код из 6 бомбы для взрыва
void DrawBomb(HDC hdc, struct Object* obj) {

	if (bomb_state == 1) {
		SelectObject(hdc, hbrush_fild[6]);
		Ellipse(hdc, bomb.pos.x - 10, bomb.pos.y - 10, bomb.pos.x + 10, bomb.pos.y + 10);

	}
	else if (bomb_state == 2) {
		Ellipse(hdc, bomb.pos.x - 12, bomb.pos.y - 12, bomb.pos.x + 12, bomb.pos.y + 12);
		Ellipse(hdc, bomb.pos.x - 15, bomb.pos.y - 15, bomb.pos.x + 15, bomb.pos.y + 15);
		bomb_state = 4;
	}
}
/*void DrawBombdestroy(HDC hdc, struct Object* obj)
	if (bomb_state == 4) {
		Rectangle(hdc, bomb.pos.x - 20, bomb.pos.y - 20, bomb.pos.x + 20, bomb.pos.y + 20);
		bomb_state = 0;

	}
}*/
// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Разместите код здесь.

	// Инициализация глобальных строк
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_BOMBERMAN, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Выполнить инициализацию приложения:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_BOMBERMAN));

	MSG msg;

	// Цикл основного сообщения:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}



//
//  ФУНКЦИЯ: MyRegisterClass()
//
//  ЦЕЛЬ: Регистрирует класс окна.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDC_BOMBERMAN));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_BOMBERMAN);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   ЦЕЛЬ: Сохраняет маркер экземпляра и создает главное окно
//
//   КОММЕНТАРИИ:
//
//        В этой функции маркер экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится главное окно программы.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}


// ДВИЖЕНИЕ ГЕРОЯ


// ДВИЖЕНИЕ ГЕРОЯ
bool scanRightCollider(struct Fild* level, struct Object* object) {

	if ((level->map[object->arrPos.y][object->arrPos.x + 1] == 0) || (level->map[object->arrPos.y][object->arrPos.x + 1] == 5))
		return true;
	else
		return false;
}

bool scanLeftCollider(struct Fild* level, struct Object* object) {

	if (level->map[object->arrPos.y][object->arrPos.x - 1] == 0 || (level->map[object->arrPos.y][object->arrPos.x + 1] == 5))
		return true;
	else
		return false;
}

bool scanUpCollider(struct Fild* level, struct Object* object) {

	if (level->map[object->arrPos.y - 1][object->arrPos.x] == 0 || (level->map[object->arrPos.y][object->arrPos.x + 1] == 5))
		return true;
	else
		return false;
}

bool scanDownCollider(struct Fild* level, struct Object* object) {

	if (level->map[object->arrPos.y + 1][object->arrPos.x] == 0 || (level->map[object->arrPos.y][object->arrPos.x + 1] == 5))
		return true;
	else
		return false;
}



void MoveObject() {
	int i = 1;
	int j = 1;
	for (i = 0; i < fild.m; i++) {
		for (j = 0; j < fild.n; j++) {

			if (hero_state == 1) {
				if (hero.pos.x <= 580)
				{
					hero.pos.x += (size_map_x * fild.entry_hero.j);
					hero_state = 0;
				}
			}
			else if (hero_state == 2) {
				if (hero.pos.y <= 440)
				{
					hero.pos.y += size_map_y * fild.entry_hero.i;
					hero_state = 0;
				}
			}
			else if (hero_state == 3) {
				if (hero.pos.x != (size_map_x * fild.entry_hero.j + size_map_x / 2)) {
					hero.pos.x -= (size_map_x * fild.entry_hero.j);
					hero_state = 0;
				}
			}
			else if (hero_state == 4) {
				if (hero.pos.y != (size_map_y * fild.entry_hero.i + size_map_y / 2 + 20)) {

					hero.pos.y -= size_map_y * fild.entry_hero.i;
					hero_state = 0;

				}
			}
		}
	}
}

void MoveHeroRight() {
	hero.arrPos.x++;
}

void MoveHeroDown() {
	hero.arrPos.y++;
}

void MoveHeroLeft() {
	hero.arrPos.x--;
}

void MoveHeroUp() {
	hero.arrPos.y--;
}



// ДВИЖЕНИЕ ВРАГА


void MoveAngry() {
	while (angry_1state == 0) {
		angry1.pos.x -= 40;
		if (angry1.pos.x <= 40) {
			angry1.pos.x = 620;

		}
		return;
	}
}


void MoveAngry2() {
	while (angry_2state == 0) {
		angry2.pos.y -= 20;
		if (angry2.pos.y <= 50) {
			angry2.pos.y = 465;

		}
		return;
	}
}


// СТОЛКНОВЕНИЕ С ВРАГОМ

void CheckContact() {

	if ((hero.pos.x == angry1.pos.x) && ((hero.pos.y - 15) == angry1.pos.y) ||
		((hero.pos.x == angry2.pos.x) && ((hero.pos.y - 15) == angry2.pos.y))) {
		hero_state = -1;
		time = 0;
	}
	return;
}
/*angry1.pos.x = size_map_x * fild.entry_angry1.j + size_map_x / 2;
angry1.pos.y = size_map_y * fild.entry_angry1.i + size_map_y / 2 + 5;
angry2.pos.x = size_map_x * fild.entry_angry2.j + size_map_x / 2;
angry2.pos.y = size_map_y * fild.entry_angry2.i + size_map_y / 2 + 5;
void bombCheckKontact() {
	if (bomb.arrPos.x == ((angry1.pos.x - size_map_x / 2) / size_map_x)&&
		(bomb.arrPos.y == ((angry1.pos.y - size_map_y / 2 -5) / size_map_y))

}*/
//БОМБА И ЕЕ АКТИВАЦИЯ + САМ ВЗРЫВ
void BombPaste() {
	bomb_state = 1;
	bomb.arrPos.x = hero.arrPos.x;
	bomb.arrPos.y = hero.arrPos.y;
	bomb.pos.x = hero.pos.x;
	bomb.pos.y = hero.pos.y;
	return;
}


//if (level->map[object->pos.y][object->pos.x - 1] == 0)


void BombBabax() {
	bomb_state = 2;
	int i = bomb.arrPos.y - 2;
	int j = bomb.arrPos.x - 2;
	for (i = bomb.arrPos.y - 2; i <= bomb.arrPos.y + 2; i++) {
		for (j = bomb.arrPos.x - 2; j <= bomb.arrPos.x + 2; j++)
		{
			if (fild.map[i][j] == 5) {
				fild.map[i][j] = 1;
				count++;
				k_wall--;

			}
		}

	}fild.map[bomb.arrPos.y][bomb.arrPos.x] = 1;
	return;
}

// ТАЙМЕР + ТЕКСТОВЫЕ ШТУКИ
void timer() {
	if (hero_state != -1) {
		while (time != 0) {
			Sleep(500);
			time--;
			return;
		}
	}
}
void DrowTextOut(HDC hdc) {
	HFONT hFont = CreateFont(20,
		0, 0, 0, 0, 0, 0, 0,
		DEFAULT_CHARSET,
		0, 0, 0, 0,
		L"Courier New"
	);
	SelectObject(hdc, hFont);
	SetTextColor(hdc, RGB(0, 0, 128));
	TCHAR  string1[] = _T("Время:");
	TextOut(hdc, 0, 0, string1, _tcslen(string1));

	TCHAR  string2[] = _T("Стены:");
	TextOut(hdc, 200, 0, string2, _tcslen(string2));

	TCHAR  string3[] = _T("Очки:");
	TextOut(hdc, 500, 0, string3, _tcslen(string3));

	char sHit[5]; // локальная переменная sHit
	TCHAR  tsHit[5];
	sprintf(sHit, "%d", time); // использование глобальной переменной hit
	OemToChar(sHit, tsHit);
	TextOut(hdc, 80, 0, tsHit, _tcslen(tsHit));
	char scountBomb[5];
	TCHAR  tscountBomb[5];
	sprintf(scountBomb, "%d", k_wall);
	OemToChar(scountBomb, tscountBomb);
	TextOut(hdc, 300, 0, tscountBomb, _tcslen(tscountBomb));

	char sMissed[5];
	TCHAR  tsMissed[5];
	sprintf(sMissed, "%d", count);
	OemToChar(sMissed, tsMissed);
	TextOut(hdc, 590, 0, tsMissed, _tcslen(tsMissed));



	if (k_wall == 0 && time != 0) {
		HFONT hFont = CreateFont(80,
			0, 0, 0, 0, 0, 0, 0,
			DEFAULT_CHARSET,
			0, 0, 0, 0,
			L"Courier New"
		);
		SelectObject(hdc, hFont);
		SetTextColor(hdc, RGB(13, 0, 255));
		TCHAR  string4[] = _T("YOU WIN!");
		TextOut(hdc, 500, 300, string4, _tcslen(string4));


	}
	else if (hero_state == -1 || time == 0) {
		HFONT hFont = CreateFont(80,
			0, 0, 0, 0, 0, 0, 0,
			DEFAULT_CHARSET,
			0, 0, 0, 0,
			L"Courier New"
		);
		SelectObject(hdc, hFont);
		SetTextColor(hdc, RGB(176, 0, 255));
		TCHAR  string5[] = _T("YOU LOSE!");
		TextOut(hdc, 500, 350, string5, _tcslen(string5));
	}
}

//
//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//(hero.pos.x != (size_map_x * fild.entry_hero.j + size_map_x / 2 && fild.map
//  ЦЕЛЬ: Обрабатывает сообщения в главном окне.
//
//  WM_COMMAND  - обработать меню приложения
//  WM_PAINT    - Отрисовка главного окна
//  WM_DESTROY  - отправить сообщение о выходе и вернуться
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Разобрать выбор в меню:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;

	case WM_CREATE:


		loadFild(&fild);

		hero.arrPos.x = 1;
		hero.arrPos.y = 1;



		angry1.pos.x = size_map_x * fild.entry_angry1.j + size_map_x / 2;
		angry1.pos.y = size_map_y * fild.entry_angry1.i + size_map_y / 2 + 5;
		angry2.pos.x = size_map_x * fild.entry_angry2.j + size_map_x / 2;
		angry2.pos.y = size_map_y * fild.entry_angry2.i + size_map_y / 2 + 5;



		SetTimer(hWnd, 1, 100, 0);

		break;

	case WM_TIMER:
		MoveObject();

		timer();
		MoveAngry();
		MoveAngry2();
		CheckContact();
		InvalidateRect(hWnd, NULL, TRUE);
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case 0x4C: // 'L'
			/*loadLevel(&level);
			sizeX = width / level.m;
			sizeY = height / level.n;
			hero.pos.x = sizeX * level.entry.j + sizeX / 2;
			hero.pos.y = sizeY * level.entry.i + sizeY / 2;
			*/
			InvalidateRect(hWnd, NULL, TRUE);
			break;
		case VK_LEFT:
			if (!scanLeftCollider(&fild, &hero))
				MoveHeroLeft();
			InvalidateRect(hWnd, NULL, TRUE);
			break;
		case VK_RIGHT:
			if (!scanRightCollider(&fild, &hero))
				MoveHeroRight();
			InvalidateRect(hWnd, NULL, TRUE);
			break;
		case VK_UP:
			if (!scanUpCollider(&fild, &hero))
				MoveHeroUp();
			InvalidateRect(hWnd, NULL, TRUE);
			break;
		case VK_DOWN:
			if (!scanDownCollider(&fild, &hero))
				MoveHeroDown();
			InvalidateRect(hWnd, NULL, TRUE);
			break;
		case VK_SPACE:
			BombPaste();
			InvalidateRect(hWnd, NULL, TRUE);
			break;
		case VK_RETURN:
			BombBabax();
			InvalidateRect(hWnd, NULL, TRUE);
			break;
		}

		break;


	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: Добавьте сюда любой код прорисовки, использующий HDC...

		Draw_Fild(hdc);
		DrawBomb(hdc, &bomb);
		DrawHero(hdc, &hero);
		DrawAngry(hdc, &angry1);
		DrawAngry(hdc, &angry2);
		//DrawBombdestroy(hdc, &bomb);
		DrowTextOut(hdc);
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Обработчик сообщений для окна "О программе".
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
