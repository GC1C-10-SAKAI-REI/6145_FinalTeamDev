#include "FuncLib.h"

void FuncLib::DrawSquare(Vec2& center, float rad, unsigned int color)
{
	Novice::DrawBox(int(center.X - rad), int(center.Y - rad), int(rad * 2), int(rad * 2), 0, color, kFillModeSolid);
}

void FuncLib::DrawSquares(Object* obj, int& i)
{
	Novice::DrawBox(int(obj[i].Center.X - obj[i].Rad), int(obj[i].Center.Y - obj[i].Rad), int(obj[i].Rad * 2), int(obj[i].Rad * 2), 0, obj[i].Color, kFillModeSolid);
}

bool FuncLib::PtoBCollision(Object obj1, Object* obj2, int& i)
{
	if (obj1.Center.X - obj1.Rad > obj2[i].Center.X - obj2[i].Rad && obj1.Center.X + obj1.Rad < obj2[i].Center.X + obj2[i].Rad)
	{
		//上下の判定が欲しくなったら解除してね～
		/*if (obj1.Center.Y - obj1.Rad > obj2[i].Center.Y - obj2[i].Rad && obj1.Center.Y + obj1.Rad < obj2[i].Center.Y + obj2[i].Rad)
		{

		}*/
		return true;
	}
	return false;
}

bool FuncLib::PtoOCollision(Object& obj1, Object& obj2)
{
	if (obj1.Center.X + obj1.Rad > obj2.Center.X - obj2.Rad && obj1.Center.X - obj1.Rad < obj2.Center.X + obj2.Rad)
	{
		//上下の判定が欲しくなったら解除してね～
		/*if (obj1.Center.Y - obj1.Rad > obj2[i].Center.Y - obj2[i].Rad && obj1.Center.Y + obj1.Rad < obj2[i].Center.Y + obj2[i].Rad)
		{

		}*/
		return true;
	}
	return false;
}