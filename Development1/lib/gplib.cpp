//**************************************************************************************//
//
//　GP　ライブラリ
//		gplib.cpp
//**************************************************************************************//

#include "GpLib.h"
#include <dxerr.h>

#ifdef USE_DIRECTSOUND
#	include <dsound.h>
#endif

#include <mmsystem.h>
//#include <stdio.h>

#ifdef USE_DIRECTSHOW
#	include <dshow.h>
#endif

#include <ctime>
#include <cmath>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <tuple>
#include <random>
#include <algorithm>
#include <functional>
#include <memory>
#include <vector>
#include <map>
#include <unordered_map>


// リンクするライブラリの指示
#pragma	comment(lib,"dxguid.lib")
#pragma	comment(lib,"d3dxof.lib")
#pragma	comment(lib,"dxguid.lib")
#pragma	comment(lib,"d3d9.lib")
#pragma	comment(lib,"d3dx9.lib")
#pragma	comment(lib,"winmm")
#pragma	comment(lib,"dinput8.lib")

#ifdef USE_DIRECTSHOW
#	pragma comment(lib,"strmiids")
#endif

#ifdef USE_DIRECTSOUND
#	pragma	comment(lib,"dsound.lib")
#endif


//**************************************************************************************//
//マクロ　変数　宣言
//**************************************************************************************//

// FIXME:他のライブラリで使われているので、名前空間に属さずに定義
//D3Draw 関連
LPDIRECT3D9				pD3D;
D3DPRESENT_PARAMETERS	D3DPP;
LPDIRECT3DDEVICE9		pD3DDevice;
LPD3DXSPRITE			pSprite;
LPD3DXFONT				pD3DXFont;

namespace gplib {

	namespace system {

		HWND		hWnd;
		float		FrameTime;
		const int	REFRESHRATE = 60;
		const float	ONEFRAME_TIME = 1.0f / REFRESHRATE;

	}

	// DirectXのオブジェクトを安全に破棄する
	template<typename T>
	void RELEASE(T* ptr) {
		if (ptr)
		{
			ptr->Release();
			ptr = nullptr;
		}
	}

	//イメージ管理構造体
	struct ImgObj final
	{
		LPDIRECT3DTEXTURE9 tex;
		std::string filename;
		int color;
		D3DXIMAGE_INFO info;

		void init()
		{
			tex = nullptr;
			filename = "";
			color = 0;
			D3DXIMAGE_INFO temp = {};
			info = temp;
		}

		ImgObj()
		{
			init();
		}
	};

	// "リソース名"により管理を行う。
	std::unordered_map<std::string, ImgObj>	ImgTable;

	//-------------------------------------------------------
	//BGM再生関連
#ifdef USE_DIRECTSHOW

	struct DxShow
	{
		IGraphBuilder*	pBuilder;
		IMediaControl*	pMediaControl;
		IMediaEvent*	pMediaEvent;
		IMediaSeeking*	pMediaSeeking;
		IBasicAudio*	pBasicAudio;

		int use;
	};

	struct BgmObj
	{
		DxShow		bgm;
		std::string	filename;
	};

	// BGMを名前で管理
	std::unordered_map< std::string, BgmObj> BgmTable;

#endif

	//-------------------------------------------------------
	//SE再生関連
#ifdef USE_DIRECTSOUND
	// DirectSound8
	LPDIRECTSOUND8          lpDS;
	// プライマリサウンドバッファ
	LPDIRECTSOUNDBUFFER		lpPrimary;

	//同時発音できる最大数	
	enum					{ DSNONE, DSSTART, DSPLAY, DSSTOP, DSEND };
	struct PlayBuffer{
		LPDIRECTSOUNDBUFFER     pBuffer;		// プライマリサウンドバッファ
		int						ID;				//ベースになったバッファのID
		WORD					BufferSize;		//ベースになったバッファのサイズ
		int						State;			//バッファの状態
		bool					Loop;			//ループフラグ
	};

	const int				DSPLAYMAX = 10;
	struct SoundBuffer{
		LPDIRECTSOUNDBUFFER     sBuffer;	//セカンダリバッファ
		WORD										BufferSize;		//バッファのサイズ
		DWORD										Fre;
		struct PlayBuffer	PlayBuffer[DSPLAYMAX];
	};
	struct SeObj
	{
		SoundBuffer		se;
		std::string		filename;
	};

	std::unordered_map< std::string, SeObj>		SeTable;

#endif


	//**************************************************************************************//
	//　カメラ関連の定義
	//**************************************************************************************//
	namespace camera {

		//********************************************************************//
		//
		//				スクロール　関連関数
		//
		//********************************************************************//
		class Camera
		{
			float cx_, cy_;				//画面中央
			float addPx_, addPy_;	//1フレームあたりの移動量
			float tx_, ty_;				//注視点切り替え時目標
			int	  moveStep_;			//注視点切り替え時分割数 大きいほど遅くなる
			float scale_;					//拡大縮小率
			float addScale_;			//拡大率加算値
			int	  scalingStep_;	  //拡大率変化値
			float tScale_;				//目的拡大率

			float sx(const float x) const { return (x - viewLeft()) * scale_; }
			float sy(const float y) const { return (y - viewTop()) * scale_; }
			float offsetX() const   { return system::WINW / 2.0f / scale_; };
			float offsetY() const   { return system::WINH / 2.0f / scale_; };
		public:
			int  getMousePosX(const int x) { return static_cast<int>((x - system::WINW / 2) / scale() + cx()); }
			int  getMousePosY(const int y) { return static_cast<int>((y - system::WINH / 2) / scale() + cy()); }
			POINT getMousePos(const POINT& pt)
			{
				POINT r = { getMousePosX(pt.x), getMousePosY(pt.y) };
				return r;
			}

			float viewLeft()   const { return cx_ - offsetX(); }
			float viewTop()    const { return cy_ - offsetY(); }
			float viewRight()  const { return cx_ + offsetX(); }
			float viewBottom() const { return cy_ + offsetY(); }
			int   iviewLeft()  const { return static_cast<int>(viewLeft()); }
			int   iviewTop()   const { return static_cast<int>(viewTop()); }
			int   iviewRight() const { return static_cast<int>(viewRight()); }
			int   iviewBottom()const { return static_cast<int>(viewBottom()); }

			Camera(const float cx = system::WINW / 2, const float cy = system::WINH / 2,
				const float addpx = 0, const float addpy = 0, const float scale = 1.f)
				:
				cx_(cx),
				cy_(cy),
				addPx_(addpx),
				addPy_(addpy),
				scale_(scale)
			{
				tx_ = cx_;
				ty_ = cy_;
				tScale_ = scale_;
			}

			//---------------------------------------------------------------------------------------
			//引数座標がウインドウの中心になるようにカメラ位置を変更する
			//---------------------------------------------------------------------------------------
			void setLookAt(const float x, const float y)
			{
//				if (addPx_ == 0 && addPy_ == 0){
					tx_ = cx_ = (x);
					ty_ = cy_ = (y);
//				}
			}

			//---------------------------------------------------------------------------------------
			//引数座標をカメラ位置に変更する
			//---------------------------------------------------------------------------------------
			//void setPos(float x, float y)
			//{
			//	tx_ = cx_ = x + offsetX();
			//	ty_ = cy_ = y + offsetY();
			//}

			//---------------------------------------------------------------------------------------
			//カメラ処理（毎フレーム呼ぶこと）
			//---------------------------------------------------------------------------------------
			void step()
			{
				//X軸移動
				if (fabs(cx_ - tx_) >= 2.f){
					addPx_ = (tx_ - cx_) / moveStep_;
					if (fabs(addPx_) < 1.f){
						if (addPx_ < 1.f) addPx_ = 1.f;
						if (addPx_ > -1.f) addPx_ = -1.f;
					}
				}
				else if (fabs(cx_ - tx_) < 2.f){
					addPx_ = 0;
					cx_ = tx_;
				}
				else{
					addPx_ = 0;
					cx_ = tx_;
				}
				//Y軸移動
				if (fabs(cy_ - ty_) >= 2.f){
					addPy_ = (ty_ - cy_) / moveStep_;
					if (fabs(addPy_) < 1.f){
						if (addPy_ < 1.f) addPy_ = 1.f;
						if (addPy_ > -1.f) addPy_ = -1.f;
					}
				}
				else if (fabs(cy_ - ty_) < 2.f){
					addPy_ = 0;
					cy_ = ty_;
				}
				else{
					addPy_ = 0;
					cy_ = ty_;
				}
				cx_ += addPx_;
				cy_ += addPy_;

				if (abs(addScale_) <= 0.005f){
					scalingStep_ = 0;
					addScale_ = 0;
				}
				else{
					addScale_ = (float)((tScale_ - scale_) / scalingStep_);
					scale_ += addScale_;
				}
			}

			//---------------------------------------------------------------------------------------
			//1フレームあたりのカメラ移動量を設定
			//---------------------------------------------------------------------------------------
			void addForce(const float x, const float y)
			{
				addPx_ = x;
				addPy_ = y;
			}

			//---------------------------------------------------------------------------------------
			//指定フレーム分のカメラ移動量を設定
			//---------------------------------------------------------------------------------------
			void setTagetLength(const float x, const float y, const int step)
			{
				tx_ = x;
				ty_ = y;
				moveStep_ = step;
				addForce((tx_ - cx_) / moveStep_, (ty_ - cy_) / moveStep_);
			}

			//---------------------------------------------------------------------------------------
			//拡大率変更
			//---------------------------------------------------------------------------------------
			void setScale(const float scale)
			{
				scale_ = scale;
			}

			void addScale(const float offset)
			{
				scale_ += offset;
			}

			void addScaleSteps(const float newScale, const int step)
			{
				scalingStep_ = step;
				tScale_ = newScale;
				addScale_ = (float)((newScale - scale_) / step);
			}

			float cx()  const { return cx_; }
			float cy()  const { return cy_; }
			int   icx() const { return static_cast<int>(cx_); }
			int   icy() const { return static_cast<int>(cy_); }
			float scale() const { return scale_; }

			RECT getCameraViewRt() const
			{
				RECT rc = { iviewLeft(), iviewTop(), iviewRight(), iviewBottom() };
				return rc;
			}

			//入力スケールを、カメラを考慮したスケールに変換
			float changeScale(const float s) const { return s * scale_; }

			//入力座標に対して、ウインドウの座標(float)に変換
			float xchange(const float x) const { return sx(x); }
			float ychange(const float y) const { return sy(y); }
			float xchange(const int x)   const { return sx(static_cast<float>(x)); }
			float ychange(const int y)   const { return sy(static_cast<float>(y)); }

			//入力座標に対して、ウインドウの座標(int)に変換
			int   ixchange(const float x) const { return static_cast<int>(sx(x)); }
			int   iychange(const float y) const { return static_cast<int>(sy(y)); }
			int   ixchange(const int x)   const { return static_cast<int>(sx(static_cast<float>(x))); }
			int   iychange(const int y)   const { return static_cast<int>(sy(static_cast<float>(y))); }
		};

		static std::unique_ptr<Camera> camera;


		//---------------------------------------------------------------------------------------
		// 初期化
		// x, y         視点の位置
		// addpx, addpy 視点が変わった時の、１フレームあたりの移動量
		//---------------------------------------------------------------------------------------
		void InitCamera(const float x, const float y, const float addpx, const float addpy)
		{
			camera = std::unique_ptr<Camera>(new Camera(x, y, addpx, addpy));
		}

		//---------------------------------------------------------------------------------------
		// カメラ更新
		//---------------------------------------------------------------------------------------
		static void StepCamera()
		{
			camera->step();
		}

		//---------------------------------------------------------------------------------------
		// 視点指定
		// x, y 視点
		//---------------------------------------------------------------------------------------
		void SetLookAt(const float x, const float y)
		{
			camera->setLookAt(x, y);
		}

		//---------------------------------------------------------------------------------------
		//カメラ左上点指定　→　基本的にSetLookAtで問題なし
		//---------------------------------------------------------------------------------------
		//void SetPos(float x, float y)
		//{
		//	camera->setPos(x, y);
		//}

		//---------------------------------------------------------------------------------------
		// １フレームあたりのカメラの視点移動量を指定
		// x, y 視点移動量
		//---------------------------------------------------------------------------------------
		void AddCameraForce(const float x, const float y)
		{
			camera->addForce(x, y);
		}

		//---------------------------------------------------------------------------------------
		// 視点の変更
		// x, y   移動先の視点
		// speed １フレームあたりの移動量
		//---------------------------------------------------------------------------------------
		void SetCameraTarget(const float x, const float y, const int speed)
		{
			camera->setTagetLength(x, y, speed);
		}

		//---------------------------------------------------------------------------------------
		// 倍率の変更
		//---------------------------------------------------------------------------------------
		void SetScale(const float scale)
		{
			camera->setScale(scale);
		}

		//---------------------------------------------------------------------------------------
		// 倍率の取得
		//---------------------------------------------------------------------------------------
		float GetScale()
		{
			return camera->scale();
		}

		//---------------------------------------------------------------------------------------
		// 倍率を
		//---------------------------------------------------------------------------------------
		void AddScale(const float add)
		{
			camera->addScaleSteps(add, 0);
		}

		//---------------------------------------------------------------------------------------
		// 倍率
		//---------------------------------------------------------------------------------------
		void AddScale(const float add, const int step)
		{
			camera->addScaleSteps(add, step);
		}

		//---------------------------------------------------------------------------------------
		// 視点を取得
		//---------------------------------------------------------------------------------------
		POINT GetLookAt()
		{
			POINT pt = { camera->icx(), camera->icy() };
			return pt;
		}

		//---------------------------------------------------------------------------------------
		// 位置(ウィンドウ左上座標)を取得
		//---------------------------------------------------------------------------------------
		POINT GetCameraPos()
		{
			POINT pt = { camera->iviewLeft(), camera->iviewTop() };
			return pt;
		}

		//---------------------------------------------------------------------------------------
		// カメラ位置を加えたマウスのカーソル座標を取得
		//---------------------------------------------------------------------------------------
		POINT GetCursorPosition()
		{
			return camera->getMousePos(input::GetMousePosition());
		}

		//---------------------------------------------------------------------------------------
		// マウスのカーソル座標を取得
		// 左上が(0,0)
		//---------------------------------------------------------------------------------------
		POINT GetCursorPositionNC()
		{
			POINT pt = input::GetMousePosition();
			return pt;
		}

		//---------------------------------------------------------------------------------------
		// マウスの座標にカメラ位置を加える
		//---------------------------------------------------------------------------------------
		void ChangeScreenMouse(POINT& pt)
		{
			pt = camera->getMousePos(pt);
		}

		//---------------------------------------------------------------------------------------
		// カメラ位置を加えたマウスのカーソル座標を取得
		//---------------------------------------------------------------------------------------
		void GetCursorPosition(POINT& pt)
		{
			pt = camera->getMousePos(input::GetMousePosition());
		}

		//---------------------------------------------------------------------------------------
		// マウスのカーソル座標を取得
		// 左上が(0,0)
		//---------------------------------------------------------------------------------------
		void GetCursorPositionNC(POINT* pt_ptr)
		{
			input::GetMousePosition(pt_ptr);
		}

		//---------------------------------------------------------------------------------------
		// カメラを通した画面サイズを取得
		// カメラの倍率でサイズが変化します
		//---------------------------------------------------------------------------------------
		RECT GetScreenRect()
		{
			RECT cameraRt = camera->getCameraViewRt();
			//	debug::TToM("%d:%d:%d:%d",cameraRt.left,cameraRt.top,cameraRt.right,cameraRt.bottom);
			return cameraRt;
		}

