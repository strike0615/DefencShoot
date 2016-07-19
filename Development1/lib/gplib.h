//**************************************************************************************//
//
//　GP　ライブラリ
//		gplib.h
//**************************************************************************************//
//**************************************************************************************//
//インクルード
//**************************************************************************************//
#pragma once

#define _USE_MATH_DEFINES
#define _CRT_SECURE_NO_DEPRECATE 1
#pragma warning(disable : 4995)	

//DirectSound DorectShowを使わない場合はコメントアウト
#define USE_DIRECTSOUND
#define USE_DIRECTSHOW

#include <crtdbg.h>
#include <windows.h>
#include <d3dx9.h>

#include <string>
#include <random>
#include <memory>
#include <cassert>



//**************************************************************************************//
//マクロ　変数　宣言
//**************************************************************************************//

namespace gplib
{
	//キャスト関連
	// weak_ptr → shared_ptr(継承クラスが対象)
	template<typename T1, typename T2>
	std::shared_ptr<T1> weak_to_shared(const std::weak_ptr<T2>& ptr)
	{
		return std::static_pointer_cast<T1>(ptr.lock());
	}

	// weak_ptr → shared_ptr(同じクラス)
	template<typename T>
	std::shared_ptr<T> weak_to_shared(const std::weak_ptr<T>& ptr)
	{
		return std::static_pointer_cast<T>(ptr.lock());
	}

	// shared_ptr → shared_ptr(継承クラスが対象)
	template<typename T1, typename T2>
	std::shared_ptr<T1> shared_cast(const std::shared_ptr<T2>& ptr)
	{
		return std::static_pointer_cast<T1>(ptr);
	}

	namespace input
	{
		// JOYPADの定義
		enum JOYID{ JOY1, JOY2 };
		enum JOY4DIR
		{
			JOY_DOWN = 0x1000,
			JOY_LEFT = 0x1001,
			JOY_UP = 0x1002,
			JOY_RIGHT = 0x1004
		};

		// マウスホイールの定義
		enum WHEEL
		{
			MWHEEL_UP = -1,
			MWHEEL_NEUTRAL,
			MWHEEL_DOWN,
		};

		// キーの状態の定義
		enum KEYSTATE{ FREE_KEY, PUSH_KEY, HOLD_KEY, PULL_KEY };

		// キーの定義
		enum CODE_KEY
		{
			KEY_NEUTRAL = -1,				// 何も押されていない
			KEY_DOWN,						//下キー
			KEY_LEFT,						//左キー
			KEY_UP,							//上キー
			KEY_RIGHT,						//右キー
			KEY_BTN0,						//ボタン0
			KEY_BTN1,						//ボタン1
			KEY_BTN2,						//ボタン2
			KEY_SPACE,						//スペースキー
			KEY_F1,							//F1キー
			KEY_F2,							//F2キー

			KEY_BTNMAX,						//キーボードの最大数　キー追加の際はこの上に記述

			KEY_MOUSE_LBTN = KEY_BTNMAX,	//マウスの左ボタン
			KEY_MOUSE_RBTN,					//マウスの右ボタン
			KEY_MAX							//最大数
		};
	}

	// 半透明描画の定義
	enum
	{
		NORMAL,		//半透明描画
		ADD,		//加算合成
		HALFADD,	//半加算合成
		MULTIPL,	//乗算合成
		SUBTRACT,	//減算
		BY,

		BF_MAX
	};

	// エラーダイアログの定義
	enum DLG_ERROR
	{
		DLG_ERROR_INVALIDBUFFER,	//getのポインタがNULLになっている
		DLG_ERROR_CANCEL,			//キャンセルが押された
		DLG_OK
	};

	namespace math
	{
		// PIの定義
		const double	PI_D = M_PI;
		const float		PAI = static_cast<float>(PI_D);
		const float		PI = PAI;
	}


