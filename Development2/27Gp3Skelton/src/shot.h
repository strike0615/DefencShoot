#pragma once
#include "../../lib/ci_ext/object.hpp"

#include "myUtility/movingObj.h"
#include "myUtility/myCollider.h"

namespace game
{
	class Shot : public ci_ext::Object
	{
		myLib::MovingObj shot_;
		ci_ext::Vec3f birthPos_;
		ci_ext::Vec3f targetPos_;
		float speed_;
		int damage_;
		float colliderRadius_;

		void Shot::loadData();
		float Shot::calculateAngle();
		void Shot::killByOutCamera();
		void Shot::turning();
		void Shot::checkHit();
	public:
		Shot::Shot(const std::string& objectName, const int damage, ci_ext::Vec3f birthPos, ci_ext::Vec3f targetPos);

		void init() override;
		void render() override;
		void update() override;
	};
}