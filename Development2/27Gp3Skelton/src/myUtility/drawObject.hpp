#pragma once

#include "../../../lib/ci_ext/vec3.hpp"
#include "../../../lib/ci_ext/color.hpp"
#include "../../../lib/gplib.h"
#include "animator.hpp"

#include <string>

namespace myLib
{
	using namespace ci_ext;

	class DrawObject
	{public:
		enum class Type{Normal,LeftTop,LeftTopNC,NC};
	protected:
		Vec3f pos_;
		Vec3f offsetPos_;
		Vec3f scale_;
		std::string resName_;
		Vec3f src_;
		Vec3f srcSize_;
		Color color_;
		float degree_;
		bool draw_;
		Type type_;
	public:
		DrawObject()
			:
			pos_(Vec3f::zero()),
			offsetPos_(Vec3f::zero()),
			scale_(Vec3f::one()),
			resName_(""),
			src_(Vec3f::zero()),
			srcSize_(Vec3f::zero()),
			color_(Color(0xFFFFFFFF)),
			degree_(0.f),
			draw_(false),
			type_(Type::Normal)
		{
		}
		
		void initDrawObject(

			const Vec3f& pos, const Vec3f& scale,
			const std::string& resName,
			const Vec3f& src, const Vec3f& srcSize
			)
		{
			pos_ = pos;
			scale_ = scale;
			resName_ = resName;
			src_ = src;
			srcSize_ = srcSize;
		}
		void invisible() { draw_ = false; }
		void visible()   { draw_ = true; }
		void changeVisible(bool f) { draw_ = f; }

		void render()
		{
			if (!draw_) return;
			if (resName_ == "") return;
			pos_ += offsetPos_;

			switch (type_)
			{
			case Type::Normal:
				gplib::graph::Draw_Graphics(
					pos_.x(), pos_.y(), pos_.z(),
					resName_,
					src_.ix() * srcSize_.ix(), src_.iy()*srcSize_.iy(),
					srcSize_.ix(), srcSize_.iy(),
					degree_,
					scale_.x(), scale_.y(),
					color_.a(), color_.r(), color_.g(), color_.b()
					);
				break;
			case Type::LeftTop:
				gplib::graph::Draw_GraphicsLeftTop(
					pos_.x(), pos_.y(), pos_.z(),
					resName_,
					src_.ix() * srcSize_.ix(), src_.iy()*srcSize_.iy(),
					srcSize_.ix(), srcSize_.iy(),
					degree_,
					nullptr,
					scale_.x(), scale_.y(),
					color_.a(), color_.r(), color_.g(), color_.b()
					);
				break;
			case Type::LeftTopNC:
				gplib::graph::Draw_GraphicsLeftTopNC(
					pos_.x(), pos_.y(), pos_.z(),
					resName_,
					src_.ix() * srcSize_.ix(), src_.iy()*srcSize_.iy(),
					srcSize_.ix(), srcSize_.iy(),
					degree_,
					nullptr,
					scale_.x(), scale_.y(),
					color_.a(), color_.r(), color_.g(), color_.b()
					);
				break;
			case Type::NC:
				gplib::graph::Draw_GraphicsNC(
					pos_.x(), pos_.y(), pos_.z(),
					resName_,
					src_.ix() * srcSize_.ix(), src_.iy()*srcSize_.iy(),
					srcSize_.ix(), srcSize_.iy(),
					degree_,
					scale_.x(), scale_.y(),
					color_.a(), color_.r(), color_.g(), color_.b()
					);
				break;
			default:
				break;
			}


			
		}


		void setResName(const std::string& resName)
		{
			resName_ = resName;
		}

		void setPos(const Vec3f& pos)
		{
			pos_ = Vec3f(pos.x(), pos.y(), pos_.z());
		}
		void setPosWithZ(const Vec3f& pos)
		{
			pos_ = pos;
		}
		void setPos(const float x, const float y, const float z)
		{
			pos_ = Vec3f(x, y, z);
		}
		void setPos(const float x, const float y)
		{
			pos_ = Vec3f(x, y, pos_.z());
		}
		void setPos(const float z)
		{
			pos_ = Vec3f(pos_.x(), pos_.y(), z);
		}
		void offsetPos(const Vec3f& offset)
		{
			pos_ += offset;
		}
		void setOffsetPos(const Vec3f& offsetPos)
		{
			offsetPos_ = offsetPos;
		}

