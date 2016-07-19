



#include "gplib.h"
#include "gamestart.h"

using namespace gplib;

struct GameStart
{
	float count;
	bool	isStart;
	float	changeTime;
};

static GameStart gamestart_;

//�Q�[���J�n���ɑ���n�܂�܂ł̎��Ԃ��w�肷��B
//�f�t�H���g3�b��Init�Őݒ肷�邽��
//Init
//SetTime
//Start
//�̏��ŊJ�n���邱�ƁB
//SetTime�������΃f�t�H���g3�b���K�p
void gamestart::SetTime(float time)
{
	gamestart_.changeTime = time;
}

//�X�e�[�W�����J�n
void gamestart::Start()
{
	gamestart_.isStart = true;
	gamestart_.count = gamestart_.changeTime;
}

//������
void gamestart::Init()
{
	gamestart_.isStart = true;
	gamestart_.changeTime = 3.0f;
}

//��������
//�L�����N�^�̈ړ��}�~�Ȃǂ�
bool gamestart::isGameStart()
{
	return gamestart_.isStart;
}

//�I�����X�e�[�W�ڍs����
//Start��Ɉ�莞�Ԃɏ�����������s����B
void gamestart::Step()
{
	if (gamestart_.isStart == true){
		gamestart_.count -= system::ONEFRAME_TIME;//60���̂Psec
		if (gamestart_.count <= 0){
			gamestart_.isStart = false;
		}
	}
}

float gamestart::GetCount()
{
	return gamestart_.count;
}

