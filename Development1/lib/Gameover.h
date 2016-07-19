#pragma once

namespace gameover{
	//ゲームオーバー時に切り替わる時間を指定する。
	//デフォルトは3秒
	//任意変更の場合はStartGameOverで行うこと
	void SetChangeTime(float time);
	//終了処理開始
	//ゲームオーバー成立時に呼び出し
	void StartGameOver();
	//終了処理初期化
	void InitGameOver();
	//終了処理判別
	//キャラクタの移動抑止などに
	//StartGameOver呼び出し後はtrue
	//InitGameOverでfalse+
	bool isGameOver();
	//終了時ステージ移行処理
	//StartGameOver後に一定時間で画面切り替えを実行する。
	//next : 切り替えるステージの番号
	void CheckToChangeScreen(int next);
}

