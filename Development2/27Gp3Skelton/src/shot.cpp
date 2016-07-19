#pragma once

#include "shot.h"
#include "enemy.h"
#include "myUtility/data.h"
#include "../../lib/gplib.h"
#include "effect.h"

namespace game
{
	
	Shot::Shot(const std::string& objectName, const int damage, ci_ext::Vec3f birthPos, ci_ext::Vec3f targetPos)
		:
		Object(objectName),
		damage_(1+damage),		//Å’á‚Å‚à‚P‚ÌUŒ‚—Í
		birthPos_(birthPos),
		targetPos_(targetPos)
	{
		loadData();
	}

	//data“Ç‚İ‚Ş
	void Shot::loadData()
	{
		std::string pathName = "res/data/shot.txt";
		shot_.loadDataFromFile("shot=", pathName);
		speed_ = myLib::getFloatFromFile("speed=", pathName);
		colliderRadius_ = myLib::getFloatFromFile("colliderRadius=", pathName);
	}

	//•ûŒüŒvZ
	float Shot::calculateAngle()
	{
		return gplib::math::DegreeOfPoints2(birthPos_.x(),birthPos_.y(),targetPos_.x(),targetPos_.y());
	}

	//‰æ–ÊŠO‚Éo‚½‚çÁ‹
	void Shot::killByOutCamera()
	{
		if (!gplib::camera::InScreenCameraCenter(shot_.posX(), shot_.posY(), shot_.srcSizeX(), shot_.srcSizeY()))
		{
			kill();
		}
	}

	//‰æ‘œ‚ğ‰ñ‚·
	void Shot::turning()
	{
		shot_.offsetDegree(10.f);
	}

	//‚ ‚½‚è”»’è
	void Shot::checkHit()
	{
		auto enemys = getObjectsFromRoot({ "enemy" });
		for (auto e : enemys)
		{
			auto enemy = ci_ext::weak_to_shared<Enemy>(e);
			//”»’è‹——£
			float hitDistance = colliderRadius_ + enemy->colliderRadius();
			//Œ»İ‹——£
			float distance = gplib::math::Distance2(enemy->posX(), enemy->posY(), shot_.posX(), shot_.posY());
			if (distance < hitDistance)
			{
				effect::Create((int)shot_.posX()+(int)colliderRadius_, (int)shot_.posY(), effect::EFFECTTYPE::PBOMB);
				enemy->getDamage(damage_);
				gplib::se::DSound_Play("break1");
				kill();
			}
		}
	}


	void Shot::init()
	{
		//¶¬êŠİ’è
		shot_.setPos(birthPos_);
		//ˆÚ“®İ’è
		shot_.setVelocity(calculateAngle(), speed_);
		//Œ©‚¦‚é—l‚É
		shot_.visible();
	}
	void Shot::render()
	{
		shot_.render();
	}
	void Shot::update()
	{
		turning();
		checkHit();
		shot_.update();
	}

}