
#include "define.h"
//gplibの名前空間を省略＝入力短縮のため
//外部のライブラリの場合は名前重複の恐れがあるため、消すこと。
using namespace gplib;

//---------------------------------
// End
//---------------------------------
//初期化処理		
void EndInit()
{
}

//フレーム処理
void EndStep()
{
	if (input::CheckPush(input::KEY_BTN0))
	{

		stage::ChangeStage(TITLESCENE);
		
	}
}

//描画処理
void EndDisp()
{


}

void End()
{
	if (gplib::stage::InitStage()){ EndInit(); }
	EndStep();
	EndDisp();
}
