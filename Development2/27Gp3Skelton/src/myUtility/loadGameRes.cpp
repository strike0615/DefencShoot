#pragma once
#include "../../../lib/ci_ext/object.hpp"

#include "../tiltle.hpp"

#include "loadGameRes.h"
#include "data.h"
#include <iostream>

namespace myLib
{
	LoadGameRes::LoadGameRes()
		:
		Object("LoadGameRes"),
		state_(State::Loading),
		loadFileName_("res/data/loadGameRes.txt"),
		loadCountNow_(0.f),
		imageCount_(0),
		bgmCount_(0),
		seCount_(0)
	{
		//リソースの名前とパスを読み込む
		readFileData();
	}

	


	void LoadGameRes::init()
	{
		//読み込んだ数の総数を計算しnowLoadingImageに送る、barの長さ値になる
		float countMax = (float)(imageCount_ + bgmCount_ + seCount_);
		nowLoadingImage_.setLoadCountMax(countMax);
		
	}
	void LoadGameRes::update()
	{
		//nowLoading画像をupdate
		nowLoadingImage_.update();

		switch (state_)
		{
		case State::Loading:
			loadFile();
			break;
		case State::Finish :
			
			//卵が割れる演出を待つ
			if (nowLoadingImage_.isFinish())
			{
				//演出終了の確認取れたらtitle生成
				insertToParent(new game::Title("title"));

				//tileが一回のみ生成される様に、安全のためにsate_を処理のない状態に外す
				state_ = State::NextStage;
				kill();
			}	
			break;
		default:
			break;
		}
	}
	void LoadGameRes::render()
	{
		
		nowLoadingImage_.render();

	}

	////リソースの名前とパスを保存する
	void LoadGameRes::pushResAndFileName(const std::string& str)
	{
		std::stringstream stream(str);
		std::string data;
		stream >> data;
		resName_.push_back(data);
		stream >> data;
		fileName_.push_back(data);
	}
	
	//リソースの名前とパスを読み込む
	void LoadGameRes::readFileData()
	{
		std::ifstream f(loadFileName_);
		if (f.fail()) assert(false);
		if (f.is_open())
		{
			//イメージのタグまで探す
			myLib::lineFinder(f, "type=image");

			//イメージの名前とパスを読み込む
			while (true)
			{
				std::string line;
				std::getline(f, line);

				//BGMのタグまで読み込んだら抜ける。
				if (line == "type=BGM")
				{
					break;
				}
				if (f.eof())
				{
					assert(false);
				}

				//リソースの名前とパスを保存する
				pushResAndFileName(line);
				//イメージの数を数える
				imageCount_++;
			}

			//BGMの名前とパスを読み込む
			while (true)
			{
				std::string line;
				std::getline(f, line);
				//SEのタグまで読み込んだら抜ける。
				if (line == "type=SE")
				{
					break;
				}
				if (f.eof())
				{
					assert(false);
				}
				//リソースの名前とパスを保存する
				pushResAndFileName(line);
				//BGMの数を数える
				bgmCount_++;
			}

			//サウンドエフェクトの名前とパスを読み込む
			while (true)
			{
				std::string line;
				std::getline(f, line);
				//Endのタグまで読み込んだら抜ける。
				if (line == "type=End")
				{
					break;
				}
				if (f.eof())
				{
					assert(false);
				}
				//リソースの名前とパスを保存する
				pushResAndFileName(line);
				//SEの数を数える
				seCount_++;
			}
		}
		f.close();
	}

	//リソースをロードする
	void LoadGameRes::loadFile()
	{

		if (loadCountNow_ < imageCount_)
		{
			//画像
			gplib::graph::Draw_LoadObject(resName_[(unsigned int)loadCountNow_], fileName_[(unsigned int)loadCountNow_]);
		}
		else if (loadCountNow_ < imageCount_ + bgmCount_)
		{
			//BGM
			gplib::bgm::DShow_LoadFile(resName_[(unsigned int)loadCountNow_], fileName_[(unsigned int)loadCountNow_]);
		}
		else if (loadCountNow_ < imageCount_ + bgmCount_ + seCount_)
		{
			//SE
			gplib::se::DSound_LoadFile(resName_[(unsigned int)loadCountNow_], fileName_[(unsigned int)loadCountNow_]);
		}
		else
		{
			//ロードが終わったら卵が割れる演出命令をする
			nowLoadingImage_.changeState(game::NowLoadingImage::State::Break);
			state_ = State::Finish;
			return;
		}

		//1フレームに一個ロードする。
		loadCountNow_++;

		//barの緑部分の値を送る
		nowLoadingImage_.setLoadCountNow(loadCountNow_);
	}

}

