#pragma once



namespace myLib
{
	struct Animator
	{
		int		xNo_;			//���݂̃A�j����ԍ��@���̒l��0,1,2,3,4,�ƃJ�E���g���邾��
		int		yNo_;			//���݂̃A�j���s�ԍ�
		float	count_;			//�A�j���X�V�J�E���^
		float	speed_;			//�A�j���X�V���x
		int		max_;			//�A�j�������@���̒l��no�������0����J�E���g


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