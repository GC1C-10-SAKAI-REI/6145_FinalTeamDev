#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#include <Novice.h>
#include <Vector2.h>
#include "Struct.h"

#include <stdlib.h>
#include <time.h>

const char kWindowTitle[] = "6145_刹ニャのイタズラ";


void DrawSquare(Vec2& center, float rad, unsigned int color);

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	// ライブラリの初期化

	Novice::Initialize(kWindowTitle, WINDOW_WIDTH, WINDOW_HEIGHT);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	unsigned int currentTime = (unsigned int)time(nullptr);
	srand(currentTime);

	int number[5] = { rand() ,rand() ,rand() ,rand() ,rand() };
#pragma region //Objecet


	Object object[5] =
	{
		{{320.0f,320.0f},{0.0f,0.0f},16.0f,0.0f,RED},
		{{480.0f,320.0f},{0.0f,0.0f},16.0f,0.0f,RED},
		{{640.0f,320.0f},{0.0f,0.0f},16.0f,0.0f,RED},
		{{800.0f,320.0f},{0.0f,0.0f},16.0f,0.0f,RED},
		{{960.0f,320.0f},{0.0f,0.0f},16.0f,0.0f,RED}
	};


	int positionFlag[5] = { 1,1,1,1,1 };

	int hpA = 100;
	int hpB = 150;

	int objectHp[5] = { 0 };
	int objectFlag[5] = { 0 };
#pragma endregion


	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0)
	{

		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);




		/// ↓更新処理ここから



		for (int i = 0; i < 5; i++)
		{
			objectHp[i]--;
			//life分け
			if (objectHp[i] <= 0 && number[i] % 2 == 0)
			{
				objectHp[i] = hpA;

			}
			if (objectHp[i] <= 0 && number[i] % 2 == 1)
			{
				objectHp[i] = hpB;

			}

			//軽いものと重いもの分け
			if ((positionFlag[i] == 1) && (objectHp[i] == hpA))
			{
				objectFlag[i] = 0; //軽いもの
				

			}

			if ((positionFlag[i] == 1) && (objectHp[i] == hpB))
			{
				objectFlag[i] = 1; //重いもの
				
			}

			//テスト用
			if (objectFlag[i] == 0 && objectHp[i] == hpA)
			{
				number[i] %= 2;
			}
			if (objectFlag[i] == 1 && objectHp[i] == hpB)
			{
				number[i] %= 2;
			}



		}


		/// ↑更新処理ここまで



		/// ↓描画処理ここから


#pragma region  //テストテキスト
		for (int i = 0; i < 5; i++)
		{
			Novice::ScreenPrintf(20, 40, "hpA %d\n", objectHp[0]);
			Novice::ScreenPrintf(20, 50, "hpB %d\n", objectHp[1]);
			Novice::ScreenPrintf(20, 60, "hpC %d\n", objectHp[2]);
			Novice::ScreenPrintf(20, 70, "hpD %d\n", objectHp[3]);
			Novice::ScreenPrintf(20, 80, "hpE %d\n", objectHp[4]);

		}
#pragma endregion



		for (int i = 0; i < 5; i++)
		{
			if (objectFlag[i] == 0)
			{
				object[i].Color = RED;
				DrawSquare(object[i].Center, object[i].Rad, object[i].Color);

			}
			if (objectFlag[i] == 1)
			{
				object[i].Color = GREEN;
				DrawSquare(object[i].Center, object[i].Rad, object[i].Color);

			}
		}



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
void DrawSquare(Vec2& center, float rad, unsigned int color)
{
	Novice::DrawBox(int(center.X - rad), int(center.Y - rad), int(rad * 2), int(rad * 2), 0, color, kFillModeSolid);
}