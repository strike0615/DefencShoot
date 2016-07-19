#pragma once



namespace myLib
{
	struct Animator
	{
		int		xNo_;			//現在のアニメ列番号　この値は0,1,2,3,4,とカウントするだけ
		int		yNo_;			//現在のアニメ行番号
		float	count_;			//アニメ更新カウンタ
		float	speed_;			//アニメ更新速度
		int		max_;			//アニメ枚数　この値にnoが来ると0からカウント


		Animator::Animator()
			:
			xNo_(0),
			yNo_(0),
			count_(0.f),
			speed_(0.f),
			max_(0)
		{
		}
		void Animator::init()
		{
			xNo_ = 0;
			yNo_ = 0;
			count_ = 0.f;
			speed_ = 0.f;
			max_ = 0;
		}
		void Animator::update()
		{
			count_ += speed_;
			if (count_ >= 1.f)
			{
				count_ = 0;
				xNo_++;
			}
			if (xNo_ >= max_)
			{
				xNo_ = 0;
			}
		}
		void Animator::setAnimation(const int max, const float speed)
		{
			count_ = 0.f;
			xNo_ = 0;
			max_ = max;
			speed_ = speed;
		}
		
	};

}