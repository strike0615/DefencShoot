
#include "define.h"

//gplib‚Ì–¼‘O‹óŠÔ‚ğÈ—ª“ü—Í’Zk‚Ì‚½‚ß
//ŠO•”‚Ìƒ‰ƒCƒuƒ‰ƒŠ‚Ìê‡‚Í–¼‘Od•¡‚Ì‹°‚ê‚ª‚ ‚é‚½‚ßAÁ‚·‚±‚ÆB
using namespace gplib;
//-----------------------------------
//•Ï”éŒ¾
//-----------------------------------

//yashiro
const float Yashiro_PosX = 150.f;
const float Yashiro_PosY = system::WINH / 2.f;
const int YashiroMaxLife = 1000;
const int MaxPow = 50;
const int chargeMax = 5;

//enemy
const int EnemyMax = 10;
const int EnemySizeX = 225;
const int EnemySizeY = 225;
const float EnemyStartX = (float)(system::WINW + EnemySizeX);
const float EnemyStartY = 200.f;
const int EnemyMaxLife = 12;
const int EnemySpeedMax = -1;
enum State{Walk,Attack,Rush};
float backForce = 10.f;

//shot
const int ShotMax = 10;
const float ShotSpeed = 10;
enum shotType{Normal,Beam};
int beamcnt;

//-----------------------------------
//\‘¢‘ÌéŒ¾
//-----------------------------------

//yashiro
struct Yashiro
{
	charabase::CharaBase	cb_;
	int	life_;
	RECT rt_;
	float	pow_;
	int charge;
	int impactCD;

}yashiro;

//enemey
struct Enemy
{
	charabase::CharaBase cb_;
	int		life_;
	RECT rt_;
	charabase::Anim anim_;
	State state;


}enemies[EnemyMax];


//shot
struct Shot
{
	charabase::CharaBase cb_;
	RECT rt_;
	int pow_;
	shotType type;
}shots[ShotMax];



//--------------------------------
//shot
//--------------------------------

void DamageEnemy(Enemy& e, int pow)
{
	e.life_ -= pow;
	if (e.life_ <= 0)
	{
		charabase::NoUseChar(e.cb_);
		if (yashiro.charge <chargeMax) ++ yashiro.charge;
		
	}
}


void InitShot()
{
	for (auto& shot : shots)
	{
		charabase::NoUseChar(shot.cb_);
		shot.type = shotType::Normal;
	}
}

void DispShot()
{
	for (auto& shot : shots)
	{
		if (charabase::CheckUseChar(shot.cb_))
		{
			charabase::Draw_Char(shot.cb_);
		}
	}
}

void StepShot()
{
	for (auto& shot : shots)
	{
		if (charabase::CheckUseChar(shot.cb_) && shot.type == shotType::Normal)
		{
			charabase::CharaBase& cb = shot.cb_;
			charabase::MoveChar(cb);
			shot.cb_.angle+=10;
			if (shot.cb_.angle > 359.f) shot.cb_.angle = 0.f;

			if (!camera::InScreenCameraCenter(cb.pos.x, cb.pos.y, (float)cb.width, (float)cb.height))
			{
				charabase::NoUseChar(cb);

			}
			for (auto& enemy : enemies)
			{
				if (charabase::CheckUseChar(enemy.cb_))
				{
					if (charabase::HitCheckScales(enemy.cb_, cb))
					{
						charabase::NoUseChar(cb);
						effect::Create((int)cb.pos.x, (int)cb.pos.y, effect::EFFECTTYPE::EFFECT_P3);
						DamageEnemy(enemy, shot.pow_);
					}
				}
			}
		}
	}
}

