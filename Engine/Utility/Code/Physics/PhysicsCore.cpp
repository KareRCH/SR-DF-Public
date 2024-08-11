#include "PhysicsCore.h"

using namespace Engine;

const FVector3 FVector3::GRAVITY = FVector3(0, -9.81, 0);
const FVector3 FVector3::HIGH_GRAVITY = FVector3(0, -19.62, 0);
const FVector3 FVector3::UP = FVector3(0, 1, 0);
const FVector3 FVector3::RIGHT = FVector3(1, 0, 0);
const FVector3 FVector3::OUT_OF_SCREEN = FVector3(0, 0, 1);
const FVector3 FVector3::X = FVector3(0, 1, 0);
const FVector3 FVector3::Y = FVector3(1, 0, 0);
const FVector3 FVector3::Z = FVector3(0, 0, 1);

//Real Engine::fSleepEpsilon = static_cast<Real>(0.3);

void Set_SleepEpsilon(Real value)
{
    //fSleepEpsilon = value;
}

//Real Get_SleepEpsilon()
//{
//    //return fSleepEpsilon;
//}
