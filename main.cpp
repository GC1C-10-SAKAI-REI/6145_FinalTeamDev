#define WINDOW_WIDTH 1600
#define WINDOW_HEIGHT 720

#include <Novice.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "FuncLib.h"

const char kWindowTitle[] = "6145_刹ニャのイタズラ";

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, WINDOW_WIDTH, WINDOW_HEIGHT);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

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
		{800,580},
		{0,0},
		64,
		6,
		BLACK
	};
	//true：生きてる false：死んでる
	bool isPAlive = true;
	//走っているフラグ
	bool runFlag = false;
	//慣性を持たせるための変数
	float runPower = 0.0f;
	//プレイヤーが隠れた時のエフェクト
	unsigned int hideEffect = 0x69696900;

	/*プレイヤーが落とすオブジェクト*/
	const int remainObj = 5;
	FallenObj obj[remainObj];
	for (int i = 0; i < remainObj; i++)
	{
		obj[i] =
		{
			{
				{float(290 + (256 * i)),580},
				{0,0},
				64,
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
	//オーナーがこちらを向いているかどうか
	bool isOwnerLook = false;
	//赤くするための四角の色
	unsigned int ownerEffect = 0xFF000000;
	//α値の増減を切り替える為のフラグ
	int changeFlag = 0;
	//エフェクト
	int oEffectEnd = 0;

	/*ブロック(隠れる場所)*/
	const int bNum = 2;
	Object block[bNum];
	//初期化
	for (int i = 0; i < bNum; i++)
	{
		block[i] =
		{
			{0,540},
			{0,0},
			96,
			0,
			WHITE
		};
	}
	block[0].Center.X = block[0].Rad + 32;
	block[1].Center.X = WINDOW_WIDTH - (block[1].Rad + 32);
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

	//
	int score[4] = { 0 };

	/*リソース関連*/

	//飼い主
	int ownerTexHundle[] =
	{
		Novice::LoadTexture("./Resources./Pictures./mother_back.png"),
		Novice::LoadTexture("./Resources./Pictures./mother_front.png")
	};
	//落下物
	int objTexHundle[] =
	{
		Novice::LoadTexture("./Resources./Pictures./lightObject.png"),
		Novice::LoadTexture("./Resources./Pictures./heavyObject.png"),
		Novice::LoadTexture("./Resources./Pictures./heavyObject_1.png"),
		Novice::LoadTexture("./Resources./Pictures./heavyObject_2.png")
	};
	//背景
	int bgTexHundle[] =
	{
		Novice::LoadTexture("./Resources./Pictures./title.png"),
		Novice::LoadTexture("./Resources./Pictures./background.png"),//差し替えました
		Novice::LoadTexture("./Resources./Pictures./stage.png"),//差し替えました
		Novice::LoadTexture("./Resources./Pictures./gameOver.png")
	};
	//スコア用の数字
	int numberHandle = Novice::LoadTexture("./Resources./Pictures./num.png");

	//プレイヤー
#pragma region //WalkTexturhandle


	int walkTimerL = 0;
	int walkTimerR = 0;

	int walkFlagL = 0;
	int walkFlagR = 0;
	int walkSheetL = Novice::LoadTexture("./Resources./Pictures./walkSheetL.png");
	int walkSheetR = Novice::LoadTexture("./Resources./Pictures./walkSheetR.png");
#pragma endregion

#pragma region //runTexturhandle
	int runTimerL = 0;
	int runTimerR = 0;

	int runFlagL = 0;
	int runFlagR = 0;

	int runSheetL = Novice::LoadTexture("./Resources./Pictures./RunL.png");
	int runSheetR = Novice::LoadTexture("./Resources./Pictures./RunR.png");
#pragma endregion

#pragma region //Attack
	int attackTimer = 0;
	/*int attackTimerR = 0;*/

	int attackFlag = 0;
	/*int attackFlagR = 0;*/

	int attackSheetL = Novice::LoadTexture("./Resources./Pictures./AttackL.png");
	int attackSheetR = Novice::LoadTexture("./Resources./Pictures./AttackR.png");

#pragma endregion

#pragma region //Stand by

	int standbyFlagL = 0;
	int standbyFlagR = 1;



	int standbyTextureL = Novice::LoadTexture("./Resources./Pictures./standbyL.png");
	int standbyTextureR = Novice::LoadTexture("./Resources./Pictures./standbyR.png");

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

		switch (scene)
		{
		case TITLE: //タイトル

			if (keys[DIK_RETURN] && !preKeys[DIK_RETURN])
			{
				scene = TUTORIAL;

				/*初期化*/
				//プレイヤー
				player.Center.X = 800;
				player.Velocity.X = 0;
				isPAlive = true;
				runFlag = false;
				runPower = 0;
				hideEffect = 0x69696900;
				//飼い主
				isOwnerLook = false;
				ownerTimer = 0;
				oEffectEnd = 0;
				//落とすオブジェクト
				for (int i = 0; i < remainObj; i++)
				{
					obj[i].Hp = 1;
				}
			}

			break;

		case TUTORIAL: //チュートリアル

			if (keys[DIK_RETURN] && !preKeys[DIK_RETURN])
			{
				scene = GAMEPLAY;
			}

			break;

		case GAMEPLAY: //プレイ画面

			/*自機の移動処理(担当：ゾ)*/
			player.Velocity.X = 0;

			//ベクトル決め
			if (keys[DIK_A] || keys[DIK_LEFT])
			{
				player.Velocity.X = -1;

				//歩く描画Left
				standbyFlagL = 1;
				standbyFlagR = 0;
				walkFlagL = 1;
				walkTimerL++;
				if (walkTimerL > 39)
				{
					walkTimerL = 0;
				}
			}
			else
			{
				walkFlagL = 0;
				walkTimerL = 0;
			}
			if (keys[DIK_D] || keys[DIK_RIGHT])
			{
				player.Velocity.X = 1;
				//歩く描画Right
				standbyFlagL = 0;
				standbyFlagR = 1;
				walkFlagR = 1;
				walkTimerR++;
				if (walkTimerR > 39)
				{
					walkTimerR = 0;
				}
			}
			else
			{
				walkFlagR = 0;
				walkTimerR = 0;
			}

			/*ダッシュの処理(担当：サカイ)*/
			//ダッシュのトリガー
			if (keys[DIK_RETURN])
			{

				runFlag = true;
			}
			else
			{
				runFlag = false;
			}

			//ダッシュ中の処理
			if (runFlag)
			{
				//ダッシュしているときのみ慣性を持たせる
				if (keys[DIK_A] || keys[DIK_LEFT])
				{

					runFlagL = 1;

					if (runPower >= -6.0f)
					{
						runPower -= 0.2f;
					}
					runTimerL++;
					if (runTimerL > 11)
					{
						runTimerL = 0;
					}
				}
				else
				{
					runFlagL = 0;
					runTimerL = 0;
				}

				if (keys[DIK_D] || keys[DIK_RIGHT])
				{
					runFlagR = 1;
					if (runPower <= 6.0f)
					{
						runPower += 0.2f;
					}
					runTimerR++;
					if (runTimerR > 11)
					{
						runTimerR = 0;
					}
				}
				else
				{
					runFlagR = 0;
					runTimerR = 0;
				}
				//Enterは押しているが左右キーを離した場合は減速する
				if (!keys[DIK_A])
				{
					if (runPower <= 0 && runPower >= -7)
					{
						runPower += 0.2f;
						if (runPower > -1)
						{
							runPower = 0.0f;
						}
					}
				}
				if (!keys[DIK_D])
				{
					if (runPower <= 7 && runPower >= 0)
					{
						runPower -= 0.2f;
						if (runPower < 1)
						{
							runPower = 0.0f;
						}
					}
				}
			}
			else if (!runFlag)
			{
				if (runPower <= 7 && runPower >= 0)
				{
					//ダッシュボタンを離したら徐々に減速
					runPower -= 0.2f;

					if (runPower < 1)
					{
						//上記の処理に引っかかってしまうので0に収束
						runPower = 0.0f;
					}
				}
				else if (runPower <= 0 && runPower >= -7)
				{
					runPower += 0.2f;
					if (runPower > -1)
					{
						runPower = 0.0f;
					}
				}
			}

			//ベクトルとスピードを掛け合わせる
			if (!attackFlag)
			{

				player.Center.X += (player.Velocity.X * player.Spd) + runPower;
			}

			//自機の移動制限
			if (player.Center.X - player.Rad < 0)
			{
				player.Center.X = player.Rad;
			}
			else if (player.Center.X + player.Rad > WINDOW_WIDTH)
			{
				player.Center.X = WINDOW_WIDTH - player.Rad;
			}

			/*オブジェクトのランダム生成(担当：リュウ)*/
			for (int i = 0; i < 5; i++)
			{
				//life分け
				if (obj[i].IsAlive && obj[i].Hp <= 0)
				{
					number[i] = rand();
					if (number[i] % 2 == 0)
					{
						obj[i].Info.Center.Y = 580;
						obj[i].WeightFlag = false;
						obj[i].Hp = lightObjHp;
					}
					if (number[i] % 2 == 1)
					{
						obj[i].Info.Center.Y = 520;
						obj[i].WeightFlag = true;
						obj[i].Hp = heavyObjHp;
					}
				}
			}

			//動作の処理描画関数，当たり判定とは関係ない
		/*	if (keys[DIK_SPACE] && !preKeys[DIK_SPACE])
			{
				attackFlag = 1;
				attackTimer = 0;

			}*/
			if (attackFlag == 1)
			{
				attackTimer++;
				if (attackTimer > 23)
				{
					attackTimer = 0;
					attackFlag = 0;
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
								attackFlag = 1;
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

						//攻撃されたら消える
						if (obj[i].Hp < 1)
						{
							obj[i].IsAlive = false;
							//10ポイント加算
							score[2] += 1;
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
								attackFlag = 1;
							}
							if (attackFlag == 1)
							{
								if (attackTimer == 1)
								{
									obj[i].Hp -= 1;
								}

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
							//100点加算
							score[1] += 1;
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

			//プレイヤーが隠れている時の演出
			if (safeFlag)
			{
				if (hideEffect <= 0x696969B4)
				{
					hideEffect += 4;
				}
			}
			else if (!safeFlag)
			{
				if (hideEffect > 0x69696900)
				{
					hideEffect -= 4;
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
				else if (!safeFlag && isOwnerLook)
				{
					//プレイヤーの死亡処理
					//isPAlive = false;
				}
				//ownerTimerが660となっているがサカイのPCが165fpsであるため
				//165*4(つまりサカイのPC上で3秒)で660となっている。
				//皆のPCが60fpsなら240に値を修正しといてくれ
				if (ownerTimer > 825)
				{
					ownerTimer = 0;
					oEffectEnd = 0;
				}

				if (ownerTimer >= 0 && ownerTimer < 660)
				{
					isOwnerLook = false;
				}
				else if (ownerTimer >= 660 && ownerTimer <= 742)
				{
					isOwnerLook = true;
				}
			}
			//飼い主が振り向く演出の処理
			if (ownerTimer >= 220)
			{
				if (oEffectEnd <= 2)
				{
					if (changeFlag % 2 == 0)
					{
						ownerEffect += 3;
					}
					else if (changeFlag % 2 == 1)
					{
						ownerEffect -= 3;
					}

					if (ownerEffect >= 0xFF00006F)
					{
						changeFlag++;
					}
					else if (ownerEffect <= 0xFF000000)
					{
						changeFlag--;
						oEffectEnd++;
					}
				}
			}

			/*スコア関係(担当：リュウ)*/
			if (score[3] > 9)
			{
				score[2] += 1;
				score[3] = 0;
			}
			if (score[2] > 9)
			{
				score[1] += 1;
				score[2] = 0;
			}
			if (score[1] > 9)
			{
				score[0] += 1;
				score[1] = 0;
			}

			//ゲームオーバーへの遷移
			if (!isPAlive)
			{
				scene = GAMEOVER;
			}

			break;

		case GAMEOVER: //ゲームオーバー

			if (keys[DIK_RETURN] && !preKeys[DIK_RETURN])
			{
				scene = TITLE;
			}

			break;
		}

		/// ↑更新処理ここまで



		/// ↓描画処理ここから

		switch (scene)
		{
		case TITLE: //タイトル

			Novice::DrawSprite(0, 0, bgTexHundle[0], 1, 1, 0, WHITE);

			break;

		case TUTORIAL: //チュートリアル

			Novice::ScreenPrintf(0, 0, "scene = TUTORIAL");

			break;

		case GAMEPLAY: //ゲームプレイ

			//背景
			Novice::DrawSprite(0, 0, bgTexHundle[1], 1, 1, 0, WHITE);
			//飼い主
			if (ownerTimer <= 650)
			{
				Novice::DrawSprite(280, 0, ownerTexHundle[0], 1, 1, 0, WHITE);
			}
			else if (ownerTimer > 650)
			{
				Novice::DrawSprite(280, 0, ownerTexHundle[1], 1, 1, 0, WHITE);
			}
			//ステージ
			Novice::DrawSprite(0, 600, bgTexHundle[2], 1, 1, 0, WHITE);

			//ブレンドモード変更
			Novice::SetBlendMode(BlendMode::kBlendModeNormal);

			//ブロック(隠れる場所)
			for (int i = 0; i < bNum; i++)
			{
				fLib->DrawSquares(block, i);
			}
			// 自機

			if (isPAlive)
			{
				if (!attackFlag)
				{
					if ((standbyFlagL && !walkFlagL) && !attackFlag)
					{
						Novice::DrawSprite(int(player.Center.X - player.Rad), int(player.Center.Y - player.Rad), standbyTextureL, 1, 1, 0.0f, WHITE);
					}

					if ((standbyFlagR && !walkFlagR) && !attackFlag)
					{
						Novice::DrawSprite(int(player.Center.X - player.Rad), int(player.Center.Y - player.Rad), standbyTextureR, 1, 1, 0.0f, WHITE);
					}
				}
				if (attackFlag)
				{
					if (standbyFlagL == 1) {

						if (attackTimer < 3)
						{
							Novice::DrawSprite(int(player.Center.X - player.Rad), int(player.Center.Y - player.Rad), standbyTextureL, 1, 1, 0.0f, WHITE);

						}
						if (attackTimer < 6 && attackTimer>2)
						{
							Novice::DrawSpriteRect(int(player.Center.X - player.Rad), int(player.Center.Y - player.Rad), 0, 0, 128, 128, attackSheetL, ((float)128 / (float)512), 1, 0.0f, 0xFFFFFFFF);
						}
						if (attackTimer < 12 && attackTimer>5)
						{
							Novice::DrawSpriteRect(int(player.Center.X - player.Rad), int(player.Center.Y - player.Rad), 128, 0, 128, 128, attackSheetL, ((float)128 / (float)512), 1, 0.0f, 0xFFFFFFFF);
						}
						if (attackTimer < 18 && attackTimer>11)
						{
							Novice::DrawSpriteRect(int(player.Center.X - player.Rad), int(player.Center.Y - player.Rad), 256, 0, 128, 128, attackSheetL, ((float)128 / (float)512), 1, 0.0f, 0xFFFFFFFF);
						}
						if (attackTimer < 24 && attackTimer>17)
						{
							Novice::DrawSpriteRect(int(player.Center.X - player.Rad), int(player.Center.Y - player.Rad), 384, 0, 128, 128, attackSheetL, ((float)128 / (float)512), 1, 0.0f, 0xFFFFFFFF);
						}
					}

					if (standbyFlagR == 1) {

						if (attackTimer < 3)
						{
							Novice::DrawSprite(int(player.Center.X - player.Rad), int(player.Center.Y - player.Rad), standbyTextureR, 1, 1, 0.0f, WHITE);

						}
						if (attackTimer < 6 && attackTimer>2)
						{
							Novice::DrawSpriteRect(int(player.Center.X - player.Rad), int(player.Center.Y - player.Rad), 0, 0, 128, 128, attackSheetR, ((float)128 / (float)512), 1, 0.0f, 0xFFFFFFFF);
						}
						if (attackTimer < 12 && attackTimer>5)
						{
							Novice::DrawSpriteRect(int(player.Center.X - player.Rad), int(player.Center.Y - player.Rad), 128, 0, 128, 128, attackSheetR, ((float)128 / (float)512), 1, 0.0f, 0xFFFFFFFF);
						}
						if (attackTimer < 18 && attackTimer>11)
						{
							Novice::DrawSpriteRect(int(player.Center.X - player.Rad), int(player.Center.Y - player.Rad), 256, 0, 128, 128, attackSheetR, ((float)128 / (float)512), 1, 0.0f, 0xFFFFFFFF);
						}
						if (attackTimer < 24 && attackTimer>17)
						{
							Novice::DrawSpriteRect(int(player.Center.X - player.Rad), int(player.Center.Y - player.Rad), 384, 0, 128, 128, attackSheetR, ((float)128 / (float)512), 1, 0.0f, 0xFFFFFFFF);
						}
					}


				}

				if (runFlag == false)
				{
					if (!attackFlag)
					{

						if (walkFlagL && !walkFlagR)
						{
							if (walkTimerL < 10) {

								Novice::DrawSpriteRect(int(player.Center.X - player.Rad), int(player.Center.Y - player.Rad), 0, 0, 128, 128, walkSheetL, ((float)128 / (float)512), 1, 0.0f, 0xFFFFFFFF);
							}
							else  if (walkTimerL < 20 && walkTimerL > 9)
							{
								Novice::DrawSpriteRect(int(player.Center.X - player.Rad), int(player.Center.Y - player.Rad), 128, 0, 128, 128, walkSheetL, ((float)128 / (float)512), 1, 0.0f, 0xFFFFFFFF);
							}
							else  if (walkTimerL < 30 && walkTimerL > 19)
							{
								Novice::DrawSpriteRect(int(player.Center.X - player.Rad), int(player.Center.Y - player.Rad), 256, 0, 128, 128, walkSheetL, ((float)128 / (float)512), 1, 0.0f, 0xFFFFFFFF);
							}
							else  if (walkTimerL < 40 && walkTimerL > 29)
							{
								Novice::DrawSpriteRect(int(player.Center.X - player.Rad), int(player.Center.Y - player.Rad), 384, 0, 128, 128, walkSheetL, ((float)128 / (float)512), 1, 0.0f, 0xFFFFFFFF);
							}
						}
						if ((!walkFlagL && walkFlagR) || (walkFlagL && walkFlagR))
						{

							if (walkTimerR < 10) {

								Novice::DrawSpriteRect(int(player.Center.X - player.Rad), int(player.Center.Y - player.Rad), 0, 0, 128, 128, walkSheetR, ((float)128 / (float)512), 1, 0.0f, 0xFFFFFFFF);
							}
							else  if (walkTimerR < 20 && walkTimerR > 9)
							{
								Novice::DrawSpriteRect(int(player.Center.X - player.Rad), int(player.Center.Y - player.Rad), 128, 0, 128, 128, walkSheetR, ((float)128 / (float)512), 1, 0.0f, 0xFFFFFFFF);
							}
							else  if (walkTimerR < 30 && walkTimerR > 19)
							{
								Novice::DrawSpriteRect(int(player.Center.X - player.Rad), int(player.Center.Y - player.Rad), 256, 0, 128, 128, walkSheetR, ((float)128 / (float)512), 1, 0.0f, 0xFFFFFFFF);
							}
							else  if (walkTimerR < 40 && walkTimerR > 29)
							{
								Novice::DrawSpriteRect(int(player.Center.X - player.Rad), int(player.Center.Y - player.Rad), 384, 0, 128, 128, walkSheetR, ((float)128 / (float)512), 1, 0.0f, 0xFFFFFFFF);
							}
						}
					}
				}

				if (runFlag == true)
				{
					if (runFlagL == 1)
					{
						if (runTimerL < 6)
						{
							Novice::DrawSpriteRect(int(player.Center.X - player.Rad), int(player.Center.Y - player.Rad), 0, 0, 160, 128, runSheetL, ((float)160 / (float)320), 1, 0.0f, 0xFFFFFFFF);
						}
						if (runTimerL > 5 && runTimerL < 12)
						{
							Novice::DrawSpriteRect(int(player.Center.X - player.Rad), int(player.Center.Y - player.Rad), 160, 0, 160, 128, runSheetL, ((float)160 / (float)320), 1, 0.0f, 0xFFFFFFFF);
						}
					}
					if (runFlagR == 1)
					{
						if (runTimerR < 6)
						{
							Novice::DrawSpriteRect(int(player.Center.X - player.Rad), int(player.Center.Y - player.Rad), 0, 0, 160, 128, runSheetR, ((float)160 / (float)320), 1, 0.0f, 0xFFFFFFFF);
						}
						if (runTimerR > 5 && runTimerR < 12)
						{
							Novice::DrawSpriteRect(int(player.Center.X - player.Rad), int(player.Center.Y - player.Rad), 160, 0, 160, 128, runSheetR, ((float)160 / (float)320), 1, 0.0f, 0xFFFFFFFF);
						}
					}

				}
			}




			//軽いオブジェクト
			for (int i = 0; i < remainObj; i++)
			{
				if (obj[i].IsAlive && !obj[i].WeightFlag)
				{
					Novice::DrawSprite(int(obj[i].Info.Center.X - obj[i].Info.Rad), int(obj[i].Info.Center.Y - obj[i].Info.Rad), objTexHundle[0], 1, 1, 0, WHITE);
				}
			}
			//重いオブジェクト
			for (int i = 0; i < remainObj; i++)
			{
				if (obj[i].IsAlive && obj[i].WeightFlag)
				{
					if (obj[i].Hp == 3)
					{
						Novice::DrawSprite(int(obj[i].Info.Center.X - obj[i].Info.Rad * 2), int(obj[i].Info.Center.Y - obj[i].Info.Rad * 2), objTexHundle[1], 1, 1, 0, WHITE);
					}
					else if (obj[i].Hp == 2)
					{
						Novice::DrawSprite(int(obj[i].Info.Center.X - obj[i].Info.Rad * 2), int(obj[i].Info.Center.Y - obj[i].Info.Rad * 2), objTexHundle[2], 1, 1, 0, WHITE);
					}
					else if (obj[i].Hp == 1)
					{
						Novice::DrawSprite(int(obj[i].Info.Center.X - obj[i].Info.Rad * 2), int(obj[i].Info.Center.Y - obj[i].Info.Rad * 2), objTexHundle[3], 1, 1, 0, WHITE);
					}
				}
			}
			//警告の演出
			Novice::DrawBox(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, ownerEffect, kFillModeSolid);
			//隠れる演出
			Novice::DrawBox(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, hideEffect, kFillModeSolid);
			//スコア
			for (int i = 0; i < 4; i++)
			{
				if (score[i] == 0)
				{
					Novice::DrawSpriteRect(1400 + i * 32, 0, 0, 0, 32, 32, numberHandle, ((float)32 / (float)320), 1, 0.0f, 0xFFFFFFFF);
				}
				if (score[i] == 1)
				{
					Novice::DrawSpriteRect(1400 + i * 32, 0, 32, 0, 32, 32, numberHandle, ((float)32 / (float)320), 1, 0.0f, 0xFFFFFFFF);
				}
				if (score[i] == 2)
				{
					Novice::DrawSpriteRect(1400 + i * 32, 0, 64, 0, 32, 32, numberHandle, ((float)32 / (float)320), 1, 0.0f, 0xFFFFFFFF);
				}
				if (score[i] == 3)
				{
					Novice::DrawSpriteRect(1400 + i * 32, 0, 96, 0, 32, 32, numberHandle, ((float)32 / (float)320), 1, 0.0f, 0xFFFFFFFF);
				}
				if (score[i] == 4)
				{
					Novice::DrawSpriteRect(1400 + i * 32, 0, 128, 0, 32, 32, numberHandle, ((float)32 / (float)320), 1, 0.0f, 0xFFFFFFFF);
				}
				if (score[i] == 5)
				{
					Novice::DrawSpriteRect(1400 + i * 32, 0, 160, 0, 32, 32, numberHandle, ((float)32 / (float)320), 1, 0.0f, 0xFFFFFFFF);
				}
				if (score[i] == 6)
				{
					Novice::DrawSpriteRect(1400 + i * 32, 0, 192, 0, 32, 32, numberHandle, ((float)32 / (float)320), 1, 0.0f, 0xFFFFFFFF);
				}
				if (score[i] == 7)
				{
					Novice::DrawSpriteRect(1400 + i * 32, 0, 224, 0, 32, 32, numberHandle, ((float)32 / (float)320), 1, 0.0f, 0xFFFFFFFF);
				}
				if (score[i] == 8)
				{
					Novice::DrawSpriteRect(1400 + i * 32, 0, 256, 0, 32, 32, numberHandle, ((float)32 / (float)320), 1, 0.0f, 0xFFFFFFFF);
				}
				if (score[i] == 9)
				{
					Novice::DrawSpriteRect(1400 + i * 32, 0, 288, 0, 32, 32, numberHandle, ((float)32 / (float)320), 1, 0.0f, 0xFFFFFFFF);
				}
			}

			break;

		case GAMEOVER: //ゲームオーバー

			Novice::DrawSprite(0, 0, bgTexHundle[3], 1, 1, 0, WHITE);

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
