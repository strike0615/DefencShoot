#pragma once

#include "data.h"

#include "nowLoadingImage.h"


namespace game
{
	NowLoadingImage::NowLoadingImage()
		:
		frameCount_(0),
		state_(State::Swing),
		Object("nowLoadingImage")
	{
		loadData();
		startEgg_.visible();
	}
	using namespace ci_ext;

	void NowLoadingImage::loadData()
	{



		std::ifstream f("res/data/nowLoadingData.txt");
		if (f.fail()) assert(false);
		if (f.is_open())
		{
			myLib::lineFinder(f, "type=image");
			//画像をロード
			for (int i = 0; i < 2; i++)
			{
				std::string line;
				std::getline(f, line);
				std::stringstream stream(line);
				std::string resName, fileName;
				stream >> resName;
				stream >> fileName;
				gplib::graph::Draw_LoadObject(resName, fileName);
			}

			myLib::lineFinder(f, "start_egg=");
			myLib::loadDrawObjData(f, startEgg_);
			myLib::loadDrawObjData(f, startEgg2_);
			myLib::lineFinder(f, "barPos_=");
			myLib::loadVec3fData(f, barPos_);
			myLib::lineFinder(f, "barSize_=");
			myLib::loadVec3fData(f, barSize_);


		}
		f.close();
	}

	void NowLoadingImage::init()
	{

	}
	void NowLoadingImage::update()
	{
		switch (state_)
		{
		case game::NowLoadingImage::State::Swing:
			//ロード中の揺れ
			swingEgg();
			break;
		case game::NowLoadingImage::State::Break:
			//卵が割れる演出
			breakingEgg();
			break;
		case game::NowLoadingImage::State::Finish:
			//何もしない、loadGameResで演出終わりの信号が取れるのを待つ
			break;
		default:
			break;
		}
	}
	void NowLoadingImage::render()
	{
		renderBack();
		startEgg_.render();
		
		if (state_ == State::Break && frameCount_ > 120)
		{
			startEgg2_.visible();
			startEgg2_.render();
		}
		renderLoadingBar();
	}


	void NowLoadingImage::swingEgg()
	{
		

		if ((frameCount_ / 30) % 2 == 0)
		{
			float haba = (float)(10 - frameCount_ % 10);
			float angle = (float)frameCount_;

			float degree = haba * sinf(angle);
			startEgg_.setDegree(degree);
		}
		else
		{
			startEgg_.setDegree(0);
		}
		++frameCount_;
		if (frameCount_ > 140) frameCount_ = 0;
	}
	void NowLoadingImage::breakingEgg()
	{
		
		startEgg_.setSrc((float)(frameCount_ / 40), 0.f);
		++frameCount_;
		if (frameCount_ > 140)
		{
			changeState(State::Finish);
		}
	}

	void NowLoadingImage::changeState(State state)
	{
		frameCount_ = 0;
		state_ = state;
		startEgg_.setDegree(0);
		
	}
	void NowLoadingImage::renderBack()
	{
		gplib::graph::Draw_Box(
			0, 0,
			gplib::system::WINW, gplib::system::WINH,
			1.f,
			ARGB(255, 0, 0, 0), ARGB(255, 0, 0, 0),
			1, 1
			);
	}
	void NowLoadingImage::renderLoadingBar()
	{


		int now = (int)((float)barSize_.x() * loadCountNow_ / loadCountMax_);

		gplib::graph::Draw_Box(
			barPos_.ix() - barSize_.ix() / 2, barPos_.iy(),
			barPos_.ix() + barSize_.ix() / 2, barPos_.iy() +barSize_.iy(),
			barPos_.z(),
			ARGB(255, 255, 0, 0), ARGB(255, 0, 0, 0),
			3, 1
			);
		gplib::graph::Draw_Box(
			barPos_.ix() - barSize_.ix() / 2, barPos_.iy(),
			barPos_.ix() - barSize_.ix() / 2 + now, barPos_.iy() +barSize_.iy(),
			barPos_.z(),
			ARGB(255, 0, 255, 0), ARGB(255, 0, 0, 0),
			3, 1
			);
	}

	void NowLoadingImage::setLoadCountNow(float & count)
	{
		loadCountNow_ = count;
	}
	void NowLoadingImage::setLoadCountMax(float & countMax)
	{
		loadCountMax_ = countMax;
	}

	//loadGameResで確認できる様に
	bool NowLoadingImage::isFinish()
	{
		if (state_ == State::Finish)
			return true;
		else
			return false;
	}
}