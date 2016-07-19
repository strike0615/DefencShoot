#pragma once

#include "../../lib/ci_ext/object.hpp"
#include "../../lib/gplib.h"
#include "effect.h"
#include "stage.h"

namespace game
{
	class Title : public ci_ext::Object
	{
	public :
		Title(const std::string& objectName) :
			Object(objectName)
		{
			effect::Create(0, 0, effect::EFFECTTYPE::FADEINWHITE);

		}

		void render() override
		{
			float w = (float)gplib::graph::Draw_GetImageWidth("titleback");
			float h = (float)gplib::graph::Draw_GetImageHeight("titleback");
			gplib::graph::Draw_GraphicsLeftTop(0, 0, 1.f, "titleback",
				0, 0, (int)w, (int)h, 0.f, nullptr,
				gplib::system::WINW / w,
				gplib::system::WINH / h);

			w = (float)gplib::graph::Draw_GetImageWidth("titlelogo");
			h = (float)gplib::graph::Draw_GetImageHeight("titlelogo");
			gplib::graph::Draw_Graphics(gplib::system::WINW / 2, 200, 1.f, "titlelogo",
				0, 0, (int)w, (int)h);

			gplib::font::Draw_FontText(gplib::system::WINW / 2, gplib::system::WINH / 2, 0.0f, "Aキーでゲーム開始", ARGB(255, 0, 0, 0), 0);
			
		}

		void resume() override
		{
		

			insertToParent(new Stage("Stage"));
			effect::Create(0, 0, effect::EFFECTTYPE::CURTAIN_OPEN);
			gplib::bgm::DShow_Play("pinchi_bgm");
			
			kill();

		}
		void update() override
		{
			if (gplib::input::CheckPush(gplib::input::KEY_BTN0))
			{

				effect::Create(0, 0, effect::EFFECTTYPE::CURTAIN_CLOSE);
				sleep(80);
			}
			if (gplib::input::CheckPush(gplib::input::KEY_BTN1))
			{
				effect::Create(0, 0, effect::EFFECTTYPE::CURTAIN_OPEN);
			}
			if (gplib::input::CheckPush(gplib::input::KEY_BTN2))
			{
				effect::Create(0, 0, effect::EFFECTTYPE::CURTAIN_CLOSE);
			}
		}
	};
}