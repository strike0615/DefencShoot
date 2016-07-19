#pragma once
#include "../../lib/ci_ext/object.hpp"
#include "../../lib/ci_ext/vec3.hpp"

namespace game
{

class Collider : public ci_ext::Object
{
  std::string srcname_;
	std::string destname_;

public:
	Collider(const std::string& objName, const std::string& srcName,const std::string& destName);

  void render() override;
  void update() override;
};

}
