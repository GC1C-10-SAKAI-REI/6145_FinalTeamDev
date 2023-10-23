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
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	Object player =
	{
		{200,200},
		{1,1},
		16,
		4,
		WHITE
	};
	//落とす軽いオブジェクト
	Object _object =
	{
		{400,200},
		{0,0},
		16,
		0,
		BLACK
	};
	//落とす重いオブジェクト
	Object object_heavy =
	{
		{600,200},
		{0,0},
		16,
		0,
		RED
	};

	int object_life = 10;

	bool light_colliFlag = false;
	bool heavy_colliFlag = false;

	bool runFlag = false;

	bool attackFlag = false;
	bool heavy_attackFlag = false;

	bool playerAlive = true;
	bool aliveFlag = true;
	bool heavy_aliveFlag = true;

	int respwanTimer = 0;
	int heavy_respwanTimer = 0;

	enum Scene {
		TITLE,
		TUTORIAL,
		GAMEPLAY,
		GAMEOVER,
	};
	int game = TITLE;

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0)
	{
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		/// ↓更新処理ここから

		switch (game)
		{
		case 0://title
			if (keys[DIK_RETURN] && preKeys[DIK_RETURN] == 0)
			{
				game = TUTORIAL;
				playerAlive = true;
				object_life = 10;
			}break;
		case 1://tutorial
			if (keys[DIK_RETURN] && preKeys[DIK_RETURN] == 0)
			{
				game = GAMEPLAY;
			}break;
		case 2://gameplay
			//歩き
			if (playerAlive == true)
			{
				if (keys[DIK_A])
				{
					player.Center.X -= player.Spd * player.Velocity.X;
					//ダッシュ中の処理
					if (keys[DIK_RETURN])
					{
						runFlag = true;
						player.Velocity.X = 2;
					}
					else {
						runFlag = false;
						player.Velocity.X = 1;
					}
				}
				if (keys[DIK_D])
				{
					player.Center.X += player.Spd * player.Velocity.X;
					if (keys[DIK_RETURN])
					{
						runFlag = true;
						player.Velocity.X = 2;
					}
					else {
						runFlag = false;
						player.Velocity.X = 1;
					}
				}

				if (aliveFlag == true)
				{
					//オブジェクトとプレイヤーの当たり判定
					if (_object.Center.X - 16 < player.Center.X + 16 && player.Center.X < _object.Center.X + 32)
					{
						light_colliFlag = true;
						if (runFlag == true)//ダッシュ中にオブジェクトと接触したらオブジェクトが落ちる
						{
							attackFlag = true;
							aliveFlag = false;
						}
					}
					else//そうでなければ落ちない
					{
						light_colliFlag = false;
					}
					//接触中にスペースキーを押すとオブジェクトを落とす
					if (light_colliFlag == true)
					{
						if (keys[DIK_SPACE] && preKeys[DIK_SPACE] == 0)
						{
							attackFlag = true;
							aliveFlag = false;
						}
					}
				}

				if (heavy_aliveFlag == true)
				{
					//ヘビーオブジェクトとプレイヤーの当たり判定
					if (object_heavy.Center.X - 16 < player.Center.X + 16 && player.Center.X < object_heavy.Center.X + 32)
					{
						heavy_colliFlag = true;
						if (runFlag == true)//ダッシュ中にオブジェクトと接触したらオブジェクトが落ちる
						{
							heavy_attackFlag = true;
							object_life -= 1;
						}
					}
					else//そうでなければ落ちない
					{
						heavy_colliFlag = false;
					}

					//ヘビーオブジェクトのライフが０になったら消える
					if (object_life <= 0)
					{
						heavy_aliveFlag = false;
					}
					//接触中にスペースキーを押すとヘビーオブジェクトを落とす
					if (heavy_colliFlag == true)
					{
						if (keys[DIK_SPACE] && preKeys[DIK_SPACE] == 0)
						{
							heavy_attackFlag = true;
							object_life -= 1;
						}
					}
				}

				//オブジェクトが落ちた
				if (attackFlag == true)
				{
					light_colliFlag = false;
					respwanTimer++;
				}

				//ヘビーオブジェクトが落ちた
				if (heavy_attackFlag == true && object_life <= 0)
				{
					heavy_colliFlag = false;
					heavy_respwanTimer++;
				}

				//オブジェクト復活
				if (respwanTimer > 100)
				{
					if (aliveFlag == false)
					{
						aliveFlag = true;
						respwanTimer = 0;
						attackFlag = false;
					}
				}
				if (heavy_respwanTimer > 100)
				{
					if (heavy_aliveFlag == false)
					{
						heavy_aliveFlag = true;
						heavy_attackFlag = false;
						heavy_respwanTimer = 0;
						object_life = 10;
					}
				}
				//デバック用↓
				if (keys[DIK_J] && preKeys[DIK_J] == 0)
				{
					playerAlive = false;
				}
				//デバック用↑

			}
			else 
			{
				game = GAMEOVER;
			}break;
		case 3://gameover
			if (keys[DIK_RETURN] && preKeys[DIK_RETURN] == 0)
			{
				game = TITLE;
			}break;
		}



		/// ↑更新処理ここまで



		/// ↓描画処理ここから
		switch (game)
		{
		case 0:
			Novice::ScreenPrintf(0, 60, "game=%d", game);
			break;
		case 1:
			Novice::ScreenPrintf(0, 60, "game=%d", game);
			break;
		case 2:
			DrawSquare(player.Center, player.Rad, player.Color);
			if (heavy_aliveFlag == true)
			{
				DrawSquare(object_heavy.Center, object_heavy.Rad, object_heavy.Color);
			}
			if (aliveFlag == true)
			{
				DrawSquare(_object.Center, _object.Rad, _object.Color);
			}
			Novice::ScreenPrintf(0, 0, "RunFlag=%d", runFlag);
			Novice::ScreenPrintf(0, 15, "object_life=%d", object_life);
			Novice::ScreenPrintf(0, 30, "heavy_colliFlag=%d", heavy_colliFlag);
			Novice::ScreenPrintf(0, 45, "heavy_respwanTimer=%d", heavy_respwanTimer);
			Novice::ScreenPrintf(0, 60, "game=%d", game);
			break;
		case 3:
			Novice::ScreenPrintf(0, 60, "game=%d", game);
			break;
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
