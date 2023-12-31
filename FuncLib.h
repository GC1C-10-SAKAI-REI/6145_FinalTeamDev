#pragma once
#include "Struct.h"
#include "Novice.h"

class FuncLib
{
public:
	//矩形描画(単体)
	//center : 中心座標、rad : 半径、color : 色
	void DrawSquare(Vec2& center, float rad, unsigned int color);

	//矩形描画(複数)
	//obj : オブジェクト型の変数、i : for文のi
	void DrawSquares(Object* obj, int& i);

	//自機とブロック(隠れる場所)の判定
	//obj1 : 自機、obj2 : ブロック,i : for文のi
	bool PtoBCollision(Object obj1, Object* obj2, int& i);

	//自機と落とすオブジェクトの判定
	//obj1 : 自機、obj2 : 落とすオブジェクト,i : for文のi
	bool PtoOCollision(Object& obj1, Object& obj2);

	//シーン遷移用の演出(徐々に暗転)
	bool SceneEnd(unsigned int &c);

	//シーン遷移用の演出(徐々に明転)
	bool SceneStart(unsigned int &c);
};