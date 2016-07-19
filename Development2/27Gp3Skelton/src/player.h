#pragma once
#include "../../lib/ci_ext/object.hpp"
#include "../../lib/ci_ext/vec3.hpp"
//#include "../../lib/ci_ext/DrawObj.hpp"
#include "../../lib/CharBase.h"

#include "myUtility/drawObject.hpp"



namespace game
{

	class Player : public ci_ext::Object
	{
	private:
		myLib::DrawObject yashiro_;
		myLib::DrawObject aura_;
		std::string auraName[3];
		float colliderRadius_;

		bool isGameOver;
		
		float	pow_;
		int beamCharge_;
		int impactCharge_;
		int life_;

		int	lifeMax_;
		int impactCoolFrame_;
		int powMax_;
		int beamChargeMax_;

		void Player::loadData();

		void Player::createShot();
		void Player::createBeam();
		void Player::createImpact();
		void Player::chargeImpact();
		void Player::pushEnemy();
		void Player::renderLifeBar();
		void Player::renderPowerBar();
		void Player::renderBeamBar();

	public:
		Player(const std::string& objectName);

		float Player::posX();
		float Player::posY();
		float Player::playerRadius();
		void Player::getDamage(int damage);


		void Player::beamCharge();


		void init() override;
		void render() override;
		void update() override;

	};





}