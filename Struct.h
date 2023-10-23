#pragma once

typedef struct Vec2
{
	float X;
	float Y;
} Vec2;

typedef struct Object
{
	Vec2 Center;
	Vec2 Velocity;
	float Rad;
	float Spd;
	unsigned int Color;
} Object;

typedef struct fallenObj
{
	Object Info;
	bool ColFlag;
	bool AtackFlag;
	bool IsAlive;
	int ResTimer;
}fallenObj;