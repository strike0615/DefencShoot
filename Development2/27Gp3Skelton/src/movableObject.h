#pragma once
#include "../../lib/ci_ext/object.hpp"
#include "../../lib/ci_ext/DrawObj.hpp"

namespace game
{

class MovableObject : public ci_ext::DrawObjf
{
protected:
	ci_ext::Vec3f velocity_;
  //�U������A���ꔻ��𕪂���ꍇ�͂��̒l��ύX����
  //���p���Ȃ��ꍇ�́A�ʏ�̍��W������Ƃ��ė��p�����
  RECT    offenseRect;
  RECT    defenseRect;
public:
  MovableObject(const ci_ext::DrawObjf& f);
  MovableObject(const std::string& objectName);
  MovableObject();

  void render() override;
  void update() override;
  //�ėp����
  RECT getCollisionRect(int sx = 0, int sy = 0, int ex = 0, int ey = 0) const;
  //�U������
  RECT getOffenseRect() ;
  //���ꔻ��
  RECT getDefenseRect() ;


};

}