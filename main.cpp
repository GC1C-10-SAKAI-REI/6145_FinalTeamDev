#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#define WORLD_WIDTH 3840

#include <Novice.h>
#include <stdlib.h>
#include <time.h>
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
	//ランダムシード生成
	unsigned int currentTime = (unsigned int)time(nullptr);
	srand(currentTime);
	//抽選の為の変数
	int number[5] = { 0 };
	
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
	const int lObjNum = 2;
	FallenObj lightObj[lObjNum];
	for (int i = 0; i < lObjNum; i++)
	{
		lightObj[i] = 
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
	}
	lightObj[1].Info.Center.X = 710;
	
	//重い物
	const int hObjNum = 2;
	FallenObj heavyObj[hObjNum];
	for (int i = 0; i < hObjNum; i++)
	{
		heavyObj[i] =
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
	}
	heavyObj[1].Info.Center.X = 1200;
	//重いオブジェクトのライフ
	int hObjLife[hObjNum] = { 3 };
	//連続で当たり判定を発生させないようにするフラグ
	bool breakFlag[hObjNum] = { false };

	/*敵*/
	//振り向くまでの時間
	int ownerTimer = 0;

	/*ブロック*/
	const int bNum = 2;
	Object block[bNum];
	//初期化
	for (int i = 0; i < bNum; i++)
	{
		block[i] =
		{
			{float(64 + i * 200),360.0f},
			{0,0},
			64,
			0,
			WHITE
		};
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
				for (int i = 0; i < hObjNum; i++)
				{
					hObjLife[i] = 3;
				}
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
			for (int i = 0; i < lObjNum; i++)
			{
				if (lightObj[i].IsAlive)
				{
					//オブジェクトとプレイヤーの当たり判定
					lightObj[i].ColFlag = fLib->PtoOCollision(player, lightObj[i].Info);
					
					if (lightObj[i].ColFlag)
					{
						//接触中にスペースキーを押すとオブジェクトを落とす
						if (keys[DIK_SPACE] && !preKeys[DIK_SPACE])
						{
							lightObj[i].IsAlive = false;
						}
						//ダッシュ中にオブジェクトと接触したらオブジェクトが落ちる
						if (runFlag)
						{
							lightObj[i].IsAlive = false;
						}
					}
					else//そうでなければ落ちない
					{
						lightObj[i].ColFlag = false;
					}
					continue;
				}				
			}
			//当たり判定後の処理
			for (int i = 0; i < lObjNum; i++)
			{
				//オブジェクトが落ちた
				if (!lightObj[i].IsAlive)
				{
					lightObj[i].ColFlag = false;
					lightObj[i].ResTimer++;
				}
				//オブジェクト復活
				if (lightObj[i].ResTimer > 100)
				{
					if (!lightObj[i].IsAlive)
					{
						lightObj[i].IsAlive = true;
						lightObj[i].ResTimer = 0;
					}
				}
			}

			/*重い物の処理(担当：ゾ)*/
			for (int i = 0; i < hObjNum; i++)
			{				
				if (heavyObj[i].IsAlive)
				{
					//ヘビーオブジェクトとプレイヤーの当たり判定
					heavyObj[i].ColFlag = fLib->PtoOCollision(player, heavyObj[i].Info);

					if (heavyObj[i].ColFlag)
					{
						//接触中にスペースキーを押すとヘビーオブジェクトを落とす
						if (keys[DIK_SPACE] && preKeys[DIK_SPACE] == 0)
						{
							heavyObj[i].AtkFlag = true;
							hObjLife[i] -= 1;
						}
						//ダッシュ中にオブジェクトと接触したらオブジェクトが落ちる
						if (runFlag && !breakFlag[i])
						{
							heavyObj[i].AtkFlag = true;
							hObjLife[i] -= 1;
							//break;
						}
						breakFlag[i] = true;
					}
					else//そうでなければ落ちない
					{
						heavyObj[i].ColFlag = false;
						breakFlag[i] = false;
					}

					//ヘビーオブジェクトのライフが０になったら消える
					if (hObjLife[i] <= 0)
					{
						heavyObj[i].IsAlive = false;
					}
				}
			}
			for (int i = 0; i < hObjNum; i++)
			{
				//ヘビーオブジェクトが落ちた
				if (heavyObj[i].AtkFlag && hObjLife[i] <= 0)
				{
					heavyObj[i].ColFlag = false;
					heavyObj[i].ResTimer++;
				}

				if (heavyObj[i].ResTimer > 100)
				{
					if (!heavyObj[i].IsAlive)
					{
						heavyObj[i].IsAlive = true;
						heavyObj[i].AtkFlag = false;
						heavyObj[i].ResTimer = 0;
						hObjLife[i] = 3;
					}
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
					isHyding[i] = fLib->PtoBCollision(player, block, i);

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
					//isPAlive = false;
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
			for (int i = 0; i < lObjNum; i++)
			{
				if (lightObj[i].IsAlive)
				{
					fLib->DrawSquare(lightObj[i].Info.Center,lightObj[i].Info.Rad,lightObj[i].Info.Color);
				}
			}			
			//重いオブジェクト
			for (int i = 0; i < hObjNum; i++)
			{
				if (heavyObj[i].IsAlive)
				{
					fLib->DrawSquare(heavyObj[i].Info.Center, heavyObj[i].Info.Rad, heavyObj[i].Info.Color);
				}
			}
			

			//デバッグ用
			Novice::ScreenPrintf(0, 0, "timer = %d", ownerTimer / 165);
			Novice::ScreenPrintf(0, 20, "isAlive = %d", isPAlive);
			Novice::ScreenPrintf(0, 40, "colFlag0 = %d", hObjLife[0]);
			Novice::ScreenPrintf(0, 60, "colFlag1 = %d", hObjLife[1]);

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