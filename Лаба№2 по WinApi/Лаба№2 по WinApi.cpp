#include <windows.h>
#include <string>
#include <iostream>
#include<fstream>
#include <vector>
#include<algorithm>
#include <math.h>
#include <list>
#include "resource.h"
#define  menu IDR_MENU1
#define ID_BUTTON 3002
#define ID_LIST   3003
#define ID_Button1 button_inter
#define ID_Button2 button_uninter
#define radio (HMENU) 3003 

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR
	szCmdLine, int iCmdShow)
{
	static char szAppName[] = "HelloWin";
	HWND hwnd;
	//HWND PoPUpHwnd;       // идентификатор временного окна
	MSG msg;
	WNDCLASSEX wndclass;
	wndclass.cbSize = sizeof(wndclass);
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName = MAKEINTRESOURCE(menu);
	wndclass.lpszClassName = szAppName;
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	RegisterClassEx(&wndclass);
	hwnd = CreateWindow(
		szAppName, // window class name
		"Лабораторная работа №2 WinApi", // window caption
		WS_OVERLAPPEDWINDOW,// window style

		500, // initial x position
		500, // initial y position
		1000, // initial x size
		1000, // initial y size
		NULL, // parent window handle
		NULL, // window menu handle
		hInstance, // program instance handle
		NULL); // creation parameters
	ShowWindow(hwnd, SW_SHOWNORMAL);
	UpdateWindow(hwnd);
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

struct Circle {
	std::pair<int, int> crd;
	bool color = false;
	bool inters = false;
	bool choose = false;
	double rad = 25;
	Circle(std::pair<int, int> pair) : crd(pair) {}
	const friend bool operator == (const Circle& c1, const Circle& c2)
	{
		return c1.crd == c2.crd;
	}
};

double length(std::pair<int, int>& p1, std::pair<int, int>& p2) {
	return sqrt((p1.first - p2.first) * (p1.first - p2.first) + (p1.second - p2.second) * (p1.second - p2.second));
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, UINT wParam, LPARAM
	lParam) {

	static std::list <Circle>  lst_all_;
	static std::list <Circle>::iterator it_list_;
	static std::list <Circle>  lst_chosen_;
	static std::list <Circle>  lst_fillen_;
	static std::list <Circle>  lst_inter_;
	static std::list <Circle>  lst_uninter_;

	PAINTSTRUCT ps;
	HDC         hdc;
	RECT rect;
	rect.top = 0;
	rect.left = 0;
	rect.bottom = 1000;
	rect.right = 750;
	static bool flag = false;
	static bool flag1 = false;
	static bool flag_per = false;
	static bool flag_draw = false;
	static bool flag_choose = false;
	static bool flag_fill = false;

	static bool flag_condition_intersection = false;
	static bool flag_condition_unintersection = false;
	static bool flag_condition_show_chosen = false;
	static bool flag_condition_show_fillen = false;

	static HMENU     hMenu;
	static HINSTANCE hInstance;
	static WNDCLASS  wndclass;
	static HWND
		group_box,
		button_inter,
		button_uninter,
		draw_or_not,
		radio_button,
		show_all_button,
	    fill_circles;

	static int count;


	static int x_pos;
	static int y_pos;
	static int rad = 25;
	static int cond_but;

	HBRUSH    hbrush;
	HBRUSH    hbrush1;
	HPEN hPen;
	HPEN hPen1;
	HPEN hPen_white;
	HPEN hPen_red;
	HBRUSH hBrush;

	switch (msg) {
	case WM_CREATE:
	{
		group_box = CreateWindow("button", "Выбор режима", WS_CHILD | WS_VISIBLE | BS_GROUPBOX
			| WS_GROUP | BS_CENTER, 1000, 500, 700, 700, hwnd, NULL, hInstance, NULL);

		button_inter = CreateWindow("button", "Показать пересек", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			1050, 620, 150, 20, hwnd, (HMENU)1001, hInstance, NULL);

		button_uninter = CreateWindow("button", "включить режим выбора", WS_CHILD | WS_VISIBLE | BS_CHECKBOX,
			1050, 550, 250, 20, hwnd, (HMENU)2002, hInstance, NULL);

		radio_button = CreateWindow("button", "показать выбранные", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			1050, 660, 150, 20, hwnd, (HMENU)3003, hInstance, NULL);

		show_all_button = CreateWindow("button", "Показать !пересек", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			1050, 700, 150, 20, hwnd, (HMENU)4004, hInstance, NULL);

		draw_or_not = CreateWindow("button", "включить режим рисования", WS_CHILD | WS_VISIBLE | BS_CHECKBOX,
			1050, 750, 250, 20, hwnd, (HMENU)5005, hInstance, NULL);

		fill_circles = CreateWindow("button", "закрасить", WS_CHILD | WS_VISIBLE | BS_CHECKBOX,
			1350, 620, 250, 20, hwnd, (HMENU)6006, hInstance, NULL);
		break;
	}
	case WM_LBUTTONDOWN: {
		HDC hdc1 = GetDC(hwnd);
		if (flag_draw == true && lst_all_.size() != 30)
		{
			if (LOWORD(lParam) != 0 && HIWORD(lParam) != 0)
			{
				x_pos = LOWORD(lParam);
				y_pos = HIWORD(lParam);

				std::pair<int, int> p1 = { x_pos, y_pos };
				Circle cir(p1);
				lst_all_.push_back(cir);
				Ellipse(hdc1, x_pos - rad, y_pos - rad, x_pos + rad, y_pos + rad);
				//MessageBox(hwnd, "Here",0,0);


				// Здесь происходит "сортировка" окружностей
				if (lst_all_.size() > 0) {
					for (auto& c : lst_all_) {
						if (length(p1, c.crd) <= 2 * rad && p1 != c.crd)
						{
							lst_inter_.push_back(p1);
							lst_inter_.push_back(c);
						}
					}
				}
			}
		}
		if (flag_choose == true) {
			if (LOWORD(lParam) != 0 && HIWORD(lParam) != 0)
			{
				flag_draw = false;
				flag_fill = false;
				x_pos = LOWORD(lParam);
				y_pos = HIWORD(lParam);

				std::pair<int, int> p1 = { x_pos, y_pos };
				Circle cir(p1);
				hPen_red = CreatePen(PS_SOLID, 1, RGB(255, 1, 2));
				SelectObject(hdc1, hPen_red);
				for (auto& c : lst_all_) {
					if (length(c.crd, p1) <= 13)
					{
						Ellipse(hdc1, c.crd.first - rad, c.crd.second - rad,
							c.crd.first + rad, c.crd.second + rad);
						lst_chosen_.push_back(c);
					}
				}
				DeleteObject(hPen_red);
			}
		}
		if (flag_fill   == true) {
			if (LOWORD(lParam) != 0 && HIWORD(lParam) != 0)
			{
				flag_draw = false;
				x_pos = LOWORD(lParam);
				y_pos = HIWORD(lParam);

				std::pair<int, int> p1 = { x_pos, y_pos };
				Circle cir(p1);
					hBrush = CreateSolidBrush(RGB(157, 157, 255));
				for (auto& c : lst_chosen_)  {

					if (length(c.crd, p1) <= 13)
					{
				       SelectObject(hdc1,hBrush);

						Ellipse(hdc1, c.crd.first - rad, c.crd.second - rad,
							          c.crd.first + rad, c.crd.second + rad);
						c.color = true;
					}
					UpdateWindow(hwnd);
					DeleteObject(hBrush);
				}
				for (auto& c : lst_uninter_) {
					if (length(c.crd, p1) <= 13)
					{

				        SelectObject(hdc1,hBrush);

						Ellipse(hdc1, c.crd.first - rad, c.crd.second - rad,
							          c.crd.first + rad, c.crd.second + rad);
						UpdateWindow(hwnd);
						c.color = true;
					}
					DeleteObject(hBrush);
				}
				for (auto& c : lst_inter_)   {
					if (length(c.crd, p1) <= 13)
					{
				        SelectObject(hdc1,hBrush);
						Ellipse(hdc1, c.crd.first - rad, c.crd.second - rad,
							          c.crd.first + rad, c.crd.second + rad);
						c.color = true;
					}
					DeleteObject(hBrush);
				}
			}
			ReleaseDC(hwnd, hdc1);
		}
		ReleaseDC(hwnd, hdc1);
		UpdateWindow(hwnd);
		break;
	}
	case WM_COMMAND:
	{
		switch LOWORD(wParam)
		{
		case ID_CLEARALLT_CLEAR: {
			InvalidateRect(hwnd, NULL, TRUE);
			lst_all_.clear();
			lst_chosen_.clear();
			lst_inter_.clear();
			lst_uninter_.clear();
			if (lst_all_.size() != 0 && lst_chosen_.size() != 0 &&
				lst_inter_.size() != 0 && lst_uninter_.size() != 0)
			{
				MessageBox(hwnd, "smth wrong", MB_OK, NULL);
			}
			else MessageBox(hwnd, "Clear was succesfuly made", MB_OK, NULL);
			break;
		}
		case ID_SHOW: {
			break;
		}
		case ID_DRAW: {
			flag_draw = true;
			flag_choose = false;
			break;
		}
		case ID_ABOUT_ABOUT: {
			MessageBox(hwnd, "Программа подготовлена студентом 1-ого курса Быстровым Михаилом", 0, NULL);
			break;
		}

		case 1001: {
			flag_condition_intersection = true;
			flag_condition_unintersection = false;
			flag_condition_show_chosen = false;

			SendMessage(button_uninter, BM_SETCHECK, 0, 0L);
			SendMessage(draw_or_not, BM_SETCHECK, 0, 0L);
			SendMessage(fill_circles, BM_SETCHECK, 0, 0L);
			//if (lst_inter.size() > 2) { MessageBox(hwnd, "Smth strange!!!", MB_OK, 0); }
			break;
		}
		case 2002: {
			cond_but = SendMessage(button_uninter, BM_GETCHECK, 0, 0L);
			if (cond_but == 0) {
				SendMessage(button_uninter, BM_SETCHECK, 1, 0L);
				SendMessage(draw_or_not, BM_SETCHECK, 0, 0L);
				SendMessage(fill_circles, BM_SETCHECK, 0, 0L);
				flag_choose = true;
				flag_draw = false;
			}
			else {
				SendMessage(button_uninter, BM_SETCHECK, 0, 0L);
				flag_choose = false;
			}

			break;
		}
        case 3003: {
			flag_condition_intersection = false;
			flag_condition_unintersection = false;
			flag_condition_show_chosen = true;
			SendMessage(button_uninter, BM_SETCHECK, 0, 0L);
			SendMessage(draw_or_not, BM_SETCHECK, 0, 0L);
			SendMessage(fill_circles, BM_SETCHECK, 0, 0L);
			break;
		}
		case 4004: {
			flag_condition_intersection = false;
			flag_condition_unintersection = true;
			flag_condition_show_chosen = false;
			flag_condition_show_fillen = false;
			SendMessage(button_uninter, BM_SETCHECK, 0, 0L);
			SendMessage(draw_or_not, BM_SETCHECK, 0, 0L);
			SendMessage(fill_circles, BM_SETCHECK, 0, 0L);
			if (lst_all_.size() > 0) {
				for (auto& c : lst_all_) {
					it_list_ = std::find(lst_inter_.begin(), lst_inter_.end(), c);
					if (it_list_ == lst_inter_.end()) {
						lst_uninter_.push_back(c);
					}
				}
			}
			//if (lst_inter.size() > 2) { MessageBox(hwnd, "Smth strange!!!", MB_OK, 0); }
			break;
		}
		case 5005: {
			count = SendMessage(draw_or_not, BM_GETCHECK, 0, 0L);
			if (count == 0) {
				SendMessage(draw_or_not, BM_SETCHECK, 1, 0L);
				SendMessage(button_uninter, BM_SETCHECK, 0, 0L);
				SendMessage(fill_circles, BM_SETCHECK, 0, 0L);
				flag_choose = false;
				flag_draw = true;
			}
			else {
				SendMessage(draw_or_not, BM_SETCHECK, 0, 0L);
				flag_draw = false;
			}
			break;
		}
		case 6006: {
			break;
		}
		}
	}
	case WM_PAINT:
	{
		hdc = BeginPaint(hwnd, &ps);
			for (auto& c : lst_all_) {
				Ellipse(hdc, c.crd.first - rad, c.crd.second - rad,
					c.crd.first + rad, c.crd.second + rad);
		}
		EndPaint(hwnd, &ps);
		break;
	}
	case WM_KILLFOCUS: {
		//MessageBox(hwnd, "here wrong", MB_OK, NULL);
		break; }
	case WM_DESTROY: {
		lst_all_.clear();
		lst_chosen_.clear();
		lst_inter_.clear();
		lst_uninter_.clear();
		if (lst_all_.size()   != 0 && lst_chosen_.size() != 0 &&
			lst_inter_.size() != 0 && lst_uninter_.size() != 0  )
		{
			MessageBox(hwnd, "smth wrong", MB_OK, NULL);
		}
		else MessageBox(hwnd, "lists were succesfuly cleared", MB_OK, NULL);
        PostQuitMessage(0);
		break;
	}
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

// Когда сворачиваю окно всё крашится