		//---------------------------------------------------------------------------------------
		// 矩形がカメラを通した画面内にあるかどうか判定
		//---------------------------------------------------------------------------------------
		bool InScreenCamera(const RECT& rt)
		{
			return math::RectCheck(&rt, &GetScreenRect());
		}

		//---------------------------------------------------------------------------------------
		// 矩形がカメラを通した画面内にあるかどうか判定
		// x, y 矩形の中心位置
		// w, h 矩形の幅と高さ
		//---------------------------------------------------------------------------------------
		bool InScreenCameraCenter(const float x, const float y, const float w, const float h)
		{
			RECT ObjRect = {
				static_cast<LONG>(x - w / 2.0f),
				static_cast<LONG>(y - h / 2.0f),
				static_cast<LONG>(x + w / 2.0f),
				static_cast<LONG>(y + h / 2.0f)
			};
			return InScreenCamera(ObjRect);
		}

		//---------------------------------------------------------------------------------------
		// 矩形がカメラを通した画面内にあるかどうか判定
		// x, y 矩形の左上座標
		// w, h 矩形の幅と高さ
		//---------------------------------------------------------------------------------------
		bool InScreenCamera(const int x, const int y, const int w, const int h)
		{
			RECT ObjRect = {
				x, y,
				x + w, y + h
			};
			return InScreenCamera(ObjRect);
		}

	}


	namespace math {

		//---------------------------------------------------------------------------------------
		//ランダム利用
		//---------------------------------------------------------------------------------------
		//int i = math::GetRandom(10,100); //10から100までのランダムな整数
		//float f = math::GetRandom(10.0f,100.0f); //10.0fから100までのランダムな実数
		//---------------------------------------------------------------------------------------
		//// [min, max]の一様な乱数を取得
		//template <typename T>
		//inline T GetRandom(const T min, const T max) {
		//	std::uniform_real_distribution<T> range(min, max);
		//	return range(engine);
		//}
		//// 特殊化はこっちに書く…
		//template <>
		//inline int GetRandom<int>(const int min, const int max) {
		//	std::uniform_int_distribution<> range(min, max);
		//	return range(engine);
		//}

		std::mt19937 engine;

		//---------------------------------------------------------------------------------------
		//ランダムの初期化
		//---------------------------------------------------------------------------------------
		static void RandomInit() {
			std::random_device device;
			engine.seed(device());
		}


		//***************************************************************************************
		// 数学関連
		//***************************************************************************************
		//---------------------------------------------------------------------------------------
		//２点間の角を求める：弧度法
		//---------------------------------------------------------------------------------------
		float RadianOfPoints2(float basisX, float basisY, float targetX, float targetY)
		{
			float x = targetX - basisX;
			float y = -(targetY - basisY);  //ディスプレイ座標に合わせるために、マイナスにする
			return std::atan2(y, x);
		}

		//---------------------------------------------------------------------------------------
		//２点間の角度を求める：度数法
		//---------------------------------------------------------------------------------------
		float DegreeOfPoints2(float basisX, float basisY, float targetX, float targetY)
		{
			return Calc_RadToDegree(RadianOfPoints2(basisX, basisY, targetX, targetY));
		}

		//---------------------------------------------------------------------------------------
		//２点間の距離を求める
		//---------------------------------------------------------------------------------------
		float Distance2(float x1, float y1, float x2, float y2)
		{
			float x = pow(x1 - x2, 2);
			float y = pow(y1 - y2, 2);
			return std::sqrt(x + y);
		}

		//-------------------------------------------------------------//
		//矩形同士の重なりを調べる
		//-------------------------------------------------------------//
		bool RectCheck(const RECT& rt1, const RECT& rt2)
		{
			return (rt1.left < rt2.right &&
				rt2.left < rt1.right &&
				rt1.top  < rt2.bottom &&
				rt2.top  < rt1.bottom);
		}

		bool RectCheck(const RECT *rt1, const RECT *rt2)
		{
			return RectCheck(*rt1, *rt2);
		}

		//-------------------------------------------------------------//
		//指定座標xyが画面外にあるかを調べます
		//-------------------------------------------------------------//
		bool IsFrameOut_Center(float x, float y, float width, float height)
		{
			width /= 2.0f;
			height /= 2.0f;
			x -= camera::camera->viewLeft();
			y -= camera::camera->viewTop();
			return ((x - width) < 0 || (y - height) < 0 || (x + width) > system::WINW || (y + height) > system::WINH);
		}
		bool IsFrameOut_LeftTop(float x, float y, float width, float height)
		{
			x -= camera::camera->viewLeft();
			y -= camera::camera->viewTop();
			return (x < 0 || y < 0 || (x + width) > system::WINW || (y + height) > system::WINH);
		}
#ifdef CI_EXT
		bool IsFrameOut_Center(const ci_ext::Vec3f& pos, const ci_ext::Vec3f& size)
		{
			return IsFrameOut_Center(pos.x(), pos.y(), size.x(), size.y());
		}
		bool IsFrameOut_LeftTop(const ci_ext::Vec3f& pos, const ci_ext::Vec3f& size)
		{
			return IsFrameOut_LeftTop(pos.x(), pos.y(), size.x(), size.y());
		}

#endif

		//-------------------------------------------------------------//
		//幅X及び高さYの値から角度θを求め、CosSinの値を求める
		//戻り値　ラジアン
		//-------------------------------------------------------------//
		// 例
		//float rad = CalcXYToSinCos(30,30);
		//-------------------------------------------------------------//
		float Calc_XYToRad(int x, int y)
		{
			return static_cast<float>(atan2((double)y, (double)x));
		}

		//-------------------------------------------------------------//
		//幅X及び高さYの値から角度θを求め、CosSinの値を求める
		//戻り値　ラジアン
		//-------------------------------------------------------------//
		// 例
		//float rad = CalcXYToSinCos(30,30,&AddPx,&AddPy);
		//-------------------------------------------------------------//
		float Calc_XYToSinCos(int x, int y, float *addx, float *addy)
		{
			float rad = atan2(static_cast<float>(y), static_cast<float>(x));
			*addx = cos(rad);
			*addy = -sin(rad);

			return rad;
		}

		//-------------------------------------------------------------//
		//ラジアン値から角度に変換する
		//戻り値　float 角度
		//-------------------------------------------------------------//
		//float deg = RadToDegree(r);
		//-------------------------------------------------------------//
		float Calc_RadToDegree(float rad)
		{
			float d = (float)(rad * 180.0f / PAI);
			if (d < 0) d += 360;
			return d;
		}
		//-------------------------------------------------------------//
		//角度からラジアン値に変換する
		//戻り値　float ラジアン
		//-------------------------------------------------------------//
		//float rad = DegreeToRad(180);
		//-------------------------------------------------------------//
		float Calc_DegreeToRad(float degree)
		{
			return static_cast<float>(degree * 2.0f * M_PI / 360.0f);
		}
		//---------------------------------------------------------------------------------------
		//コサインを求める　角度に対してのX軸の長さを求める
		//---------------------------------------------------------------------------------------
		float ROUND_X(float angle, float length, float center)
		{
			return (float)(cos(angle * M_PI * 2 / 360) * length) + center;
		}
		//---------------------------------------------------------------------------------------
		//サインを求める　角度に対してのY軸の長さを求める
		//---------------------------------------------------------------------------------------
		float ROUND_Y(float angle, float length, float center)
		{
			return (float)(sin(-angle * M_PI * 2 / 360) * length) + center;
		}
		//---------------------------------------------------------------------------------------
		//コサインを求める：角度に対してのZ軸の長さを求める
		//---------------------------------------------------------------------------------------
		float ROUND_Z(float angle, float length, float center)
		{
			return (float)(tan(angle * M_PI * 2 / 360) * length) + center;
		}

		//---------------------------------------------------------------------------------------
		//四角形を反転する。
		//---------------------------------------------------------------------------------------
		void reverseRect(RECT& baseRect, int width)
		{
			RECT temp;
			SetRect(&temp, width - (baseRect.right), baseRect.top,
				width - (baseRect.left), baseRect.bottom);
			CopyRect(&baseRect, &temp);
		}

	}

	//********************************************************************//
	//
	//				高精度タイマー　関連関数
	//
	//********************************************************************//
	namespace time {

		const int	_TIMER_MAX = 10;

		//経過時間観測用
		LARGE_INTEGER _start[_TIMER_MAX];
		LARGE_INTEGER _systimer;
		LARGE_INTEGER _now;
		LARGE_INTEGER _sys;
		LARGE_INTEGER _lastFrame;

		//---------------------------------------------------------------------------------------
		//　経過時間のリセット
		//========================================
		//	//一秒毎に真を返す
		//	if(Time_MarkOfOneSec(0)){
		//		処理
		//		Time_ResetTimer(0);
		//	}
		//	//５秒毎に真を返す
		//	if(Time_MarkOfTime(0,5)){
		//		処理
		//		Time_ResetTimer(0);
		//	}
		//	Time_Update();
		//---------------------------------------------------------------------------------------

		void Time_ResetTimer(int id)
		{
			QueryPerformanceCounter(&_start[id]);
		}
		//---------------------------------------------------------------------------------------
		//　経過時間の計測開始
		//---------------------------------------------------------------------------------------
		void Time_StartTimer(int id)
		{
			QueryPerformanceCounter(&_start[id]);
		}

		//---------------------------------------------------------------------------------------
		//　一秒経過マーカー
		//---------------------------------------------------------------------------------------
		bool Time_MarkOfOneSec(int id)
		{
			return (static_cast<float>(_now.QuadPart - _start[id].QuadPart) / static_cast<float>(_sys.QuadPart) >= 1.0f);
		}

		//---------------------------------------------------------------------------------------
		//　時間経過マーカー
		//---------------------------------------------------------------------------------------
		bool Time_MarkOfTime(int id, float marktime)
		{
			return (static_cast<float>(_now.QuadPart - _start[id].QuadPart) / static_cast<float>(_sys.QuadPart) >= marktime);
		}

		//---------------------------------------------------------------------------------------
		//　経過時間の取得
		//---------------------------------------------------------------------------------------
		float Time_GetTimerCount(int id)
		{
			return  static_cast<float>(_now.QuadPart - _start[id].QuadPart) / static_cast<float>(_sys.QuadPart);
		}

		//---------------------------------------------------------------------------------------
		//　経過時間の取得
		//---------------------------------------------------------------------------------------
		float Time_GetTimerCount()
		{
			return  static_cast<float>(_now.QuadPart - _systimer.QuadPart) / static_cast<float>(_sys.QuadPart);
		}


		//---------------------------------------------------------------------------------------
		//	タイマーの更新	1Frameにつき一回の呼び出し
		//	内部利用
		//---------------------------------------------------------------------------------------
		float Time_Update()
		{
			QueryPerformanceCounter(&_now);
			system::FrameTime = static_cast<float>(_now.QuadPart - _lastFrame.QuadPart) / static_cast<float>(_sys.QuadPart);
			QueryPerformanceCounter(&_lastFrame);
			return system::FrameTime;
		}

		//---------------------------------------------------------------------------------------
		//　１フレームあたりの経過時間
		//	static float cnt;
		//  cnt += TimeGetOneFrameCount();
		//	if((int)cnt >= 1) //1秒経過
		//---------------------------------------------------------------------------------------
		float Time_GetOneFrameCount()
		{
			return system::FrameTime;
		}

		//---------------------------------------------------------------------------------------
		//　経過時間の取得開始
		//---------------------------------------------------------------------------------------
		void Time_StartTimerCount()
		{
			QueryPerformanceCounter(&_systimer);
		}

		//---------------------------------------------------------------------------------------
		//　高精度タイマーが利用可能か調べる
		//  内部リセット込み
		//---------------------------------------------------------------------------------------
		bool Time_CheckTimer()
		{
			if (!QueryPerformanceFrequency(&_sys))
				return false;
			QueryPerformanceCounter(&_lastFrame);
			Time_StartTimerCount();
			Time_Update();
			return true;
		}

	}

	//********************************************************************//
	//
	//				デバッグ　関連関数
	//
	//********************************************************************//
	namespace debug {


		//--------------------------------------------------------------------------------------------
		// FPSの表示
		//--------------------------------------------------------------------------------------------
		//　DD_Clear();
		//	DD_ShowFPS();
		//	DFPS();		でも可
		//　DD_Refresh();
		//--------------------------------------------------------------------------------------------
		void Draw_ShowFps()
		{
			static double FpsTime = (time::Time_GetTimerCount() + 1.0);
			static double FpsData = 0.0;
			static int FpsCnt = 1;
			if (time::Time_GetTimerCount() >= (unsigned)FpsTime){
				FpsData = FpsCnt;
				FpsCnt = 0;
				FpsTime = (time::Time_GetTimerCount() + 1.0);
			}
			//FPSカウント
			FpsCnt++;
			char Buffer[1024];
			sprintf_s(Buffer, "fps=%3.2f:ft=%0.3f", FpsData, system::FrameTime);
			font::Draw_FontTextNC(0, 0, 0.0f, Buffer, ARGB(255, 255, 200, 200), 0);
		}

		//---------------------------------------------------------------------------------------
		//ウインドウへメッセージの表示
		//	引数
		//		x,y			表示位置
		//		str			フォーマット前文字列
		//		...			可変個引数指定
		//---------------------------------------------------------------------------------------
		void Dbg_DisplayToMessage(int x, int y, const char * str, ...){

			char buf[256];
			vsprintf(buf, str, (char*)(&str + 1));
			font::Draw_TextXY(x, y,
				buf, ARGB(255, 255, 0, 0));
		}

		//---------------------------------------------------------------------------------------
		//ウインドウへメッセージの表示
		//	引数
		//		x,y			表示位置
		//		str			フォーマット前文字列
		//		...			可変個引数指定
		//---------------------------------------------------------------------------------------
		void Dbg_DisplayToMessageNC(int x, int y, const char * str, ...){

			char buf[256];
			vsprintf(buf, str, (char*)(&str + 1));
			font::Draw_TextXYNC(x, y,
				buf, ARGB(255, 255, 0, 0));
		}

		//---------------------------------------------------------------------------------------
		//タイトルへメッセージの表示
		//	引数
		//		str			フォーマット前文字列
		//		...			可変個引数指定
		//---------------------------------------------------------------------------------------
		void Dbg_TilteToMessage(const char * str, ...){

			char buf[256];
			vsprintf(buf, str, (char*)(&str + 1));
			SetWindowText(system::hWnd, (LPCSTR)buf);
		}

		//---------------------------------------------------------------------------------------
		//メッセージボックスへの表示
		//	引数
		//		str			フォーマット前文字列
		//		...			可変個引数指定
		//---------------------------------------------------------------------------------------
		void Dbg_BoxToMessage(const char *str, ...){

			char buf[256];
			vsprintf(buf, str, (char*)(&str + 1));
			MessageBox(NULL, (LPCSTR)buf, "Message", MB_OK);
		}

		//---------------------------------------------------------------------------------------
		//ファイルOUT関数
		//	引数
		//		str			フォーマット前文字列
		//		...			可変個引数指定
		//---------------------------------------------------------------------------------------
		void Dbg_FileOut(const char *str, ...){

			FILE*	fp;
			char buff[128];
			vsprintf(buff, (char*)str, (char*)(&str + 1));
			strcat(buff, "\n");
			if ((fp = fopen("debug.txt", "at")) != NULL){
				fprintf(fp, "%s", buff);
				fclose(fp);
			}
		}

