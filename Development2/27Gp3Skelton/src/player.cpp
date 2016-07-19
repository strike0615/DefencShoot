#pragma once
#include "player.h"
#include "../../lib/gplib.h"
#include "myUtility/data.h"
#include "effect.h"

#include "enemy.h"
#include "beam.h"
#include "shot.h"
namespace game
{
	Player::Player(const std::string& objectName)
		:
		Object(objectName),
		beamCharge_(0),
		pow_(0.f),
		impactCharge_(impactCoolFrame_),
		isGameOver(false)
	{
		loadData();

	}

	//テキストからデータを読み取ります
	void Player::loadData()
	{
		std::string pathName = "res/data/player.txt";
		yashiro_.loadDataFromFile("yashiro=", pathName);
		lifeMax_ = myLib::getIntFromFile("life=", pathName);
		life_ = lifeMax_;
		impactCoolFrame_ = myLib::getIntFromFile("impactCoolFrame=", pathName);
		powMax_ = myLib::getIntFromFile("powMax=", pathName);
		beamChargeMax_ = myLib::getIntFromFile("beamChargeMax=", pathName);
		colliderRadius_ = myLib::getFloatFromFile("colliderRadius=", pathName);

		std::string tag[3] = { "auraName0=", "auraName1=", "auraName2=" };
		for (int i = 0; i < 3; i++)
			auraName[i] = myLib::getStringFromFile(tag[i], pathName);

		aura_.loadDataFromFile("yashiro=", pathName);
		aura_.setResName(auraName[0]);

	}
	
	void Player::createShot()
	{
		//左クリックでパワー計算
		if (gplib::input::CheckPress(gplib::input::KEY_MOUSE_LBTN))
		{
			pow_ += 1.f;
			if (pow_ >= powMax_) pow_ = (float)powMax_;
			//エフェクト
			effect::Create((int)yashiro_.posX() + 100, (int)yashiro_.posY(), effect::EFFECTTYPE::SPLASH);

		}
		if (gplib::input::CheckPull(gplib::input::KEY_MOUSE_LBTN))
		{

			POINT mouse = gplib::input::GetMousePosition();
			insertAsChild(new Shot(
				"shot", 
				(int)pow_/10, 
				ci_ext::Vec3f(yashiro_.posX() + 100.f, yashiro_.posY(), 0.f),		//生成場所
				ci_ext::Vec3f((float)mouse.x, (float)mouse.y, 0.f)));				//ターゲット
			pow_ = 0;
		}
	}
	void Player::createBeam()
	{
		if (beamCharge_ == beamChargeMax_)
		{
			if (gplib::input::CheckPull(gplib::input::KEY_MOUSE_RBTN))
			{
				effect::Create((int)yashiro_.posX() + 100, (int)yashiro_.posY() + 100, effect::EFFECTTYPE::EFFECT_P1);
				insertAsChildSleep(new Beam("beam"), 160);
				beamCharge_ = 0;

			}
		}
	}
	void Player::chargeImpact()
	{
		if (impactCharge_ >0)
		{
			impactCharge_--;


			int number=int((99 + impactCharge_ / 2) / 100);
			aura_.setResName(auraName[number]);
		}
		
	}


	void Player::pushEnemy()
	{
		auto enemys = getObjectsFromRoot({ "enemy" });
		for (auto e : enemys)
		{
			auto enemy = ci_ext::weak_to_shared<Enemy>(e);
			//判定距離
			float hitDistance = colliderRadius_ + enemy->colliderRadius()+100.f;
			//現在距離
			float distance = gplib::math::Distance2(enemy->posX(), enemy->posY(), yashiro_.posX(), yashiro_.posY());
			if (distance < hitDistance && enemy->isNotBack())
			{
				enemy->changeStateToBack();
			}
		}
	}
	void Player::createImpact()
	{
		if (gplib::input::CheckPush(gplib::input::KEY_BTN1) && impactCharge_ <= 0)
		{
			effect::Create((int)yashiro_.posX(), (int)yashiro_.posY(), effect::EFFECTTYPE::WARNINGSHAD);
			gplib::se::DSound_Play("skill");

			pushEnemy();
			impactCharge_ = impactCoolFrame_;

		}
	}

