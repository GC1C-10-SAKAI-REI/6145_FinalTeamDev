#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#define WORLD_WIDTH 3840

#include <Novice.h>
#include "Struct.h"

const char kWindowTitle[] = "6145_刹ニャのイタズラ";

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, WINDOW_WIDTH, WINDOW_HEIGHT);

	// キー入力結果を受け取る箱
	char keys[256] = {0};
	char preKeys[256] = {0};

	unsigned int color = 0xFF000000;
	int cFlag = 0;
	int endFlag = 0;

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0)
	{
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		/// ↓更新処理ここから

		if (endFlag <= 2)
		{
			if (cFlag % 2 == 0)
			{
				color += 5;
			}
			else if (cFlag % 2 == 1)
			{
				color -= 5;
			}

			if (color >= 0xFF00008F)
			{
				cFlag++;
			}
			else if (color <= 0xFF000000)
			{
				cFlag--;
				endFlag++;
			}
		}		
		
		/// ↑更新処理ここまで
		

		
		/// ↓描画処理ここから
		
		Novice::DrawBox(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, color, kFillModeSolid);
		Novice::ScreenPrintf(0, 0, "cFlag = %d", cFlag);

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