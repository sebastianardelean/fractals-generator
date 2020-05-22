#pragma once

#pragma comment(lib, "user32.lib")		// Visual Studio Only
#pragma comment(lib, "gdi32.lib")		// For other Windows Compilers please add
#pragma comment(lib, "opengl32.lib")	// these libs to your linker input
#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "winmm.lib")
#define UNICODE
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <windows.h>

#include <iostream>
#include <cstdint>
#include <memory>
#include <complex>
#include <thread>
#include <sstream>
#include <chrono>
#include <tuple>
#include <condition_variable>
#include <mutex>
#include <atomic>
#include <functional>
#include <immintrin.h>