void CreateShot(float angle, int pow)
{
	for (auto& shot : shots)
	{
		if (!charabase::CheckUseChar(shot.cb_))
		{
			charabase::CharaBase & cb = shot.cb_;
			charabase::InitCharBase(cb, Yashiro_PosX+100, Yashiro_PosY, 0.f,
				math::ROUND_X(angle, ShotSpeed), math::ROUND_Y(angle, ShotSpeed),
				0.f, "shot", 0, 0, 0, 56, 56);
			charabase::UseChar(cb);

			//UŒ‚—ÍÅ’á‚P•Ûá
			shot.pow_ = pow + 1;
			charabase::MakeRect(shot.rt_, cb);
			shot.type = shotType::Normal;
			break;
		}
	}
}
void StepBeam()
{
	
	for (auto& shot : shots)
	{
		
		
		if (charabase::CheckUseChar(shot.cb_)&& shot.type == shotType::Beam)
		{
			charabase::CharaBase& cb = shot.cb_;

			if (beamcnt<180) cb.src.y = -1;
			if (beamcnt > 180 && beamcnt < 210)
			{
				int table[2] = { -1,0 };

				cb.src.y =(float)table[beamcnt%2];
			}
			if (beamcnt >= 210 && beamcnt < 330)
			{
				int table[2] = { 1, 2 };
				cb.src.y = (float)table[beamcnt % 2];
			}
			if (beamcnt >= 330)
			{
				charabase::NoUseChar(cb);
				beamcnt = 0;
				yashiro.charge = 0;
			}
			

			
			for (auto& enemy : enemies)
			{
				if (charabase::CheckUseChar(enemy.cb_))
				{
					if (charabase::HitCheckScales(enemy.cb_, cb) && beamcnt >=210)
					{
						effect::Create((int)enemy.cb_.pos.x, (int)enemy.cb_.pos.y, effect::EFFECTTYPE::EFFECT_E);
						DamageEnemy(enemy, shot.pow_);
					}
				}
			}
			beamcnt++;
		}
	}
}
void CreateBeam()
{
	for (auto& shot : shots)
	{
		if (!charabase::CheckUseChar(shot.cb_))
		{
			charabase::CharaBase & cb = shot.cb_;
			charabase::InitCharBase(cb, Yashiro_PosX + 100+512, Yashiro_PosY+100, 0.f,
				0,0,
				0.f, "beam", 0, 0, 0, 512, 128,0.f,2.f,1.f);
			charabase::UseChar(cb);
			shot.type = shotType::Beam;
			//UŒ‚—ÍÅ’á‚P•Ûá
			shot.pow_ =  1;
			charabase::MakeRect(shot.rt_, cb);
			break;
		}
		beamcnt = 0;
	}
}



void DispGameOver()
{
	if (gameover::isGameOver())
	{
		font::Draw_FontText(100, 100, 0.0f, "‚ ‚È‚½‚ÍĞ‚ğç‚ê‚Ü‚¹‚ñ‚Å‚µ‚½...", ARGB(255, 20, 50, 50), 0);


		if (math::GetRandom(0, 10) == 0)
		{
			int ranX = gplib::math::GetRandom(-150, 150);
			int ranY = gplib::math::GetRandom(-200, 200);
			effect::Create((int)Yashiro_PosX + ranX, (int)Yashiro_PosY + ranY, effect::EFFECTTYPE::EFFECT_E);
		}
	}
}




//-----------------------------------
//Ğ
//-----------------------------------
void InitYashiro()
{
	charabase::InitCharBase(yashiro.cb_, Yashiro_PosX, Yashiro_PosY, 0.5f,
		0.f, 0.f, 0.f,
		"yashiro", 0, 0, 0,
		300, 420);
	yashiro.life_ = YashiroMaxLife;
	charabase::UseChar(yashiro.cb_);
	yashiro.charge = 0;
	yashiro.impactCD = 0;
}


void CreateImpact()
{
	for (auto& enemy : enemies)
	{
		if (charabase::CheckUseChar(enemy.cb_))
		{
			if (charabase::HitCheckScales(enemy.cb_, yashiro.cb_))
			{

				enemy.cb_.add.x += backForce;
				enemy.state = State::Walk;
				charabase::SetAnim(enemy.anim_, 4, 0.1f);
				enemy.cb_.src.y = 0;
			}
		}
	}
}



