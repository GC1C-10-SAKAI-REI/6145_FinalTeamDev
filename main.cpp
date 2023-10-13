#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#define WORLD_WIDTH 3840

#include <Novice.h>
#include "Struct.h"

const char kWindowTitle[] = "6145_刹ニャのイタズラ";

void DrawSquare(Vec2& center, float rad, unsigned int color);

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

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0)
	{
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		/// ↓更新処理ここから
		
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
		
		/// ↑更新処理ここまで
		

		
		/// ↓描画処理ここから
		
		DrawSquare(obj.Center, obj.Rad, obj.Color);
		
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