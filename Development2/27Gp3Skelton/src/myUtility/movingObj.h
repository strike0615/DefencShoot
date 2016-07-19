#pragma once


#include "drawObject.hpp"



namespace myLib
{
	class MovingObj : public DrawObject
	{
	protected:
		enum MoveMode{ NORMAL, SKATE };
		MoveMode moveMode_;
		ci_ext::Vec3f velocity_;
		ci_ext::Vec3f delayVelocity_;


		void MovingObj::skatingVelocity();

	public:

		MovingObj::MovingObj();


		void MovingObj::skateVelocityMode();
		void MovingObj::normalVelocityMode();

		void MovingObj::update();



		void MovingObj::setVelocity(const ci_ext::Vec3f& velocity);
		void MovingObj::setVelocity(const float x, const float y, const float z);
		void MovingObj::setVelocity(const float angle, const float speed);
		void MovingObj::setDelayVelocity(const ci_ext::Vec3f& delayVelocity);
		void MovingObj::setDelayVelocity(const float x, const float y, const float z);
		void MovingObj::setDelayVelocity(const float angle, const float speed);
		void MovingObj::offsetVelocity(const ci_ext::Vec3f& velocity);
		void MovingObj::offsetVelocity(const float x, const float y, const float z);


		ci_ext::Vec3f MovingObj::velocity();
	};
}

