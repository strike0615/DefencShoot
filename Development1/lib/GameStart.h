#pragma once

namespace gamestart{
	//�Q�[���J�n���ɑ���n�܂�܂ł̎��Ԃ��w�肷��B
	//�f�t�H���g3�b��Init�Őݒ肷��̂�
	//�C�ӕύX�̏ꍇ��Start�ŏ����̊J�n���s������
	void SetTime(float time);
	//�X�e�[�W�����J�n
	void Start();
	//�I������������
	void Init();
	//��������
	//�L�����N�^�̈ړ��}�~�Ȃǂ�
	//true:�J�n�������s��
	//false:�J�n�����I��
	bool isGameStart();
	//�I�����X�e�[�W�J�n����
	//Start��Ɉ�莞�Ԃɏ�����������s����B
	void Step();
	//�J�E���g���擾����
	float GetCount();
}