		//---------------------------------------------------------------------------------------
		//アプリケーションを終了させる要求をWIndowsに送ります（メモリリークに注意！）
		//---------------------------------------------------------------------------------------
		void Dbg_ExitApp()
		{
			DestroyWindow(system::hWnd);
		}

		//-------------------------------------------------------------//
		// Dialog 関連define
#define IDD_DIALOG1                     101
#define ID_DLG_INPUT                    101
#define ID_EDIT                         1001
#define ID_LABEL                        1002
		//-------------------------------------------------------------//
		//	入力関数
		//-------------------------------------------------------------//
		struct DLG_DATA
		{
			char title[MAX_PATH];
			char label[MAX_PATH];
			char get[MAX_PATH];
			int size;
		};

		//-------------------------------------------------------------//
		//ダイアログプロシージャ
		//	外部ファイルから呼び出し出来ません
		//-------------------------------------------------------------//
		static int CALLBACK DlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
		{
			static DLG_DATA* d_ptr = NULL;

			switch (msg)
			{
				//ダイアログ初期化処理
			case WM_INITDIALOG:
				d_ptr = (DLG_DATA*)lParam;	//呼び出し元からデータへのポインタを受け取る
				if (d_ptr->title[0] != '\0') SetWindowText(hDlg, d_ptr->title);	//タイトル変更
				if (d_ptr->label[0] != '\0') SetWindowText(GetDlgItem(hDlg, ID_LABEL), d_ptr->label);//ラベル変更
				SetFocus(hDlg);
				break;
				//ボタン処理
			case WM_COMMAND:
				switch (LOWORD(wParam))
				{
					//「ok」ボタンが押された
				case IDOK:
					GetDlgItemText(hDlg, ID_EDIT, d_ptr->get, MAX_PATH);
					EndDialog(hDlg, 1);
					break;
					//「キャンセル」ボタンもしくは「x」ボタンが押された
				case IDCANCEL:
					EndDialog(hDlg, 0);
					break;
				default:
					return 0;	//Windowsに任せるので0を返す
				}
				break;
				//既定の処理：Windowsに任せる
			default:
				return 0;	//Windowsに任せるので0を返す
			}
			return 1;		//自分でメッセージを処理したので1を返す
		}

