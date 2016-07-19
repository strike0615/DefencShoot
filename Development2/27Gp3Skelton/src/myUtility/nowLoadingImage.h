#pragma once

#include "drawObject.hpp"
#include "../../../lib/ci_ext/object.hpp"

namespace game
{
	class NowLoadingImage : public ci_ext::Object
	{
	public:
		enum class State{ Swing, Break, Finish };
	private:
		myLib::DrawObject startEgg_;
		myLib::DrawObject startEgg2_;

		State state_;
		int frameCount_;

		ci_ext::Vec3f barPos_;
		ci_ext::Vec3f barSize_;
		float loadCountMax_;
		float loadCountNow_;


		void NowLoadingImage::breakEgg();
		void NowLoadingImage::loadData();
		void NowLoadingImage::swingEgg();
		void NowLoadingImage::breakingEgg();
		void NowLoadingImage::renderLoadingBar();
		void NowLoadingImage::renderBack();


	public:
		
		NowLoadingImage::NowLoadingImage();



		void init() override;
		void update() override;
		void render() override;
		void NowLoadingImage::changeState(State state);

		void NowLoadingImage::setLoadCountNow(float & count);
		void NowLoadingImage::setLoadCountMax(float & countMax);
		bool NowLoadingImage::isFinish();
	};
}