#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
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
	Object player;
	Object _object;//落とすオブジェクト
	//Object object_heavy;

	//(動作確認用の情報)
	player.Center.X = 200;
	player.Center.Y = 200;
	player.Rad = 16;
	player.Color = WHITE;
	player.Spd = 4;
	player.Velocity.X = 1;

	_object.Center.X = 400;
	_object.Center.Y = 220;
	_object.Rad = 16;
	_object.Color = BLACK;
	_object.Spd = 4;

	//object_heavy.Center.X = 600;
	//object_heavy.Center.Y = 220;
	//object_heavy.Rad = 16;
	//object_heavy.Color = BLACK;
	//object_heavy.Spd = 4;
	
	//int object_life = 3;
	int color = WHITE;
	bool testFlag = false;
	bool runFlag = false;
	bool hitFlag = false;
	bool aliveFlag = true;
	int respwanTimer = 0;

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0)
	{
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		/// ↓更新処理ここから


		//歩き
		if (keys[DIK_A])
		{
			player.Center.X -= player.Spd;
		}
		if (keys[DIK_D])
		{
			player.Center.X += player.Spd;
		}

		//ダッシュ
		if (keys[DIK_RETURN])
		{
			runFlag = true;
			player.Velocity.X = 2;
			if (keys[DIK_A])
			{
				player.Center.X -= player.Spd * player.Velocity.X;
			}
			if (keys[DIK_D])
			{
				player.Center.X += player.Spd * player.Velocity.X;
			}
		}
		else {
			runFlag = false;
		}
		　
		//オブジェクトとプレイヤーの当たり判定
		if (_object.Center.X - 16 < player.Center.X + 16 && player.Center.X < _object.Center.X + 32)
		{
			testFlag = true;
			if (runFlag == true)//ダッシュ中にオブジェクトと接触したらオブジェクトが落ちる
			{
				color = BLACK;
				hitFlag = true;
				aliveFlag = false;
			}
		}
		else//そうでなければ落ちない
		{
			testFlag = false;
		}

		//接触中にスペースキーを押すとオブジェクトを落とす
		if (testFlag == true)
		{
			if (keys[DIK_SPACE] && preKeys[DIK_SPACE] == 0)
			{
				color = BLACK;
				hitFlag = true;
				aliveFlag = false;
			}
		}
		else//そうでなければ落ちない
		{
			color = WHITE;
		}
		
		//オブジェクトが落ちた
		if (hitFlag == true)
		{
			testFlag = false;
			respwanTimer++;
		}

		//オブジェクト復活
		if (respwanTimer > 100)
		{
			aliveFlag = true;
			respwanTimer = 0;
			hitFlag = false;
		}
		/// ↑更新処理ここまで
		

		
		/// ↓描画処理ここから
		DrawSquare(player.Center, player.Rad, player.Color);
		if (aliveFlag == true)
		{
			DrawSquare(_object.Center, _object.Rad, _object.Color);
		}
		Novice::ScreenPrintf(0, 0, "RunFlag=%d", runFlag);
		Novice::ScreenPrintf(0, 15, "testFlag=%d", testFlag);

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