		//-------------------------------------------------------------//
		//文字列入力関数
		//	引数
		//		get			取得した文字の格納先
		//		size		getのサイズ
		//		title		タイトルの文字列
		//		label		エディットボックスに表示する文字列
		//-------------------------------------------------------------//
		//	使用例
		//		char str[100];
		//		Dbg_InputDialog(str, 100, "確認", "名前を入力してください");
		//-------------------------------------------------------------//
		DLG_ERROR Dbg_InputDialog(char* get, int size, const char* title, const char* label)
		{
			DLG_DATA data;

			if (get == NULL) return DLG_ERROR_INVALIDBUFFER;
			//タイトルコピー
			if (title != NULL)
			{
				memcpy(&data.title, title, MAX_PATH);
			}
			else
			{
				data.title[0] = '\0';
			}
			//ラベルコピー
			if (label != NULL)
			{
				memcpy(&data.label, label, MAX_PATH);
			}
			else
			{
				data.label[0] = '\0';
			}
			//ダイアログ表示
			if (DialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(ID_DLG_INPUT),
				system::hWnd, (DLGPROC)DlgProc, (LPARAM)&data) == 0)
			{
				//「キャンセル」ボタンもしくは「x」ボタンが押された
				return DLG_ERROR_CANCEL;
			}
			//「ok」が押された（入力されていなくてもココにくる）
			memcpy(get, data.get, size - 1);
			get[size - 1] = '\0';
			return DLG_OK;
		}

	}


	//********************************************************************//
	//
	//				ステージ管理　関連関数
	//
	//********************************************************************//
	namespace stage {

		//ステージ番号
		int	nowStageNo;

		int _stageNo;
		int	_initFlag;

		//---------------------------------------------------------------------------------------
		//ステージを引数で指定されたステージに切り替える
		// ---------------------------------------------------------------------------------------
		void ChangeStage(const int stage)
		{
			_stageNo = stage;
			_initFlag = 1;
		}

		//---------------------------------------------------------------------------------------
		//ステージ切り替え時にステージ初期化を行うかを調べる
		//---------------------------------------------------------------------------------------
		bool InitStage()
		{
			if (_initFlag == 1)
			{
				_initFlag = 0;
				return true;
			}
			return false;
		}

		//---------------------------------------------------------------------------------------
		//ステージ管理関数の初期化
		//---------------------------------------------------------------------------------------
		void InitStageFlag()
		{
			_initFlag = 1;
			_stageNo = 0;
		}

		//---------------------------------------------------------------------------------------
		//ステージ番号を調べる
		//---------------------------------------------------------------------------------------
		int CheckStage()
		{
			return _stageNo;
		}

	}


	//********************************************************************//
	//
	//				キー　関連関数
	//
	//********************************************************************//
	namespace input {

		//ホイール値取得
		int delta;

		//キー情報構造体
		struct KEYINFO
		{
			int code;		// 仮想キーコード
			int status;		// キーの状態(KEYSTATE)
			int frameHold;	// 押されているフレーム数
			int frameFree;	// はなされているフレーム数
		};
		//キー情報
		static KEYINFO g_key[system::PLAYER_NUM][KEY_MAX];

		static KEYINFO padbase[KEY_MAX] =
		{
			//  仮想キーコード, KEYSTATE, 押されているフレーム数
			{ JOY_DOWN, FREE_KEY, 0 }, //DOWN,
			{ JOY_LEFT, FREE_KEY, 0 }, //LEFT,
			{ JOY_UP, FREE_KEY, 0 }, //UP,
			{ JOY_RIGHT, FREE_KEY, 0 }, //RIGHT,
			{ JOY_BUTTON1, FREE_KEY, 0 }, //BTN_0,
			{ JOY_BUTTON2, FREE_KEY, 0 }, //BTN_1,
			{ JOY_BUTTON3, FREE_KEY, 0 }, //BTN_2,
			{ JOY_BUTTON4, FREE_KEY, 0 }, //SPACE,
			{ VK_F1, FREE_KEY, 0 }, //F1,
			{ VK_F2, FREE_KEY, 0 }, //F2,
			{ MK_LBUTTON, FREE_KEY, 0 }, //KEY_MOUSE_LBTN,
			{ MK_RBUTTON, FREE_KEY, 0 }, //KEY_MOUSE_RBTN,
		};

		//キー割り当て
		//２Pでキーボード使用の際は書き換え必須
		static KEYINFO keybase[system::PLAYER_NUM][KEY_MAX] =
		{
			{
				//1P
				//  仮想キーコード, KEYSTATE, 押されているフレーム数
				{ VK_DOWN, FREE_KEY, 0 }, //DOWN,
				{ VK_LEFT, FREE_KEY, 0 }, //LEFT,
				{ VK_UP, FREE_KEY, 0 }, //UP,
				{ VK_RIGHT, FREE_KEY, 0 }, //RIGHT,
				{ 'A', FREE_KEY, 0 }, //BTN_0,
				{ 'S', FREE_KEY, 0 }, //BTN_1,
				{ 'D', FREE_KEY, 0 }, //BTN_2,
				{ VK_SPACE, FREE_KEY, 0 }, //SPACE,
				{ VK_F1, FREE_KEY, 0 }, //F1,
				{ VK_F2, FREE_KEY, 0 }, //F2,
				{ MK_LBUTTON, FREE_KEY, 0 }, //KEY_MOUSE_LBTN,
				{ MK_RBUTTON, FREE_KEY, 0 }, //KEY_MOUSE_RBTN,
			},
			{
				//2P
				//  仮想キーコード, KEYSTATE, 押されているフレーム数
				{ VK_DOWN, FREE_KEY, 0 }, //DOWN,
				{ VK_LEFT, FREE_KEY, 0 }, //LEFT,
				{ VK_UP, FREE_KEY, 0 }, //UP,
				{ VK_RIGHT, FREE_KEY, 0 }, //RIGHT,
				{ 'A', FREE_KEY, 0 }, //BTN_0,
				{ 'S', FREE_KEY, 0 }, //BTN_1,
				{ 'D', FREE_KEY, 0 }, //BTN_2,
				{ VK_SPACE, FREE_KEY, 0 }, //SPACE,
				{ VK_F1, FREE_KEY, 0 }, //F1,
				{ VK_F2, FREE_KEY, 0 }, //F2,
				{ MK_LBUTTON, FREE_KEY, 0 }, //KEY_MOUSE_LBTN,
				{ MK_RBUTTON, FREE_KEY, 0 }, //KEY_MOUSE_RBTN,
			}
		};

		JOYINFO* JoyInfo;
		int padcount;
		bool usePad[system::PLAYER_NUM]; // trueならパッド使用


		//ホイールの状態を戻す
		int GetWheelDelta()
		{
			return delta;
		}

		//ホイールの状態を戻す
		WHEEL GetWheelRoll()
		{
			if (delta < 0){
				return MWHEEL_UP;
			}
			else if (delta > 0){
				return MWHEEL_DOWN;
			}
			return MWHEEL_NEUTRAL;
		}

		//デルタ値のクリア
		void ClearWheelDelta()
		{
			delta = 0;
		}

		bool IsUsePad(int playerID)
		{
			if (playerID < 0 || playerID >= system::PLAYER_NUM) assert(0);
			return usePad[playerID];
		}

		static void SetKeysInfo()
		{
			for (int i = 0; i < system::PLAYER_NUM; i++)
			{
				//パッド優先
				if (i < padcount){
					usePad[i] = true;
					memcpy(g_key[i], padbase, sizeof(KEYINFO)* KEY_MAX);
				}
				else{
					usePad[i] = false;
					memcpy(g_key[i], keybase, sizeof(KEYINFO)* KEY_MAX);
				}
			}
		}

		void InitPlayerInput()
		{
			if (system::KeyboardMode == 0){
				//パッドカウント
				for (int i = 0; i < system::PLAYER_NUM; i++){
					JOYINFO		JoyInfoTemp;
					if (joyGetPos(i, &JoyInfoTemp) == JOYERR_NOERROR){
						padcount++;
					}
				}
			}
			else{	//1時は強制的に0個にする。
				padcount = 0;
			}
			//メモリ確保
			JoyInfo = (JOYINFO*)malloc(sizeof(JOYINFO)*padcount);
			//パッド生成
			SetKeysInfo();
		}

		//--------------------------------------------------------------------------------------------
		//	パッドデータ消去
		//--------------------------------------------------------------------------------------------
		static void DeletePlayerInput()
		{
			free(JoyInfo);
		}

		//--------------------------------------------------------------------------------------------
		//	パッドデータ取得
		//--------------------------------------------------------------------------------------------
		static BOOL JoyGet(int JoyID)
		{
			//ジョイスティック状態取得
			if (joyGetPos(JoyID, &JoyInfo[JoyID]) != JOYERR_NOERROR){
				return 0;
			}
			return 1;
		}

		//-------------------------------------------------------------//
		//　ジョイスティックのボタン押下状態判定
		//		引数：	Joy　ジョイスティックID　JOY１　JOY2のどちらか
		//				Code ボタン番号　BUTTON1　から順に２，３，４
		//				Flag　状態を受け取るポインタ変数
		//-------------------------------------------------------------//
		static void JoyPressButton(int Joy, UINT Code, int *Flag)
		{
			if ((JoyInfo[Joy].wButtons	&	Code)){
				*Flag = (*Flag == FREE_KEY) ? PUSH_KEY : HOLD_KEY;
			}
			else{
				*Flag = (*Flag == PUSH_KEY || *Flag == HOLD_KEY) ? PULL_KEY : FREE_KEY;
			}
		}

		//-------------------------------------------------------------//
		//　ジョイスティックの十字キー押下状態判定
		//		引数：	Joy　ジョイスティックID　JOY１　JOY2のどちらか
		//				Direct 方向　JOY_UP,JOY_DOWN,JOY_LEFT,JOY_RIGHTのどれか
		//				Flag　状態を受け取るポインタ変数
		//-------------------------------------------------------------//
		static void JoyPressXY(int Joy, int Direct, int *Flag)
		{
			switch (Direct){
			case JOY_DOWN:
				if (JoyInfo[Joy].wYpos > 40000){
					*Flag = (*Flag == FREE_KEY) ? PUSH_KEY : HOLD_KEY;
				}
				else{
					*Flag = (*Flag == PUSH_KEY || *Flag == HOLD_KEY) ? PULL_KEY : FREE_KEY;
				}
				break;

			case JOY_UP:
				if (JoyInfo[Joy].wYpos < 2000){
					*Flag = (*Flag == FREE_KEY) ? PUSH_KEY : HOLD_KEY;
				}
				else{
					*Flag = (*Flag == PUSH_KEY || *Flag == HOLD_KEY) ? PULL_KEY : FREE_KEY;
				}
				break;

			case JOY_RIGHT:
				if (JoyInfo[Joy].wXpos > 40000){
					*Flag = (*Flag == FREE_KEY) ? PUSH_KEY : HOLD_KEY;
				}
				else{
					*Flag = (*Flag == PUSH_KEY || *Flag == HOLD_KEY) ? PULL_KEY : FREE_KEY;
				}
				break;

			case JOY_LEFT:
				if (JoyInfo[Joy].wXpos < 2000){
					*Flag = (*Flag == FREE_KEY) ? PUSH_KEY : HOLD_KEY;
				}
				else{
					*Flag = (*Flag == PUSH_KEY || *Flag == HOLD_KEY) ? PULL_KEY : FREE_KEY;
				}
				break;
			}
		}

		//--------------------------------------------------------------------------------------------
		//押されたタイミングをチェックします
		//--------------------------------------------------------------------------------------------
		static bool IsHoldTiming(int now, int prev)
		{
			return now == PUSH_KEY ||					//今押された　もしくは
				(prev != PUSH_KEY && prev != now);	//1フレーム前に押されてなく、1フレーム前とステータスが違うとき
		}

		//--------------------------------------------------------------------------------------------
		//はなされたタイミングをチェックします
		//--------------------------------------------------------------------------------------------
		static bool IsPullTiming(int now, int prev)
		{
			return now == PULL_KEY ||					//今はなされた　もしくは
				(prev != PULL_KEY && prev != now);	//1フレーム前にはなされてなく、1フレーム前とステータスが違うとき
		}

		//--------------------------------------------------------------------------------------------
		//フレーム開始時に呼び、ゲームで使うキーの状態を調べます
		//毎フレーム必ず呼ばなければなりません
		//--------------------------------------------------------------------------------------------
		void CheckKey()
		{
			int i, prevStatus;

			for (int playernum = 0; playernum < system::PLAYER_NUM; playernum++)
			{
				if (playernum < padcount){
					for (i = 0; i < KEY_MAX; i++)
					{
						//パッドでもマウスのボタンは認識させる
						if (i < KEY_BTNMAX){
							//フレームカウンタチェック
							if (CheckPress(i, 1, playernum)) g_key[playernum][i].frameHold++;	//押されているフレームをカウント
							else						g_key[playernum][i].frameFree++;	//はなされているフレームをカウント

							//前回のキーを保持し、次に押されたキーと判別する
							prevStatus = g_key[playernum][i].status;
							JoyGet(playernum);
							if (g_key[playernum][i].code == JOY_DOWN || g_key[playernum][i].code == JOY_UP ||
								g_key[playernum][i].code == JOY_RIGHT || g_key[playernum][i].code == JOY_LEFT){
								JoyPressXY(playernum, g_key[playernum][i].code, &g_key[playernum][i].status);
							}
							else{
								JoyPressButton(playernum, g_key[playernum][i].code, &g_key[playernum][i].status);
							}
							//押されたタイミングとはなされたタイミングでカウンタを0クリアする
							if (IsHoldTiming(g_key[playernum][i].status, prevStatus))		g_key[playernum][i].frameHold = 0;	//ステータスが違うのでフレームを0に戻す
							else if (IsPullTiming(g_key[playernum][i].status, prevStatus))	g_key[playernum][i].frameFree = 0;	//ステータスが違うのでフレームを0に戻す
						}
						else{
							//フレームカウンタチェック
							if (CheckPress(i, 1, playernum)) g_key[playernum][i].frameHold++;	//押されているフレームをカウント
							else						g_key[playernum][i].frameFree++;	//はなされているフレームをカウント

							//前回のキーを保持し、次に押されたキーと判別する
							prevStatus = g_key[playernum][i].status;
							Key_GetKey(g_key[playernum][i].code, &g_key[playernum][i].status);

							//押されたタイミングとはなされたタイミングでカウンタを0クリアする
							if (IsHoldTiming(g_key[playernum][i].status, prevStatus))		g_key[playernum][i].frameHold = 0;	//ステータスが違うのでフレームを0に戻す
							else if (IsPullTiming(g_key[playernum][i].status, prevStatus))	g_key[playernum][i].frameFree = 0;	//ステータスが違うのでフレームを0に戻す
						}
					}
				}
				else{
					for (i = 0; i < KEY_MAX; i++)
					{
						//フレームカウンタチェック
						if (CheckPress(i, 1, playernum)) g_key[playernum][i].frameHold++;	//押されているフレームをカウント
						else			  g_key[playernum][i].frameFree++;	//はなされているフレームをカウント

						//前回のキーを保持し、次に押されたキーと判別する
						prevStatus = g_key[playernum][i].status;
						Key_GetKey(g_key[playernum][i].code, &g_key[playernum][i].status);

						//押されたタイミングとはなされたタイミングでカウンタを0クリアする
						if (IsHoldTiming(g_key[playernum][i].status, prevStatus))		g_key[playernum][i].frameHold = 0;	//ステータスが違うのでフレームを0に戻す
						else if (IsPullTiming(g_key[playernum][i].status, prevStatus))	g_key[playernum][i].frameFree = 0;	//ステータスが違うのでフレームを0に戻す
					}
				}
			}
		}

		//--------------------------------------------------------------------------------------------
		//キーが「おされている」かをチェックします
		//intervalで与えたフレーム単位でtrueがかえってきます
		//【例】
		//intervalが1なら毎フレームtrue
		//intervalが2なら2フレームに一回true
		//intervalが5なら5フレームに一回true
		//--------------------------------------------------------------------------------------------
		bool CheckPress(int keyno, int interval, int playernum)
		{
			interval = interval > 0 ? interval : 1;	//0除算対策

			//キーステータスが「おされた」「おされている」のどちらかで
			//キーが押されたフレームが希望設定と一致しているか？
			if ((g_key[playernum][keyno].status == HOLD_KEY ||
				g_key[playernum][keyno].status == PUSH_KEY) &&
				g_key[playernum][keyno].frameHold % interval == 0)
			{
				return true;
			}
			return false;
		}

		//--------------------------------------------------------------------------------------------
		//いずれかのデバイスのキーが押されているかをチェックします
		//--------------------------------------------------------------------------------------------
		bool CheckAnyPress(int keyno)
		{
			for (int i = 0; i < system::PLAYER_NUM; ++i)
			if (CheckPress(keyno, 1, i)) return true;

			return false;
		}

		//--------------------------------------------------------------------------------------------
		//キーが「はなされている」かをチェックします
		//intervalで与えたフレーム単位でtrueがかえってきます
		//【例】
		//intervalが1なら毎フレームtrue
		//intervalが2なら2フレームに一回true
		//intervalが5なら5フレームに一回true
		//--------------------------------------------------------------------------------------------
		bool CheckFree(int keyno, int interval, int playernum)
		{
			interval = interval > 0 ? interval : 1;	//0除算対策

			//キーステータスが「はなされた」「はなされている」のどちらかで
			//キーが押されたフレームが希望設定と一致しているか？
			if ((g_key[playernum][keyno].status == FREE_KEY ||
				g_key[playernum][keyno].status == PULL_KEY) &&
				g_key[playernum][keyno].frameFree % interval == 0)
			{
				return true;
			}
			return false;
		}

		//--------------------------------------------------------------------------------------------
		//キーがこのフレームで「おされた」かをチェックします
		//--------------------------------------------------------------------------------------------
		bool CheckPush(int keyno, int playernum)
		{
			if (g_key[playernum][keyno].status == PUSH_KEY)
			{
				return true;
			}
			return false;
		}

		//--------------------------------------------------------------------------------------------
		//いずれかのデバイスのキーが押されているかをチェックします
		//--------------------------------------------------------------------------------------------
		bool CheckAnyPush(int keyno)
		{
			for (int i = 0; i < system::PLAYER_NUM; ++i)
			if (CheckPush(keyno, i)) return true;

			return false;
		}

		//--------------------------------------------------------------------------------------------
		//キーがこのフレームで「はなされた」かをチェックします
		//--------------------------------------------------------------------------------------------
		bool CheckPull(int keyno, int playernum)
		{
			if (g_key[playernum][keyno].status == PULL_KEY)
			{
				return true;
			}
			return false;
		}

		//--------------------------------------------------------------------------------------------
		//キーのステータスを返します
		//--------------------------------------------------------------------------------------------
		int GetKeyStatus(int keyno, int playernum)
		{
			return g_key[playernum][keyno].status;
		}

		//--------------------------------------------------------------------------------------------
		//キーのフレームを返します
		//--------------------------------------------------------------------------------------------
		int GetKeyFrameHold(int keyno, int playernum)
		{
			return g_key[playernum][keyno].frameHold;
		}

		int GetKeyFrameFree(int keyno, int playernum)
		{
			return g_key[playernum][keyno].frameFree;
		}

		//--------------------------------------------------------------------------------------------
		//キーのフレームを初期化します
		//--------------------------------------------------------------------------------------------
		void InitKeyInfo(int playernum)
		{
			for (int i = 0; i < KEY_MAX; i++)
			{
				g_key[playernum][i].status = FREE_KEY;
				g_key[playernum][i].frameHold = 0;
				g_key[playernum][i].frameFree = 0;
			}
		}

		//--------------------------------------------------------------------------------------------
		//キーコンフィグ
		//--------------------------------------------------------------------------------------------
		void SetKeyFormat(CODE_KEY key, DWORD keycode, int playernum)
		{
			g_key[playernum][key].code = keycode;
			g_key[playernum][key].status = FREE_KEY;
			g_key[playernum][key].frameHold = 0;
			g_key[playernum][key].frameFree = 0;
		}

		//--------------------------------------------------------------------------------------------
		//キーコンフィグ
		//--------------------------------------------------------------------------------------------
		void SetKeysFormat(DWORD* keycode, int playernum)
		{
			for (int i = 0; i < KEY_MAX; ++i)
			{
				g_key[playernum][i].code = keycode[i];
				g_key[playernum][i].status = FREE_KEY;
				g_key[playernum][i].frameHold = 0;
				g_key[playernum][i].frameFree = 0;
			}
		}

		//---------------------------------------------------------------------------------------
		//マウスカーソルの座標を取得(戻り値)
		//---------------------------------------------------------------------------------------
		POINT GetMousePosition()
		{
			POINT pt;

			GetCursorPos(&pt);
			ScreenToClient(system::hWnd, &pt);
			return pt;
		}

		//---------------------------------------------------------------------------------------
		//マウスカーソルの座標を取得(引数)
		//---------------------------------------------------------------------------------------
		void GetMousePosition(POINT* pt_ptr)
		{
			*pt_ptr = GetMousePosition();
		}

		//---------------------------------------------------------------------------------------
		//キー状態取得
		//Code　取得したいキーの仮想キーコード、ＡＳＣＩＩコード
		//Flag　キーの状態を取得したいバッファへのポインタ
		//---------------------------------------------------------------------------------------
		void Key_GetKey(unsigned int Code, int* Flag)
		{
			if ((GetKeyState(Code) & 0x80) == 0x80){
				if (*Flag == FREE_KEY){
					*Flag = PUSH_KEY;
				}
				else{
					*Flag = HOLD_KEY;
				}
			}
			else{
				if (*Flag == PUSH_KEY || *Flag == HOLD_KEY){
					*Flag = PULL_KEY;
				}
				else{
					*Flag = FREE_KEY;
				}
			}
		}

	}


	//********************************************************************//
	//
	//				描画　関連関数
	//
	//********************************************************************//
	namespace graph {

		//---------------------------------------------------------------------------------------
		//　半透明描画時に再描画を行う
		//---------------------------------------------------------------------------------------
		static void DD_ReDraw()
		{
			pSprite->End();
			//pD3DDevice->EndScene();
			//pD3DDevice->BeginScene();
			pSprite->Begin(D3DXSPRITE_ALPHABLEND);
		}

		static void DD_ReDrawCheck(const D3DCOLOR color)
		{
			if ((color & 0xff000000) != 0xff000000) DD_ReDraw();
		}

		//--------------------------------------------------------------------------------------------
		//画像データサイズ取得
		//--------------------------------------------------------------------------------------------
		D3DSURFACE_DESC Draw_GetImageSize(const std::string& resname)
		{
			D3DSURFACE_DESC TexData;
			ImgTable.at(resname).tex->GetLevelDesc(0, &TexData);
			return TexData;
		}

		unsigned int Draw_GetImageWidth(const std::string& resname)
		{
			return ImgTable.at(resname).info.Width;
		}

		unsigned int Draw_GetImageHeight(const std::string& resname)
		{
			return ImgTable.at(resname).info.Height;
		}

		SIZE Draw_GetImageSize2(const std::string& resname)
		{
			SIZE sz = { ImgTable.at(resname).info.Width, ImgTable.at(resname).info.Height };
			return sz;
		}

		//-------------------------------------------------------------//
		//特殊合成開始
		//-------------------------------------------------------------//
		//使用例
		//	Draw_SetRenderMode(ADD);
		//	描画
		//	Draw_EndRenderMode();
		//-------------------------------------------------------------//
		void Draw_SetRenderMode(int Mode)
		{
			if (Mode < 0 || Mode >= BF_MAX) return;

			//Clear時に一度Beginやっているので一回終わらせる
			pSprite->End();
			pSprite->Begin(D3DXSPRITE_ALPHABLEND);

			//アルファブレンディングの有効化はInitDx()へ移動
			//	pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,true);

			DWORD flag[6][3] =
			{
				{ D3DBLENDOP_ADD, D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA },
				{ D3DBLENDOP_ADD, D3DBLEND_SRCALPHA, D3DBLEND_ONE },
				{ D3DBLENDOP_ADD, D3DBLEND_ONE, D3DBLEND_INVSRCALPHA },
				{ D3DBLENDOP_ADD, D3DBLEND_SRCALPHA, D3DBLEND_SRCCOLOR },
				{ D3DBLENDOP_REVSUBTRACT, D3DBLEND_SRCALPHA, D3DBLEND_INVSRCCOLOR },
				{ D3DBLENDOP_ADD, D3DBLEND_SRCALPHA, D3DBLEND_INVSRCCOLOR }
			};

			pD3DDevice->SetRenderState(D3DRS_BLENDOP, flag[Mode][0]);
			pD3DDevice->SetRenderState(D3DRS_SRCBLEND, flag[Mode][1]);  //SRCの設定
			pD3DDevice->SetRenderState(D3DRS_DESTBLEND, flag[Mode][2]);//DESTの設定
		}


		//-------------------------------------------------------------//
		//特殊効果終了
		//-------------------------------------------------------------//
		//使用例
		//	Draw_SetRenderMode参照
		//-------------------------------------------------------------//
		void Draw_EndRenderMode()
		{
			Draw_SetRenderMode(NORMAL);
		}



		//---------------------------------------------------------------------------------------
		// ImgTableを初期化する。
		//---------------------------------------------------------------------------------------
		static void Draw_InitImgTable()
		{
			ImgTable.clear();
		}

		//---------------------------------------------------------------------------------------
		// ImgTableを破棄する。
		//---------------------------------------------------------------------------------------
		static void Draw_DelImgTable()
		{
			for (auto& img : ImgTable)
			{
				RELEASE(img.second.tex);
			}
		}

		//-------------------------------------------------------------//
		//　ポリゴン描画
		//	引数
		//		X1,Y1			矩形の開始座標
		//		X2,Y2			矩形の幅高さ
		//		Color1			塗り色
		//-------------------------------------------------------------//
		static void Draw_2DPolygonNC(
			int startX, int startY,
			int endX, int endY,
			float z, D3DCOLOR color)
		{
			struct CUSTOMVERTEX {
				FLOAT x, y, z;
				FLOAT rhw;
				DWORD color;
			};

			CUSTOMVERTEX v[4] = {
				{ static_cast<float>(endX), static_cast<float>(startY), z, 1.0f, color },
				{ static_cast<float>(endX), static_cast<float>(endY), z, 1.0f, color },
				{ static_cast<float>(startX), static_cast<float>(startY), z, 1.0f, color },
				{ static_cast<float>(startX), static_cast<float>(endY), z, 1.0f, color }
			};
			pD3DDevice->SetTexture(0, NULL);
			pD3DDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
			pD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, v, sizeof (CUSTOMVERTEX));
		}

		//-------------------------------------------------------------//
		//矩形描画
		//	引数
		//		left,top			矩形の開始座標
		//		right,bottom			矩形の幅高さ
		//		fillColor			塗り色
		//		frameColor			枠色
		//		lineSize			ライン幅
		//		nakanuri		塗りつぶすか？
		//		z				表示Z軸座標0.0 から1.0の間で指定　0.0に近いほど手前になる
		//-------------------------------------------------------------//
		void Draw_BoxNC(
			int left, int top, int right, int bottom,
			float z,
			D3DCOLOR fillColor, D3DCOLOR frameColor,
			int lineSize, bool nakanuri)
		{
			DD_ReDrawCheck(fillColor);
			if (nakanuri)
			{
				Draw_2DPolygonNC(left, top, right, bottom, z, fillColor);
			}

			POINT vec[] =
			{
				{ left, top },
				{ left, bottom },
				{ right, bottom },	// ライン
				{ right, top },	// ライン
				{ left, top },	// ラインの終点
			};
			DD_ReDrawCheck(frameColor);
			for (int i = 0; i < 4; i++)
			{
				Draw_LineNC(
					vec[i].x, vec[i].y,
					vec[i + 1].x, vec[i + 1].y,
					z, frameColor, lineSize);
			}
		}
		void Draw_BoxCenterNC(
			int x, int y, int sizeX, int sizeY,
			float z,
			D3DCOLOR fillColor, D3DCOLOR frameColor,
			int lineSize, bool nakanuri)
		{
			int sx = sizeX / 2,
				sy = sizeY / 2;
			Draw_BoxNC(x - sx, y - sy, x + sx, y + sy, z,
				fillColor, frameColor, lineSize, nakanuri);
		}
		//-------------------------------------------------------------//
		//直線描画
		//	引数
		//		X1,Y1			矩形の開始座標
		//		X2,Y2			矩形の幅高さ
		//		Color2			色
		//		Size			ライン幅
		//		z				表示Z軸座標0.0 から1.0の間で指定　0.0に近いほど手前になる
		//-------------------------------------------------------------//
		void Draw_LineNC(
			int startX, int startY,
			int endX, int endY,
			float z, D3DCOLOR color, int size)
		{
			struct CUSTOMVERTEX {
				FLOAT x, y, z;
				FLOAT rhw;
				DWORD color;
			};

			D3DCOLOR c = color;
			for (int i = 0; i < size; i++){
				int tx, ty;
				if (abs(startX - endX) > abs(startY - endY)){
					tx = 0;
					ty = i - size / 2;
				}
				else{
					tx = i - size / 2;
					ty = 0;
				}
				CUSTOMVERTEX v[2] = {
					{ static_cast<float>(startX + tx), static_cast<float>(startY + ty), z, 1.0f, c },
					{ static_cast<float>(endX + tx), static_cast<float>(endY + ty), z, 1.0f, c },
				};
				pD3DDevice->SetTexture(0, NULL);
				pD3DDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
				pD3DDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1, v, sizeof (CUSTOMVERTEX));
			}
		}

		//-------------------------------------------------------------//
		//RECTを表示する。
		//-------------------------------------------------------------//
		void		Draw_CkRectNC(const RECT& rt, D3DCOLOR color)
		{
			Draw_BoxNC(rt.left, rt.top, rt.right, rt.bottom,
				0.0f, ARGB(254, 0, 0, 0), color, 1, 0);
		}

		//---------------------------------------------------------------------------------------
		//IMGOBJを表示する。回転、拡大、色変更対応　指定した座標の中心に描画
		//	引数
		//		X1,Y1			表示中心開始座標
		//		z				表示Z軸座標0.0 から1.0の間で指定　0.0に近いほど手前になる
		//		IMGOBJ			描画したいIMGNO
		//		TempX1,TempY1	IMGNOからの参照座標
		//		TempX2,TempY2	TempX1,TempY1からの幅、高さ
		//		deg				回転角度
		//		ScaleX			X軸の倍率 1.0が等倍
		//		ScaleY			X軸の倍率 1.0が等倍
		//		a				αチャンネル
		//		r				赤の強さ
		//		g				緑の強さ
		//		b				青の強さ
		//---------------------------------------------------------------------------------------
		//Draw_Graphics(100,100,1.0f,TestImg,0,0,100,100,
		//				90,1.0f,1.0f,255,255,255,255);
		//---------------------------------------------------------------------------------------
		void Draw_GraphicsNC(
			int x, int y, float z,
			const std::string& resname,
			int srcX, int srcY, int sizeX, int sizeY,
			int	degree, 
			float	scaleX, float scaleY,
			u_char a, u_char r, u_char g, u_char b)
		{
			//描画矩形計算
			RECT rt =
			{
				srcX,
				srcY,
				srcX + sizeX,
				srcY + sizeY
			};

			D3DXVECTOR2 centerPos(static_cast<float>(sizeX) / 2.0f, static_cast<float>(sizeY) / 2.0f);

			//	if(a != 255)DD_ReDraw();
			D3DXMATRIX matDraw;	//	座標変換マトリックスの格納先
			D3DXVECTOR3 ptCenter(centerPos.x, centerPos.y, 0.0f);	//	描画の基準値の設定
			D3DXVECTOR3 position(0.0f, 0.0f, z);	//	表示する位置を指定
			D3DXVECTOR2 draw(static_cast<float>(x), static_cast<float>(y));	//	描画先座標（演算前）
			D3DXVECTOR2 scale(scaleX, scaleY);	//	スケーリング係数（倍率を指定）

			D3DXVECTOR2	 sCenter;
			sCenter = D3DXVECTOR2(0, 0);

			D3DXMatrixTransformation2D(
				&matDraw,
				&sCenter,
				0.0f,
				&scale,
				NULL,
				D3DXToRadian(-degree),
				&draw);
			pSprite->SetTransform(&matDraw);
			//描画
			pSprite->Draw(
				ImgTable.at(resname).tex,
				&rt,
				&ptCenter,
				&position,
				D3DCOLOR_ARGB(a, r, g, b));
			//ワールドのリセット
			//	D3DXMatrixRotationZ(&matDraw,0.0f);
			//	pSprite->SetTransform(&matDraw);
		}
		void Draw_GraphicsNC(
			float x, float y, float z,
			const std::string& resname,
			int srcX, int srcY, int sizeX, int sizeY,
			int	degree, 
			float	scaleX, float scaleY,
			u_char a, u_char r, u_char g, u_char b)
		{
			Draw_GraphicsNC(
				static_cast<int>(x),
				static_cast<int>(y),
				z, resname,
				srcX, srcY, sizeX, sizeY,
				degree, scaleX, scaleY,
				a, r, g, b);
		}

		//---------------------------------------------------------------------------------------
		//IMGOBJを表示する。回転、拡大、色変更対応
		//	引数
		//		X1,Y1			表示開始座標
		//		z				表示Z軸座標0.0 から1.0の間で指定　0.0に近いほど手前になる
		//		IMGOBJ			描画したいIMGNO
		//		TempX1,TempY1	IMGNOからの参照座標
		//		TempX2,TempY2	TempX1,TempY1からの幅、高さ
		//		deg				回転角度
		//		*pCenter		回転の中心
		//		ScaleX			X軸の倍率 1.0が等倍
		//		ScaleY			X軸の倍率 1.0が等倍
		//		a				αチャンネル
		//		r				赤の強さ
		//		g				緑の強さ
		//		b				青の強さ
		//---------------------------------------------------------------------------------------
		//Draw_Graphics(100,100,1.0f,TestImg,0,0,100,100,
		//				90,&pt,1.0f,1.0f,255,255,255,255);
		//---------------------------------------------------------------------------------------
		void Draw_GraphicsLeftTopNC(
			int x, int y, float z,
			const std::string& resname,
			int srcX, int srcY, int sizeX, int sizeY,
			int	degree, POINT *center,
			float	scaleX, float scaleY,
			u_char a, u_char r, u_char g, u_char b)
		{
			//描画矩形計算
			RECT rt =
			{
				srcX,
				srcY,
				srcX + sizeX,
				srcY + sizeY
			};

			D3DXVECTOR2 centerPos;
			if (center == nullptr)
			{
				centerPos = D3DXVECTOR2(0, 0);
			}
			else
			{
				centerPos = D3DXVECTOR2(
					static_cast<float>(center->x),
					static_cast<float>(center->y));
			}
			//	if(a != 255)DD_ReDraw();

			D3DXMATRIX matDraw;	//	座標変換マトリックスの格納先
			D3DXVECTOR3 ptCenter(0, 0, 0.0f);	//	描画の基準値の設定
			D3DXVECTOR3 position(0.0f, 0.0f, z);	//	表示する位置を指定
			D3DXVECTOR2 draw(static_cast<float>(x), static_cast<float>(y));	//	描画先座標（演算前）
			D3DXVECTOR2 scale(scaleX, scaleY);	//	スケーリング係数（倍率を指定）

			D3DXMatrixTransformation2D(
				&matDraw,
				NULL,
				0.0f,
				&scale,
				&centerPos,
				D3DXToRadian(-degree),
				&draw);
			pSprite->SetTransform(&matDraw);
			//描画
			pSprite->Draw(
				ImgTable.at(resname).tex,
				&rt,
				&ptCenter,
				&position,
				D3DCOLOR_ARGB(a, r, g, b));
			//ワールドのリセット
			//	D3DXMatrixRotationZ(&matDraw,0.0f);
			//	pSprite->SetTransform(&matDraw);
		}
		void Draw_GraphicsLeftTopNC(
			float x, float y, float z,
			const std::string& resname,
			int srcX, int srcY, int sizeX, int sizeY,
			int	degree, POINT *center,
			float	scaleX, float scaleY,
			u_char a, u_char r, u_char g, u_char b)
		{
			Draw_GraphicsLeftTopNC(
				static_cast<int>(x),
				static_cast<int>(y),
				z, resname,
				srcX, srcY, sizeX, sizeY,
				degree, center, scaleX, scaleY,
				a, r, g, b);
		}
		//-------------------------------------------------------------//
		// ポリゴン描画
		//	引数
		//		X1,Y1			矩形の開始座標
		//		X2,Y2			矩形の幅高さ
		//		Color1			塗り色
		//-------------------------------------------------------------//
		static void Draw_2DPolygon(
			int startX, int startY,
			int endX, int endY,
			float z, D3DCOLOR color)
		{
			Draw_2DPolygonNC(
				camera::camera->ixchange(startX), camera::camera->iychange(startY),
				camera::camera->ixchange(endX), camera::camera->iychange(endY),
				z, color);
		}

		//-------------------------------------------------------------//
		//矩形描画
		//	引数
		//		X1,Y1			矩形の開始座標
		//		X2,Y2			矩形の幅高さ
		//		Color1			塗り色
		//		Color2			枠色
		//		Size			ライン幅
		//		Nakanuri		塗りつぶすか？
		//		z				表示Z軸座標0.0 から1.0の間で指定　0.0に近いほど手前になる
		//-------------------------------------------------------------//
		void Draw_Box(
			int left, int top, int right, int bottom,
			float z,
			D3DCOLOR fillColor, D3DCOLOR frameColor,
			int lineSize, bool nakanuri)
		{
			Draw_BoxNC(
				camera::camera->ixchange(left), camera::camera->iychange(top),
				camera::camera->ixchange(right), camera::camera->iychange(bottom),
				z, fillColor, frameColor,
				lineSize, nakanuri);
		}
		void Draw_BoxCenter(
			int x, int y, int sizeX, int sizeY,
			float z,
			D3DCOLOR fillColor, D3DCOLOR frameColor,
			int lineSize, bool nakanuri)
		{
			int sx = sizeX / 2,
				sy = sizeY / 2;
			Draw_BoxNC(
				camera::camera->ixchange(x) - sx, camera::camera->iychange(y) - sy,
				camera::camera->ixchange(x) + sx, camera::camera->iychange(y) + sy,
				z,
				fillColor, frameColor,
				lineSize, nakanuri);
		}

		//-------------------------------------------------------------//
		//直線描画
		//	引数
		//		X1,Y1			矩形の開始座標
		//		X2,Y2			矩形の幅高さ
		//		Color2			色
		//		Size			ライン幅
		//		z				表示Z軸座標0.0 から1.0の間で指定　0.0に近いほど手前になる
		//-------------------------------------------------------------//
		void Draw_Line(
			int startX, int startY,
			int endX, int endY,
			float z, D3DCOLOR color, int size)
		{
			Draw_LineNC(
				camera::camera->ixchange(startX), camera::camera->iychange(startY),
				camera::camera->ixchange(endX), camera::camera->iychange(endY),
				z, color, size);
		}

		//-------------------------------------------------------------//
		//RECTを表示する。
		//-------------------------------------------------------------//
		void		Draw_CkRect(const RECT& rt, D3DCOLOR color)
		{
			RECT rc = rt;

			Draw_Box(rc.left, rc.top, rc.right, rc.bottom,
				0.0f, ARGB(254, 0, 0, 0), color, 1, 0);
		}
		//---------------------------------------------------------------------------------------
		//IMGOBJを表示する。回転、拡大、色変更対応　指定した座標の中心に描画
		//	引数
		//		X1,Y1			表示中心開始座標
		//		z				表示Z軸座標0.0 から1.0の間で指定　0.0に近いほど手前になる
		//		IMGOBJ			描画したいIMGNO
		//		TempX1,TempY1	IMGNOからの参照座標
		//		TempX2,TempY2	TempX1,TempY1からの幅、高さ
		//		deg				回転角度
		//		ScaleX			X軸の倍率 1.0が等倍
		//		ScaleY			X軸の倍率 1.0が等倍
		//		a				αチャンネル
		//		r				赤の強さ
		//		g				緑の強さ
		//		b				青の強さ
		//---------------------------------------------------------------------------------------
		//Draw_Graphics(100,100,1.0f,TestImg,0,0,100,100,
		//				90,1.0f,1.0f,255,255,255,255);
		//---------------------------------------------------------------------------------------
		void Draw_Graphics(
			int x, int y, float z,
			const std::string& resname,
			int srcX, int srcY, int sizeX, int sizeY,
			int	degree,
			float	scaleX, float scaleY,
			u_char a, u_char r, u_char g, u_char b)
		{
			Draw_GraphicsNC(
				camera::camera->ixchange(x), camera::camera->iychange(y),
				z, resname,
				srcX, srcY, sizeX, sizeY,
				degree,
				camera::camera->changeScale(scaleX),
				camera::camera->changeScale(scaleY),
				a, r, g, b);
		}
		void Draw_Graphics(
			float x, float y, float z,
			const std::string& resname,
			int srcX, int srcY, int sizeX, int sizeY,
			int	degree,
			float	scaleX, float scaleY,
			u_char a, u_char r, u_char g, u_char b)
		{
			Draw_GraphicsNC(
				camera::camera->ixchange(x), camera::camera->iychange(y),
				z, resname,
				srcX, srcY, sizeX, sizeY,
				degree,
				camera::camera->changeScale(scaleX),
				camera::camera->changeScale(scaleY),
				a, r, g, b);
		}

		//---------------------------------------------------------------------------------------
		//IMGOBJを表示する。回転、拡大、色変更対応
		//	引数
		//		X1,Y1			表示開始座標
		//		z				表示Z軸座標0.0 から1.0の間で指定　0.0に近いほど手前になる
		//		IMGOBJ			描画したいIMGNO
		//		TempX1,TempY1	IMGNOからの参照座標
		//		TempX2,TempY2	TempX1,TempY1からの幅、高さ
		//		deg				回転角度
		//		*pCenter		回転の中心
		//		ScaleX			X軸の倍率 1.0が等倍
		//		ScaleY			X軸の倍率 1.0が等倍
		//		a				αチャンネル
		//		r				赤の強さ
		//		g				緑の強さ
		//		b				青の強さ
		//---------------------------------------------------------------------------------------
		//Draw_Graphics(100,100,1.0f,TestImg,0,0,100,100,
		//				90,&pt,1.0f,1.0f,255,255,255,255);
		//---------------------------------------------------------------------------------------
		void Draw_GraphicsLeftTop(
			int x, int y, float z,
			const std::string& resname,
			int srcX, int srcY, int sizeX, int sizeY,
			int	degree, POINT *center,
			float	scaleX, float scaleY,
			u_char a, u_char r, u_char g, u_char b)
		{
			Draw_GraphicsLeftTopNC(
				camera::camera->ixchange(x), camera::camera->iychange(y),
				z, resname,
				srcX, srcY, sizeX, sizeY,
				degree, center,
				camera::camera->changeScale(scaleX),
				camera::camera->changeScale(scaleY),
				a, r, g, b);
		}
		void Draw_GraphicsLeftTop(
			float x, float y, float z,
			const std::string& resname,
			int srcX, int srcY, int sizeX, int sizeY,
			int	degree, POINT *center,
			float	scaleX, float scaleY,
			u_char a, u_char r, u_char g, u_char b)
		{
			Draw_GraphicsLeftTopNC(
				camera::camera->ixchange(x), camera::camera->iychange(y),
				z, resname,
				srcX, srcY, sizeX, sizeY,
				degree, center,
				camera::camera->changeScale(scaleX),
				camera::camera->changeScale(scaleY),
				a, r, g, b);
		}

		//---------------------------------------------------------------------------------------
		//画像データを読み込む
		//---------------------------------------------------------------------------------------
		//戻り値　
		//　読み込まれたファイルを扱うIMGNO(int)
		//
		//string resname	//登録するリソース名
		//string filename	//読み込むファイル名
		//int Transparent	//透過色の設定
		//
		//---------------------------------------------------------------------------------------
		void Draw_LoadObject(const std::string& resname, const std::string& filename, D3DCOLOR Transparent)
		{
			//既に読み込まれているリソース名なら、エラー
			if (ImgTable.count(filename) != 0){
				assert(!"同じ名前のリソースが登録済みです。名前の確認をしてください。");
			}
			for (const auto img : ImgTable){
				if (img.second.filename == filename){
					assert(!"同じファイル名のリソースが登録済みです。名前の確認をしてください。");
				}
			}
			ImgObj img;
			//読み込み開始
			if (FAILED(D3DXCreateTextureFromFileEx(
				pD3DDevice,
				filename.c_str(),	//パス
				D3DX_DEFAULT,	    //幅
				D3DX_DEFAULT,		//高さ
				D3DX_DEFAULT,		//ミップマップ
				0,					//レンダリングターゲットとしてつかうかどうか
				D3DFMT_UNKNOWN,		//テクスチャファイルフォーマット
				D3DPOOL_MANAGED,	//メモリの取り方
				D3DX_FILTER_NONE,	//フィルタリング方法（拡大縮小の演算方法）
				D3DX_DEFAULT,		//mip時のフィルタリング方法
				Transparent,		//抜け色（0指定で抜け色無効）
				&img.info,			//読み込んだ画像の情報
				nullptr,			//パレットへのポインタ（使う際に面倒なので、無効）
				&img.tex)			//テクスチャオブジェクトへのポインタ
				))
			{
				debug::BToMR("%sの読み込みに失敗しました", filename.c_str());
				assert(0);
				return;
			}
			img.filename = filename;
			ImgTable.insert(std::unordered_map< std::string, ImgObj&>::value_type(resname, img));
		}

		void Draw_DeleteObject(const std::string& resname)
		{
			ImgTable.erase(resname);
		}

		void Draw_2DClear()
		{
			if (pSprite != NULL)
				pSprite->Begin(D3DXSPRITE_ALPHABLEND);

		}

		void Draw_3DClear()
		{
			//Scene Clear Black
			if (pD3DDevice != NULL)
				pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
			//Draw Start
			if (pD3DDevice != NULL)
				pD3DDevice->BeginScene();

		}

		void Draw_2DRefresh()
		{
			if (pSprite != NULL)
				pSprite->End();

		}

		bool Draw_3DRefresh()
		{
			//Draw End
			if (pD3DDevice != NULL)
				pD3DDevice->EndScene();
			//ReDraw
			HRESULT hr;
			if (pD3DDevice != NULL)
				hr = pD3DDevice->Present(NULL, NULL, NULL, NULL);

			if (hr == D3DERR_DEVICELOST){
				//状態チェック
				if (pD3DDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
				{
					pSprite->OnLostDevice();
					//リセット
					hr = pD3DDevice->Reset(&D3DPP);
					if (hr != D3D_OK){
						return false;
					}
					//再取得
					pSprite->OnResetDevice();
				}
			}
			return true;
		}

		//---------------------------------------------------------------------------------------
		//描画終了
		//---------------------------------------------------------------------------------------
		void Draw_Refresh()
		{
			Draw_2DRefresh();
			if (Draw_3DRefresh() == false)
			{
				return;
			}
		}
		//---------------------------------------------------------------------------------------
		//描画開始
		//---------------------------------------------------------------------------------------
		void Draw_Clear()
		{
			Draw_3DClear();
			Draw_2DClear();
		}

	}


	//---------------------------------------------------------------------------------------
	// フォント関連
	//---------------------------------------------------------------------------------------
	namespace font {

		// FONT管理用配列
		const int DDFONTMAX = 10;
		LPD3DXFONT pFontData[DDFONTMAX];

		//---------------------------------------------------------------------------------------
		//フォントの作成
		//	Num			フォント番号
		//	size		フォントサイズ
		//	fontname	フォントの名前
		//-------------------------------------------------------------//
		//使用例
		// void OnCreate(){
		//		Draw_CreateFont(0,30);	//指定なしはＭＳゴシック
		// }
		//使用例　0番に設定したサイズ30のフォントで描画処理を行う
		//	Draw_FontText(0,0,"MainStage",ARGB(255,255,255,0),0);
		//
		//---------------------------------------------------------------------------------------
		void Draw_CreateFont(int Num, int size, LPCTSTR	fontname)
		{
			if (pFontData[Num] == NULL){
				// フォントの作成
				D3DXCreateFont(pD3DDevice,			//LPDIRECT3DDEVICE9	lpD3DDev,
					size,							//UINT				Height,
					0,								//UINT				Width,
					FW_NORMAL,						//UINT				Weight,
					D3DX_DEFAULT,					//UINT				MipLevels,
					FALSE,							//BOOL				Italic,
					DEFAULT_CHARSET,				//DWORD				CharSet,
					OUT_DEFAULT_PRECIS,				//DWORD				OutputPrecision,
					PROOF_QUALITY,					//DWORD				Quality,
					DEFAULT_PITCH | FF_DONTCARE,	//DWORD				PitchAndFamily,
					fontname,						//LPCTSTR			pFacename,
					&pFontData[Num]						//LPD3DXFONT		*ppFont		&pD3DXFont
					);
			}
			else{
				return;
			}
		}

		void Draw_CreateFontItalic(int Num, int size, LPCTSTR	fontname)
		{
			if (pFontData[Num] == NULL){
				// フォントの作成
				D3DXCreateFont(pD3DDevice,			//LPDIRECT3DDEVICE9	lpD3DDev,
					size,							//UINT				Height,
					0,								//UINT				Width,
					FW_NORMAL,						//UINT				Weight,
					D3DX_DEFAULT,					//UINT				MipLevels,
					TRUE,							//BOOL				Italic,
					DEFAULT_CHARSET,				//DWORD				CharSet,
					OUT_DEFAULT_PRECIS,				//DWORD				OutputPrecision,
					PROOF_QUALITY,					//DWORD				Quality,
					DEFAULT_PITCH | FF_DONTCARE,	//DWORD				PitchAndFamily,
					fontname,						//LPCTSTR			pFacename,
					&pFontData[Num]						//LPD3DXFONT		*ppFont		&pD3DXFont
					);
			}
			else{
				return;
			}
		}


		//---------------------------------------------------------------------------------------
		//フォントテーブル初期化
		//---------------------------------------------------------------------------------------
		static void Draw_InitFont()
		{
			for (int i = 0; i < DDFONTMAX; i++){
				pFontData[i] = NULL;
			}
		}
		//---------------------------------------------------------------------------------------
		//フォント削除
		//---------------------------------------------------------------------------------------
		static void Draw_DeleteFont()
		{
			for (int i = 0; i < DDFONTMAX; i++){
				if (pFontData[i] != NULL){
					RELEASE(pFontData[i]);
				}
			}
		}

		//-------------------------------------------------------------//
		//文字描画
		//	x,y,		文字描画位置
		//	string	文字の幅
		//	color	文字色
		//	t = 0	フォント番号
		//-------------------------------------------------------------//
		//使用例
		//	Draw_TextXY(0,0,"MainStage",ARGB(255,255,255,0),1);
		//	
		//フォント番号指定時はOnCreateにてフォントを用意すること
		//void OnCreate()
		//{
		//		Draw_CreateFont(0,15);
		//		Draw_CreateFont(1,30,"HG創英角ﾎﾟｯﾌﾟ体");
		//}
		//
		//	Draw_TextXY(0,0,"MainStage",ARGB(255,255,255,0),1);
		//-------------------------------------------------------------//
		void Draw_TextXYNC(int x, int y, const std::string& msg, D3DCOLOR color, int fontID)
		{
			Draw_FontTextNC(x, y, 0.0f, msg, color, fontID);
		}

		//---------------------------------------------------------------------------------------
		//文字描画
		//	rt	表示領域
		//	z	文字描画位置
		//	string	文字の幅
		//	color	文字色
		//	Num		使用フォント番号
		//-------------------------------------------------------------//
		//使用例
		//	Draw_FontDrawText(0,0,"MainStage",ARGB(255,255,0),1);
		//---------------------------------------------------------------------------------------
		int Draw_FontTextNC(const RECT& rt, float z, const std::string& msg, D3DCOLOR color, int fontID)
		{
			graph::DD_ReDrawCheck(color);
			if (pFontData[fontID] == NULL)	return 0;

			//Z軸
			D3DXMATRIX matDraw;	//	座標変換マトリックスの格納先
			D3DXVECTOR3 position(0.0f, 0.0f, z);	//	表示する位置を指定
			D3DXMatrixTransformation(&matDraw, NULL, NULL, NULL,
				NULL, NULL, &position);
			pSprite->SetTransform(&matDraw);
			//描画
			RECT rc = rt;
			int h = pFontData[fontID]->DrawText(
				pSprite,
				msg.c_str(),
				-1,
				&rc,
				DT_LEFT | DT_WORDBREAK,
				color);
			//ワールドのリセット
			D3DXMatrixRotationZ(&matDraw, 0.0f);
			pSprite->SetTransform(&matDraw);
			return h;
		}

		//---------------------------------------------------------------------------------------
		//文字描画
		//	x,y,z	文字描画位置
		//	string	文字の幅
		//	color	文字色
		//	Num		使用フォント番号
		//-------------------------------------------------------------//
		//使用例
		//	Draw_FontDrawText(0,0,"MainStage",ARGB(255,255,0),1);
		//---------------------------------------------------------------------------------------
		void Draw_FontTextNC(int x, int y, float z, const std::string& msg, D3DCOLOR color, int fontID)
		{
			graph::DD_ReDrawCheck(color);
			if (pFontData[fontID] == NULL)	return;
			//表示テキストサイズ計算
			TEXTMETRIC  TextMetric;
			pFontData[fontID]->GetTextMetrics(&TextMetric);
			int w = x + (TextMetric.tmMaxCharWidth * msg.length());
			int h = y + TextMetric.tmHeight;
			//表示用RECT作成
			RECT	rt = { x, y, w, h };

			//Z軸
			D3DXMATRIX matDraw;	//	座標変換マトリックスの格納先
			D3DXVECTOR3 position(0.0f, 0.0f, z);	//	表示する位置を指定
			D3DXMatrixTransformation(&matDraw, NULL, NULL, NULL,
				NULL, NULL, &position);
			pSprite->SetTransform(&matDraw);
			pFontData[fontID]->DrawText(pSprite, msg.c_str(), -1, &rt, DT_LEFT, color);
			//ワールドのリセット
			D3DXMatrixRotationZ(&matDraw, 0.0f);
			pSprite->SetTransform(&matDraw);
		}

		//-------------------------------------------------------------//
		//文字描画
		//	x,y,		文字描画位置
		//	string	文字の幅
		//	color	文字色
		//	t = 0	フォント番号
		//-------------------------------------------------------------//
		//使用例
		//	Draw_TextXY(0,0,"MainStage",ARGB(255,255,255,0),1);
		//	
		//フォント番号指定時はOnCreateにてフォントを用意すること
		//void OnCreate()
		//{
		//		Draw_CreateFont(0,15);
		//		Draw_CreateFont(1,30,"HG創英角ﾎﾟｯﾌﾟ体");
		//}
		//
		//	Draw_TextXY(0,0,"MainStage",ARGB(255,255,255,0),1);
		//-------------------------------------------------------------//
		void Draw_TextXY(int x, int y, const std::string& msg, D3DCOLOR color, int fontID)
		{
			Draw_FontTextNC(
				camera::camera->ixchange(x), camera::camera->iychange(y),
				0, msg, color, fontID);
		}

		//---------------------------------------------------------------------------------------
		//文字描画
		//	rt	表示領域
		//	z	文字描画位置
		//	string	文字の幅
		//	color	文字色
		//	Num		使用フォント番号
		//-------------------------------------------------------------//
		//使用例
		//	Draw_FontDrawText(0,0,"MainStage",ARGB(255,255,0),1);
		//---------------------------------------------------------------------------------------
		int Draw_FontText(const RECT& rt, float z, const std::string& msg, D3DCOLOR color, int fontID)
		{
			RECT rc = rt;
			OffsetRect(&rc, -camera::camera->iviewLeft(), -camera::camera->iviewTop());
			return Draw_FontTextNC(rc, z, msg, color, fontID);
		}

		//---------------------------------------------------------------------------------------
		//文字描画
		//	x,y,z	文字描画位置
		//	string	文字の幅
		//	color	文字色
		//	Num		使用フォント番号
		//-------------------------------------------------------------//
		//使用例
		//	Draw_FontDrawText(0,0,"MainStage",ARGB(255,255,0),1);
		//---------------------------------------------------------------------------------------
		void Draw_FontText(int x, int y, float z, const std::string& msg, D3DCOLOR color, int fontID)
		{
			Draw_FontTextNC(
				x - camera::camera->iviewLeft(), y - camera::camera->iviewTop(),
				z, msg, color, fontID);
		}

	}


	//---------------------------------------------------------------------------------------
	// システム関連
	//---------------------------------------------------------------------------------------
	namespace system {

		//---------------------------------------------------------------------------------------
		//DirectDraw初期化
		//---------------------------------------------------------------------------------------
		static void InitDx()
		{
			//DX MAIN INIT
			pD3D = Direct3DCreate9(D3D_SDK_VERSION);
			if (pD3D == NULL){
				return;
			}

			//WINDOW MODE INIT
			ZeroMemory(&D3DPP, sizeof(D3DPP));
			if (WindowMode == true){
				D3DPP.Windowed = TRUE;							//WINDOW MODE or FULL MODE
				D3DPP.SwapEffect = D3DSWAPEFFECT_DISCARD;		//SWAP AUTO Select
				D3DPP.BackBufferFormat = D3DFMT_UNKNOWN;				//FORMAT AUTO Select
				D3DPP.BackBufferCount = 1;
				D3DPP.EnableAutoDepthStencil = TRUE;
				D3DPP.AutoDepthStencilFormat = D3DFMT_D16;
				D3DPP.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
			}
			else{
				D3DPP.BackBufferWidth = WINW;
				D3DPP.BackBufferHeight = WINH;
				D3DPP.BackBufferCount = 1;
				D3DPP.BackBufferFormat = D3DFMT_X8R8G8B8;				//FORMAT Select
				D3DPP.Windowed = FALSE;						//WINDOW MODE or FULL MODE
				D3DPP.SwapEffect = D3DSWAPEFFECT_DISCARD;		//SWAP AUTO Select		
				D3DPP.EnableAutoDepthStencil = TRUE;
				D3DPP.AutoDepthStencilFormat = D3DFMT_D16;
				D3DPP.FullScreen_RefreshRateInHz = REFRESHRATE;
				D3DPP.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
			}
			//Create Device
			HRESULT hr;
			//HAL(pure vp)
			if (FAILED(hr = pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
				D3DCREATE_HARDWARE_VERTEXPROCESSING, &D3DPP, &pD3DDevice)))
			{
				//HAL(soft vp)
				if (FAILED(hr = pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
					D3DCREATE_SOFTWARE_VERTEXPROCESSING, &D3DPP, &pD3DDevice)))
				{
					//REF
					if (FAILED(hr = pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, hWnd,
						D3DCREATE_HARDWARE_VERTEXPROCESSING, &D3DPP, &pD3DDevice)))
					{
						assert(0 && "Can't create DirectX Device.");
						return;
					}
				}
			}

			// 両面描画モードの指定
			pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
			// Ｚ比較を行なう
			pD3DDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
			// ディザリングを行なう（高品質描画）
			pD3DDevice->SetRenderState(D3DRS_DITHERENABLE, TRUE);
			// ノーライティングモード 	
			pD3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
			//--------------------------------------
			// テクスチャステージの設定
			//--------------------------------------
			// テクスチャステージの設定 
			pD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
			pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			pD3DDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE);
			pD3DDevice->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_CURRENT);
			pD3DDevice->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_TEXTURE);

			// テクスチャフィルターの設定
			pD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
			pD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
			pD3DDevice->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
			pD3DDevice->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

			D3DXCreateSprite(pD3DDevice, &pSprite);

		}

		//---------------------------------------------------------------------------------------
		//DirectDraw終了
		//---------------------------------------------------------------------------------------
		static void DelDx()
		{
			RELEASE(pSprite);
			RELEASE(pD3DDevice);
			RELEASE(pD3D);
		}

		//---------------------------------------------------------------------------------------
		// ループ制御
		//---------------------------------------------------------------------------------------
		static bool System_LoopControl(int FPS) {
			static int _Time = timeGetTime();	//初期時間格納
			static float _FPS;					//FPS計算用バッファ
			if (FPS != 0){
				_FPS = 1000.0f / static_cast<float>(FPS);		//与えられたFPSからFrameTimeを計算	
				if (timeGetTime() - _Time > (unsigned)_FPS){		//FrameTimeが経過するまで待つ
					_Time = timeGetTime();
					return 1;
				}
				return 0;
			}
			else{
				return 1;
			}
		}

		//---------------------------------------------------------------------------------------
		//WINDOW　プロシージャ
		//---------------------------------------------------------------------------------------
		static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
		{
			PAINTSTRUCT	ps;
			HRESULT res;
			switch (msg) {
			case WM_KEYDOWN:
				if (wParam == VK_ESCAPE){ DestroyWindow(hWnd); }
				break;
			case WM_CREATE:
				res = timeBeginPeriod(1);
				break;
			case WM_PAINT:
				BeginPaint(hWnd, &ps);
				EndPaint(hWnd, &ps);
				break;
			case WM_DESTROY:
				timeEndPeriod(1);
				PostQuitMessage(0);
				break;
			case WM_MOUSEWHEEL:
				input::delta = GET_WHEEL_DELTA_WPARAM(wParam);
				break;
			default: return(DefWindowProc(hWnd, msg, wParam, lParam));
			}
			return (0L);
		}

		//---------------------------------------------------------------------------------------
		// アプリ更新
		//---------------------------------------------------------------------------------------
		int DoWindow(HINSTANCE hInstance, HINSTANCE hPreInst, LPSTR lpszCmdLine, int nCmdShow)
		{
			_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
			MSG lpMsg;

			{

				WNDCLASS	myProg;
				LPCSTR		Name = "Window_Class_Name";
				if (!hPreInst)
				{
					myProg.style = CS_HREDRAW | CS_VREDRAW;
					myProg.lpfnWndProc = WndProc;
					myProg.cbClsExtra = 0;
					myProg.cbWndExtra = 0;
					myProg.hInstance = hInstance;
					myProg.hIcon = LoadIcon(hInstance, "MAIN");
					myProg.hCursor = LoadCursor(NULL, IDC_ARROW);
					myProg.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
					myProg.lpszMenuName = NULL;
					myProg.lpszClassName = (Name);
					if (!RegisterClass(&myProg)) return false;
				}

				RECT rc;
				SetRect(&rc, 0, 0, WINW, WINH);
				AdjustWindowRectEx(&rc, WS_OVERLAPPEDWINDOW, FALSE, 0);

				DWORD style;
				if (WindowMode) style = WS_OVERLAPPEDWINDOW;
				else           style = WS_POPUP;
				hWnd = CreateWindow(
					(Name),
					(USERNAME),
					style,
					0, 0,
					rc.right - rc.left,
					rc.bottom - rc.top,
					NULL,
					NULL,
					hInstance,
					NULL);


				InitDx();
				//resource table init
				graph::Draw_InitImgTable();
				font::Draw_InitFont();
				//player input init
				input::InitPlayerInput();
				for (int i = 0; i < PLAYER_NUM; i++)
					input::InitKeyInfo(i);
				//stage flag init
				stage::InitStageFlag();
				math::RandomInit();
				//sound com init
#ifdef CI_EXT 
				CoInitialize(NULL);
				//	サウンド環境の初期化
				{
					ci_ext::MYERROR_DS ret_ds;	//	DirectSoundエラー番号格納用
					ret_ds = ci_ext::dsInitDirectSound(hWnd);
					if (ret_ds != ci_ext::eERRORDS_NOTHING)
					{
						//	環境が初期化できないので、ウインドウを終了させる
						std::string szMsg = ci_ext::dsGetErrorMessage(ret_ds);
						MessageBox(hWnd, szMsg.c_str(), NULL, 0);
						DestroyWindow(hWnd);
					}
				}
#endif
#ifdef USE_DIRECTSOUND
				se::DSound_Init();
#endif
#ifdef USE_DIRECTSHOW
				bgm::DShow_Init();
#endif
				srand((unsigned)::time(NULL));
				time::Time_CheckTimer();
				//user init
				OnCreate();
#ifdef CI_EXT
				//	StartOutputDbgString();
				ShowConsole();
				ci_ext::TaskManager game;
				//TaskManagerのポインタをタスクに登録している
				{ci_ext::TaskFirst dummy(&game); } //{}は意図的なので消去しないこと
				game.changeScene(0);
#endif

				ShowWindow(hWnd, nCmdShow);
				UpdateWindow(hWnd);
				while (true){
					if (PeekMessage(&lpMsg, NULL, 0, 0, PM_REMOVE)){			//メッセージ処理
						if (lpMsg.message == WM_QUIT)	break;
						TranslateMessage(&lpMsg);
						DispatchMessage(&lpMsg);
					}
					else if (hWnd == GetActiveWindow()){
						graph::Draw_Clear();
						input::CheckKey();
						//ゲームループ
#ifdef CI_EXT
						if (game.run()) DestroyWindow(hWnd);
						ci_ext::dsCheck_Stream();
#else
						GameLoop();
#endif
						camera::StepCamera();
						graph::Draw_Refresh();
						//サウンド終了チェック
#ifdef USE_DIRECTSHOW
						bgm::DShow_EndCheck();
#endif
#ifdef USE_DIRECTSOUND
						se::DSound_EndCheck();
#endif
						//ホイールdeltaクリア
						input::ClearWheelDelta();
						//TIMER更新
						FrameTime = time::Time_Update();
					}
				}
				//user release
				OnDestroy();			//ゲーム後始末処理
				//resource release
				graph::Draw_DelImgTable();
				font::Draw_DeleteFont();
				//sound com release
#ifdef CI_EXT
				ci_ext::dsRelease();
#endif
#ifdef USE_DIRECTSOUND
				se::DSound_Del();

#endif
#ifdef USE_DIRECTSHOW
				bgm::DShow_Del();
#endif
				//directx release
				DelDx();
				//pad delete
				input::DeletePlayerInput();
				//memory leaks dump
				//	_CrtDumpMemoryLeaks();
			}
			return static_cast<int>(lpMsg.wParam);
		}

	}


