#pragma once
#include "enemy.h"

#include "player.h"

#include "myUtility/data.h"
#include "../../lib/gplib.h"
#include "effect.h"

namespace game
{
	Enemy::Enemy(const std::string& objectName)
		:
		Object(objectName),
		backCount_(0)
	{
		loatData();
	}

	void Enemy::loatData()
	{
		std::string pathName = "res/data/enemy.txt";
		kuma_.loadDataFromFile("kuma=", pathName);
		speed_ = myLib::getFloatFromFile("speed=", pathName);
		lifeMax_ = myLib::getIntFromFile("lifeMax=", pathName);
		attackPow_ = myLib::getIntFromFile("attackPow=", pathName);
		rushPow_ = myLib::getIntFromFile("rushPow=", pathName);
		colliderRadius_ = myLib::getFloatFromFile("colliderRadius=", pathName);
		backFrame_ = myLib::getIntFromFile("backFrame=", pathName);
	}
	void Enemy::walk()
	{
		float table[4] = { 0.f, 2.f, 3.f, 1.f };
		kuma_.setSrcX(table[anim_.xNo_]);
	}
	void Enemy::attack()
	{
		float table[4] = { 0.f, 1.f ,2.f};
		kuma_.setSrcX(table[anim_.xNo_]);
		if (anim_.xNo_ == 1)
		{
			auto p = getObjectFromRoot("1P");
			auto player = ci_ext::weak_to_shared<Player>(p);
			player->getDamage(attackPow_);
		}
	}
	void Enemy::Rush()
	{
		float table[4] = { 0.f,  1.f };
		kuma_.setSrcX(table[anim_.xNo_]);

	}
	void Enemy::changeStateToBack()
	{
		kuma_.setVelocity(ci_ext::Vec3f(4*speed_, 0.f, 0.f));
		anim_.setAnimation(2, 0.2f);
		kuma_.setSrcY(3.f);
		gplib::se::DSound_Play("backSE");
		state_ = State::Back;
	}
	void Enemy::back()
	{
		float table[2] = { 0.f, 1.f};
		kuma_.setSrcX(table[anim_.xNo_]);
		backCount_++;
		if (backCount_ >= backFrame_)
		{
			backCount_ = 0;
			kuma_.setVelocity(ci_ext::Vec3f(-speed_, 0.f, 0.f));
			kuma_.setSrcY(0.f);
			anim_.setAnimation(4, 0.1f);
			state_ = State::Walk;

		}

	}
	bool Enemy::isNotBack()
	{
		if (state_ != State::Back) return true;
		return false;
	}

	//åFÇÃâeÇï`Ç≠
	void Enemy::renderShadow()
	{

		gplib::graph::Draw_GraphicsLeftTop(
			kuma_.posX() - kuma_.srcSizeX() / 2.f,
			kuma_.posY() + kuma_.srcSizeY() / 2.f, kuma_.posZ() + 0.1f,
			"kuma",
			anim_.xNo_*(int)kuma_.srcSizeX(), (int)kuma_.srcSizeY(),
			(int)kuma_.srcSizeX(), (int)-kuma_.srcSizeY(),
			0, nullptr, 1.f, 0.5f,
			255, 0, 0, 0);
	}
	void Enemy::renderLifeBar()
	{
		const int Enemybar_Length = 100;
		const int Enemybar_Height = 10;

		int now = life_*Enemybar_Length / lifeMax_;
		int x = (int)kuma_.posX() - Enemybar_Length / 2;
		int y = (int)kuma_.posY();
		float z = kuma_.posZ() - 0.1f;

		gplib::graph::Draw_Box(x, y,
			x + Enemybar_Length, y + Enemybar_Height, z,
			ARGB(255, 255, 0, 0), ARGB(255, 0, 0, 0), 3, 1);
		gplib::graph::Draw_Box(x, y,
			x + now, y + Enemybar_Height, z,
			ARGB(255, 0, 255, 0), ARGB(255, 0, 0, 0), 3, 1);
	}

	//Ç†ÇΩÇËîªíËóp
	float Enemy::colliderRadius()
	{
		return colliderRadius_;
	}
	float Enemy::posX(){ return kuma_.posX(); }
	float Enemy::posY(){ return kuma_.posY(); }

	void Enemy::distanceCheck()
	{
		if (state_ != State::Attack && state_ != State::Back)
		{
			auto p = getObjectFromRoot("1P");
			auto player = ci_ext::weak_to_shared<Player>(p);
			float distance = player->posX() + player->playerRadius();
			if (kuma_.posX() < distance)
			{

				if (state_ == State::Rush)
				{
					//ïIçUåÇ
					player->getDamage(rushPow_);
				}
				state_ = State::Attack;
				kuma_.setVelocity(ci_ext::Vec3f::zero());
				anim_.setAnimation(3, 0.1f);
				kuma_.setSrcY(1.f);
				

			}
		}
	}


	void Enemy::init()
	{
		
		life_ = lifeMax_;
		state_ = State::Walk;

		//startà íuåvéZ
		float x = (float)gplib::system::WINW + kuma_.srcSizeX();
		float y = gplib::math::GetRandom(kuma_.posY(), ((float)gplib::system::WINH - kuma_.srcSizeX() / 2.f));
		float z = ((float)gplib::system::WINH - y) / (float)gplib::system::WINH;
		kuma_.setPosWithZ(ci_ext::Vec3f(x, y, z));
		//ï‡Ç≠ÉXÉsÅ[Éhë„ì¸
		kuma_.setVelocity(ci_ext::Vec3f(-speed_, 0.f, 0.f));

		anim_.setAnimation(4, 0.1f);
		kuma_.visible();
		
	}
	void Enemy::update()
	{
		switch (state_)
		{
		case game::Enemy::State::Walk:
			walk();
			break;
		case game::Enemy::State::Attack:
			attack();
			break;
		case game::Enemy::State::Rush:
			Rush();
			break;
		case game::Enemy::State::Back:
			back();
			break;
		default:
			break;
		}
		distanceCheck();
		//lifeÇ™îºï™à»â∫Ç…Ç»ÇÈÇ∆ã•ñ\Ç»ïIçUåÇ
		if (life_ < lifeMax_ / 2 && state_ == State::Walk)
		{
			anim_.setAnimation(2, 0.3f);
			kuma_.setSrcY(2);
			kuma_.setVelocity(ci_ext::Vec3f(-4 * speed_, 0.f, 0.f));
			state_ = State::Rush;
		}

		
		anim_.update();
		kuma_.update();
	

		

	}
	void Enemy::render()
	{
		renderShadow();
		kuma_.render();
		renderLifeBar();
	}
	void Enemy::getDamage(int damage)
	{
		life_ -= damage;
		if (life_ <= 0)
		{
			effect::Create((int)kuma_.posX(), (int)kuma_.posY(), effect::EFFECTTYPE::BOMB);
			gplib::se::DSound_Play("break2");
			auto p = getObjectFromRoot("1P");
			auto player = ci_ext::weak_to_shared<Player>(p);
			player->beamCharge();
			kill();
		}
	}
	void Enemy::beamDamage(int damage)
	{
		life_-=damage;
		if (life_ <= 0)
		{
			effect::Create((int)kuma_.posX(), (int)kuma_.posY(), effect::EFFECTTYPE::PBOMB);
			gplib::se::DSound_Play("break2");
			kill();
		}
	}
}