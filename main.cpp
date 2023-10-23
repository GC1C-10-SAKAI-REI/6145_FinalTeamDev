#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#define WORLD_WIDTH 3840

#include <Novice.h>
#include "FuncLib.h"

const char kWindowTitle[] = "6145_刹ニャのイタズラ";

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, WINDOW_WIDTH, WINDOW_HEIGHT);

	// キー入力結果を受け取る箱
	char keys[256] = {0};
	char preKeys[256] = {0};

	//自作関数クラス
	FuncLib* fLib = new FuncLib();
	
	//自機
	Object player =
	{
		{640,360},
		{0,0},
		32,
		6,
		BLACK
	};
	//true：生きてる false：死んでる
	bool isObjAlive = true;

	//オブジェクトのライフ
	int object_life = 10;

	//自機と当たっているかのフラグ
	bool light_colliFlag = false;
	bool heavy_colliFlag = false;
	//走っているフラグ
	bool runFlag = false;
	//スペースを押したかどうか
	bool attackFlag = false;
	bool heavy_attackFlag = false;

	//各生存フラグ
	bool playerAlive = true;
	bool aliveFlag = true;
	bool heavy_aliveFlag = true;

	//再出現させるタイマー
	int respwanTimer = 0;
	int heavy_respwanTimer = 0;

	/*敵*/
	//振り向くまでの時間
	int ownerTimer = 0;

	/*ブロック*/
	const int bNum = 2;
	Object block[bNum];
	//初期化
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
	// オブジェクトの一つどれかに隠れてたらtrue
	bool safeFlag = false;

	//シーン切り替え用列挙体
	enum Scene
	{
		TITLE,   
		TUTORIAL,
		GAMEPLAY,
		GAMEOVER,
	};
	//switch用変数
	Scene scene = TITLE;

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0)
	{
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		/// ↓更新処理ここから

		switch (scene)
		{
		case TITLE:

			break;

		case TUTORIAL:
			break;

		case GAMEPLAY:

			/*自機の移動処理*/
			player.Velocity.X = 0;

			if (keys[DIK_A])
			{
				player.Velocity.X = -1;
			}
			if (keys[DIK_D])
			{
				player.Velocity.X = 1;
			}

			player.Center.X += player.Velocity.X * player.Spd;

			//※テストプレイ用に死亡フラグリセット
			if (keys[DIK_SPACE] && !preKeys[DIK_SPACE])
			{
				isObjAlive = true;
			}

			//隠れていない時に黒くする
			player.Color = BLACK;

			//当たり判定
			for (int i = 0; i < bNum; i++)
			{
				if (isObjAlive)
				{
					isHyding[i] = fLib->PtoOCollision(player, block, i);

					//もし隠れていたら
					if (isHyding[i])
					{
						safeFlag = true;
						break;
					}
					else
					{
						safeFlag = false;
					}
				}
			}
			//プレイヤーの死亡条件
			for (int i = 0; i < bNum; i++)
			{
				//タイマー処理
				if (isObjAlive)
				{
					ownerTimer++;
				}

				if (safeFlag)
				{
					player.Color = RED;
				}
				else if (!safeFlag && ownerTimer == 495)
				{
					isObjAlive = false;
				}

				if (ownerTimer > 495)
				{
					ownerTimer = 0;
				}
			}

			break;

		case GAMEOVER:
			break;
		}
		
		
		
		/// ↑更新処理ここまで
		

		
		/// ↓描画処理ここから

		switch (scene)
		{
		case TITLE:



			break;

		case TUTORIAL:



			break;

		case GAMEPLAY:

			//ブロック(隠れる場所)
			for (int i = 0; i < bNum; i++)
			{
				fLib->DrawSquares(block, i);
			}
			// 自機
			if (isObjAlive)
			{
				fLib->DrawSquare(player.Center, player.Rad, player.Color);
			}

			//デバッグ用
			Novice::ScreenPrintf(0, 0, "timer = %d", ownerTimer / 165);
			Novice::ScreenPrintf(0, 20, "isAlive = %d", isObjAlive);

			break;

		case GAMEOVER:
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