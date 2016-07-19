#pragma once
#include "enemy.h"
#include "beam.h"
#include "myUtility/data.h"

namespace game
{

		Beam::Beam(const std::string& objectName)
			:
			Object(objectName),
			state_(State::NoUse),
			frameCount_(0)
		{
			loadData();
		}

		void Beam::loadData()
		{
			std::string pathName = "res/data/beam.txt";
			beam_.loadDataFromFile("beam=", pathName);
			preLightFrame_ = myLib::getIntFromFile("preLightFrame=", pathName);
			attackinFrame_ = myLib::getIntFromFile("attackingFrame=", pathName);
			
			//preLightのアニメーション設定
			anim_.setAnimation(2, 0.5);

			beam_.visible();
			state_ = State::PreLight;
			//最初は表示しない
			beam_.setSrcY(-1.f);		
		}
		void Beam::updatePreLightAnim()
		{
			float table[2] = { -1.f, 0.f };
			beam_.setSrcY(table[anim_.xNo_]);
			anim_.update();
		}
		void Beam::updateAttackingAnim()
		{
			float table[2] = { 1.f, 2.f };
			beam_.setSrcY(table[anim_.xNo_]);
			anim_.update();
		}



		void Beam::render()
		{
			beam_.render();
		}
		void Beam::update()
		{
			switch (state_)
			{
			case game::Beam::State::PreLight:
				updatePreLightAnim();
				frameCount_++;
				//細い光線が終わったら攻撃開始え移行
				if (frameCount_ >= preLightFrame_)
				{
					frameCount_ = 0;
					state_ = State::Attacking;
				}
				break;
			case game::Beam::State::Attacking:
			
				updateAttackingAnim();
				{
					auto enemys = getObjectsFromRoot({ "enemy" });
					for (auto e : enemys)
					{
						auto enemy = ci_ext::weak_to_shared<Enemy>(e);
						enemy->beamDamage(1);

					}
				}
				frameCount_++;
				//攻撃が終わったらお亡くなります
				if (frameCount_ >= attackinFrame_)
				{
					kill();
				}
			
				break;
			default:
				break;
			}
		}
	
}