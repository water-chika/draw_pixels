#include "draw_pixels.hpp"

#include "draw_pixels.hpp"
#include <Windows.h>
#include <cassert>
#include <vector>

struct draw_pixels_data {
	uint32_t width;
	uint32_t height;
	uint32_t* data;
	uint32_t scale;
};

static BOOL DrawBitmap(HDC hDC, INT x, INT y, HBITMAP hBitmap, DWORD dwROP)
{
	HDC       hDCBits;
	BITMAP    Bitmap;
	BOOL      bResult;

	if (!hDC || !hBitmap)
		return FALSE;

	hDCBits = CreateCompatibleDC(hDC);
	GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&Bitmap);
	SelectObject(hDCBits, hBitmap);
	bResult = BitBlt(hDC, x, y, Bitmap.bmWidth, Bitmap.bmHeight, hDCBits, 0, 0, dwROP);
	DeleteDC(hDCBits);

	return bResult;
}

static LRESULT CALLBACK window_process(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	static draw_pixels_data* draw_data;
	switch (uMsg)
	{
	case WM_CREATE:
	{
		auto create_struct = reinterpret_cast<CREATESTRUCT*>(lParam);
		draw_data = reinterpret_cast<draw_pixels_data * >(create_struct->lpCreateParams);
		break;
	}
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);

		std::vector<uint32_t> scaled_draw_data(draw_data->width * draw_data->scale * draw_data->height * draw_data->scale);
		for (uint32_t y = 0; y < draw_data->height; y++) {
			for (uint32_t x = 0; x < draw_data->width; x++) {
				for (uint32_t scale_x = 0; scale_x < draw_data->scale; scale_x++) {
					for (uint32_t scale_y = 0; scale_y < draw_data->scale; scale_y++) {
						scaled_draw_data[(y  * draw_data->scale + scale_y)* draw_data->width *draw_data->scale + (x * draw_data->scale+ scale_x)] = draw_data->data[y * draw_data->width + x];
					}
				}
			}
		}

		HBITMAP bitmap = CreateBitmap(draw_data->width*draw_data->scale, draw_data->height*draw_data->scale, 1, 32, scaled_draw_data.data());
		assert(bitmap);

		DrawBitmap(hdc, 0, 0, bitmap, SRCCOPY);

		EndPaint(hwnd, &ps);
	}
	return 0;
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		break;
	}
	}
	return DefWindowProcA(hwnd, uMsg, wParam, lParam);
}

void water::draw_pixels(uint32_t width, uint32_t height, uint32_t* data, uint32_t scale)
{
	const char* window_class_name = "draw_pixels";
	WNDCLASS window_class{};
	window_class.hInstance = GetModuleHandle(NULL);
	window_class.lpszClassName = window_class_name;
	window_class.lpfnWndProc = window_process;
	RegisterClass(&window_class);
	draw_pixels_data draw_data{ width, height, data, scale};
	RECT rect{0, 0,long(width*scale), long(height*scale)};
	auto window_style = WS_OVERLAPPEDWINDOW;
	assert(window_style != WS_OVERLAPPED);
	BOOL adjust_res = AdjustWindowRect(&rect, window_style, FALSE);
	assert(adjust_res);
	HWND hwnd = CreateWindowA(window_class_name, "draw_pixels", window_style, CW_USEDEFAULT, CW_USEDEFAULT,
		rect.right - rect.left, rect.bottom - rect.top, NULL, NULL, GetModuleHandle(NULL), &draw_data);
	assert(hwnd);
	ShowWindow(hwnd, SW_NORMAL);

	MSG msg = { };
	while (GetMessage(&msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}