#ifdef CI_EXT
	ci_ext::TaskManager* ci_ext::Task::p_ = nullptr;
	int ci_ext::Task::uid_ = 0;
#endif


	//********************************************************************//
	//
	//				BGM再生　関連関数 
	//
	//********************************************************************//
	namespace bgm {
#ifdef USE_DIRECTSHOW

		//関数一覧
		/*
		bool		DShow_Init()						DShowの初期化
		void		DShow_Del();						DShow_の後始末
		DSHOWOBJ	DShow_LoadFile(LPCWSTR)				音楽ファイルの読み込み
		bool		DShow_Play();						再生
		void		DShow_RateControl(DSHOWOBJ,float)	再生速度変更
		void		DShow_EndCheck()					終了チェック
		void		DShow_Stop(DSHOWOBJ)				再生ストップ
		void		DShow_AllStop()						全停止
		LONGLONG	DShow_GetCurrentPos(int index)		再生位置取得
		void		DShow_SetStartPos(int index)		スタート位置に設定
		*/
		//-------------------------------------------------------
		/*
		使い方

		グローバルとして宣言
		DSHOWOBJ		test;

		再生処理
		DShow_Play(test);		//通常再生-すべてループ再生

		停止処理
		DShow_Stop(test);		//同じ音声をすべて停止
		DShow_AllStop();		//再生中のすべての音声を停止

		エフェクト
		DShow_RateControl(test,1.0);	//testの再生速度を指定した倍率に変更する。

		*/
		//--------------------------------------------------------------------------------------------
		// DirectShowの初期化
		//--------------------------------------------------------------------------------------------
		void DShow_ReleaseObj(DxShow& bgm){
			bgm.pMediaControl->Release();
			bgm.pMediaEvent->Release();
			bgm.pMediaSeeking->Release();
			bgm.pBuilder->Release();
			bgm.pBasicAudio->Release();
		}

		void	DShow_LoadFile(const std::string& resname, const std::string& filename)
		{
			//既に読み込まれているリソース名なら、エラー
			if (BgmTable.count(filename) != 0){
				assert(!"同じ名前のリソースが登録済みです。名前の確認をしてください。");
			}
			for (const auto bgm : BgmTable){
				if (bgm.second.filename == filename){
					assert(!"同じファイル名のリソースが登録済みです。名前の確認をしてください。");
				}
			}
			BgmObj bgmObj;
			GUID format = TIME_FORMAT_FRAME;
			wchar_t name[256];
			MultiByteToWideChar(CP_ACP, 0, (LPCSTR)filename.c_str(), -1, name, 256);

			HRESULT	hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER,
				IID_IGraphBuilder, (void **)&bgmObj.bgm.pBuilder);
			if (FAILED(hr)){
				debug::BToM("DShow:GraphBuilderオブジェクトの生成に失敗しました");
				goto error;
			}

			if (FAILED(bgmObj.bgm.pBuilder->RenderFile(name, NULL))){
				debug::BToM("DShow\nファイル読み込みに失敗しました\nファイル名、コーデックの確認をお願いします。");
				goto error;
			}
			if (FAILED(bgmObj.bgm.pBuilder->QueryInterface(IID_IMediaControl,
				(void**)&bgmObj.bgm.pMediaControl))){
				debug::BToMR("DShow\nMediaControlの設定に失敗しました");
				goto error;
			}
			if (FAILED(bgmObj.bgm.pBuilder->QueryInterface(IID_IMediaEvent,
				(void**)&bgmObj.bgm.pMediaEvent))){
				debug::BToMR("DShow\nMediaEventの設定に失敗しました");
				goto error;
			}
			if (FAILED(bgmObj.bgm.pBuilder->QueryInterface(IID_IMediaSeeking,
				(void**)&bgmObj.bgm.pMediaSeeking))){
				debug::BToMR("DShow\nMediaSeekingの設定に失敗しました");
				goto error;
			}
			if (FAILED(bgmObj.bgm.pBuilder->QueryInterface(IID_IBasicAudio,
				(void**)&bgmObj.bgm.pBasicAudio))){
				debug::BToMR("DShow\nBasicAudioの設定に失敗しました");
				goto error;
			}

			bgmObj.bgm.pMediaSeeking->SetTimeFormat(&format);
			bgmObj.bgm.use = 1;
			bgmObj.filename = filename;
			BgmTable.insert(std::unordered_map<std::string, BgmObj>::value_type(resname, bgmObj));
			return;

		error:
			DShow_ReleaseObj(bgmObj.bgm);

		}
		//--------------------------------------------------------------------------------------------
		// DirectShowの後始末
		//--------------------------------------------------------------------------------------------
		void	DShow_Del()
		{
			DShow_AllStop();
			for (auto& bgm : BgmTable){
				if (bgm.second.bgm.use == 1){
					DShow_ReleaseObj(bgm.second.bgm);
				}
			}
			CoUninitialize();
		}

		//--------------------------------------------------------------------------------------------
		// GraphBuilderの生成
		//--------------------------------------------------------------------------------------------
		void DShow_Init()
		{
			CoInitialize(NULL);
		}

		//--------------------------------------------------------------------------------------------
		// ファイルの再生
		//--------------------------------------------------------------------------------------------
		bool	DShow_Play(const std::string& resname)
		{
			if (BgmTable.at(resname).bgm.use == 0) return false;
			HRESULT	hr = BgmTable.at(resname).bgm.pMediaControl->Run();
			if (FAILED(hr)){
				debug::BToMR("DShow:再生に失敗しました1");
				return false;
			}
			return true;
		}

		//--------------------------------------------------------------------------------------------
		// 再生速度の再生
		//--------------------------------------------------------------------------------------------
		void DShow_RateControl(const std::string& resname, float rate)
		{
			if (BgmTable.at(resname).bgm.use == 0) return;
			if (rate < 0)	return;
			BgmTable.at(resname).bgm.pMediaSeeking->SetRate(rate);
		}
		//--------------------------------------------------------------------------------------------
		// 再生音量の設定
		// 0から100で設定　0は無音 100は最大 
		//--------------------------------------------------------------------------------------------
		void DShow_VolumeControl(const std::string& resname, int volume)
		{
			if (BgmTable.at(resname).bgm.use == 0) return;
			if (volume < 0 || volume > 100)	return;
			//0から-10000で設定 0が最大　-10000は無音
			//100で割った値がデシベル
			long vol = (long)(-10000 + (volume * 100));
			BgmTable.at(resname).bgm.pBasicAudio->put_Volume(vol);
		}
		//--------------------------------------------------------------------------------------------
		// 停止
		//--------------------------------------------------------------------------------------------
		void DShow_Stop(const std::string& resname)
		{
			if (BgmTable.at(resname).bgm.use == 0) return;
			LONGLONG	start = 0;
			BgmTable.at(resname).bgm.pMediaSeeking->SetPositions(&start, AM_SEEKING_AbsolutePositioning,
				NULL, AM_SEEKING_NoPositioning);
			BgmTable.at(resname).bgm.pMediaControl->Stop();
		}

		//--------------------------------------------------------------------------------------------
		// 一時停止
		//--------------------------------------------------------------------------------------------
		void DShow_Pause(const std::string& resname)
		{
			if (BgmTable.at(resname).bgm.use == 0) return;
			if (BgmTable.at(resname).bgm.use == 1){
				BgmTable.at(resname).bgm.pMediaControl->Stop();
			}
		}

		//--------------------------------------------------------------------------------------------
		// 全停止
		//--------------------------------------------------------------------------------------------
		void DShow_AllStop()
		{
			for (auto& bgm : BgmTable){
				if (bgm.second.bgm.use == 1){
					bgm.second.bgm.pMediaControl->Stop();
				}
			}
		}

		//--------------------------------------------------------------------------------------------
		// 終了確認
		//--------------------------------------------------------------------------------------------
		void DShow_EndCheck()
		{
			for (auto& bgm : BgmTable){
				if (bgm.second.bgm.use == 1){
		//			long code;
					//再生位置取得
					LONGLONG endPos = bgm::DShow_GetEndPosition(bgm.first);
					LONGLONG nowPos = bgm::DShow_GetCurrentPos(bgm.first);
					//ブロッキングするようなのでやめ
					//bgm.second.bgm.pMediaEvent->WaitForCompletion(0, &code);
					//再生終了時はループを行う。
		//			if(code == EC_COMPLETE){
					if (endPos <= nowPos){
						LONGLONG	start = 0;
						bgm.second.bgm.pMediaSeeking->SetPositions(&start, AM_SEEKING_AbsolutePositioning,
							NULL,AM_SEEKING_NoPositioning);
					}
				}
			}
		}

		//--------------------------------------------------------------------------------------------
		// 再生終了位置取得
		//--------------------------------------------------------------------------------------------
		LONGLONG DShow_GetEndPosition(const std::string& resname)
		{
			if (BgmTable.at(resname).bgm.use == 0) return -1L;
			LONGLONG	end;
			//終了位置の取得
			BgmTable.at(resname).bgm.pMediaSeeking->GetStopPosition(&end);
			return end;
		}

		//--------------------------------------------------------------------------------------------
		// 再生位置取得
		//--------------------------------------------------------------------------------------------
		LONGLONG DShow_GetCurrentPos(const std::string& resname)
		{
			if (BgmTable.at(resname).bgm.use == 0) return -1L;
			LONGLONG	Current;
			BgmTable.at(resname).bgm.pMediaSeeking->GetCurrentPosition(&Current);
			return Current;
		}

		//--------------------------------------------------------------------------------------------
		// 再生位置初期化
		//--------------------------------------------------------------------------------------------
		void DShow_SetStartPos(const std::string& resname)
		{
			if (BgmTable.at(resname).bgm.use == 0) return;
			LONGLONG	start = 0;
			BgmTable.at(resname).bgm.pMediaSeeking->SetPositions(&start, AM_SEEKING_AbsolutePositioning,
				NULL, AM_SEEKING_NoPositioning);
		}

