#include "GameObject/Components/RigidBody.h"

#include "Tools/Convertisseur.h"


RigidBody::RigidBody(btDiscreteDynamicsWorld* _dynamicsWorld, btCollisionShape* _shape, float _mass,
                     glm::vec3&               _position)
{
	btVector3 localInertia(0, 0, 0);
	_shape->calculateLocalInertia(_mass, localInertia);

	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(Convertisseur::GlmVec3ToBtVector3(_position));

	auto                                     motion_state = new btDefaultMotionState(transform);
	btRigidBody::btRigidBodyConstructionInfo rb_info(_mass, motion_state, _shape, localInertia);
	rigidBody = new btRigidBody(rb_info);

	dynamicsWorld->addRigidBody(rigidBody);
}

void RigidBody::Update()
{
	dynamicsWorld->stepSimulation(1 / 60.f);
}

void RigidBody::AddForce(const glm::vec3& _force) const
{
	rigidBody->applyCentralForce(Convertisseur::GlmVec3ToBtVector3(_force));
}

/**
 * \brief 
 * \param _velocity 
 */
void RigidBody::SetVelocity(const glm::vec3& _velocity) const
{
	rigidBody->setLinearVelocity(Convertisseur::GlmVec3ToBtVector3(_velocity));
}

btVector3 RigidBody::GetVelocity() const
{
	return rigidBody->getLinearVelocity();
}

void RigidBody::SetGravity(const glm::vec3& _gravity) const
{
	rigidBody->setGravity(Convertisseur::GlmVec3ToBtVector3(_gravity));
}

void RigidBody::SetKinematic(const bool _isKinematic) const
{
	if (_isKinematic)
	{
		rigidBody->setCollisionFlags(rigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
		rigidBody->setActivationState(DISABLE_DEACTIVATION);
	}
	else
	{
		rigidBody->setCollisionFlags(rigidBody->getCollisionFlags() & ~btCollisionObject::CF_KINEMATIC_OBJECT);
	}
}

void RigidBody::SetRotationLocked(const bool _lockRotationX, const bool _lockRotationY, const bool _lockRotationZ) const
{
	rigidBody->setAngularFactor(btVector3(_lockRotationX ? 0 : 1, _lockRotationY ? 0 : 1, _lockRotationZ ? 0 : 1));
}

void RigidBody::SetPosition(const glm::vec3& _position, const bool _lockX, const bool _lockY, const bool _lockZ) const
{
	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(Convertisseur::GlmVec3ToBtVector3(_position));

	if (_lockX)
		transform.getBasis().setValue
		(0, 0, 0,
		 0, 1, 0,
		 0, 0, 1);
	if (_lockY)
		transform.getBasis().setValue
		(1, 0, 0,
		 0, 0, 0,
		 0, 0, 1);
	if (_lockZ)
		transform.getBasis().setValue
		(1, 0, 0,
		 0, 1, 0,
		 0, 0, 0);

	rigidBody->setWorldTransform(transform);
}


RigidBody::~RigidBody()
{
	dynamicsWorld->removeRigidBody(rigidBody);
	delete rigidBody->getMotionState();
	delete rigidBody;
}
