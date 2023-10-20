#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#define WORLD_WIDTH 3840

#include <Novice.h>
#include "Struct.h"

const char kWindowTitle[] = "6145_刹ニャのイタズラ";

//矩形描画(単体)
//center : 中心座標、rad : 半径、color : 色
void DrawSquare(Vec2 &center, float rad, unsigned int color);
//矩形描画(複数)
//obj : オブジェクト型の変数、i : for文のi
void DrawSquares(Object *obj,int &i);
//自機とブロックの判定
//obj1 : 自機、obj2 : ブロック,i : for文のi
bool PtoOCollision(Object obj1, Object *obj2,int &i);

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, WINDOW_WIDTH, WINDOW_HEIGHT);

	// キー入力結果を受け取る箱
	char keys[256] = {0};
	char preKeys[256] = {0};
	
	//自機
	Object obj =
	{
		{640,360},
		{0,0},
		32,
		6,
		BLACK
	};
	//true：生きてる false：死んでる
	bool isObjAlive = true;

	//振り向くまでの時間
	int ownerTimer = 0;

	/*ブロック*/
	const int bNum = 2;
	Object block[bNum];

	for (int i = 0; i < bNum; i++)
	{
		block[i].Center.X = float(64 + i * 200);
		block[i].Center.Y = 360.0f;
		block[i].Rad = 64.0f;
		block[i].Spd = 0.0f;
		block[i].Color = WHITE;
	}
	//true：隠れている false：はみ出てる
	bool isHyding[bNum] = { false };
	bool flag = false;

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0)
	{
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		/// ↓更新処理ここから
		
		/*自機の移動処理*/
		obj.Velocity.X = 0;
		
		if (keys[DIK_A])
		{
			obj.Velocity.X = -1;
		}
		if(keys[DIK_D])
		{
			obj.Velocity.X = 1;
		}

		obj.Center.X += obj.Velocity.X * obj.Spd;

		//※テストプレイ用に死亡フラグリセット
		if (keys[DIK_SPACE] && !preKeys[DIK_SPACE])
		{
			isObjAlive = true;
		}

		obj.Color = BLACK;

		//当たり判定
		for (int i = 0; i < bNum; i++)
		{
			if (isObjAlive)
			{
				//タイマー処理
				isHyding[i] = PtoOCollision(obj, block, i);

				if (isHyding[i])
				{
					flag = true;
					break;
				}
				else
				{
					flag = false;
				}
			}
		}
		for (int i = 0; i < bNum; i++)
		{
			if (isObjAlive)
			{
				ownerTimer++;
			}

			if (flag)
			{
				obj.Color = RED;
			}
			else if (!flag && ownerTimer == 495)
			{
				isObjAlive = false;
			}

			if (ownerTimer > 495)
			{
				ownerTimer = 0;
			}
		}
		
		/// ↑更新処理ここまで
		

		
		/// ↓描画処理ここから
		
		//ブロック(隠れる場所)
		for (int i = 0; i < bNum; i++)
		{
			DrawSquares(block, i);
		}
		// 自機
		if (isObjAlive)
		{
			DrawSquare(obj.Center, obj.Rad, obj.Color);
		}

		//デバッグ用
		Novice::ScreenPrintf(0, 0, "timer = %d", ownerTimer / 165);
		Novice::ScreenPrintf(0, 20, "isHyding1 = %d", isHyding[0]);
		Novice::ScreenPrintf(0, 40, "isHyding2 = %d", isHyding[1]);
		Novice::ScreenPrintf(0, 60, "isAlive = %d", isObjAlive);
		Novice::ScreenPrintf(0, 80, "flag = %d", flag);

		/// ↑描画処理ここまで

		// フレームの終了
		Novice::EndFrame();

		// ESCキーが押されたらループを抜ける
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0)
		{
			break;
		}
	}

	// ライブラリの終了
	Novice::Finalize();
	return 0;
}

void DrawSquare(Vec2 &center, float rad, unsigned int color)
{
	Novice::DrawBox(int(center.X - rad), int(center.Y - rad), int(rad * 2), int(rad * 2), 0, color, kFillModeSolid);
}

void DrawSquares(Object* obj, int& i)
{
	Novice::DrawBox(int(obj[i].Center.X - obj[i].Rad), int(obj[i].Center.Y - obj[i].Rad), int(obj[i].Rad * 2), int(obj[i].Rad * 2), 0, obj[i].Color, kFillModeSolid);
}

bool PtoOCollision(Object obj1, Object* obj2, int& i)
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