	//**************************************************************************************//
	//外部変数
	//**************************************************************************************//
	namespace system{
		extern	HWND		hWnd;				//ウインドウハンドル
		extern	const char	USERNAME[256];		//タイトル名
		extern	const int 	WINW;				//WINDOW幅
		extern	const int		WINH;				//WINDOW高さ
		extern	bool		WindowMode;			//ウインドウモード
		extern	float		FrameTime;			//フレームタイム
		//extern const int PLAYER_NUM;			//　プレイヤーの数
		const int	PLAYER_NUM = 2;

		extern	int			KeyboardMode;			//強制キーボードフラグ
		extern	int			nowStageNo;

		extern const int  REFRESHRATE;
		extern const float  ONEFRAME_TIME;
	}

	//**************************************************************************************//
	// 外部宣言
	//**************************************************************************************//
	namespace system
	{
		// 以下、アプリ側で用意する関数
		// アプリ起動時に一度だけ呼ばれる
		void OnCreate();
		// アプリ終了時に一度だけ呼ばれる
		void OnDestroy();
		// アプリの再描画の度に呼ばれる
		void GameLoop();

		// アプリの処理を実行
		int  DoWindow(HINSTANCE hInstance, HINSTANCE hPreInst,
			LPSTR lpszCmdLine, int nCmdShow);
	}

	namespace camera
	{
		// カメラ制御
		void InitCamera(const float x, const float y, const float addpx, const float addpy);
		void SetLookAt(const float x, const float y);
		void AddCameraForce(const float x, const float y);
		void SetCameraTarget(const float x, const float y, const int speed);
		void SetScale(const float scale);
		float GetScale();
		void AddScale(const float add);
		void AddScale(const float add, const int step);
		POINT GetCameraPos();
		POINT GetLookAt();

		bool InScreenCamera(const int x, const int y, const int w, const int h);
		bool InScreenCamera(const RECT& rt);
		bool InScreenCameraCenter(const float x, const float y, const float w, const float h);

		POINT GetCursorPosition();
		POINT GetCursorPositionNC();
		void ChangeScreenMouse(POINT& pt);
		void GetCursorPosition(POINT& pt);
		void GetCursorPositionNC(POINT* pt_ptr);

		RECT GetScreenRect();

		// SetCameraTargetの別名(互換性用途)
		inline void SetLookAtTarget(const float x, const float y, const int speed) { SetCameraTarget(x, y, speed); }
	}

	namespace stage{
		//ステージ管理関連
		void ChangeStage(const int);
		bool InitStage();
		void InitStageFlag();
		int  CheckStage();
	}

	namespace graph{

#define ARGB D3DCOLOR_ARGB
		//描画関連
		void Draw_Clear();
		void Draw_Refresh();

		//画像関連 
		void Draw_LoadObject(const std::string& resname, const std::string& filename,
			D3DCOLOR Transparent = ARGB(255, 0, 0, 0));
		void Draw_DeleteObject(const std::string& resname);
		void Draw_SetRenderMode(int Mode);
		void Draw_EndRenderMode();

