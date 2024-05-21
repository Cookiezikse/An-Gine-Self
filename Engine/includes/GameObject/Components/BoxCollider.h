#pragma

#include <glm.hpp>

#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <BulletCollision/CollisionShapes/btBoxShape.h>
#include <BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h>

#include "Component.h"

/**
 * @brief Représente un composant de collision de type boîte.
 *
 * Cette classe hérite de la classe Component et contient des fonctionnalités spécifiques
 * aux boîtes de collision dans un environnement physique.
 */
class BoxCollider : public Component
{
	public:
		/**
		 * @brief Constructeur de la classe BoxCollider.
		 *
		 * @param _dynamicsWorld Un pointeur vers le monde dynamique de physique Bullet.
		 * @param _center La position centrale du box collider.
		 * @param _position La position du box collider.
		 * @param _collisionWorld Un pointeur vers le monde de collision Bullet.
		 */
		BoxCollider(btDiscreteDynamicsWorld* _dynamicsWorld, glm::vec3 _center, glm::vec3 _position,
		            btCollisionWorld*        _collisionWorld);
		/**
		 * @brief Définit la taille du box collider.
		 *
		 * @param _size La taille à définir pour le box collider.
		 */
		void SetSize(glm::vec3 _size);
		/**
		* @brief Définit la position centrale du box collider.
		*
		* @param _center La position centrale à définir pour le box collider.
		*/
		void SetCenter(glm::vec3 _center) const;
		/**
		* @brief Vérifie si le box collider est en collision avec un autre objet.
		*
		* @return Vrai si le box collider est en collision avec un autre objet, sinon faux.
		*/
		bool OnTrigger() const
		{
			return ghostObject->getNumOverlappingObjects() > 0;
		}

		/**
		* @brief Destructeur de l'objet BoxCollider, libérant les ressources associées.
		*/
		~BoxCollider() override;
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
		btBoxShape* collisionShape = nullptr; /**< La forme de collision de la boîte. */
		btGhostObject* ghostObject = nullptr; /**< L'objet fantôme associé à la boîte. */
		btRigidBody* rigidBody = nullptr;     /**< Le corps rigide associé à la boîte. */
};
