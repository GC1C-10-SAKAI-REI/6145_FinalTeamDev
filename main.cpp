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
	const int remainObj = 5;
	FallenObj obj[remainObj];
	for (int i = 0; i < remainObj; i++)
	{
		obj[i] = 
		{
			{
				{float(120 + (120 * i)),360},
				{0,0},
				32,
				0,
				0x00000000
			},
			false,
			false,
			true,
			0,
			false,
			1
		};
	}
	//連続で当たり判定を発生させないようにするフラグ
	bool breakFlag[remainObj] = { false };
	//軽いオブジェクトのHP
	const int lightObjHp = 1;
	//重いオブジェクトのHP
	const int heavyObjHp = 3;

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
				for (int i = 0; i < remainObj; i++)
				{
					obj[i].Hp = 1;
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
			if (keys[DIK_A] || keys[DIK_LEFT])
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
			if (keys[DIK_D] || keys[DIK_RIGHT])
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
			//ベクトルとスピードを掛け合わせる
			player.Center.X += player.Velocity.X * player.Spd;

			/*オブジェクトのランダム生成(担当：リュウ)*/
			for (int i = 0; i < 5; i++)
			{
				//life分け
				if (obj[i].IsAlive && obj[i].Hp <= 0)
				{
					number[i] = rand();
					if (number[i] % 2 == 0)
					{
						obj[i].WeightFlag = false;
						obj[i].Hp = lightObjHp;
					}
					if (number[i] % 2 == 1)
					{
						obj[i].WeightFlag = true;
						obj[i].Hp = heavyObjHp;
					}
				}
			}
			
			/*軽い物の処理(担当：ゾ)*/
			for (int i = 0; i < remainObj; i++)
			{
				if (!obj[i].WeightFlag)
				{
					if (obj[i].IsAlive)
					{
						//オブジェクトとプレイヤーの当たり判定
						obj[i].ColFlag = fLib->PtoOCollision(player, obj[i].Info);

						if (obj[i].ColFlag)
						{
							//接触中にスペースキーを押すとオブジェクトを落とす
							if (keys[DIK_SPACE] && !preKeys[DIK_SPACE])
							{
								obj[i].Hp -= 1;								
							}
							//ダッシュ中にオブジェクトと接触したらオブジェクトが落ちる
							if (runFlag)
							{
								obj[i].Hp -= 1;
							}
						}
						else//そうでなければ落ちない
						{
							obj[i].ColFlag = false;
						}

						if (obj[i].Hp < 1)
						{
							obj[i].IsAlive = false;
						}
						continue;
					}
				}
			}
			//当たり判定後の処理
			for (int i = 0; i < remainObj; i++)
			{
				if (!obj[i].WeightFlag)
				{
					//オブジェクトが落ちた
					if (!obj[i].IsAlive)
					{
						obj[i].ColFlag = false;
						obj[i].ResTimer++;
					}
					//オブジェクト復活
					if (obj[i].ResTimer > 100)
					{
						if (!obj[i].IsAlive)
						{
							obj[i].IsAlive = true;
							obj[i].ResTimer = 0;
						}
					}
				}				
			}

			/*重い物の処理(担当：ゾ)*/
			for (int i = 0; i < remainObj; i++)
			{
				if (obj[i].WeightFlag)
				{
					if (obj[i].IsAlive)
					{
						//ヘビーオブジェクトとプレイヤーの当たり判定
						obj[i].ColFlag = fLib->PtoOCollision(player, obj[i].Info);

						if (obj[i].ColFlag)
						{
							//接触中にスペースキーを押すとヘビーオブジェクトを落とす
							if (keys[DIK_SPACE] && preKeys[DIK_SPACE] == 0)
							{
								obj[i].AtkFlag = true;
								obj[i].Hp -= 1;
							}
							//ダッシュ中にオブジェクトと接触したらオブジェクトが落ちる
							if (runFlag && !breakFlag[i])
							{
								obj[i].AtkFlag = true;
								obj[i].Hp -= 1;
								//break;
							}
							breakFlag[i] = true;
						}
						else//そうでなければ落ちない
						{
							obj[i].ColFlag = false;
							breakFlag[i] = false;
						}

						//ヘビーオブジェクトのライフが０になったら消える
						if (obj[i].Hp <= 0)
						{
							obj[i].IsAlive = false;
						}
					}
				}
			}
			//当たり判定後の処理
			for (int i = 0; i < remainObj; i++)
			{
				if (obj[i].WeightFlag)
				{
					//ヘビーオブジェクトが落ちた
					if (obj[i].AtkFlag && obj[i].Hp <= 0)
					{
						obj[i].ColFlag = false;
						obj[i].ResTimer++;
					}

					if (obj[i].ResTimer > 100)
					{
						if (!obj[i].IsAlive)
						{
							obj[i].IsAlive = true;
							obj[i].AtkFlag = false;
							obj[i].ResTimer = 0;
						}
					}
				}
			}
			//objcet再生成
			for (int i = 0; i < remainObj; i++)
			{
				if (obj[i].Hp <= 0)
				{
					obj[i].IsAlive = false;
					obj[i].ResTimer++;
				}
				if (obj[i].ResTimer >= 100)
				{
					obj[i].IsAlive = true;
					obj[i].ResTimer = 0;
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
			for (int i = 0; i < remainObj; i++)
			{
				if (obj[i].IsAlive && !obj[i].WeightFlag)
				{
					obj[i].Info.Color = GREEN;

					fLib->DrawSquare(obj[i].Info.Center,obj[i].Info.Rad,obj[i].Info.Color);
				}
			}			
			//重いオブジェクト
			for (int i = 0; i < remainObj; i++)
			{
				if (obj[i].IsAlive && obj[i].WeightFlag)
				{
					obj[i].Info.Color = BLUE;

					fLib->DrawSquare(obj[i].Info.Center, obj[i].Info.Rad, obj[i].Info.Color);
				}
			}			

			//デバッグ用
			Novice::ScreenPrintf(0, 0, "timer = %d", ownerTimer / 165);
			for (int i = 0; i < remainObj; i++)
			{
				Novice::ScreenPrintf(0, 20 + 20 * i, "timer = %d", obj[i].WeightFlag);
			}

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