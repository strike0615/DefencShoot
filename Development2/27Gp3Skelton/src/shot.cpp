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
		damage_(1+damage),		//�Œ�ł��P�̍U����
		birthPos_(birthPos),
		targetPos_(targetPos)
	{
		loadData();
	}

	//data�ǂݍ���
	void Shot::loadData()
	{
		std::string pathName = "res/data/shot.txt";
		shot_.loadDataFromFile("shot=", pathName);
		speed_ = myLib::getFloatFromFile("speed=", pathName);
		colliderRadius_ = myLib::getFloatFromFile("colliderRadius=", pathName);
	}

	//�����v�Z
	float Shot::calculateAngle()
	{
		return gplib::math::DegreeOfPoints2(birthPos_.x(),birthPos_.y(),targetPos_.x(),targetPos_.y());
	}

	//��ʊO�ɏo�������
	void Shot::killByOutCamera()
	{
		if (!gplib::camera::InScreenCameraCenter(shot_.posX(), shot_.posY(), shot_.srcSizeX(), shot_.srcSizeY()))
		{
			kill();
		}
	}

	//�摜����
	void Shot::turning()
	{
		shot_.offsetDegree(10.f);
	}

	//�����蔻��
	void Shot::checkHit()
	{
		auto enemys = getObjectsFromRoot({ "enemy" });
		for (auto e : enemys)
		{
			auto enemy = ci_ext::weak_to_shared<Enemy>(e);
			//���苗��
			float hitDistance = colliderRadius_ + enemy->colliderRadius();
			//���݋���
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
		//�����ꏊ�ݒ�
		shot_.setPos(birthPos_);
		//�ړ��ݒ�
		shot_.setVelocity(calculateAngle(), speed_);
		//������l��
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