void StepYashiro()
{
	if (gameover::isGameOver()) return;

	if (yashiro.impactCD > 0) --yashiro.impactCD;
	if (input::CheckPush(input::KEY_BTN1) && yashiro.impactCD == 0)
	{
		effect::Create((int)Yashiro_PosX, (int)Yashiro_PosY, effect::EFFECTTYPE::EFFECT_B);
		CreateImpact();
		yashiro.impactCD = 400;
		
	}



	if (input::CheckPress(input::KEY_MOUSE_LBTN))
	{
		yashiro.pow_ += 1.f;
		if (yashiro.pow_ >= MaxPow) yashiro.pow_ = MaxPow;
		effect::Create((int)yashiro.cb_.pos.x+100, (int)yashiro.cb_.pos.y, effect::EFFECTTYPE::EFFECT_D);
		
	}
	if (input::CheckPull(input::KEY_MOUSE_LBTN))
	{
		POINT mouse = input::GetMousePosition();
		float angle = math::DegreeOfPoints2(yashiro.cb_.pos.x, yashiro.cb_.pos.y, (float)mouse.x, (float)mouse.y);

		CreateShot(angle, (int)(yashiro.pow_ / 10));

		yashiro.pow_ = 0;
	}

	if (input::CheckPull(input::KEY_MOUSE_RBTN) && yashiro.charge >= chargeMax)
	{

		effect::Create((int)yashiro.cb_.pos.x + 100, (int)yashiro.cb_.pos.y+100, effect::EFFECTTYPE::EFFECT_P1);
		CreateBeam();
		yashiro.charge= 0;
	}
}

void DispYashiro()
{
	charabase::Draw_Char(yashiro.cb_);



	//ora
	std::string buff = std::to_string(int((99+yashiro.impactCD/2) / 100));
	graph::Draw_Graphics(Yashiro_PosX, Yashiro_PosY, 0.1f, "ora" + buff, 0, 0, 300, 420);



	//life bar

	const int LifeBar_Length = 200;
	const int Lifebar_Height = 30;

	int now = yashiro.life_ * LifeBar_Length / YashiroMaxLife;
	//‰º•~‚«‚ÌÔ‚¢•”•ª
	graph::Draw_Box((int)(yashiro.cb_.pos.x - LifeBar_Length / 2), (int)(yashiro.cb_.pos.y + 30),
		(int)(yashiro.cb_.pos.x + LifeBar_Length / 2), (int)(yashiro.cb_.pos.y + 30 + Lifebar_Height), 0.f,
		ARGB(255, 255, 0, 0), ARGB(255, 0, 0, 0),3, 1);
	//—Î‚ÌŒ»İlife•\¦
	graph::Draw_Box((int)(yashiro.cb_.pos.x - LifeBar_Length / 2), (int)(yashiro.cb_.pos.y + 30),
		(int)(yashiro.cb_.pos.x - LifeBar_Length / 2+now), (int)(yashiro.cb_.pos.y + 30 + Lifebar_Height), 0.f,
		ARGB(255, 0,255, 0), ARGB(255, 0, 0, 0), 3, 1);


	//‚½‚ß‚½UŒ‚—Í
	const int PowBar_Length = 100;
	const int PowBar_Height = 10;

	now = (int)(yashiro.pow_ * PowBar_Length / MaxPow);


	//‰º•~‚«‚ÌÔ‚¢•”•ª
	graph::Draw_Box((int)(yashiro.cb_.pos.x - PowBar_Length / 2), (int)yashiro.cb_.pos.y,
		(int)(yashiro.cb_.pos.x + PowBar_Length / 2), (int)yashiro.cb_.pos.y  + PowBar_Height, 0.f,
		ARGB(255, 255, 0, 0), ARGB(255, 0, 0, 0), 3, 1);
	//—Î‚ÌŒ»İlife•\¦
	graph::Draw_Box((int)(yashiro.cb_.pos.x - PowBar_Length / 2), (int)yashiro.cb_.pos.y,
		(int)(yashiro.cb_.pos.x - PowBar_Length / 2 + now), (int)yashiro.cb_.pos.y +  PowBar_Height, 0.f,
		ARGB(255, 0, 255, 0), ARGB(255, 0, 0, 0), 3, 1);



	//‚½‚ß‚½charge
	const int ChargeBar_Length = 100;
	const int ChargeBar_Height = 10;

	now = (int)(yashiro.charge * ChargeBar_Length / chargeMax);


	//‰º•~‚«‚ÌÔ‚¢•”•ª
	graph::Draw_Box((int)(yashiro.cb_.pos.x - ChargeBar_Length / 2), (int)yashiro.cb_.pos.y-30,
		(int)(yashiro.cb_.pos.x + ChargeBar_Length / 2), (int)yashiro.cb_.pos.y + ChargeBar_Height-30, 0.f,
		ARGB(255, 255, 0, 0), ARGB(255, 0, 0, 0), 3, 1);
	//—Î‚ÌŒ»İlife•\¦
	graph::Draw_Box((int)(yashiro.cb_.pos.x - ChargeBar_Length / 2), (int)yashiro.cb_.pos.y-30,
		(int)(yashiro.cb_.pos.x - ChargeBar_Length / 2 + now), (int)yashiro.cb_.pos.y + ChargeBar_Height-30, 0.f,
		ARGB(255, 0, 255, 0), ARGB(255, 0, 0, 0), 3, 1);



}
//---------------------------------
//kuma
//---------------------------------

