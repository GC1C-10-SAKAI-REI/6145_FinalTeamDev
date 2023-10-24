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
	bool isPAlive = true;
	//走っているフラグ
	bool runFlag = false;

	/*プレイヤーが落とすオブジェクト*/
	//軽い物
	FallenObj lightObj =
	{
		{
			{640,360},
			{0,0},
			32,
			0,
			GREEN
		},
		false,
		false,
		true,
		0
	};
	//重い物
	FallenObj heavyObj =
	{
		{
			{1000,360},
			{0,0},
			32,
			0,
			0xCC00CCFF
		},
		false,
		false,
		true,
		0
	};
	//重いオブジェクトのライフ
	int hObjLife = 3;
	//連続で当たり判定を発生させないようにするフラグ
	bool breakFlag = false;

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

			if (keys[DIK_RETURN] && !preKeys[DIK_RETURN])
			{
				scene = TUTORIAL;
				isPAlive = true;
				hObjLife = 3;
			}

			break;

		case TUTORIAL:

			if (keys[DIK_RETURN] && !preKeys[DIK_RETURN])
			{
				scene = GAMEPLAY;
			}

			break;

		case GAMEPLAY:

			/*自機の移動処理(担当：ゾ)*/
			player.Velocity.X = 0;
			//左方向
			if (keys[DIK_A])
			{
				player.Velocity.X = -1;
				//ダッシュ中の処理
				if (keys[DIK_RETURN])
				{
					runFlag = true;
					player.Velocity.X = -2;
				}
				else
				{
					runFlag = false;
					player.Velocity.X = -1;
				}
			}
			//右方向
			if (keys[DIK_D])
			{
				player.Velocity.X = 1;
				//ダッシュ中の処理
				if (keys[DIK_RETURN])
				{
					runFlag = true;
					player.Velocity.X = 2;
				}
				else
				{
					runFlag = false;
					player.Velocity.X = 1;
				}
			}

			player.Center.X += player.Velocity.X * player.Spd;
		
			/*軽い物の処理(担当：ゾ)*/
			if (lightObj.IsAlive)
			{			
				//オブジェクトとプレイヤーの当たり判定
				if (lightObj.Info.Center.X - lightObj.Info.Rad < player.Center.X + player.Rad && player.Center.X - player.Rad < lightObj.Info.Center.X + lightObj.Info.Rad)
				{					
					lightObj.ColFlag = true;
					//ダッシュ中にオブジェクトと接触したらオブジェクトが落ちる
					if (runFlag)
					{
						lightObj.IsAlive = false;
					}
				}
				else//そうでなければ落ちない
				{
					lightObj.ColFlag = false;
				}
				//接触中にスペースキーを押すとオブジェクトを落とす
				if (lightObj.ColFlag)
				{
					if (keys[DIK_SPACE] && !preKeys[DIK_SPACE])
					{
						lightObj.IsAlive = false;
					}
				}
			}
			//オブジェクトが落ちた
			if (!lightObj.IsAlive)
			{
				lightObj.ColFlag = false;
				lightObj.ResTimer++;
			}
			//オブジェクト復活
			if (lightObj.ResTimer > 100)
			{
				if (!lightObj.IsAlive)
				{
					lightObj.IsAlive = true;
					lightObj.ResTimer = 0;
				}
			}

			/*重い物の処理(担当：ゾ)*/
			if (heavyObj.IsAlive)
			{
				//ヘビーオブジェクトとプレイヤーの当たり判定
				if (heavyObj.Info.Center.X - heavyObj.Info.Rad < player.Center.X + player.Rad && player.Center.X - player.Rad < heavyObj.Info.Center.X + heavyObj.Info.Rad)
				{
					heavyObj.ColFlag = true;
					if (runFlag && !breakFlag)//ダッシュ中にオブジェクトと接触したらオブジェクトが落ちる
					{
						heavyObj.AtkFlag = true;
						hObjLife -= 1;
					}
					breakFlag = true;
				}
				else//そうでなければ落ちない
				{
					heavyObj.ColFlag = false;
					breakFlag = false;
				}

				//ヘビーオブジェクトのライフが０になったら消える
				if (hObjLife <= 0)
				{
					heavyObj.IsAlive = false;
				}
				//接触中にスペースキーを押すとヘビーオブジェクトを落とす
				if (heavyObj.ColFlag)
				{
					if (keys[DIK_SPACE] && preKeys[DIK_SPACE] == 0)
					{
						heavyObj.AtkFlag = true;
						hObjLife -= 1;
					}
				}
			}	

			//ヘビーオブジェクトが落ちた
			if (heavyObj.AtkFlag && hObjLife <= 0)
			{
				heavyObj.ColFlag = false;
				heavyObj.ResTimer++;
			}
			
			if (heavyObj.ResTimer > 100)
			{
				if (!heavyObj.IsAlive)
				{
					heavyObj.IsAlive = true;
					heavyObj.AtkFlag = false;
					heavyObj.ResTimer = 0;
					hObjLife = 3;
				}
			}

			//※テストプレイ用に死亡フラグリセット
			if (keys[DIK_R] && !preKeys[DIK_R])
			{
				isPAlive = true;
			}

			//隠れていない時に黒くする
			player.Color = BLACK;

			//当たり判定
			for (int i = 0; i < bNum; i++)
			{
				if (isPAlive)
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
				if (isPAlive)
				{
					ownerTimer++;
				}
				
				if (safeFlag)
				{
					player.Color = RED;
				}
				else if (!safeFlag && ownerTimer == 495)
				{
					//プレイヤーの死亡処理
					isPAlive = false;
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
			if (isPAlive)
			{
				fLib->DrawSquare(player.Center, player.Rad, player.Color);
			}
			//軽いオブジェクト
			if (lightObj.IsAlive)
			{
				fLib->DrawSquare(lightObj.Info.Center, lightObj.Info.Rad, lightObj.Info.Color);
			}
			//重いオブジェクト
			if (heavyObj.IsAlive)
			{
				fLib->DrawSquare(heavyObj.Info.Center, heavyObj.Info.Rad, heavyObj.Info.Color);
			}

			//デバッグ用
			Novice::ScreenPrintf(0, 0, "timer = %d", ownerTimer / 165);
			Novice::ScreenPrintf(0, 20, "isAlive = %d", isPAlive);

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