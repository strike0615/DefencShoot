#pragma once

#include"../../../../lib/gplib.h"
#include"../../../../lib/ci_ext/vec3.hpp"

namespace myLib
{


	struct Fade
	{
		bool fadeIn_;
		bool fadeOut_;
		float alpha_;
		float speed_;
		ci_ext::Vec3f pos_;


		Fade()
			:
			fadeIn_(false),
			fadeOut_(false),
			alpha_(255.f),
			speed_(0.f),
			pos_(ci_ext::Vec3f::zero())
		{
		}



		void startFadeIN(const float& speed)
		{
			if (alpha_ < 254.f) alpha_ = 254.f;
			pos_.z(0.1f);
			speed_ = speed;
			fadeIn_ = true;
		}
		void startFadeOut(const float& speed)
		{
			if (alpha_ > 0.f) alpha_ = 0.f;
			speed_ = speed;
			pos_.z(0.1f);
			fadeOut_ = true;
		}

		bool UpdateFade()
		{
			if (fadeIn_)
			{

				alpha_ -= speed_;
				if (alpha_ < 0)
				{
					alpha_ = 0.f;
					pos_.z(1.f);
					fadeIn_ = false;
				}
			}
			if (fadeOut_)
			{
				alpha_ += speed_;
				if (alpha_ > 254)
				{
					alpha_ = 254.f;
					pos_.z(1.f);
					fadeOut_ = false;
				}
			}
			return fadeIn_ | fadeOut_;

		}
		
		void render()
		{
			if (fadeIn_ || fadeOut_)
			{

				gplib::graph::Draw_Box(
					pos_.ix() - gplib::system::WINW, pos_.iy() - gplib::system::WINH,
					pos_.ix() + gplib::system::WINW, pos_.iy() + gplib::system::WINH,
					pos_.z(),
					ARGB((int)alpha_, 0, 0, 0), ARGB((int)alpha_, 0, 0, 0),
					1, 1
					);
			}
		}

	};
}