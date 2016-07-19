#pragma once
#include "../../lib/ci_ext/object.hpp"

#include "myUtility/movingObj.h"
#include "myUtility/myCollider.h"

namespace game
{
	class Enemy : public ci_ext::Object
	{
	private:
		myLib::MovingObj kuma_;
		int life_;
		int lifeMax_;
		float speed_;
		int attackPow_;
		int rushPow_;
		myLib::Animator anim_;
		float colliderRadius_;
		int backFrame_;
		int backCount_;
		

		void Enemy::loatData();
		void Enemy::walk();
		void Enemy::attack();
		void Enemy::Rush();
		void Enemy::back();
		void Enemy::renderShadow();
		void Enemy::renderLifeBar();
		void Enemy::distanceCheck();


		


	public:
		enum class State{ Walk, Attack, Rush, Back };
		State state_;


		Enemy(const std::string& objectName);

		bool Enemy::isNotBack();
		void Enemy::changeStateToBack();
		void Enemy::getDamage(int damage);
		void Enemy::beamDamage(int damage);
		float Enemy::colliderRadius();
		float Enemy::posX();
		float Enemy::posY();

		void init() override;
		void render() override;
		void update() override;

	};
}