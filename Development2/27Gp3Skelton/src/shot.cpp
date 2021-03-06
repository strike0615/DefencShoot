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
		damage_(1+damage),		//最低でも１の攻撃力
		birthPos_(birthPos),
		targetPos_(targetPos)
	{
		loadData();
	}

	//data読み込む
	void Shot::loadData()
	{
		std::string pathName = "res/data/shot.txt";
		shot_.loadDataFromFile("shot=", pathName);
		speed_ = myLib::getFloatFromFile("speed=", pathName);
		colliderRadius_ = myLib::getFloatFromFile("colliderRadius=", pathName);
	}

	//方向計算
	float Shot::calculateAngle()
	{
		return gplib::math::DegreeOfPoints2(birthPos_.x(),birthPos_.y(),targetPos_.x(),targetPos_.y());
	}

	//画面外に出たら消去
	void Shot::killByOutCamera()
	{
		if (!gplib::camera::InScreenCameraCenter(shot_.posX(), shot_.posY(), shot_.srcSizeX(), shot_.srcSizeY()))
		{
			kill();
		}
	}

	//画像を回す
	void Shot::turning()
	{
		shot_.offsetDegree(10.f);
	}

	//あたり判定
	void Shot::checkHit()
	{
		auto enemys = getObjectsFromRoot({ "enemy" });
		for (auto e : enemys)
		{
			auto enemy = ci_ext::weak_to_shared<Enemy>(e);
			//判定距離
			float hitDistance = colliderRadius_ + enemy->colliderRadius();
			//現在距離
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
		//生成場所設定
		shot_.setPos(birthPos_);
		//移動設定
		shot_.setVelocity(calculateAngle(), speed_);
		//見える様に
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