		void Draw_Graphics(
			int x, int y, float z,
			const std::string& resname,
			int srcX, int srcY,
			int sizeX, int sizeY,
			int	degree = 0,
			float	scaleX = 1.0f, float scaleY = 1.0f,
			u_char a = 255,
			u_char r = 255,
			u_char g = 255,
			u_char b = 255
			);
		void Draw_GraphicsLeftTop(
			int x, int y, float z,
			const std::string& resname,
			int srcX, int srcY,
			int sizeX, int sizeY,
			int	degree = 0, POINT *center = nullptr,
			float	scaleX = 1.0f, float scaleY = 1.0f,
			u_char a = 255,
			u_char r = 255,
			u_char g = 255,
			u_char b = 255
			);
		void Draw_Graphics(
			float x, float y, float z,
			const std::string& resname,
			int srcX, int srcY,
			int sizeX, int sizeY,
			int	degree = 0,
			float	scaleX = 1.0f, float scaleY = 1.0f,
			u_char a = 255,
			u_char r = 255,
			u_char g = 255,
			u_char b = 255
			);
		void Draw_GraphicsLeftTop(
			float x, float y, float z,
			const std::string& resname,
			int srcX, int srcY,
			int sizeX, int sizeY,
			int	degree = 0, POINT *center = nullptr,
			float	scaleX = 1.0f, float scaleY = 1.0f,
			u_char a = 255,
			u_char r = 255,
			u_char g = 255,
			u_char b = 255
			);
		void Draw_Line(
			int startX, int startY,
			int endX, int endY,
			float z, D3DCOLOR color, int size
			);
		void Draw_GraphicsNC(
			int x, int y, float z,
			const std::string& resname,
			int srcX, int srcY,
			int sizeX, int sizeY,
			int	degree = 0, 
			float	scaleX = 1.0f, float scaleY = 1.0f,
			u_char a = 255,
			u_char r = 255,
			u_char g = 255,
			u_char b = 255
			);
		void Draw_GraphicsLeftTopNC(
			int x, int y, float z,
			const std::string& resname,
			int srcX, int srcY,
			int sizeX, int sizeY,
			int	degree = 0, POINT *center = nullptr,
			float	scaleX = 1.0f, float scaleY = 1.0f,
			u_char a = 255,
			u_char r = 255,
			u_char g = 255,
			u_char b = 255
			);
		void Draw_GraphicsNC(
			float x, float y, float z,
			const std::string& resname,
			int srcX, int srcY,
			int sizeX, int sizeY,
			int	degree = 0, 
			float	scaleX = 1.0f, float scaleY = 1.0f,
			u_char a = 255,
			u_char r = 255,
			u_char g = 255,
			u_char b = 255
			);
		void Draw_GraphicsLeftTopNC(
			float x, float y, float z,
			const std::string& resname,
			int srcX, int srcY,
			int sizeX, int sizeY,
			int	degree = 0, POINT *center = nullptr,
			float	scaleX = 1.0f, float scaleY = 1.0f,
			u_char a = 255,
			u_char r = 255,
			u_char g = 255,
			u_char b = 255
			);
		void Draw_Box(
			int left, int top, int right, int bottom,
			float z,
			D3DCOLOR fillColor, D3DCOLOR frameColor,
			int lineSize, bool nakanuri
			);
		void Draw_BoxCenter(
			int x, int y, int sizeX, int sizeY,
			float z,
			D3DCOLOR fillColor, D3DCOLOR frameColor,
			int lineSize, bool nakanuri
			);
		void Draw_BoxNC(
			int left, int top, int right, int bottom,
			float z,
			D3DCOLOR fillColor, D3DCOLOR frameColor,
			int lineSize, bool nakanuri
			);
		void Draw_BoxCenterNC(
			int left, int top, int right, int bottom,
			float z,
			D3DCOLOR fillColor, D3DCOLOR frameColor,
			int lineSize, bool nakanuri
			);
		void Draw_LineNC(
			int startX, int startY,
			int endX, int endY,
			float z, D3DCOLOR color, int size
			);

		void Draw_2DClear();
		void Draw_3DClear();
		void Draw_2DRefresh();
		bool Draw_3DRefresh();