#endif

	}


	//********************************************************************//
	//
	//				SE再生　関連関数
	//
	//********************************************************************//
	namespace se {
#ifdef USE_DIRECTSOUND
		//--------------------------------------------------------------------------------------------
		// DirectSoundの初期化
		//--------------------------------------------------------------------------------------------
		void DSound_Init()
		{
			if (DSound_Create()){
				if (!DSound_CreatePrimaryBuffer()){
					debug::BToMR("プライマリバッファの生成に失敗しました");
				}
				DSound_CreateSecondaryBuffer();
			}
			else{
				debug::BToMR("DSoundの生成に失敗しました");
			}

		}

		//--------------------------------------------------------------------------------------------
		// DirectSoundの作成
		//--------------------------------------------------------------------------------------------
		bool DSound_Create(void)
		{
			HRESULT ret;

			// DirectSound8を作成
			ret = DirectSoundCreate8(NULL, &lpDS, NULL);
			if (FAILED(ret)) {
				debug::BToMR("サウンドオブジェクト作成失敗\n", "");
				return false;
			}

			// 強調モード
			ret = lpDS->SetCooperativeLevel(system::hWnd, DSSCL_EXCLUSIVE | DSSCL_PRIORITY);
			if (FAILED(ret)) {
				debug::BToMR("強調レベル設定失敗\n", "");
				return false;
			}

			return true;
		}

		//--------------------------------------------------------------------------------------------
		// バッファの作成
		//--------------------------------------------------------------------------------------------
		void DSound_CreateSecondaryBuffer()
		{
			SeTable.clear();
		}

		//--------------------------------------------------------------------------------------------
		// サウンド再生
		//--------------------------------------------------------------------------------------------
		void DSound_Play(const std::string& resname)
		{
			int i;
			auto& se = SeTable.at(resname).se;
			for (i = 0; i < DSPLAYMAX; i++){
				if (se.PlayBuffer[i].State == DSNONE){
					//再生開始
					//			se.PlayBuffer[i].ID = SoundNo;
					se.PlayBuffer[i].State = DSPLAY;
					se.PlayBuffer[i].pBuffer->Play(0, 0, 0);
					se.PlayBuffer[i].Loop = 0;
					break;
				}
			}
			debug::TToM("%d", se.PlayBuffer[0].State);
		}

		//--------------------------------------------------------------------------------------------
		// サウンドループ再生
		//--------------------------------------------------------------------------------------------
		void DSound_PlayLoop(const std::string& resname)
		{
			int i;
			auto& se = SeTable.at(resname).se;
			for (i = 0; i < DSPLAYMAX; i++){
				if (se.PlayBuffer[i].State == DSNONE){
					//再生開始
					//			se.PlayBuffer[i].ID = SoundNo;
					se.PlayBuffer[i].State = DSPLAY;
					se.PlayBuffer[i].pBuffer->Play(0, 0, 0);
					se.PlayBuffer[i].Loop = 1;
					break;
				}
			}
		}

		//--------------------------------------------------------------------------------------------
		// サウンド停止
		//	同じ効果音をすべて停止する
		//--------------------------------------------------------------------------------------------
		void DSound_Stop(const std::string& resname)
		{
			auto& se = SeTable.at(resname).se;
			for (int j = 0; j < DSPLAYMAX; j++){
				if (se.PlayBuffer[j].State == DSPLAY){
					se.PlayBuffer[j].pBuffer->Stop();
					se.PlayBuffer[j].State = DSNONE;
				}
			}
		}

		//--------------------------------------------------------------------------------------------
		// すべてのサウンド停止
		//--------------------------------------------------------------------------------------------
		void DSound_AllStop()
		{
			for (auto& seObj : SeTable){
				for (int j = 0; j < DSPLAYMAX; j++){
					if (seObj.second.se.PlayBuffer[j].State == DSPLAY){
						seObj.second.se.PlayBuffer[j].pBuffer->Stop();
						seObj.second.se.PlayBuffer[j].State = DSNONE;
					}
				}
			}
		}

		//--------------------------------------------------------------------------------------------
		// 終了チェック
		//--------------------------------------------------------------------------------------------
		void DSound_EndCheck()
		{
			DWORD Status;
			int a = 0;
			for (auto& seObj : SeTable){
				for (int j = 0; j < DSPLAYMAX; j++){
					if (seObj.second.se.PlayBuffer[j].State == DSPLAY){		//現在再生中の複製バッファ
						//終了と認識する
						seObj.second.se.PlayBuffer[j].pBuffer->GetStatus(&Status);
						if ((Status & DSBSTATUS_PLAYING) != 1){
							if (seObj.second.se.PlayBuffer[j].Loop){
								seObj.second.se.PlayBuffer[j].pBuffer->SetCurrentPosition(0);
								seObj.second.se.PlayBuffer[j].pBuffer->Play(0, 0, 0);
							}
							else{
								seObj.second.se.PlayBuffer[j].State = DSNONE;
								seObj.second.se.PlayBuffer[j].BufferSize = 0;
								seObj.second.se.PlayBuffer[j].pBuffer->Stop();
							}
						}
					}
				}
			}
		}
		//--------------------------------------------------------------------------------------------
		// プライマリサウンドバッファの作成
		//--------------------------------------------------------------------------------------------
		bool DSound_CreatePrimaryBuffer(void)
		{
			HRESULT ret;
			WAVEFORMATEX wf;

			// プライマリサウンドバッファの作成
			DSBUFFERDESC dsdesc;
			ZeroMemory(&dsdesc, sizeof(DSBUFFERDESC));
			dsdesc.dwSize = sizeof(DSBUFFERDESC);
			dsdesc.dwFlags = DSBCAPS_PRIMARYBUFFER;
			dsdesc.dwBufferBytes = 0;
			dsdesc.lpwfxFormat = NULL;
			ret = lpDS->CreateSoundBuffer(&dsdesc, &lpPrimary, NULL);
			if (FAILED(ret)) {
				debug::BToMR("プライマリサウンドバッファ作成失敗\n", "");
				return false;
			}

			// プライマリバッファのステータスを決定
			wf.cbSize = sizeof(WAVEFORMATEX);
			wf.wFormatTag = WAVE_FORMAT_PCM;
			wf.nChannels = 2;
			wf.nSamplesPerSec = 44100;
			wf.wBitsPerSample = 16;
			wf.nBlockAlign = wf.nChannels * wf.wBitsPerSample / 8;
			wf.nAvgBytesPerSec = wf.nSamplesPerSec * wf.nBlockAlign;
			ret = lpPrimary->SetFormat(&wf);
			if (FAILED(ret)) {
				debug::BToMR("プライマリバッファのステータス失敗\n", "");
				return false;
			}
			return true;
		}

		//--------------------------------------------------------------------------------------------
		//DSoundの後始末
		//--------------------------------------------------------------------------------------------
		bool DSound_Del(void)
		{
			for (auto& seObj : SeTable){
				if (seObj.second.se.BufferSize != 0){
					seObj.second.se.sBuffer->Release();
				}
			}
			if (lpPrimary) {
				lpPrimary->Release();
				lpPrimary = NULL;
			}
			if (lpDS) {
				lpDS->Release();
				lpDS = NULL;
			}

			return true;
		}

		//--------------------------------------------------------------------------------------------
		// サウンドバッファの作成
		//--------------------------------------------------------------------------------------------
		void DSound_LoadFile(const std::string& resname, const std::string& filename)
		{
			HRESULT ret;
			MMCKINFO mSrcWaveFile;
			MMCKINFO mSrcWaveFmt;
			MMCKINFO mSrcWaveData;
			LPWAVEFORMATEX wf;

			//既に読み込まれているリソース名なら、エラー
			if (SeTable.count(filename) != 0){
				assert(!"DSound:同じ名前のリソースが登録済みです。名前の確認をしてください。");
			}
			for (const auto& se : SeTable){
				if (se.second.filename == filename){
					assert(!"DSound:同じファイル名のリソースが登録済みです。名前の確認をしてください。");
				}
			}

			// WAVファイルをロード
			HMMIO hSrc;
			hSrc = mmioOpen((LPSTR)filename.c_str(), NULL, MMIO_ALLOCBUF | MMIO_READ | MMIO_COMPAT);
			if (!hSrc) {
				debug::BToMR("WAVファイルロードエラー\n", "");
				return;
			}

			// 'WAVE'チャンクチェック
			ZeroMemory(&mSrcWaveFile, sizeof(mSrcWaveFile));
			ret = mmioDescend(hSrc, &mSrcWaveFile, NULL, MMIO_FINDRIFF);
			if (mSrcWaveFile.fccType != mmioFOURCC('W', 'A', 'V', 'E')) {
				debug::BToMR("WAVEチャンクチェックエラー\n", "");
				mmioClose(hSrc, 0);
				return;
			}

			// 'fmt 'チャンクチェック
			ZeroMemory(&mSrcWaveFmt, sizeof(mSrcWaveFmt));
			ret = mmioDescend(hSrc, &mSrcWaveFmt, &mSrcWaveFile, MMIO_FINDCHUNK);
			if (mSrcWaveFmt.ckid != mmioFOURCC('f', 'm', 't', ' ')) {
				debug::BToMR("fmt チャンクチェックエラー\n", "");
				mmioClose(hSrc, 0);
				return;
			}

			// ヘッダサイズの計算
			int iSrcHeaderSize = mSrcWaveFmt.cksize;
			if (iSrcHeaderSize<sizeof(WAVEFORMATEX))
				iSrcHeaderSize = sizeof(WAVEFORMATEX);

			// ヘッダメモリ確保
			wf = (LPWAVEFORMATEX)malloc(iSrcHeaderSize);
			if (!wf) {
				debug::BToMR("メモリ確保エラー\n", "");
				mmioClose(hSrc, 0);
				return;
			}
			ZeroMemory(wf, iSrcHeaderSize);

			// WAVEフォーマットのロード
			ret = mmioRead(hSrc, (char*)wf, mSrcWaveFmt.cksize);
			if (FAILED(ret)) {
				debug::BToMR("WAVEフォーマットロードエラー\n", "");
				free(wf);
				mmioClose(hSrc, 0);
				return;
			}

			// fmtチャンクに戻る
			mmioAscend(hSrc, &mSrcWaveFmt, 0);

			// dataチャンクを探す
			while (1) {
				// 検索
				ret = mmioDescend(hSrc, &mSrcWaveData, &mSrcWaveFile, 0);
				if (FAILED(ret)) {
					debug::BToMR("dataチャンクが見つからない\n", "");
					free(wf);
					mmioClose(hSrc, 0);
					return;
				}
				if (mSrcWaveData.ckid == mmioStringToFOURCC("data", 0)){
					break;
				}
				// 次のチャンクへ
				ret = mmioAscend(hSrc, &mSrcWaveData, 0);
			}

			SeObj seObj;
			// サウンドバッファの作成
			DSBUFFERDESC dsdesc;
			ZeroMemory(&dsdesc, sizeof(DSBUFFERDESC));
			dsdesc.dwSize = sizeof(DSBUFFERDESC);
			dsdesc.dwFlags = DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_STATIC | DSBCAPS_LOCDEFER | DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLVOLUME;
			dsdesc.dwBufferBytes = mSrcWaveData.cksize;
			dsdesc.lpwfxFormat = wf;
			dsdesc.guid3DAlgorithm = DS3DALG_DEFAULT;
			ret = lpDS->CreateSoundBuffer(&dsdesc, &seObj.se.sBuffer, NULL);
			seObj.se.BufferSize = (WORD)dsdesc.dwBufferBytes;
			if (FAILED(ret)) {
				debug::BToMR("サウンドバッファの作成エラー\n", "");
				free(wf);
				mmioClose(hSrc, 0);
				return;
			}

			// ロック開始
			LPVOID pMem1, pMem2;
			DWORD dwSize1, dwSize2;
			ret = (seObj.se.sBuffer)->Lock(0, mSrcWaveData.cksize, &pMem1, &dwSize1, &pMem2, &dwSize2, 0);
			seObj.se.sBuffer->GetFrequency(&seObj.se.Fre);
			if (FAILED(ret)) {
				debug::BToMR("ロック失敗\n", "");
				free(wf);
				mmioClose(hSrc, 0);
				return;
			}

			// データ書き込み
			mmioRead(hSrc, (char*)pMem1, dwSize1);
			mmioRead(hSrc, (char*)pMem2, dwSize2);

			// ロック解除
			(seObj.se.sBuffer)->Unlock(pMem1, dwSize1, pMem2, dwSize2);
			// ヘッダ用メモリを開放
			free(wf);
			// WAVを閉じる
			mmioClose(hSrc, 0);

			seObj.se.sBuffer->SetVolume(-50);

			//複製開始
			for (int t = 0; t < 10; t++){
				if (lpDS->DuplicateSoundBuffer(seObj.se.sBuffer,
					&(seObj.se.PlayBuffer[t].pBuffer)) != DS_OK){
					debug::BToMR("再生用バッファ作成失敗\n", "");
					break;
				}
				else{
					seObj.se.PlayBuffer[t].State = DSNONE;
					seObj.se.PlayBuffer[t].BufferSize = seObj.se.BufferSize;
					seObj.se.PlayBuffer[t].Loop = 0;
				}
			}
			SeTable.insert(std::unordered_map<std::string, SeObj>::value_type(resname, seObj));
		}


		//--------------------------------------------------------------------------------------------
		// 周波数変更 再生速度が変わる
		//--------------------------------------------------------------------------------------------
		void DSound_SetFrequency(const std::string& resname, int Fre)
		{
			auto se = SeTable.at(resname).se;
			se.sBuffer->SetFrequency(se.Fre + Fre);
		}

		void DSound_SetVolume(const std::string& resname, int Vol)
		{
			auto se = SeTable.at(resname).se;
			long volume;
			if (Vol >= 1.0f){
				volume = DSBVOLUME_MAX;
			}
			else if (Vol <= 0.0f){
				volume = DSBVOLUME_MIN;
			}
			else{
				volume = static_cast<long>((10 * log10((double)(Vol / 100))) * 100);
			}

			HRESULT hr = se.sBuffer->SetVolume(volume);
		}

#endif
	}

}
