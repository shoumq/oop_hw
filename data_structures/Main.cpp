#include <conio.h>
#include <windows.h>
#include <vector>

using namespace std;

class shape {
public:
	virtual void show(HDC hdc) {}
}; 

class point : shape {
public:
	int x, y;
	COLORREF color;
	point(int x, int y, COLORREF c = RGB(255, 255, 255)) : x(x), y(y), color(c) {}
	void show(HDC hdc)
	{
		SetPixel(hdc, x, y, color);
	}
};

class rectangle : shape {
	point top_left;
	point bottom_right;
	COLORREF color;
	COLORREF background_color;
	bool filled;
public:
	rectangle(int x1, int y1, int x2, int y2, COLORREF c, bool filled = false, COLORREF bc = 0) : top_left(x1, y1), bottom_right(x2, y2), color(c), background_color(bc), filled(filled) {}
	void show(HDC hdc) {
		SelectObject(hdc, GetStockObject(DC_PEN));
		SetDCPenColor(hdc, color);
		SetDCBrushColor(hdc, background_color);
		if (filled)
		{
			SelectObject(hdc, GetStockObject(DC_BRUSH));
		}
		else
		{
			SelectObject(hdc, GetStockObject(NULL_BRUSH));
		}
		Rectangle(hdc, top_left.x, top_left.y, bottom_right.x, bottom_right.y);
	}
};

class line : public shape {
private:
	point start;
	point end;

public:
	line(int x1, int y1, int x2, int y2, COLORREF c) : start(x1, y1, c), end(x2, y2, c) {}

	void show(HDC hdc) override {
		HPEN hPen = CreatePen(PS_SOLID, 1, start.color);
		HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
		MoveToEx(hdc, start.x, start.y, NULL);
		LineTo(hdc, end.x, end.y);
		SelectObject(hdc, hOldPen);
		DeleteObject(hPen);
	}
};

class ellipse : public shape {
private:
	int centerX;
	int centerY;
	int radiusX;
	int radiusY;
	COLORREF color;
	bool filled;

public:
	ellipse(int cx, int cy, int rx, int ry, COLORREF c, bool fill = false) : centerX(cx), centerY(cy), radiusX(rx), radiusY(ry), color(c), filled(fill) {}

	void show(HDC hdc) override {
		if (filled) {
			HBRUSH hBrush = CreateSolidBrush(color);
			HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
			Ellipse(hdc, centerX - radiusX, centerY - radiusY, centerX + radiusX, centerY + radiusY);
			SelectObject(hdc, hOldBrush);
			DeleteObject(hBrush);
		}
		else {
			HPEN hPen = CreatePen(PS_SOLID, 1, color);
			HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
			Ellipse(hdc, centerX - radiusX, centerY - radiusY, centerX + radiusX, centerY + radiusY);
			SelectObject(hdc, hOldPen);
			DeleteObject(hPen);
		}
	}
};

class polygon : public shape {
private:
	vector<POINT> vertices;
	COLORREF color;
	bool filled;

public:
	polygon(const vector<POINT>& verts, COLORREF c, bool fill = false) : vertices(verts), color(c), filled(fill) {}

	void show(HDC hdc) override {
		if (filled) {
			HBRUSH hBrush = CreateSolidBrush(color);
			HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);

			int vertexCount = static_cast<int>(vertices.size());
			Polygon(hdc, vertices.data(), vertexCount);

			SelectObject(hdc, hOldBrush);
			DeleteObject(hBrush);
		}
		else {
			HPEN hPen = CreatePen(PS_SOLID, 1, color);
			HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);

			int vertexCount = static_cast<int>(vertices.size());
			Polygon(hdc, vertices.data(), vertexCount);

			SelectObject(hdc, hOldPen);
			DeleteObject(hPen);
		}
	}
};

class canvas
{
	vector<shape*> figures;
	HWND hWnd;
	HDC hdc;
public:
	canvas()
	{
		hWnd = GetConsoleWindow();
		hdc = GetDC(hWnd);
	}
	~canvas()
	{
		ReleaseDC(hWnd, hdc);
	}
	void add(shape* s)
	{
		figures.push_back(s);
	}
	void clear()
	{
		figures.clear();
	}
	void show() {
		for (size_t i = 0; i < figures.size(); i++)
			figures[i]->show(hdc);
	}
};


int main()
{
	canvas c;
	c.add((shape*)new rectangle(10, 10, 100, 100, RGB(255, 0, 0), true, RGB(255, 255, 0)));
	c.add((shape*)new rectangle(150, 100, 200, 250, RGB(0, 0, 255), true, RGB(0, 255, 0)));
	c.add((shape*)new rectangle(10, 100, 200, 150, RGB(255, 100, 0)));
	c.add((shape*)new point(250, 50));
	c.add((shape*)new point(200, 50, RGB(255, 0, 255)));
	c.add((shape*)new line(10, 100, 100, 450, RGB(255, 0, 255)));
	c.add((shape*)new ellipse(150, 150, 80, 50, RGB(239, 129, 129), true));

	vector<POINT> vertices = {
		{100, 100},
		{150, 50},
		{200, 100},
		{175, 150},
		{125, 150}
	};

	c.add((shape*)new polygon(vertices, RGB(0, 0, 255), true));

	c.show();
	_getch();
	return 0;
}