#pragma once

namespace gamestart{
	//ゲーム開始時に操作始まるまでの時間を指定する。
	//デフォルト3秒はInitで設定するので
	//任意変更の場合はStartで処理の開始を行うこと
	void SetTime(float time);
	//ステージ処理開始
	void Start();
	//終了処理初期化
	void Init();
	//処理判別
	//キャラクタの移動抑止などに
	//true:開始処理実行中
	//false:開始処理終了
	bool isGameStart();
	//終了時ステージ開始処理
	//Start後に一定時間に処理解放を実行する。
	void Step();
	//カウントを取得する
	float GetCount();
}

