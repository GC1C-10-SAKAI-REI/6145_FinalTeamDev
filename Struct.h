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

typedef struct FallenObj
{
	Object Info;
	bool ColFlag;
	bool AtkFlag;
	bool IsAlive;
	int ResTimer;
	bool weightFlag;//falseÅFåyÇ¢ï® trueÅFèdÇ¢ï®
}FallenObj;