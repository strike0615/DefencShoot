#include "movableObject.h"
#include "../../lib/ci_ext/math.hpp"

#include <iostream>

namespace game
{
  using namespace ci_ext;

MovableObject::MovableObject(const ci_ext::DrawObjf& f)
  :
  ci_ext::DrawObjf(f)
{
    //矩形の初期化
    SetRect(&offenseRect, -1, -1, -1, -1);
    SetRect(&defenseRect, -1, -1, -1, -1);
}

MovableObject::MovableObject()
:
ci_ext::DrawObjf("",
ci_ext::Vec3f::one(), ci_ext::Vec3f::one(), "", ci_ext::Vec3f::one(), ci_ext::Vec3f::one(), ci_ext::Color(0xFFFFFFFF), 0)
{
  SetRect(&offenseRect, -1, -1, -1, -1);
  SetRect(&defenseRect, -1, -1, -1, -1);
}

MovableObject::MovableObject(const std::string& objectName)
:
ci_ext::DrawObjf(objectName,
ci_ext::Vec3f::one(), ci_ext::Vec3f::one(), "", ci_ext::Vec3f::one(), ci_ext::Vec3f::one(), ci_ext::Color(0xFFFFFFFF), 0)
{
  SetRect(&offenseRect ,- 1, -1, -1, -1);
  SetRect(&defenseRect, -1, -1, -1, -1);
}

void MovableObject::render()
{
  DrawObjf::render();
}

void MovableObject::update()
{
  offsetPos(velocity_);
  Vec3f hSize = halfsize<float>();
  if (!gplib::camera::InScreenCameraCenter(
    x(), y(),
    hSize.x(),
    hSize.y()))
  {
    kill();
  }

}

//オフセット値考慮の矩形生成
RECT MovableObject::getCollisionRect(int sx,int sy,int ex,int ey) const
{
  return makeDrawRect(sx,sy,ex,ey);
}

//攻撃矩形生成
RECT MovableObject::getOffenseRect() 
{
  return offenseRect;
}
//やられ矩形生成
RECT MovableObject::getDefenseRect() 
{
  return defenseRect;
}



}