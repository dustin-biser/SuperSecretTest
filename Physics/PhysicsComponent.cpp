//
// PhysicsComponent.cpp
//
#if false

#include "PhysicsComponent.hpp"

#include "PhysicsSystem.hpp"

//---------------------------------------------------------------------------------------
PhysicsComponent::PhysicsComponent () 
{

}

//---------------------------------------------------------------------------------------
PhysicsComponent::~PhysicsComponent()
{

}

//---------------------------------------------------------------------------------------
PhysicsComponent * PhysicsComponent::clone() const
{
	return new PhysicsComponent(*this);
}

#endif
