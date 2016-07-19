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
  //�w�肵�����O�̃I�u�W�F�N�g���擾
  auto src = getObjectsFromRoot({ srcname_ });
  auto dest = getObjectsFromRoot({ destname_ });
  //����`�F�b�N
  for (auto s : src){
    for (auto d : dest){
      //�󂯑��Ǝ�葤�̔�����擾
      auto srcObject = ci_ext::weak_to_shared<MovableObject>(s);
      auto destObject = ci_ext::weak_to_shared<MovableObject>(d);
      RECT srcRect;
      RECT destRect;

      //�U����Rect�𖢎g�p�Ȃ��
      if (srcObject->getOffenseRect().top == -1 && srcObject->getOffenseRect().bottom == -1){
        srcRect = srcObject->getCollisionRect();
      }
      else{
        srcRect = srcObject->getOffenseRect();
      }
      //�U����Rect�𖢎g�p�Ȃ��
      if (destObject->getDefenseRect().top == -1 && destObject->getDefenseRect().bottom == -1){
        destRect = destObject->getCollisionRect();
      }
      else{
        destRect = destObject->getDefenseRect();
      }

      //����
      if (gplib::math::RectCheck(srcRect, destRect)){
/*      //����
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