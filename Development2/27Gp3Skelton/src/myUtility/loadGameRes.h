#pragma once
#include "../../../lib/ci_ext/object.hpp"

#include "nowLoadingImage.h"

#include "drawObject.hpp"
#include "animator.hpp"
#include <vector>

namespace myLib
{
	class LoadGameRes : public ci_ext::Object
	{
		
		float loadCountNow_;
		float loadCountMax_;
		int imageCount_;
		int bgmCount_;
		int seCount_;
		game::NowLoadingImage nowLoadingImage_;

		enum class State
		{
			Loading,Finish,NextStage
		};
		State state_;
		std::string loadFileName_;
		std::vector<std::string> resName_;
		std::vector<std::string> fileName_;


		void LoadGameRes::pushResAndFileName(const std::string& str);
		void LoadGameRes::readFileData();
		void LoadGameRes::loadFile();

	public:
		LoadGameRes::LoadGameRes();

		void init() override;
		void update() override;
		void render() override;





	};
}