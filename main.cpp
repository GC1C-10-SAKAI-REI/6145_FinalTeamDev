#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
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
	//スコア用
	int score0 = 0;
	int score10 = 0;
	int score100 = 0;
	int score1000 = 0;
	
	//テスト用
	int hp = 10;

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0)
	{
		// フレームの開始
		Novice::BeginFrame();
		
		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		/// ↓更新処理ここから
		if (keys[DIK_SPACE] && preKeys[DIK_SPACE])
		{
			hp--;
		}

		if (hp <= 0)
		{
			score0 += 1;
			hp = 10;
		}

		if (score0 >9)
		{
			score10 += 1;
			score0 = 0;
		}
		if (score10 > 9)
		{
			score100 += 1;
			score10 = 0;
		}
		if (score100 > 9)
		{
			score1000 += 1;
			score100 = 0;
		}
		
		if (keys[DIK_R] && preKeys[DIK_R])
		{
			score0 = 0;
			score10 = 0;
			score100 = 0;
			score1000 = 0;
			hp = 10;
		}


		/// ↑更新処理ここまで
		

		
		/// ↓描画処理ここから

		Novice::ScreenPrintf(20, 20, "hp %d", hp);

		Novice::ScreenPrintf(0, 0, "%d%d%d%d", score1000, score100, score10, score0);

		/*Novice::ScreenPrintf(20, 40, "Score0 %d", score0);
		Novice::ScreenPrintf(20, 60, "Score10 %d", score10);
		Novice::ScreenPrintf(20, 80, "Score100 %d", score100);
		Novice::ScreenPrintf(20, 100, "Score1000 %d", score1000);*/

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
