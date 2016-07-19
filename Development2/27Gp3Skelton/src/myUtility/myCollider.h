#pragma once
#include "../../../lib/ci_ext/vec3.hpp"

namespace myLib
{
	using namespace ci_ext;
	class rectCollider
	{
		Vec3f pos_;
		Vec3f offset_;
		float height_;
		float width_;

	public:

		rectCollider::rectCollider()
			:
			pos_(Vec3f::zero()),
			offset_(Vec3f::zero()),
			height_(0.f),
			width_(0.f)
		{
		}

		void setHeight(const float& height)
		{
			height_ = height;
		}
		void setWidth(const float& with)
		{
			width_ = with;
		}
		void setPos(const Vec3f& pos)
		{
			pos_ = pos;
		}
		void setPos(const float& x, const float& y, const float& z)
		{
			pos_ = Vec3f(x, y, z);
		}
		void setPos(const float& x, const float& y)
		{
			pos_ = Vec3f(x, y, pos_.z());
		}
		void offset(const Vec3f& offset)
		{
			offset_ = offset;
		}
		void offset(const float& x, const float& y, const float& z)
		{
			offset_ = Vec3f(x, y, z);
		}
		void fllow(const Vec3f& target)
		{
			pos_ = target + offset_;
		}
		Vec3f pos(){ return pos_; }
		float posX(){ return pos_.x(); }
		float posY(){ return pos_.y(); }
		float height(){ return height_; }
		float with(){ return width_; }
		float heightHalf(){ return height_/2.f; }
		float withHalf(){ return width_/2.f; }
	};
	class roundCollider
	{
		Vec3f pos_;
		Vec3f offset_;
		float radius_;

	public:

		roundCollider::roundCollider()
			:
			pos_(Vec3f::zero()),
			offset_(Vec3f::zero()),
			radius_(0.f)
		{

		}
		void setRadius(const float& radius)
		{
			radius_ = radius;
		}
		void setPos(const Vec3f& pos)
		{
			pos_ = pos;
		}
		void setPos(const float& x, const float& y, const float& z)
		{
			pos_ = Vec3f(x, y, z);
		}
		void setPos(const float& x, const float& y)
		{
			pos_ = Vec3f(x, y, pos_.z());
		}
		void offset(const Vec3f& offset)
		{
			offset_ = offset;
		}
		void offset(const float& x, const float& y, const float& z)
		{
			offset_ = Vec3f(x, y, z);
		}
		void fllow(const Vec3f& target)
		{
			pos_ = target + offset_;
		}
		Vec3f pos(){ return pos_; }
		float posX(){ return pos_.x(); }
		float posY(){ return pos_.y(); }
		float radius(){ return radius_; }
	};


	bool checkRoundToRound(roundCollider& col1, roundCollider& col2);
}