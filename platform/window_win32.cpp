// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <platform/window.h>
#include <lib/exception.h>

#define NOMINMAX
#define UNICODE
#include <windows.h>

using namespace ionengine;
using namespace ionengine::platform;

struct Window::Impl {

	WindowLoop* loop;

	HWND hwnd;

	Size _client_size;

	void initialize(std::u8string const& label, uint32_t const width, uint32_t const height, bool const fullscreen, WindowLoop& loop);

	void deinitialize();

	Size client_size() const;

    void* native_handle() const;

    void label(std::u8string const& label);

	static LRESULT wnd_proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

void Window::impl_deleter::operator()(Impl* ptr) const {

	delete ptr;
}

//===========================================================
//
//
//          Window Implementation
//
//
//===========================================================

void Window::Impl::initialize(std::u8string const& label, uint32_t const width, uint32_t const height, bool const fullscreen, WindowLoop& loop) {

	this->loop = &loop;

    WNDCLASS wnd_class{};
	wnd_class.lpszClassName = TEXT("IONENGINE_WINDOW");
	wnd_class.hInstance = GetModuleHandle(nullptr);
	wnd_class.lpfnWndProc = Impl::wnd_proc;
	wnd_class.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));

    if (!RegisterClass(&wnd_class)) {
		throw lib::Exception(u8"An error occurred while registering the window");
    }

	size_t length = strlen(reinterpret_cast<const char*>(label.c_str())) + 1;
    size_t result = 0;
	
	std::u16string out_str(length - 1, 0);

    mbstowcs_s(&result, reinterpret_cast<wchar_t*>(out_str.data()), length, reinterpret_cast<const char*>(label.c_str()), length - 1);

    hwnd = CreateWindow(
		wnd_class.lpszClassName,
		reinterpret_cast<const wchar_t*>(out_str.c_str()),
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,
		0, 0,
		width, height,
		nullptr, 
		nullptr,
		wnd_class.hInstance,
		nullptr
	);

	if (!hwnd) {
		throw lib::Exception(u8"An error occurred while creating the window");
	}

    SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
	ShowWindow(hwnd, SW_SHOWDEFAULT);
}

void Window::Impl::deinitialize() {

	DestroyWindow(hwnd);
}

Size Window::Impl::client_size() const {

	return _client_size;
}

void* Window::Impl::native_handle() const {

	return reinterpret_cast<void*>(hwnd);
}

void Window::Impl::label(std::u8string const& label) {

	size_t length = strlen(reinterpret_cast<const char*>(label.c_str())) + 1;
    size_t result = 0;

	std::u16string out_str(length - 1, 0);

    mbstowcs_s(&result, reinterpret_cast<wchar_t*>(out_str.data()), length, reinterpret_cast<const char*>(label.c_str()), length - 1);

	SetWindowText(hwnd, reinterpret_cast<const wchar_t*>(out_str.c_str()));
}

LRESULT Window::Impl::wnd_proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	auto window_impl = reinterpret_cast<Impl*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	
	if(!window_impl) {
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	switch(msg) {
		case WM_CLOSE: {
			window_impl->loop->_events.emplace(WindowEventType::Closed);
		} break;

		case WM_SIZE: {
			if(wParam & SIZE_MINIMIZED) {
				break;
			}

			WORD width = LOWORD(lParam);
			WORD height = HIWORD(lParam);

			RECT style_rect{};
			AdjustWindowRect(&style_rect, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX, false);
			
			LONG style_width = style_rect.right - style_rect.left;
			LONG style_height = style_rect.bottom - style_rect.top;

			window_impl->_client_size = Size { std::max<uint32_t>(1, width - style_width), std::max<uint32_t>(1, height - style_height) };

			window_impl->loop->_events.emplace(WindowEventType::Sized, window_impl->_client_size);
		} break;
	}
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

//===========================================================
//
//
//          Window Pointer to Implementation
//
//
//===========================================================

Window::Window(std::u8string const& label, uint32_t const width, uint32_t const height, bool const fullscreen, WindowLoop& loop) :
	_impl(std::unique_ptr<Impl, impl_deleter>(new Impl())) {

	_impl->initialize(label, width, height, fullscreen, loop);
}

Window::~Window() {

	_impl->deinitialize();
}

Size Window::client_size() const {
	
	return _impl->client_size();
}

void* Window::native_handle() const {

	return _impl->native_handle();
}

void Window::label(std::u8string const& label) {

	_impl->label(label);
}
