#include "GameObject/Components/BoxCollider.h"

#include "Tools/Convertisseur.h"

BoxCollider::BoxCollider(btDiscreteDynamicsWorld* _dynamicsWorld, const glm::vec3 _center, const glm::vec3 _position,
                         btCollisionWorld*        _collisionWorld): rigidBody(nullptr)
{
	collisionShape = new btBoxShape(Convertisseur::GlmVec3ToBtVector3(_position));

	ghostObject = new btGhostObject();
	ghostObject->setCollisionShape(collisionShape);
	ghostObject->setCollisionFlags(ghostObject->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);

	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(Convertisseur::GlmVec3ToBtVector3(_center));
	ghostObject->setWorldTransform(transform);

	_collisionWorld->addCollisionObject(ghostObject);
	_dynamicsWorld->addRigidBody(rigidBody);
}

void BoxCollider::SetSize(const glm::vec3 _size)
{
	delete collisionShape;
	collisionShape = new btBoxShape((Convertisseur::GlmVec3ToBtVector3(_size)));
	ghostObject->setCollisionShape(collisionShape);
}

void BoxCollider::SetCenter(const glm::vec3 _center) const
{
	btTransform transform = ghostObject->getWorldTransform();
	transform.setOrigin(Convertisseur::GlmVec3ToBtVector3(_center));
	ghostObject->setWorldTransform(transform);
}

BoxCollider::~BoxCollider()
{
	delete ghostObject;
	delete collisionShape;
}
