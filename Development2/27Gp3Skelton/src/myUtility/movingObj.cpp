#pragma once

#include"movingObj.h"

namespace myLib
{

	MovingObj::MovingObj()
		:
		DrawObject(),
		moveMode_(NORMAL),
		velocity_(Vec3f::zero()),
		delayVelocity_(Vec3f::zero())

	{
	}

	void MovingObj::skateVelocityMode(){ moveMode_ = SKATE; }
	void MovingObj::normalVelocityMode(){ moveMode_ = NORMAL; }

	void MovingObj::update()
	{

		if (moveMode_ == SKATE)
		{
			skatingVelocity();
		}

		DrawObject::pos_ += velocity_;
	}

	void MovingObj::skatingVelocity()
	{
		if (delayVelocity_.x() != velocity_.x())
		{
			if (velocity_.x() - delayVelocity_.x() > 0)
			{
				delayVelocity_ += Vec3f(0.5f, 0.f, 0.f);

				if (velocity_.x() == 0 && delayVelocity_.x() > 0){
					delayVelocity_ = Vec3f(0.f, delayVelocity_.y(), 0.f);
				}
			}

			else if (velocity_.x() - delayVelocity_.x() < 0)
			{
				delayVelocity_ -= Vec3f(0.5f, 0.f, 0.f);

				if (velocity_.x() == 0 && delayVelocity_.x() < 0){
					delayVelocity_ = Vec3f(0.f, delayVelocity_.y(), 0.f);
				}
			}
		}

		if (delayVelocity_.y() != velocity_.y())
		{
			if (velocity_.y() - delayVelocity_.y() > 0)
			{
				delayVelocity_ += Vec3f(0.f, 0.5f, 0.f);

				if (velocity_.y() == 0 && delayVelocity_.y() > 0){
					delayVelocity_ = Vec3f(delayVelocity_.x(), 0.f, 0.f);
				}
			}

			else if (velocity_.y() - delayVelocity_.y() < 0)
			{
				delayVelocity_ -= Vec3f(0.f, 0.5f, 0.f);

				if (velocity_.y() == 0 && delayVelocity_.y() < 0){
					delayVelocity_ = Vec3f(delayVelocity_.x(), 0.f, 0.f);
				}
			}
		}

		velocity_ = delayVelocity_;
	}
	void MovingObj::setVelocity(const Vec3f& velocity)
	{
		velocity_ = velocity;
	}
	void MovingObj::setVelocity(const float x, const float y, const float z)
	{
		velocity_ = Vec3f(x, y, z);
	}
	
	void MovingObj::setVelocity(const float angle, const float speed)
	{
		float x = gplib::math::ROUND_X(angle, speed);
		float y = gplib::math::ROUND_Y(angle, speed);
		velocity_ = Vec3f(x, y, 0.f);
	}
	void MovingObj::setDelayVelocity(const ci_ext::Vec3f& delayVelocity)
	{
		delayVelocity_ = delayVelocity;
	}
	void MovingObj::setDelayVelocity(const float x, const float y, const float z)
	{
		delayVelocity_ = Vec3f(x, y, z);
	}
	void MovingObj::setDelayVelocity(const float angle, const float speed)
	{
		float x = gplib::math::ROUND_X(angle, speed);
		float y = gplib::math::ROUND_Y(angle, speed);
		delayVelocity_ = Vec3f(x, y, 0.f);
	}


	void MovingObj::offsetVelocity(const Vec3f& velocity)
	{
		velocity_ += velocity;
	}
	void MovingObj::offsetVelocity(const float x, const float y, const float z)
	{
		velocity_ += Vec3f(x, y, z);
	}


	Vec3f MovingObj::velocity(){ return velocity_; }


}