		void Draw_CkRect(const RECT& rt, D3DCOLOR color = ARGB(255, 255, 0, 0));
		void Draw_CkRectNC(const RECT& rt, D3DCOLOR color = ARGB(255, 255, 0, 0));
		D3DSURFACE_DESC Draw_GetImageSize(const std::string& resname);
		SIZE Draw_GetImageSize2(const std::string& resname);
		unsigned int Draw_GetImageWidth(const std::string& resname);
		unsigned int Draw_GetImageHeight(const std::string& resname);

	}
	namespace font{
		//フォント関連	
		void Draw_FontText(int x, int y, float z, const std::string& msg, D3DCOLOR color, int fontID);
		void Draw_FontTextNC(int x, int y, float z, const std::string& msg, D3DCOLOR color, int fontID);
		int  Draw_FontText(const RECT& rt, float z, const std::string& msg, D3DCOLOR color, int fontID);
		int  Draw_FontTextNC(const RECT& rt, float z, const std::string& msg, D3DCOLOR color, int fontID);
		void Draw_TextXY(int x, int y, const std::string& msg, D3DCOLOR Textcolor, int fontID = 0);
		void Draw_TextXYNC(int x, int y, const std::string& msg, D3DCOLOR Textcolor, int fontID = 0);
		void Draw_CreateFont(int Num, int size, LPCTSTR	fontname);
		void Draw_CreateFontItalic(int Num, int size, LPCTSTR	fontname);
	}
	namespace input{
		WHEEL GetWheelRoll();
		int GetWheelDelta();
		//入力関連
		void CheckKey();
		bool CheckPress(int keyno, int interval = 1, int player = 0);	//押されているかチェック
		bool CheckAnyPress(int keyno);
		bool CheckFree(int keyno, int interval = 1, int player = 0);		//はなされているかチェック
		bool CheckPush(int keyno, int player = 0);		//押したかをチェック
		bool CheckAnyPush(int keyno);
		bool CheckPull(int keyno, int player = 0);		//はなしたかをチェック
		int  GetKeyStatus(int keyno, int player = 0);	//キーの状態をチェック
		int  GetKeyFrameHold(int keyno, int player = 0);	//押されているフレーム数を取得
		int  GetKeyFrameFree(int keyno, int player = 0);	//はなされているフレーム数を取得
		void InitKeyInfo(int player = 0);				//ステータスとフレームを初期化
		void Key_GetKey(unsigned int Code, int* Flag);
		void SetKeyFormat(CODE_KEY key, DWORD keycode, int playernum);
		void SetKeysFormat(DWORD* keycode, int playernum);
		bool IsUsePad(int playerID);

		POINT GetMousePosition();				//マウスカーソルの座標を取得
		void  GetMousePosition(POINT* pt_ptr);//マウスカーソルの座標を取得
	}
	namespace debug{
		// デバッグ関連
		void Dbg_FileOut(const char *str, ...);
		void Dbg_ExitApp();  //アプリケーションを終了させる要求をWIndowsに送ります（メモリリークに注意！）
		void Dbg_DisplayToMessage(int x, int y, const char * str, ...);
		void Dbg_TilteToMessage(const char * str, ...);
		void Dbg_BoxToMessage(const char *str, ...);
		void Dbg_StringOutXY(int x, int y, char* str, ...);
		void Draw_ShowFps();
		DLG_ERROR Dbg_InputDialog(char* get, int size, const char* title, const char* label);
		void Dbg_DisplayToMessageNC(int x, int y, const char * str, ...);

		// Releaseビルドでは無効になる
#ifdef _DEBUG

#define TToM(...)   Dbg_TilteToMessage(__VA_ARGS__)
#define FToM(...)   Dbg_FileOut(__VA_ARGS__)
#define BToM(...)   Dbg_BoxToMessage(__VA_ARGS__)
#define SToM(...)   Dbg_DisplayToMessage(__VA_ARGS__)
#define SToMNC(...)	Dbg_DisplayToMessageNC(__VA_ARGS__)
		inline void DFPS() { Draw_ShowFps(); }

#else
		inline void TToM(...) {}
		inline void FToM(...) {}
		inline void BToM(...) {}
		inline void SToM(...) {}
		inline void SToMNC(...) {}
		inline void DFPS() {}
#endif

		// Releaseビルドでも有効
#define TToMR(...)		Dbg_TilteToMessage(__VA_ARGS__)
#define FToMR(...)		Dbg_FileOut(__VA_ARGS__)
#define BToMR(...)		Dbg_BoxToMessage(__VA_ARGS__)
#define SToMR(...)		Dbg_DisplayToMessage(__VA_ARGS__)
#define SToMNCR(...)	Dbg_DisplayToMessageNC(__VA_ARGS__)
	}

