// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine {

enum class KeyCode : uint32 {
	Q = 81,
	W = 87,
	E = 69,
	R = 82,
	T = 84,
	Y = 89,
	U = 85,
	I = 73,
	O = 79,
	P = 80,
	A = 65,
	S = 83,
	D = 68,
	F = 70,
	G = 71,
	H = 72,
	J = 74,
	K = 75,
	L = 76,
	Z = 90,
	X = 88,
	C = 67,
	V = 86,
	B = 66,
	N = 78,
	M = 77,
	Space = 32,
	Return = 13,
	Escape = 27,
	Ctrl = 17,
	Alt = 18,
	Shift = 16,
	Capslock = 20,
	Tab = 9,
	Tilde = 192,
	Alpha0 = 48,
	Alpha1 = 49,
	Alpha2 = 50,
	Alpha3 = 51,
	Alpha4 = 52,
	Alpha5 = 53,
	Alpha6 = 54,
	Alpha7 = 55,
	Alpha8 = 56,
	Alpha9 = 57,
	Minus = 189,
	Plus = 187,
	Backspace = 8,
	Win = 91,
	F1 = 112,
	F2 = 113,
	F3 = 114,
	F4 = 115,
	F5 = 116,
	F6 = 117,
	F7 = 118,
	F8 = 119,
	F9 = 120,
	F10 = 121,
	F11 = 122,
	F12 = 123,
	Insert = 45,
	Home = 36,
	End = 35,
	Delete = 46,
	PageUp = 33,
	PageDown = 34,
	BracketLeft = 219,
	BracketRight = 221,
	Question = 191,
	Greater = 190,
	Less = 188,
	Colon = 186,
	Period = 222,
	Slash = 220,
	Print = 255,

	MouseLeft = 1,
	MouseRight = 2,
	MouseMiddle = 3,
	MouseFour = 4,
	MouseFive = 5,

	GamepadA = 301,
	GamepadB = 302,
	GamepadX = 303,
	GamepadY = 304,
	DPadUp = 305,
	DPadDown = 306,
	DPadLeft = 307,
	DPadRight = 308,
	GamepadStart = 309,
	GamepadBack = 310,
	ThumbLeft = 311,
	ThumbRight = 312,
	ShoulderLeft = 313,
	ShoulderRight = 314
};

enum class AxisCode : uint32 {
	MouseX,
	MouseY,
	MouseWheel,
	ThumbLeftX,
	ThumbLeftY,
	ThumbRightX,
	ThumbRightY,
	TriggerLeft,
	TriggerRight
};

constexpr uint32 key_codes_max = 400;
constexpr uint32 axis_codes_max = 10;

}