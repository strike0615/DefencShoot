
#include "define.h"
//gplib�̖��O��Ԃ��ȗ������͒Z�k�̂���
//�O���̃��C�u�����̏ꍇ�͖��O�d���̋��ꂪ���邽�߁A�������ƁB
using namespace gplib;

//---------------------------------
// End
//---------------------------------
//����������		
void EndInit()
{
}

//�t���[������
void EndStep()
{
	if (input::CheckPush(input::KEY_BTN0))
	{

		stage::ChangeStage(TITLESCENE);
		
	}
}

//�`�揈��
void EndDisp()
{


}

void End()
{
	if (gplib::stage::InitStage()){ EndInit(); }
	EndStep();
	EndDisp();
}