void DamageYashiro(int dam)
{
	if (gameover::isGameOver()) return;

	if (yashiro.life_>0) yashiro.life_ -= dam;
	if (yashiro.life_ <= 0)
	{
		yashiro.life_ = 0;
		gameover::StartGameOver();
	}
}


void InitEnemy()
{
	for (auto& enemy : enemies)
	{
		charabase::NoUseChar(enemy.cb_);
		enemy.life_ = 0;
		
	}
}

void CreateEnemy()
{
	for (auto& enemy : enemies)
	{
		charabase::CharaBase& cb = enemy.cb_;
		if (!charabase::CheckUseChar(cb))
		{
			//YˆÊ’uŒˆ’è
			float y = math::GetRandom(EnemyStartY, (system::WINH - EnemySizeX / 2.f));
			charabase::InitCharBase(cb,
				EnemyStartX, y, (system::WINH - y) / system::WINH,
				(float)EnemySpeedMax, 0.f, 0.f,
				"kuma", 0, 0, 0,
				EnemySizeX, EnemySizeY);

			enemy.life_ = EnemyMaxLife;
			enemy.anim_.no = 0;
			enemy.anim_.cnt = 0;
			enemy.anim_.max = 4;
			enemy.anim_.speed = 0.1f;
			enemy.state = Walk;

			charabase::UseChar(cb);
			break;
		}
	}
}


void StepEnemy()
{
	int animTable[] = { 3, 1, 3, 2 };
	const int EnemyAnimTableMax = 4;
	for (auto& enemy : enemies)
	{
		charabase::CharaBase& cb = enemy.cb_;
		if (charabase::CheckUseChar(cb))
		{
			if (enemy.cb_.add.x > EnemySpeedMax && enemy.state !=State::Attack)
			{
				enemy.cb_.add.x -= 0.3f;
			}
			//if (enemy.cb_.add.x <= EnemySpeedMax) enemy.cb_.add.x = EnemySpeedMax;
			
			charabase::MoveChar(cb);
			if (cb.pos.x <= -cb.width)
			{
				charabase::NoUseChar(cb);
			}
			
			if (enemy.life_ <= EnemyMaxLife / 2)
			{
				cb.src.y = 2;
				cb.add.x = -5;
				
				if (enemy.state == State::Walk)
				{
					effect::Create((int)cb.pos.x, (int)cb.pos.y, effect::EFFECTTYPE::EFFECT_P4);
					enemy.state = State::Rush;

					charabase::SetAnim(enemy.anim_, 2, 0.3f);
				}
			}
			

			if (charabase::HitCheck(yashiro.cb_, cb))
			{

				cb.src.y = 1;
				cb.add.x = 0.f;

				if (enemy.state == State::Walk)
				{

					enemy.state = State::Attack;

					charabase::SetAnim(enemy.anim_, 3, 0.1f);
				}
				if (enemy.state == State::Rush)
				{
					DamageYashiro(100);
					effect::Create((int)yashiro.cb_.pos.x, (int)yashiro.cb_.pos.y, effect::EFFECTTYPE::EFFECT_P2);
					enemy.state = State::Attack;

					charabase::SetAnim(enemy.anim_, 3, 0.1f);

				}
				if (enemy.anim_.no==1)	DamageYashiro(1);
			}

			//anime 
			charabase::StepAnim(enemy.anim_);

			switch (enemy.state)
			{
			case State::Walk:
				cb.src.x = (float)animTable[enemy.anim_.no];
				break;

			case State::Attack:

				cb.src.x = (float)enemy.anim_.no;
				
				break;
			case State::Rush:

				cb.src.x = (float)enemy.anim_.no;

				break;

			}
			
			
		}

	}
	if (math::GetRandom(0, 100) == 0)
	{
		CreateEnemy();
	}
}

