#pragma once
#include "../../lib/ci_ext/object.hpp"

#include "myUtility/drawObject.hpp"
#include "myUtility/myCollider.h"

namespace game
{
	class Beam : public ci_ext::Object
	{
		myLib::DrawObject beam_;
		myLib::Animator anim_;
		enum class State{ NoUse,PreLight, Attacking };
		State state_;
		int preLightFrame_;
		int attackinFrame_;
		int frameCount_;
		

		void Beam::loadData();
		void Beam::updatePreLightAnim();
		void Beam::updateAttackingAnim();

	public:
		Beam(const std::string& objectName);


		void render() override;
		void update() override;
	};
}