		void setScale(const Vec3f& scale)
		{
			scale_ = scale;
		}
		void setScale(const float x, const float y)
		{
			scale_ = Vec3f(x, y,1.f);
		}
		void setScaleX(const float x)
		{
			scale_ = Vec3f(x, scale_.y(), 1.f);
		}
		void setScaleY(const float y)
		{
			scale_ = Vec3f(scale_.x(), y, 1.f);
		}
		void setScale(const float scale)
		{
			scale_ = Vec3f(scale, scale, 1.f);
		}
		void offsetScale(const Vec3f& offset)
		{
			scale_ += offset;
		}
		void offsetScale(const float& offset)
		{
			scale_ += Vec3f(offset, offset, 0.f);
		}

		void setSrc(const Vec3f& src)
		{
			src_ = src;
		}
		void setSrc(const float x, const float y)
		{
			src_ = Vec3f(x, y, 0.f);
		}
		void setSrcX(const float x)
		{
			src_ = Vec3f(x, src_.y(), 0.f);
		}
		void setSrcY(const float y)
		{
			src_ = Vec3f(src_.x(), y, 0.f);
		}
		void setSrcSize(const Vec3f& size)
		{
			srcSize_ = size;
		}
		void setSrcSize(const float x, const float y)
		{
			srcSize_ = Vec3f(x, y,0.f);
		}
		void setSrcInfo(const Vec3f& src, const Vec3f& size)
		{
			src_ = src;
			srcSize_ = size;
		}

		void setDegree(const float degree)
		{
			degree_ = degree;
		}
		void offsetDegree(const float offset)
		{
			float temp0 = 0.f;
			float temp360 = 360.f;
			degree_ += offset;
			if (degree_ > temp360)    degree_ -= temp360;
			else if (degree_ < temp0) degree_ += temp360;
		}

		void setColor(const Color& color)
		{
			color_ = color;
		}

		void setColor(
			unsigned int a, unsigned int r,
			unsigned int g, unsigned int b)
		{
			color_.set(a, r, g, b);
		}
		void offsetColor(
			unsigned int a, unsigned int r,
			unsigned int g, unsigned int b)
		{
			color_.offset(a, r, g, b);
		}
		void setAlpha(unsigned int alpha)
		{
			color_.set(alpha, color_.r(), color_.g(), color_.b());
		}

		void setType(const Type type)
		{
			type_ = type;
		}



		void useAnim(Animator& anim, const float table[])
		{
			setSrc(Vec3f(table[anim.xNo_], (float)anim.yNo_, 0.f));
		}
		void useAnim(Animator& anim)
		{
			setSrc(Vec3f((float)anim.xNo_, (float)anim.yNo_, 0.f));
		}
		void useAnimXonly(Animator& anim, const float table[])
		{
			setSrcX(table[anim.xNo_]);
		}
		void useAnimXonly(Animator& anim)
		{
			setSrcX((float)anim.xNo_);
		}

		//tagNameは"="で終わります。pathNameはresフォルダーから拡張子まで書きます
		void loadDataFromFile(std::string tagName, std::string pathName)
		{
			//////////////////////////////////////////////////////////////////////
			//dataファイルはtagNameの下に
			//resName pos　scale　src　srcSize順にVec3f型は(x,y,z)の形に書きます
			//例
			//tagName
			//resName (1,1,1) (0,0,0) (1,1,1) (1,1,1)
			//////////////////////////////////////////////////////////////////////

			std::ifstream f(pathName);
			if (f.fail()) assert(false);
			if (f.is_open())
			{
				while (true)
				{
					std::string line;
					std::getline(f, line);
					if (line == tagName)
					{
						break;
					}
					if (f.eof())
					{
						assert(false);
					}
				}
				std::string line;
				std::getline(f, line);
				std::stringstream stream(line);
				std::string resName;

				std::string str;
				getline(stream, str, ' ');
				resName = str;

				//pos, scale, src, srcSize;
				Vec3f temp[4];
				for (int i = 0; i < 4; i++)
				{
					getline(stream, str, ' ');
					float x, y, z;
					sscanf_s(str.c_str(),
						"(%f,%f,%f)",
						&x, &y, &z
						);
					temp[i].set(x, y, z);
				}
				DrawObject::initDrawObject(temp[0], temp[1], resName, temp[2], temp[3]);
			}
			f.close();
		}

		Vec3f pos(){return pos_;}
		float posX(){ return pos_.x(); }
		float posY(){ return pos_.y(); }
		float posZ(){ return pos_.z(); }
		Vec3f src(){ return src_; }
		float srcSizeX(){return srcSize_.x();}
		float srcSizeY(){ return srcSize_.y(); }
		Vec3f srcSize(){ return srcSize_; }
		Vec3f scale(){return scale_;}
		float scaleSize(){ return scale_.x(); }
		std::string resName(){return resName_;}

		
	};
}