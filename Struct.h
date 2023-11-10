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
	int IsAlive;//0：死んでる 1：生きてる 2：落ちる演出中
	int ResTimer;
	bool WeightFlag;//false：軽い物 true：重い物
	int Hp;
}FallenObj;