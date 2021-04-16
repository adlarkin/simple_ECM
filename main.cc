#include "Components.hpp"
#include "Ecm.hpp"

int main()
{
  ECM ecm;

  auto entity = ecm.CreateEntity();
  Position position;
  ecm.AddComponent<Position>(entity, position);

  return 0;
}