	namespace bgm{
#ifdef USE_DIRECTSHOW
		typedef int DSHOWOBJ;
		//BGM再生関数
		void DShow_Init();
		void DShow_Del();
		void DShow_LoadFile(const std::string& resname, const std::string& filename);
		bool DShow_Play(const std::string& resname);
		void DShow_RateControl(const std::string& resname, float rate);
		void DShow_EndCheck();
		void DShow_Stop(const std::string& resname);
		void DShow_AllStop();
		LONGLONG DShow_GetCurrentPos(const std::string& resname);
		void DShow_SetStartPos(const std::string& resname);
		void DShow_VolumeControl(const std::string& resname, int volume);
		void DShow_Pause(const std::string& resname);
		LONGLONG DShow_GetEndPosition(const std::string& resname);
#endif
	}
	namespace se{
#ifdef USE_DIRECTSOUND
		typedef int DSOUNDOBJ;
		//SE再生関数
		void DSound_Init();
		bool DSound_Create();
		void DSound_CreateSecondaryBuffer();
		bool DSound_CreatePrimaryBuffer();
		bool DSound_Del();
		void DSound_Play(const std::string& resname);
		void DSound_PlayLoop(const std::string& resname);
		bool DSound_Del();
		void DSound_LoadFile(const std::string& resname, const std::string& filename);
		void DSound_AllStop();
		void DSound_Stop(const std::string& resname);
		void DSound_EndCheck();
		void DSound_SetFrequency(const std::string& resname, int Fre);
		void DSound_SetVolume(const std::string& resname, int Vol);
#endif
	}
	namespace math{
		//数学関連
		bool RectCheck(const RECT& rt1, const RECT& rt2);
		bool RectCheck(const RECT *rt1, const RECT *rt2);
		bool IsFrameOut_Center(float x, float y, float width, float height);
		bool IsFrameOut_LeftTop(float x, float y, float width, float height);

		float Calc_XYToRad(int x, int y);		//座標XYへの角度を求める
		float Calc_XYToSinCos(int x, int y, float *addx, float *addy);	//座標XYへの移動量XYを求める
		float Calc_RadToDegree(float rad);		//ラジアンから角度へ
		float Calc_DegreeToRad(float degree);		//角度からラジアンへ

		extern std::mt19937 engine;

		void RandomInit();

		// [min, max]の一様な乱数を取得
		template <typename T>
		T GetRandom(const T min, const T max) {
			std::uniform_real_distribution<T> range(min, max);
			return range(engine);
		}

		// 特殊化
		// ヘッダに実装を書く場合はinlineをつける
		template <>
		inline int GetRandom<int>(const int min, const int max) {
			std::uniform_int_distribution<> range(min, max);
			return range(engine);
		}

		// 点と点の角度を求める（-180~180）
		float RadianOfPoints2(float basisX, float basisY, float targetX, float targetY);
		float DegreeOfPoints2(float basisX, float basisY, float targetX, float targetY);

		// 二点間の距離をもとめる
		float Distance2(float x1, float y1, float x2, float y2);

		float ROUND_X(float angle, float length, float center = 0.f);
		float ROUND_Y(float angle, float length, float center = 0.f);
		float ROUND_Z(float angle, float length, float center = 0.f);

		void reverseRect(RECT& baseRect, int width);
	}

	namespace time{
		//高精度タイマー関連
		void  Time_ResetTimer(int id);
		void  Time_StartTimer(int id);
		bool  Time_MarkOfOneSec(int id);
		bool  Time_MarkOfTime(int id, float marktime);
		float Time_GetTimerCount();
		float Time_GetTimerCount(int id);
		float Time_Update();
		float Time_GetOneFrameCount();
		void  Time_StartTimerCount();
		bool  Time_CheckTimer();
	}

	//拡張機能を使わない場合はコメントアウトすること
	//#include "ci_ext/ci_ext.hpp"
}

#ifdef CI_EXT
bool IsFrameOut_Center(const ci_ext::Vec3f& pos, const ci_ext::Vec3f& size);
bool IsFrameOut_LeftTop(const ci_ext::Vec3f& pos, const ci_ext::Vec3f& size);
#endif

