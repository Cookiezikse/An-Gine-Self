#pragma once

#include "Component.h"

#include <glm.hpp>

#include <btBulletDynamicsCommon.h>

class RigidBody : public Component
{
	public:
		/**
		* @brief Constructeur de la classe RigidBody.
		*
		* @param _dynamicsWorld Un pointeur vers le monde dynamique de physique Bullet.
		* @param _shape La forme de collision du corps rigide.
		* @param _mass La masse du corps rigide.
		* @param _position La position initiale du corps rigide.
		*/
		RigidBody(btDiscreteDynamicsWorld* _dynamicsWorld, btCollisionShape* _shape, float _mass, glm::vec3& _position);

		void Update() override;
		/**
		* @brief Ajoute une force au corps rigide.
		*
		* @param _force La force à ajouter au corps rigide.
		*/
		void AddForce(const glm::vec3& _force) const;
		/**
		* @brief Définit la vélocité du corps rigide.
		*
		* @param _gravity
		* @param _velocity La vélocité à définir pour le corps rigide.
		*/
		void SetGravity(const glm::vec3& _gravity) const;
		/**
		* @brief Récupère la vélocité du corps rigide.
		*
		* @return La vélocité actuelle du corps rigide.
		*/
		void SetVelocity(const glm::vec3& _velocity) const;
		/**
		* @brief Définit la gravité agissant sur le corps rigide.
		*
		* @param _gravity La gravité à définir pour le corps rigide.
		*/
		btVector3 GetVelocity() const;
		/**
		* @brief Définit si le corps rigide est cinématique.
		*
		* @param _isKinematic True pour définir le corps rigide comme cinématique, sinon false.
		*/
		void SetKinematic(bool _isKinematic) const;
		/**
		* @brief Verrouille ou déverrouille la rotation du corps rigide sur les axes spécifiés.
		*
		* @param _lockRotationX Si vrai, verrouille la rotation sur l'axe X, sinon la rotation sur cet axe est libre.
		* @param _lockRotationY Si vrai, verrouille la rotation sur l'axe Y, sinon la rotation sur cet axe est libre.
		* @param _lockRotationZ Si vrai, verrouille la rotation sur l'axe Z, sinon la rotation sur cet axe est libre.
		*/
		void SetRotationLocked(bool _lockRotationX, bool _lockRotationY, bool _lockRotationZ) const;
		/**
		* @brief Définit la position du corps rigide avec des options de verrouillage d'axes.
		*
		* @param _position La nouvelle position du corps rigide.
		* @param _lockX Si vrai, verrouille la position sur l'axe X.
		* @param _lockY Si vrai, verrouille la position sur l'axe Y.
		* @param _lockZ Si vrai, verrouille la position sur l'axe Z.
		*/
		void SetPosition(const glm::vec3& _position, bool _lockX, bool _lockY, bool _lockZ) const;
		/**
		* @brief Destructeur de l'objet RigidBody, libérant les ressources associées.
		*/
		~RigidBody() override;

		// Implémentation de la méthode toJson() pour convertir en JSON
		json toJson() const override {
			json j = Component::toJson(); // Appeler la méthode de la classe de base pour obtenir les données communes

			return j;
		}

		// Implémentation de la méthode fromJson() pour initialiser à partir de JSON
		void fromJson(const json& j) override {
			Component::fromJson(j); // Appeler la méthode de la classe de base pour initialiser les données communes
		}
	private:
		btDiscreteDynamicsWorld* dynamicsWorld;
		btRigidBody*             rigidBody;
};
