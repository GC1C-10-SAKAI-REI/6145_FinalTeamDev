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
	//シェイクの変数
	int randX[5];
	int randY[5];
	bool shakeFlag[5];
	int frame[5];
	int shake[5];
	for (int i = 0; i < 5; i++)
	{
		randX[i] = rand();
		randY[i] = rand();
		shakeFlag[i] = false;
		frame[i] = 30;
		shake[i] = 17;
	}
	//自機
	Object player =
	{
		{800,580},{0,0},64,6,BLACK
	};
	//true：生きてる false：死んでる
	bool isPAlive = true;
	//走っているフラグ
	bool runFlag = false;
	//慣性を持たせるための変数
	float runPower = 0.0f;
	//プレイヤーが隠れた時のエフェクト
	unsigned int hideEffect = 0x69696900;
	//待機するフラグ
	bool idleFlagL = false;
	bool idleFlagR = true;
	//歩く向きのフラグ
	bool walkFlagL = false;
	bool walkFlagR = false;
	//走る向きのフラグ
	bool runFlagL = false;
	bool runFlagR = false;
	//アタックのフラグ
	bool attackFlag = false;
	//歩くアニメーションのタイマー
	int walkTimerL = 0;
	int walkTimerR = 0;
	//走るアニメーションのタイマー
	int runTimerL = 0;
	int runTimerR = 0;
	//アタックアニメーションのタイマー
	int attackTimer = 0;	

	/*プレイヤーが落とすオブジェクト*/
	const int remainObj = 5;
	FallenObj obj[remainObj];
	for (int i = 0; i < remainObj; i++)
	{
		obj[i] =
		{
			{ {float(290 + (256 * i)),580},{0,0},64,0,WHITE },false,true,0,false,1
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
			{0,540},{0,0},96,0,WHITE
		};
	}
	block[0].Center.X = block[0].Rad + 32;
	block[1].Center.X = WINDOW_WIDTH - (block[1].Rad + 32);
	//true：隠れている false：はみ出てる
	bool isHyding[bNum] = { false };
	// オブジェクトの一つどれかに隠れてたらtrue
	bool safeFlag = false;

	/*システム関連*/
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
	//明転&暗転用の黒い四角
	Object sceneTrans =
	{
		{0,0},{0,0},0,0,0x00000000
	};
	int sceneTransFlag = 0;	
	//スコアの桁数
	const int scoreDigits = 4;
	//スコア
	int score[scoreDigits] = { 0 };
	//タイトルに関するタイマー
	int titleTimer = 0;
	int titleTimer2 = 0;
	//ゲームオーバーに関するタイマー
	int gameOverTimer = 0;

	/*テクスチャ関連*/
	int playerTexHundle[] =
	{
		//待機差分
		Novice::LoadTexture("./Resources./Pictures./standbyL.png"),
		Novice::LoadTexture("./Resources./Pictures./standbyR.png"),
		//歩き差分
		Novice::LoadTexture("./Resources./Pictures./walkSheetL.png"),
		Novice::LoadTexture("./Resources./Pictures./walkSheetR.png"),
		//走り差分
		Novice::LoadTexture("./Resources./Pictures./runL.png"),
		Novice::LoadTexture("./Resources./Pictures./runR.png"),
		//攻撃差分
		Novice::LoadTexture("./Resources./Pictures./attackL.png"),
		Novice::LoadTexture("./Resources./Pictures./attackR.png")
	};
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
		Novice::LoadTexture("./Resources./Pictures./background.png"),
		Novice::LoadTexture("./Resources./Pictures./stage.png"),
		Novice::LoadTexture("./Resources./Pictures./gameover.png"),
		Novice::LoadTexture("./Resources./Pictures./title2.png"),
		Novice::LoadTexture("./Resources./Pictures./gameover2.png")
	};
	//スコア用の数字
	int numberHandle = Novice::LoadTexture("./Resources./Pictures./num.png");
	int scoreHandle = Novice::LoadTexture("./Resources./Pictures./score.png");
	//隠れ場所
	int bookHandle = Novice::LoadTexture("./Resources./Pictures./book.png");

	/*音楽*/
	const int audio = 20;
	int audioHundle[audio] =
	{
		Novice::LoadAudio("./Resources./Audios./Title.mp3"),
		Novice::LoadAudio("./Resources./Audios./GamePlay1.mp3"),
		Novice::LoadAudio("./Resources./Audios./GamePlay2.mp3"),
		Novice::LoadAudio("./Resources./Audios./GameOver.mp3"),
	};

	int titleBGMplay = -1;
	int gameplayBGMplay = -1;
	int gameoverBGMPlay = -1;

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
			//デバック用
			//if(keys[DIK_S] && preKeys[DIK_S] == 0)
			//{
			//	scene = GAMEPLAY;
			//}
			if (sceneTransFlag == 2)
			{
				if (fLib->SceneStart(sceneTrans.Color))
				{
					sceneTransFlag = 0;
				}
			}

			if (sceneTransFlag == 0)
			{
				if (titleTimer < 30)
				{
					titleTimer++;
				}
				else
				{
					titleTimer = 0;
				}

				if (titleTimer2 < 39)
				{
					titleTimer2++;
				}
				else
				{
					titleTimer2 = 0;
				}

				if (keys[DIK_RETURN] && !preKeys[DIK_RETURN])
				{
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
						if (!obj[i].WeightFlag)
						{
							obj[i].Hp = lightObjHp;
						}
						else if (obj[i].WeightFlag)
						{
							obj[i].Hp = heavyObjHp;
						}
					}
					for (int i = 0; i < scoreDigits; i++)
					{
						score[i] = 0;
					}
					sceneTransFlag = 1;

					//遷移処理はここで記述すると
					//不具合が起きるため下の描画処理に有り
				}
			}			

			if (sceneTransFlag == 1)
			{
				if (fLib->SceneEnd(sceneTrans.Color))
				{
					sceneTransFlag = 2;
					scene = TUTORIAL;
					//タイトルの表示タイマ					
				}
			}

			break;

		case TUTORIAL: //チュートリアル
			//シーン遷移用の処理
			if (sceneTransFlag == 2)
			{
				if (fLib->SceneStart(sceneTrans.Color))
				{
					sceneTransFlag = 0;
				}
			}
			if (sceneTransFlag == 0)
			{
				if (keys[DIK_RETURN] && !preKeys[DIK_RETURN])
				{
					sceneTransFlag = 1;
				}
			}
			if (sceneTransFlag == 1)
			{
				if (fLib->SceneEnd(sceneTrans.Color))
				{
					sceneTransFlag = 2;
					scene = GAMEPLAY;
				}
			}

			break;

		case GAMEPLAY: //プレイ画面
			//シーン遷移時の演出
			if (sceneTransFlag == 2)
			{
				if (fLib->SceneStart(sceneTrans.Color))
				{
					sceneTransFlag = 0;
				}
			}

			/*自機の移動処理(担当：ゾ)*/
			player.Velocity.X = 0;

			//ベクトル決め
			if (keys[DIK_A])
			{
				player.Velocity.X = -1;

				//歩く描画Left
				idleFlagL = true;
				idleFlagR = false;
				walkFlagL = true;
				walkTimerL++;
				if (walkTimerL > 39)
				{
					walkTimerL = 0;
				}
			}
			else
			{
				walkFlagL = false;
				walkTimerL = 0;
			}
			if (keys[DIK_D])
			{
				player.Velocity.X = 1;
				//歩く描画Right
				idleFlagL = false;
				idleFlagR = true;
				walkFlagR = true;
				walkTimerR++;
				if (walkTimerR > 39)
				{
					walkTimerR = 0;
				}
			}
			else
			{
				walkFlagR = false;
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
				if (keys[DIK_A])
				{
					runFlagL = true;

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
					runFlagL = false;
					runTimerL = 0;
				}

				if (keys[DIK_D])
				{
					runFlagR = true;
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
					runFlagR = false;
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
						obj[i].Info.Color = WHITE;
						obj[i].Hp = lightObjHp;
					}
					if (number[i] % 2 == 1)
					{
						obj[i].Info.Center.Y = 520;
						obj[i].WeightFlag = true;
						obj[i].Info.Color = WHITE;
						obj[i].Hp = heavyObjHp;
					}
				}
			}

			//動作の処理描画関数，当たり判定とは関係ない
			if (keys[DIK_SPACE] && !preKeys[DIK_SPACE])
			{
				attackFlag = true;
			}
			if (attackFlag)
			{
				attackTimer++;
				if (attackTimer > 23)
				{
					attackTimer = 0;
					attackFlag = false;
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
								attackFlag = true;
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
						if (obj[i].Info.Color >= 0xFFFFFF00)
						{
							obj[i].Info.Color -= 16;
						}
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
								shakeFlag[i] = true;
								shake[i] = 17;
								frame[i] = 30;
							}
							if (attackFlag)
							{
								if (shakeFlag[i] == true)
								{
									frame[i] = 30;
									shake[i] = 21;
									randX[i] = rand() % shake[i] - (shake[i] / 2);
									randY[i] = rand() % shake[i] - (shake[i] / 2);
									frame[i]--;
									if (frame[i] % 3 == 0)
									{
										shake[i]--;
									}
								}
								if (shake[i] < 1)
								{
									shake[i] = 1;
									shakeFlag[i] = false;
									if (shakeFlag[i] == false)
									{
										obj[i].Hp -= 1;
									}
								}
								if (attackTimer == 1)
								{
									obj[i].Hp -= 1;
								}
							}
							//ダッシュ中にオブジェクトと接触したらオブジェクトが落ちる
							if (runFlag && !breakFlag[i])
							{
								attackFlag = 1;
								shakeFlag[i] = true;
								shake[i] = 17;
								frame[i] = 30;

								if (shakeFlag[i] == true)
								{
									frame[i] = 30;
									shake[i] = 21;
									randX[i] = rand() % shake[i] - (shake[i] / 2);
									randY[i] = rand() % shake[i] - (shake[i] / 2);
									frame[i]--;
									if (frame[i] % 3 == 0)
									{
										shake[i]--;
									}
								}
								if (shake[i] < 1)
								{
									shake[i] = 1;
									shakeFlag[i] = false;
									if (shakeFlag[i] == false)
									{
										obj[i].Hp -= 1;
									}
								}
								//obj[i].Hp -= 1;
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
					if (!obj[i].IsAlive)
					{
						obj[i].ColFlag = false;
						obj[i].ResTimer++;
						if (obj[i].Info.Color >= 0xFFFFFF00)
						{
							obj[i].Info.Color -= 16;
						}
					}
					//リスポーン処理
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
					isPAlive = false;
				}
				//デバック用
				//if (keys[DIK_J] && preKeys[DIK_J] == 0)
				//{
				//	isPAlive = false;
				//}
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

			//遷移処理はここで記述すると
			//不具合が起きるため下の描画処理に有り

			break;

		case GAMEOVER: //ゲームオーバー

			//遷移処理はここで記述すると
			//不具合が起きるため下の描画処理に有り			
			//ゲームオーバーに関するタイマー
			if (gameOverTimer < 30)
			{
				gameOverTimer++;
			}
			else
			{
				gameOverTimer = 0;
			}

			if (sceneTransFlag == 0)
			{
				if (keys[DIK_RETURN] && !preKeys[DIK_RETURN])
				{
					sceneTransFlag = 1;
				}
			}
			if (sceneTransFlag == 1)
			{
				if (fLib->SceneEnd(sceneTrans.Color))
				{
					Novice::StopAudio(gameoverBGMPlay);
					sceneTransFlag = 2;
					scene = TITLE;
				}
			}

			break;
		}

		/// ↑更新処理ここまで



		/// ↓描画処理ここから


		switch (scene)
		{
		case TITLE: //タイトル
			
			/*テクスチャ*/
			//背景
			Novice::DrawSprite(0, 0, bgTexHundle[1], 1, 1, 0, WHITE);
			//
			if (titleTimer < 15)
			{
				Novice::DrawSprite(600, 400, bgTexHundle[4], 1, 1, 0, WHITE);
			}
			//猫
			if (titleTimer2 < 10)
			{
				Novice::DrawSpriteRect(600, 592, 0, 0, 128, 128, playerTexHundle[3], ((float)128 / (float)512), 1, 0.0f, 0xFFFFFFFF);
			}
			if (titleTimer2 > 9 && titleTimer2 < 20)
			{
				Novice::DrawSpriteRect(600, 592, 128, 0, 128, 128, playerTexHundle[3], ((float)128 / (float)512), 1, 0.0f, 0xFFFFFFFF);
			}
			if (titleTimer2 > 19 && titleTimer2 < 30)
			{
				Novice::DrawSpriteRect(600, 592, 256, 0, 128, 128, playerTexHundle[3], ((float)128 / (float)512), 1, 0.0f, 0xFFFFFFFF);
			}
			if (titleTimer2 > 29 && titleTimer2 < 40)
			{
				Novice::DrawSpriteRect(600, 592, 384, 0, 128, 128, playerTexHundle[3], ((float)128 / (float)512), 1, 0.0f, 0xFFFFFFFF);
			}
			//
			Novice::DrawSprite(0, 0, bgTexHundle[0], 1, 1, 0, WHITE);
			//遷移用の黒い四角
			Novice::DrawBox((int)sceneTrans.Center.X, (int)sceneTrans.Center.Y, WINDOW_WIDTH, WINDOW_HEIGHT, 0, sceneTrans.Color, kFillModeSolid);
			
			/*オーディオ関係*/
			gameoverBGMPlay = -1;
			if (Novice::IsPlayingAudio(titleBGMplay) == 0 || titleBGMplay == -1)
			{
				if (scene == TITLE)
				{
					titleBGMplay = Novice::PlayAudio(audioHundle[0], 1, 0.3f);
				}
			}

			break;

		case TUTORIAL: //チュートリアル			
			
			//遷移用の黒い四角
			Novice::DrawBox((int)sceneTrans.Center.X, (int)sceneTrans.Center.Y, WINDOW_WIDTH, WINDOW_HEIGHT, 0, sceneTrans.Color, kFillModeSolid);

			break;

		case GAMEPLAY: //ゲームプレイ
			/*オーディオ関係*/
			if (Novice::IsPlayingAudio(titleBGMplay) == 1)
			{
				Novice::StopAudio(titleBGMplay);
			}
			if (Novice::IsPlayingAudio(gameplayBGMplay) == 0 || gameplayBGMplay == -1)
			{
				if (scene == GAMEPLAY)
				{
					gameplayBGMplay = Novice::PlayAudio(audioHundle[1], 1, 0.5);
				}
			}

			//ゲームオーバーへの遷移
			if (!isPAlive)
			{
				Novice::StopAudio(gameplayBGMplay);
				Novice::StopAudio(titleBGMplay);
				scene = GAMEOVER;
			}

			//背景
			Novice::DrawSprite(0, 0, bgTexHundle[1], 1, 1, 0, WHITE);

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
				Novice::DrawSprite((int)(block[i].Center.X - block[i].Rad), (int)(block[i].Center.Y - block[i].Rad), bookHandle, 1, 1, 0.0f, WHITE);
			}

			//軽いオブジェクト
			for (int i = 0; i < remainObj; i++)
			{
				if (obj[i].Info.Color >= 0xFFFFFF00 && !obj[i].WeightFlag)
				{
					Novice::DrawSprite(int(obj[i].Info.Center.X - obj[i].Info.Rad), int(obj[i].Info.Center.Y - obj[i].Info.Rad), objTexHundle[0], 1, 1, 0, obj[i].Info.Color);
				}
			}
			//重いオブジェクト
			for (int i = 0; i < remainObj; i++)
			{
				if (obj[i].Info.Color >= 0xFFFFFF00 && obj[i].WeightFlag)
				{
					if (obj[i].Hp == 3)
					{
						Novice::DrawSprite(int(obj[i].Info.Center.X - obj[i].Info.Rad * 2), int(obj[i].Info.Center.Y - obj[i].Info.Rad * 2), objTexHundle[1], 1, 1, 0, obj[i].Info.Color);
					}
					else if (obj[i].Hp == 2)
					{
						Novice::DrawSprite(int(obj[i].Info.Center.X + randX[i] - obj[i].Info.Rad * 2), int(obj[i].Info.Center.Y + randY[i] - obj[i].Info.Rad * 2), objTexHundle[2], 1, 1, 0, obj[i].Info.Color);
					}
					else if (obj[i].Hp <= 1)
					{
						Novice::DrawSprite(int(obj[i].Info.Center.X + randX[i] - obj[i].Info.Rad * 2), int(obj[i].Info.Center.Y + randY[i] - obj[i].Info.Rad * 2), objTexHundle[3], 1, 1, 0, obj[i].Info.Color);
					}
				}
			}
			//
			if (isPAlive)
			{
				//待機する描画
				if (!attackFlag)
				{
					if (idleFlagL && !walkFlagL)
					{
						Novice::DrawSprite(int(player.Center.X - player.Rad), int(player.Center.Y - player.Rad), playerTexHundle[0], 1, 1, 0.0f, WHITE);
					}

					if (idleFlagR && !walkFlagR)
					{
						Novice::DrawSprite(int(player.Center.X - player.Rad), int(player.Center.Y - player.Rad), playerTexHundle[1], 1, 1, 0.0f, WHITE);
					}
				}
				//歩く描画
				if (runFlag == false)
				{
					if (!attackFlag)
					{
						if (walkFlagL && !walkFlagR)
						{
							if (walkTimerL < 10)
							{
								Novice::DrawSpriteRect(int(player.Center.X - player.Rad), int(player.Center.Y - player.Rad), 0, 0, 128, 128, playerTexHundle[2], ((float)128 / (float)512), 1, 0.0f, 0xFFFFFFFF);
							}
							else  if (walkTimerL < 20 && walkTimerL > 9)
							{
								Novice::DrawSpriteRect(int(player.Center.X - player.Rad), int(player.Center.Y - player.Rad), 128, 0, 128, 128, playerTexHundle[2], ((float)128 / (float)512), 1, 0.0f, 0xFFFFFFFF);
							}
							else  if (walkTimerL < 30 && walkTimerL > 19)
							{
								Novice::DrawSpriteRect(int(player.Center.X - player.Rad), int(player.Center.Y - player.Rad), 256, 0, 128, 128, playerTexHundle[2], ((float)128 / (float)512), 1, 0.0f, 0xFFFFFFFF);
							}
							else  if (walkTimerL < 40 && walkTimerL > 29)
							{
								Novice::DrawSpriteRect(int(player.Center.X - player.Rad), int(player.Center.Y - player.Rad), 384, 0, 128, 128, playerTexHundle[2], ((float)128 / (float)512), 1, 0.0f, 0xFFFFFFFF);
							}
						}
						if ((!walkFlagL && walkFlagR) || (walkFlagL && walkFlagR))
						{
							if (walkTimerR < 10)
							{
								Novice::DrawSpriteRect(int(player.Center.X - player.Rad), int(player.Center.Y - player.Rad), 0, 0, 128, 128, playerTexHundle[3], ((float)128 / (float)512), 1, 0.0f, 0xFFFFFFFF);
							}
							else  if (walkTimerR < 20 && walkTimerR > 9)
							{
								Novice::DrawSpriteRect(int(player.Center.X - player.Rad), int(player.Center.Y - player.Rad), 128, 0, 128, 128, playerTexHundle[3], ((float)128 / (float)512), 1, 0.0f, 0xFFFFFFFF);
							}
							else  if (walkTimerR < 30 && walkTimerR > 19)
							{
								Novice::DrawSpriteRect(int(player.Center.X - player.Rad), int(player.Center.Y - player.Rad), 256, 0, 128, 128, playerTexHundle[3], ((float)128 / (float)512), 1, 0.0f, 0xFFFFFFFF);
							}
							else  if (walkTimerR < 40 && walkTimerR > 29)
							{
								Novice::DrawSpriteRect(int(player.Center.X - player.Rad), int(player.Center.Y - player.Rad), 384, 0, 128, 128, playerTexHundle[3], ((float)128 / (float)512), 1, 0.0f, 0xFFFFFFFF);
							}
						}
					}
				}
				//走る描画
				if (runFlag == true)
				{
					if (!attackFlag)
					{
						if (runFlagL && !runFlagR)
						{
							if (runTimerL < 6)
							{
								Novice::DrawSpriteRect(int(player.Center.X - player.Rad), int(player.Center.Y - player.Rad), 0, 0, 160, 128, playerTexHundle[4], ((float)160 / (float)320), 1, 0.0f, 0xFFFFFFFF);
							}
							if (runTimerL > 5 && runTimerL < 12)
							{
								Novice::DrawSpriteRect(int(player.Center.X - player.Rad), int(player.Center.Y - player.Rad), 160, 0, 160, 128, playerTexHundle[4], ((float)160 / (float)320), 1, 0.0f, 0xFFFFFFFF);
							}
						}
						if ((runFlagR && !runFlagL) || (runFlagL && runFlagR))
						{
							if (runTimerR < 6)
							{
								Novice::DrawSpriteRect(int(player.Center.X - player.Rad), int(player.Center.Y - player.Rad), 0, 0, 160, 128, playerTexHundle[5], ((float)160 / (float)320), 1, 0.0f, 0xFFFFFFFF);
							}
							if (runTimerR > 5 && runTimerR < 12)
							{
								Novice::DrawSpriteRect(int(player.Center.X - player.Rad), int(player.Center.Y - player.Rad), 160, 0, 160, 128, playerTexHundle[5], ((float)160 / (float)320), 1, 0.0f, 0xFFFFFFFF);
							}
						}
					}
				}
				//アタックする描画
				if (attackFlag)
				{
					if (idleFlagL)
					{
						if (attackTimer < 3)
						{
							Novice::DrawSprite(int(player.Center.X - player.Rad), int(player.Center.Y - player.Rad), playerTexHundle[0], 1, 1, 0.0f, WHITE);

						}
						if (attackTimer < 6 && attackTimer>2)
						{
							Novice::DrawSpriteRect(int(player.Center.X - player.Rad), int(player.Center.Y - player.Rad), 0, 0, 128, 128, playerTexHundle[6], ((float)128 / (float)512), 1, 0.0f, 0xFFFFFFFF);
						}
						if (attackTimer < 12 && attackTimer>5)
						{
							Novice::DrawSpriteRect(int(player.Center.X - player.Rad), int(player.Center.Y - player.Rad), 128, 0, 128, 128, playerTexHundle[6], ((float)128 / (float)512), 1, 0.0f, 0xFFFFFFFF);
						}
						if (attackTimer < 18 && attackTimer>11)
						{
							Novice::DrawSpriteRect(int(player.Center.X - player.Rad), int(player.Center.Y - player.Rad), 256, 0, 128, 128, playerTexHundle[6], ((float)128 / (float)512), 1, 0.0f, 0xFFFFFFFF);
						}
						if (attackTimer < 24 && attackTimer>17)
						{
							Novice::DrawSpriteRect(int(player.Center.X - player.Rad), int(player.Center.Y - player.Rad), 384, 0, 128, 128, playerTexHundle[6], ((float)128 / (float)512), 1, 0.0f, 0xFFFFFFFF);
						}
					}

					if (idleFlagR)
					{
						if (attackTimer < 3)
						{
							Novice::DrawSprite(int(player.Center.X - player.Rad), int(player.Center.Y - player.Rad), playerTexHundle[1], 1, 1, 0.0f, WHITE);

						}
						if (attackTimer < 6 && attackTimer>2)
						{
							Novice::DrawSpriteRect(int(player.Center.X - player.Rad), int(player.Center.Y - player.Rad), 0, 0, 128, 128, playerTexHundle[7], ((float)128 / (float)512), 1, 0.0f, 0xFFFFFFFF);
						}
						if (attackTimer < 12 && attackTimer>5)
						{
							Novice::DrawSpriteRect(int(player.Center.X - player.Rad), int(player.Center.Y - player.Rad), 128, 0, 128, 128, playerTexHundle[7], ((float)128 / (float)512), 1, 0.0f, 0xFFFFFFFF);
						}
						if (attackTimer < 18 && attackTimer>11)
						{
							Novice::DrawSpriteRect(int(player.Center.X - player.Rad), int(player.Center.Y - player.Rad), 256, 0, 128, 128, playerTexHundle[7], ((float)128 / (float)512), 1, 0.0f, 0xFFFFFFFF);
						}
						if (attackTimer < 24 && attackTimer>17)
						{
							Novice::DrawSpriteRect(int(player.Center.X - player.Rad), int(player.Center.Y - player.Rad), 384, 0, 128, 128, playerTexHundle[7], ((float)128 / (float)512), 1, 0.0f, 0xFFFFFFFF);
						}
					}
				}
			}

			//警告の演出
			Novice::DrawBox(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, ownerEffect, kFillModeSolid);
			//隠れる演出
			Novice::DrawBox(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, hideEffect, kFillModeSolid);
			//スコア
			Novice::DrawSprite(1225, 0, scoreHandle, 1, 1, 0, WHITE);
			for (int i = 0; i < scoreDigits; i++)
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
			//遷移用の黒い四角
			Novice::DrawBox((int)sceneTrans.Center.X, (int)sceneTrans.Center.Y, WINDOW_WIDTH, WINDOW_HEIGHT, 0, sceneTrans.Color, kFillModeSolid);

			break;

		case GAMEOVER: //ゲームオーバー
			/*オーディオ関係*/
			if (gameoverBGMPlay == -1)
			{
				if (scene == GAMEOVER)
				{
					gameoverBGMPlay = Novice::PlayAudio(audioHundle[3], 0, 0.5);
				}
			}
			
			//遷移
			
			if (sceneTransFlag == 1)
			{
				if (fLib->SceneEnd(sceneTrans.Color))
				{
					Novice::StopAudio(gameoverBGMPlay);
				}
			}

			Novice::DrawSprite(0, 0, bgTexHundle[1], 1, 1, 0, WHITE);
			//背景？
			if (gameOverTimer < 15)
			{
				Novice::DrawSprite(0, 0, bgTexHundle[5], 1, 1, 0, WHITE);
			}
			//スコア
			Novice::DrawSprite(640, 0, scoreHandle, 1, 1, 0, WHITE);
			for (int i = 0; i < scoreDigits; i++)
			{
				if (score[i] == 0)
				{
					Novice::DrawSpriteRect(840 + i * 32, 0, 0, 0, 32, 32, numberHandle, ((float)32 / (float)320), 1, 0.0f, 0xFFFFFFFF);
				}
				if (score[i] == 1)
				{
					Novice::DrawSpriteRect(840 + i * 32, 0, 32, 0, 32, 32, numberHandle, ((float)32 / (float)320), 1, 0.0f, 0xFFFFFFFF);
				}
				if (score[i] == 2)
				{
					Novice::DrawSpriteRect(840 + i * 32, 0, 64, 0, 32, 32, numberHandle, ((float)32 / (float)320), 1, 0.0f, 0xFFFFFFFF);
				}
				if (score[i] == 3)
				{
					Novice::DrawSpriteRect(840 + i * 32, 0, 96, 0, 32, 32, numberHandle, ((float)32 / (float)320), 1, 0.0f, 0xFFFFFFFF);
				}
				if (score[i] == 4)
				{
					Novice::DrawSpriteRect(840 + i * 32, 0, 128, 0, 32, 32, numberHandle, ((float)32 / (float)320), 1, 0.0f, 0xFFFFFFFF);
				}
				if (score[i] == 5)
				{
					Novice::DrawSpriteRect(840 + i * 32, 0, 160, 0, 32, 32, numberHandle, ((float)32 / (float)320), 1, 0.0f, 0xFFFFFFFF);
				}
				if (score[i] == 6)
				{
					Novice::DrawSpriteRect(840 + i * 32, 0, 192, 0, 32, 32, numberHandle, ((float)32 / (float)320), 1, 0.0f, 0xFFFFFFFF);
				}
				if (score[i] == 7)
				{
					Novice::DrawSpriteRect(840 + i * 32, 0, 224, 0, 32, 32, numberHandle, ((float)32 / (float)320), 1, 0.0f, 0xFFFFFFFF);
				}
				if (score[i] == 8)
				{
					Novice::DrawSpriteRect(840 + i * 32, 0, 256, 0, 32, 32, numberHandle, ((float)32 / (float)320), 1, 0.0f, 0xFFFFFFFF);
				}
				if (score[i] == 9)
				{
					Novice::DrawSpriteRect(840 + i * 32, 0, 288, 0, 32, 32, numberHandle, ((float)32 / (float)320), 1, 0.0f, 0xFFFFFFFF);
				}
			}

			Novice::DrawSprite(0, 0, bgTexHundle[3], 1, 1, 0, WHITE);
			//遷移用の黒い四角
			Novice::DrawBox((int)sceneTrans.Center.X, (int)sceneTrans.Center.Y, WINDOW_WIDTH, WINDOW_HEIGHT, 0, sceneTrans.Color, kFillModeSolid);
			
			break;
		}

		Novice::ScreenPrintf(0, 0, "flag = %d", sceneTransFlag);
		Novice::ScreenPrintf(0, 20, "color = %x", sceneTrans.Color);

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