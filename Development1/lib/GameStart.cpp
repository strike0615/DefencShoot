



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

//ゲーム開始時に操作始まるまでの時間を指定する。
//デフォルト3秒はInitで設定するため
//Init
//SetTime
//Start
//の順で開始すること。
//SetTimeを除けばデフォルト3秒が適用
void gamestart::SetTime(float time)
{
	gamestart_.changeTime = time;
}

//ステージ処理開始
void gamestart::Start()
{
	gamestart_.isStart = true;
	gamestart_.count = gamestart_.changeTime;
}

//初期化
void gamestart::Init()
{
	gamestart_.isStart = true;
	gamestart_.changeTime = 3.0f;
}

//処理判別
//キャラクタの移動抑止などに
bool gamestart::isGameStart()
{
	return gamestart_.isStart;
}

//終了時ステージ移行処理
//Start後に一定時間に処理解放を実行する。
void gamestart::Step()
{
	if (gamestart_.isStart == true){
		gamestart_.count -= system::ONEFRAME_TIME;//60分の１sec
		if (gamestart_.count <= 0){
			gamestart_.isStart = false;
		}
	}
}

float gamestart::GetCount()
{
	return gamestart_.count;
}

