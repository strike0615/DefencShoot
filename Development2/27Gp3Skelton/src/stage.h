#pragma once
#include "../../lib/ci_ext/object.hpp"
#include "timer.h"

namespace game
{
	class Stage : public ci_ext::Object
	{
		std::weak_ptr<ci_ext::Object>timer_;

		void Stage::renderBack();
	public:
		Stage(const std::string& objectName);

		

		void init() override;
		void resume() override;
		void render() override;
		void update() override;

	};

}