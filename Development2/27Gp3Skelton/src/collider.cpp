#include "Collider.h"
#include "../../lib/gplib.h"
#include "movableObject.h"

#include "effect.h"

namespace game
{

	Collider::Collider(const std::string& objectName, const std::string& srcName, const std::string& destName)
  :
	Object(objectName),
  srcname_(srcName),
	destname_(destName)
{
}

void Collider::render()
{
}

void Collider::update()
{
  //指定した名前のオブジェクトを取得
  auto src = getObjectsFromRoot({ srcname_ });
  auto dest = getObjectsFromRoot({ destname_ });
  //判定チェック
  for (auto s : src){
    for (auto d : dest){
      //受け側と守り側の判定を取得
      auto srcObject = ci_ext::weak_to_shared<MovableObject>(s);
      auto destObject = ci_ext::weak_to_shared<MovableObject>(d);
      RECT srcRect;
      RECT destRect;

      //攻撃側Rectを未使用ならば
      if (srcObject->getOffenseRect().top == -1 && srcObject->getOffenseRect().bottom == -1){
        srcRect = srcObject->getCollisionRect();
      }
      else{
        srcRect = srcObject->getOffenseRect();
      }
      //攻撃側Rectを未使用ならば
      if (destObject->getDefenseRect().top == -1 && destObject->getDefenseRect().bottom == -1){
        destRect = destObject->getCollisionRect();
      }
      else{
        destRect = destObject->getDefenseRect();
      }

      //判定
      if (gplib::math::RectCheck(srcRect, destRect)){
/*      //判定
      if (gplib::math::RectCheck( srcObject->getCollisionRect() ,
                                  destObject->getCollisionRect())){
*/
        srcObject->offenseHit(d);
        destObject->defenseHit(s);
      }
    }
  }  
}


}