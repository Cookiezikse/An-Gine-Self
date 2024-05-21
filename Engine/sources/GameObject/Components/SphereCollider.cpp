#include "GameObject/Components/SphereCollider.h"

#include "Tools/Convertisseur.h"

SphereCollider::SphereCollider(btDiscreteDynamicsWorld* _dynamicsWorld, const glm::vec3 _center, glm::vec3 _position,
                               btCollisionWorld*        _collisionWorld): rigidBody(nullptr)
{
	// Créer la forme de collision
	collisionShape = new btSphereShape(1.0f); // Par exemple, rayon de la sphère de 1 unité

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

void SphereCollider::SetRadius(const float _radius)
{
	delete collisionShape;
	collisionShape = new btSphereShape(_radius);
	ghostObject->setCollisionShape(collisionShape);
}

void SphereCollider::SetCenter(const glm::vec3 _center) const
{
	btTransform transform = ghostObject->getWorldTransform();
	transform.setOrigin(Convertisseur::GlmVec3ToBtVector3(_center));
	ghostObject->setWorldTransform(transform);
}

SphereCollider::~SphereCollider()
{
	delete ghostObject;
	delete collisionShape;
}
