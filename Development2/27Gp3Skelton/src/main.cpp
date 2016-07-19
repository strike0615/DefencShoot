#include "../../lib/ci_ext/object.hpp"
#include "../../lib/ci_ext/counter.hpp"
#include <iostream>
#include <Windows.h>
#include "../../lib/gplib.h"
//3D処理外部ライブラリ
#include "../../lib/dx_media_/dx_graphics3d.h"
#include "../../lib/dx_media_/dx3dmeshmanager.h"
//Xinputライブラリ
#include "../../lib/ci_ext/xinput.hpp" 
#include "logo.hpp"

#include "effect.h"

namespace gplib{
  namespace system{
	  const char USERNAME[256] = " 製作者:崔憲　ゲーム制作科";

    const int	WINW = 960;										//PS VITAサイズ
    const int	WINH = 544;										//
    bool	WindowMode = true;								//フルスクリーン対応
    int		KeyboardMode = 0;									//pad disable:1 pad enable:0
  }
}
int ci_ext::Object::uid_ = 0;

//ゲームクラス
//ルートになるクラスで、mainで宣言する。
class Game : public ci_ext::Object
{
  int frame_;
public:
  Game(const std::string& name)
    :
    Object(name),
    frame_(0)
  {
  }

  //コンストラクタでは自分のweak_ptrを取得できないので別途ポインタを登録する関数を作成
  void initGame(const std::weak_ptr<Game>& this_object_pointer)
  {
    //まず自分自身のポインタをつける
    setWeakPtr(this_object_pointer);

    //最初に生成するオブジェクトを記述
    insertAsChild(new game::Logo("Logo"));
  }

  //アクセサ
  int frame() const { return frame_; }

  //お作法
  void render() override {}
  void update() override { ++frame_; }
};

//クラスに内包できるが、改造が多くなるのでグローバルにおく
std::shared_ptr<Game> root;

//エントリポイント
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInst, LPSTR lpszCmdLine, int nCmdShow)
{
  return gplib::system::DoWindow(hInstance, hPreInst, lpszCmdLine, nCmdShow);
}

void gplib::system::GameLoop()
{
//  std::cout << "==" << root->frame() << "回目" << std::endl;
  XInput::CheckKey();

  //毎フレーム呼ぶ
  gplib::graph::Draw_3DClear();
  gplib::graph::Draw_2DClear();
  root->updateWithChildren();
  effect::Step();
  root->renderWithChildren();
  effect::Draw();
  gplib::graph::Draw_2DRefresh();
  gplib::graph::Draw_3DRefresh();
}
void gplib::system::OnCreate()
{
  //エフェクトロード
  effect::Init();
  effect::LoadReses();

  //フォント
  gplib::font::Draw_CreateFont(0, 30, "メイリオ");

  //カメラ初期化
  gplib::camera::InitCamera(gplib::system::WINW / 2, gplib::system::WINH / 2, 0, 0);
  //ルートオブジェクト生成
  root = std::make_shared<Game>("Root");
  root->initGame(root);
}

void gplib::system::OnDestroy(){}

