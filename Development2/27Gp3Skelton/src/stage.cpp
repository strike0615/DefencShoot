#pragma once

#include "player.h"
#include "movableObject.h"
#include <memory>
#include <string>
#include <iostream>
#include "tiltle.hpp"
#include "collider.h"
#include "enemy.h"
#include "Logo.hpp"
#include "effect.h"

namespace game
{
	Stage::Stage(const std::string& objectName)
		:
		Object(objectName)
	{


		effect::Create(0, 0, effect::EFFECTTYPE::FADEINWHITE);

	}
	void Stage::renderBack()
	{
		float w = (float)gplib::graph::Draw_GetImageWidth("back");
		float h = (float)gplib::graph::Draw_GetImageHeight("back");
		gplib::graph::Draw_GraphicsLeftTop(0, 0, 1.f, "back",
			0, 0, (int)w, (int)h, 0.f, nullptr,
			gplib::system::WINW / w,
			gplib::system::WINH / h);
	}

	void Stage::init()
	{
		insertAsChild(new Player("1P"));
		
	}



	void Stage::render()
	{
		renderBack();

	}


	void Stage::resume()
	{
		//titleを生成し幕を開ける
		insertToParent(new Title("Title"));
		effect::Create(0, 0, effect::EFFECTTYPE::CURTAIN_OPEN);
		gplib::bgm::DShow_Stop("pinchi_bgm");
		kill();

	}
	void Stage::update()
	{
		//game中titleに戻る
		//幕を閉じる時間分sleepさせる、resumeでtitleに移行
		if (gplib::input::CheckPush(gplib::input::KEY_BTN0))
		{
			sleep(80);
			effect::Create(0, 0, effect::EFFECTTYPE::CURTAIN_CLOSE);
		}


		//敵生成
		if (gplib::math::GetRandom(0, 100) == 0)
		{
			insertAsChild(new Enemy("enemy"));
		}
	}
}