void DispEnemy()
{
	for (auto& enemy : enemies)
	{
		charabase::CharaBase cb = enemy.cb_;
		if (charabase::CheckUseChar(cb))
		{
			//shadow
			graph::Draw_GraphicsLeftTop(
				cb.pos.x - cb.width / 2,
				cb.pos.y + cb.height / 2, cb.pos.z + 0.1f,
				"kuma",
				enemy.anim_.no*cb.width, cb.height,
				cb.width, -cb.height,
				0, nullptr, 1.f, 0.5f,
				255, 0, 0, 0);
			//–{‘Ì
			charabase::Draw_Char(cb);

			//–{‘ÌƒQ[ƒW
			const int Enemybar_Length = 100;
			const int Enemybar_Height = 10;

			int now = enemy.life_*Enemybar_Length / EnemyMaxLife;
			int x = (int)cb.pos.x;
			int y = (int)cb.pos.y;

			graph::Draw_Box(x, y,
				x + Enemybar_Length, y + Enemybar_Height, 0.f,
				ARGB(255, 255, 0, 0), ARGB(255, 0, 0, 0), 3, 1);
			graph::Draw_Box(x, y,
				x + now, y + Enemybar_Height, 0.f,
				ARGB(255, 0, 255, 0), ARGB(255, 0, 0, 0), 3, 1);

		}
	}
}

//---------------------------------
// Stage1
//---------------------------------
//‰Šú‰»ˆ—		
void Stage1Init()
{
	gamestart::Init();
	gamestart::Start();

	InitYashiro();
	InitEnemy();
	InitShot();


	gameover::InitGameOver();

}

//ƒtƒŒ[ƒ€ˆ—
void Stage1Step()
{
	if (gamestart::isGameStart() == false){
		if (input::CheckPush(input::KEY_BTN0))
		{

			stage::ChangeStage(ENDSCENE);

		}

		StepYashiro();
		StepEnemy();
		StepShot();
		StepBeam();


		gameover::CheckToChangeScreen(ENDSCENE);
	}
	gamestart::Step();
}

//•`‰æˆ—
void Stage1Disp()
{
	graph::Draw_GraphicsLeftTop(0, 0, 1.f, "back", 0, 0, 800, 600, 0, nullptr, system::WINW / 800.f, system::WINH / 600.f);
	
	DispYashiro();
	DispEnemy();
	DispShot();
	DispGameOver();

  if (gamestart::isGameStart()){
		char msg[256];
		wsprintf(msg, "GameStart %d", (int)gamestart::GetCount());
		gplib::font::Draw_FontText(100, 200, 0.f, msg, ARGB(255, 0, 0, 0), 0);
	}


}

void Stage1()
{
	if (gplib::stage::InitStage()){ Stage1Init(); }
	Stage1Step();
	Stage1Disp();
}
