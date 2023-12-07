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
				WHITE
			},
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

	Object sceneTrans =
	{
		{0,0},
		{0,0},
		0,
		0,
		0x00000000
	};
	int sceneTransFlag = 0;

	/*音楽*/
	const int audio = 20;
	int audioHundle[audio] =
	{
		Novice::LoadAudio("./Title.mp3"),//0
		Novice::LoadAudio("./GamePlay1.mp3"),//1
		Novice::LoadAudio("./GamePlay2.mp3"),//2
		Novice::LoadAudio("./GameOver.mp3"),//3
	};

	int titleBGMplay = -1;
	int playBGMplay = -1;
	int gameoverplay = -1;

	int score[4] = { 0 };

	/*リソース関連*/
	//プレイヤー
	int playerTexHundle[] =
	{
		Novice::LoadTexture("./Resources./Pictures./player_Left.png"),
		Novice::LoadTexture("./Resources./Pictures./player_Right.png")
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
		Novice::LoadTexture("./Resources./Pictures./gamePlay.png"),
		Novice::LoadTexture("./Resources./Pictures./stage.png"),
		Novice::LoadTexture("./Resources./Pictures./gameOver.png")
	};
	//スコア用の数字
	int numTexHundle[] =
	{
		Novice::LoadTexture("./Resources./Pictures./num_0.png"),
		Novice::LoadTexture("./Resources./Pictures./num_1.png"),
		Novice::LoadTexture("./Resources./Pictures./num_2.png"),
		Novice::LoadTexture("./Resources./Pictures./num_3.png"),
		Novice::LoadTexture("./Resources./Pictures./num_4.png"),
		Novice::LoadTexture("./Resources./Pictures./num_5.png"),
		Novice::LoadTexture("./Resources./Pictures./num_6.png"),
		Novice::LoadTexture("./Resources./Pictures./num_7.png"),
		Novice::LoadTexture("./Resources./Pictures./num_8.png"),
		Novice::LoadTexture("./Resources./Pictures./num_9.png"),
	};

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
			if (keys[DIK_A] || keys[DIK_LEFT])
			{
				player.Velocity.X = -1;				
			}
			else if (keys[DIK_D] || keys[DIK_RIGHT])
			{
				player.Velocity.X = 1;
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
					if (runPower >= -6.0f)
					{
						runPower -= 0.2f;
					}
				}
				else if (keys[DIK_D])
				{
					if (runPower <= 6.0f)
					{
						runPower += 0.2f;
					}
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
			player.Center.X += (player.Velocity.X * player.Spd) + runPower;

			//自機の移動制限
			if (player.Center.X - player.Rad < 0)
			{
				player.Center.X = player.Rad;
			}
			else if(player.Center.X + player.Rad > WINDOW_WIDTH)
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
								obj[i].Hp -= 1;
							}
							//ダッシュ中にオブジェクトと接触したらオブジェクトが落ちる
							if (runFlag && !breakFlag[i])
							{
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
				else if(ownerTimer >= 660 && ownerTimer <= 742)
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

			

			break;
		}
		
		/// ↑更新処理ここまで
		

		
		/// ↓描画処理ここから

		switch (scene)
		{
		case TITLE: //タイトル
			/*オーディオ関係*/
			gameoverplay = -1;

			if (Novice::IsPlayingAudio(titleBGMplay) == 0 || titleBGMplay == -1)
			{
				if (scene == TITLE)
				{
					titleBGMplay = Novice::PlayAudio(audioHundle[0], 1, 0.3f);
				}
			}
			
			//
			Novice::DrawSprite(0, 0, bgTexHundle[0], 1, 1, 0, WHITE);
			//遷移用の黒い四角
			Novice::DrawBox((int)sceneTrans.Center.X, (int)sceneTrans.Center.Y, WINDOW_WIDTH, WINDOW_HEIGHT, 0, sceneTrans.Color, kFillModeSolid);

			break;

		case TUTORIAL: //チュートリアル			
			
			//遷移用の黒い四角
			Novice::DrawBox((int)sceneTrans.Center.X, (int)sceneTrans.Center.Y, WINDOW_WIDTH, WINDOW_HEIGHT, 0, sceneTrans.Color, kFillModeSolid);
			
			Novice::ScreenPrintf(0, 0, "flag = %d",sceneTransFlag);

			break;

		case GAMEPLAY: //ゲームプレイ
			/*オーディオ関係*/
			if (Novice::IsPlayingAudio(titleBGMplay) == 1)
			{
				Novice::StopAudio(titleBGMplay);
			}
			if (Novice::IsPlayingAudio(playBGMplay) == 0 || playBGMplay == -1)
			{
				if (scene == GAMEPLAY)
				{
					playBGMplay = Novice::PlayAudio(audioHundle[1], 1, 0.5);
				}
			}

			//ゲームオーバーへの遷移
			if (!isPAlive)
			{
				Novice::StopAudio(playBGMplay);
				Novice::StopAudio(titleBGMplay);
				scene = GAMEOVER;
			}

			//背景
			Novice::DrawSprite(0, 0, bgTexHundle[1], 1, 1, 0, WHITE);
			//飼い主
			if (ownerTimer <= 650)
			{
				Novice::DrawSprite(280, 0, ownerTexHundle[0], 1, 1, 0, WHITE);
			}
			else if(ownerTimer > 650)
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
				if (player.Velocity.X <= 0)
				{
					Novice::DrawSprite(int(player.Center.X - player.Rad), int(player.Center.Y - player.Rad), playerTexHundle[0], 1, 1, 0, WHITE);
				}
				if (player.Velocity.X > 0)
				{
					Novice::DrawSprite(int(player.Center.X - player.Rad), int(player.Center.Y - player.Rad), playerTexHundle[1], 1, 1, 0, WHITE);
				}
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
						Novice::DrawSprite(int(obj[i].Info.Center.X - obj[i].Info.Rad * 2), int(obj[i].Info.Center.Y - obj[i].Info.Rad * 2), objTexHundle[2], 1, 1, 0, obj[i].Info.Color);
					}
					else if (obj[i].Hp <= 1)
					{
						Novice::DrawSprite(int(obj[i].Info.Center.X - obj[i].Info.Rad * 2), int(obj[i].Info.Center.Y - obj[i].Info.Rad * 2), objTexHundle[3], 1, 1, 0, obj[i].Info.Color);
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
					Novice::DrawSprite(1100 + i * 32, 0, numTexHundle[0], 1, 1, 0, WHITE);
				}
				if (score[i] == 1)
				{
					Novice::DrawSprite(1100 + i * 32, 0, numTexHundle[1], 1, 1, 0, WHITE);
				}
				if (score[i] == 2)
				{
					Novice::DrawSprite(1100 + i * 32, 0, numTexHundle[2], 1, 1, 0, WHITE);
				}
				if (score[i] == 3)
				{
					Novice::DrawSprite(1100 + i * 32, 0, numTexHundle[3], 1, 1, 0, WHITE);
				}
				if (score[i] == 4)
				{
					Novice::DrawSprite(1100 + i * 32, 0, numTexHundle[4], 1, 1, 0, WHITE);
				}
				if (score[i] == 5)
				{
					Novice::DrawSprite(1100 + i * 32, 0, numTexHundle[5], 1, 1, 0, WHITE);
				}
				if (score[i] == 6)
				{
					Novice::DrawSprite(1100 + i * 32, 0, numTexHundle[6], 1, 1, 0, WHITE);
				}
				if (score[i] == 7)
				{
					Novice::DrawSprite(1100 + i * 32, 0, numTexHundle[7], 1, 1, 0, WHITE);
				}
				if (score[i] == 8)
				{
					Novice::DrawSprite(1100 + i * 32, 0, numTexHundle[8], 1, 1, 0, WHITE);
				}
				if (score[i] == 9)
				{
					Novice::DrawSprite(1100 + i * 32, 0, numTexHundle[9], 1, 1, 0, WHITE);
				}
			}
			//遷移用の黒い四角
			Novice::DrawBox((int)sceneTrans.Center.X, (int)sceneTrans.Center.Y, WINDOW_WIDTH, WINDOW_HEIGHT, 0, sceneTrans.Color, kFillModeSolid);

			break;

		case GAMEOVER: //ゲームオーバー
			/*オーディオ関係*/
			if (gameoverplay == -1)
			{
				if (scene == GAMEOVER)
				{
					gameoverplay = Novice::PlayAudio(audioHundle[3], 0, 0.5);
				}
			}
			//遷移
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
					Novice::StopAudio(gameoverplay);
					sceneTransFlag = 2;
					scene = TITLE;
				}
			}

			Novice::DrawSprite(0, 0, bgTexHundle[3], 1, 1, 0, WHITE);
			//遷移用の黒い四角
			Novice::DrawBox((int)sceneTrans.Center.X, (int)sceneTrans.Center.Y, WINDOW_WIDTH, WINDOW_HEIGHT, 0, sceneTrans.Color, kFillModeSolid);
			
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