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
		//���\�[�X�̖��O�ƃp�X��ǂݍ���
		readFileData();
	}

	


	void LoadGameRes::init()
	{
		//�ǂݍ��񂾐��̑������v�Z��nowLoadingImage�ɑ���Abar�̒����l�ɂȂ�
		float countMax = (float)(imageCount_ + bgmCount_ + seCount_);
		nowLoadingImage_.setLoadCountMax(countMax);
		
	}
	void LoadGameRes::update()
	{
		//nowLoading�摜��update
		nowLoadingImage_.update();

		switch (state_)
		{
		case State::Loading:
			loadFile();
			break;
		case State::Finish :
			
			//��������鉉�o��҂�
			if (nowLoadingImage_.isFinish())
			{
				//���o�I���̊m�F��ꂽ��title����
				insertToParent(new game::Title("title"));

				//tile�����̂ݐ��������l�ɁA���S�̂��߂�sate_�������̂Ȃ���ԂɊO��
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

	////���\�[�X�̖��O�ƃp�X��ۑ�����
	void LoadGameRes::pushResAndFileName(const std::string& str)
	{
		std::stringstream stream(str);
		std::string data;
		stream >> data;
		resName_.push_back(data);
		stream >> data;
		fileName_.push_back(data);
	}
	
	//���\�[�X�̖��O�ƃp�X��ǂݍ���
	void LoadGameRes::readFileData()
	{
		std::ifstream f(loadFileName_);
		if (f.fail()) assert(false);
		if (f.is_open())
		{
			//�C���[�W�̃^�O�܂ŒT��
			myLib::lineFinder(f, "type=image");

			//�C���[�W�̖��O�ƃp�X��ǂݍ���
			while (true)
			{
				std::string line;
				std::getline(f, line);

				//BGM�̃^�O�܂œǂݍ��񂾂甲����B
				if (line == "type=BGM")
				{
					break;
				}
				if (f.eof())
				{
					assert(false);
				}

				//���\�[�X�̖��O�ƃp�X��ۑ�����
				pushResAndFileName(line);
				//�C���[�W�̐��𐔂���
				imageCount_++;
			}

			//BGM�̖��O�ƃp�X��ǂݍ���
			while (true)
			{
				std::string line;
				std::getline(f, line);
				//SE�̃^�O�܂œǂݍ��񂾂甲����B
				if (line == "type=SE")
				{
					break;
				}
				if (f.eof())
				{
					assert(false);
				}
				//���\�[�X�̖��O�ƃp�X��ۑ�����
				pushResAndFileName(line);
				//BGM�̐��𐔂���
				bgmCount_++;
			}

			//�T�E���h�G�t�F�N�g�̖��O�ƃp�X��ǂݍ���
			while (true)
			{
				std::string line;
				std::getline(f, line);
				//End�̃^�O�܂œǂݍ��񂾂甲����B
				if (line == "type=End")
				{
					break;
				}
				if (f.eof())
				{
					assert(false);
				}
				//���\�[�X�̖��O�ƃp�X��ۑ�����
				pushResAndFileName(line);
				//SE�̐��𐔂���
				seCount_++;
			}
		}
		f.close();
	}

	//���\�[�X�����[�h����
	void LoadGameRes::loadFile()
	{

		if (loadCountNow_ < imageCount_)
		{
			//�摜
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
			//���[�h���I������痑������鉉�o���߂�����
			nowLoadingImage_.changeState(game::NowLoadingImage::State::Break);
			state_ = State::Finish;
			return;
		}

		//1�t���[���Ɉ���[�h����B
		loadCountNow_++;

		//bar�̗Ε����̒l�𑗂�
		nowLoadingImage_.setLoadCountNow(loadCountNow_);
	}

}

