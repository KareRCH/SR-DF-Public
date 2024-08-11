#include "Contact.h"


#pragma region ¡¢√À «ÿ∞·¿⁄

FContactResolver::FContactResolver(_uint iIterations, Real fVelocityEpsilon, Real fPositionEpsilon)
{
	Set_Iterations(iIterations, iIterations);
	Set_Epsilon(fVelocityEpsilon, fPositionEpsilon);
}

FContactResolver::FContactResolver(_uint iVelocityIterations, _uint iPositionIterations, Real fVelocityEpsilon, Real fPositionEpsilon)
{
	Set_Iterations(iVelocityIterations);
	Set_Epsilon(fVelocityEpsilon, iPositionIterations);
}

void FContactResolver::AdjustVelocities(FContact* pContactArray, _uint iContacts, Real fDuration)
{

}

void FContactResolver::AdjustPositions(FContact* pContacts, _uint iContacts, Real fDuration)
{

}

#pragma endregion