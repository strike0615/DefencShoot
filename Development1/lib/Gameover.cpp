



#include "gplib.h"
#include "Gameover.h"

using namespace gplib;


struct GameOver
{
	float count;
	bool	isOver;
	float	changeTime;
};

static GameOver gameover_;

//�Q�[���I�[�o�[���ɐ؂�ւ�鎞�Ԃ��w�肷��B
//�f�t�H���g��3�bInitGameOver�Őݒ肷��̂�
//�C�ӕύX�̏ꍇ��StartGameOver�ŏ����̊J�n���s������
void gameover::SetChangeTime(float time)
{
	gameover_.changeTime = time;
}

//�I�������J�n
//�Q�[���I�[�o�[�������ɌĂяo��
void gameover::StartGameOver()
{
	gameover_.isOver = true;
	gameover_.count = 0.f;
}

//�I������������
void gameover::InitGameOver()
{
	gameover_.isOver = false;
	gameover_.count = 0.f;
	gameover_.changeTime = 3.0f;
}

//�I����������
//�L�����N�^�̈ړ��}�~�Ȃǂ�
bool gameover::isGameOver()
{
	return gameover_.isOver;
}

//�I�����X�e�[�W�ڍs����
//StartGameOver��Ɉ�莞�Ԃŉ�ʐ؂�ւ������s����B
void gameover::CheckToChangeScreen(int next)
{
	if (gameover_.isOver == true){
		gameover_.count += system::ONEFRAME_TIME;//60���̂Psec
		if (gameover_.count >= gameover_.changeTime){
			gplib::stage::ChangeStage(next);
			gplib::bgm::DShow_AllStop();
			gplib::se::DSound_AllStop();
		}
	}
}