	void Player::renderLifeBar()
	{
		//life bar
		const int LifeBar_Length = 200;
		const int Lifebar_Height = 30;
		int now = life_ * LifeBar_Length / lifeMax_;
		//下敷きの赤い部分
		gplib::graph::Draw_Box((int)(yashiro_.posX() - LifeBar_Length / 2), (int)(yashiro_.posY() + 30),
			(int)(yashiro_.posX() + LifeBar_Length / 2), (int)(yashiro_.posY() + 30 + Lifebar_Height), 0.f,
			ARGB(255, 255, 0, 0), ARGB(255, 0, 0, 0), 3, 1);
		//緑の現在life表示
		gplib::graph::Draw_Box((int)(yashiro_.posX() - LifeBar_Length / 2), (int)(yashiro_.posY() + 30),
			(int)(yashiro_.posX() - LifeBar_Length / 2 + now), (int)(yashiro_.posY() + 30 + Lifebar_Height), 0.f,
			ARGB(255, 0, 255, 0), ARGB(255, 0, 0, 0), 3, 1);
	}
	void Player::renderPowerBar()
	{
		//ためた攻撃力
		const int PowBar_Length = 100;
		const int PowBar_Height = 10;
		int now = (int)(pow_ * PowBar_Length / powMax_);
		//下敷きの赤い部分
		gplib::graph::Draw_Box((int)(yashiro_.posX() - PowBar_Length / 2), (int)yashiro_.posY(),
			(int)(yashiro_.posX() + PowBar_Length / 2), (int)yashiro_.posY() + PowBar_Height, 0.f,
			ARGB(255, 255, 0, 0), ARGB(255, 0, 0, 0), 3, 1);
		//緑の現在life表示
		gplib::graph::Draw_Box((int)(yashiro_.posX() - PowBar_Length / 2), (int)yashiro_.posY(),
			(int)(yashiro_.posX() - PowBar_Length / 2 + now), (int)yashiro_.posY() + PowBar_Height, 0.f,
			ARGB(255, 0, 255, 0), ARGB(255, 0, 0, 0), 3, 1);
	}
	void Player::renderBeamBar()
	{

		//ためたcharge
		const int ChargeBar_Length = 100;
		const int ChargeBar_Height = 10;
		int now = (int)(beamCharge_ * ChargeBar_Length / beamChargeMax_);
		//下敷きの赤い部分
		gplib::graph::Draw_Box((int)(yashiro_.posX() - ChargeBar_Length / 2), (int)yashiro_.posY() - 30,
			(int)(yashiro_.posX() + ChargeBar_Length / 2), (int)yashiro_.posY() + ChargeBar_Height - 30, 0.f,
			ARGB(255, 255, 0, 0), ARGB(255, 0, 0, 0), 3, 1);
		//緑の現在life表示
		gplib::graph::Draw_Box((int)(yashiro_.posX() - ChargeBar_Length / 2), (int)yashiro_.posY() - 30,
			(int)(yashiro_.posX() - ChargeBar_Length / 2 + now), (int)yashiro_.posY() + ChargeBar_Height - 30, 0.f,
			ARGB(255, 0, 255, 0), ARGB(255, 0, 0, 0), 3, 1);
	}


	//あたり判定用
	float Player::playerRadius()
	{
		return colliderRadius_;
	}
	float Player::posX(){ return yashiro_.posX(); }
	float Player::posY(){ return yashiro_.posY(); }
	void Player::beamCharge()
	{
		beamCharge_++;
		if (beamCharge_ >= beamChargeMax_)
		{
			beamCharge_ = beamChargeMax_;
		}
	}

	//playerのダメージ処理、外部から呼び出しできます
	void Player::getDamage(int damage)
	{
		if (isGameOver == false)
		{
			life_ -= damage;
		}
		if (life_ < 0 && isGameOver==false)
		{
			life_ = 0;
			isGameOver = true;
			gplib::se::DSound_Play("hukitobi");
		}
	}


	void Player::init()
	{
		yashiro_.visible();
		aura_.visible();
	}
	void Player::render()
	{
		yashiro_.render();
		aura_.render();
		renderLifeBar();
		renderPowerBar();
		renderBeamBar();
	}
	void Player::update()
	{
		
		if (!isGameOver)
		{
			createImpact();
			chargeImpact();
			createShot();
			createBeam();
		}
		else
		{
			//gameover処理
			gplib::font::Draw_FontText(100, 100, 0.0f, "あなたは社を守れませんでした...Aボタンでtitleに移行します", ARGB(255, 0, 0, 0), 0);

			//社の爆発演出
			if (gplib::math::GetRandom(0, 10) == 0)
			{
				int ranX = gplib::math::GetRandom(-150, 150);
				int ranY = gplib::math::GetRandom(-200, 200);
				effect::Create((int)yashiro_.posX() + ranX, (int)yashiro_.posY() + ranY, effect::EFFECTTYPE::PBOMB);
			}
		}


	}


}