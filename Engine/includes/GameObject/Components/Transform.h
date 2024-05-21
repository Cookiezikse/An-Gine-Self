#pragma once

#include <cmath>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include "Component.h"

class Transform final : public Component
{
	public:
		Transform()
		{
			type = "Transform";
			name = "Transform";
		}
		// Méthode pour récupérer et définir la position
		glm::vec3 GetPosition() const { return position; }
		void      SetPosition(const float& _x, const float& _y, const float& _z) { position = glm::vec3(_x, -_y, -_z); }
		void      SetPosition(const glm::vec3& _position) { position = _position;}

		// Méthode pour récupérer et définir la rotation en utilisant un angle
		glm::vec3 GetRotation() const { return rotation; }
		void  SetRotation(const float _angle) { rotation = glm::vec3{ _angle, _angle,_angle }; }
		void  SetRotation(const glm::vec3& _rotation) { rotation = _rotation; }

		glm::vec3 GetRotationDegrees() const {
			return rotation * (180.0f / glm::pi<float>());
		}

		void SetRotationDegrees(const glm::vec3& degrees) {
			rotation = degrees * (glm::pi<float>() / 180.0f);
		}

		// Méthode pour récupérer et définir l'échelle
		glm::vec3 GetScale() const { return scale; }

		void SetScale(const float& _scaleX, const float& _scaleY, const float& _scaleZ)
		{
			scale = glm::vec3(_scaleX, _scaleY, _scaleZ);
		}

		void SetScale(const glm::vec3& _scale) { scale = _scale; } 

		/*void Rotate(const float _angle)
		{
			// Création de la matrice de rotation supplémentaire
			glm::mat4 rotation_matrix = glm::rotate(glm::mat4(1.0f), _angle, rotation);

			// Mise à jour de la matrice de rotation actuelle
			rotation_matrix = rotation_matrix * rotation;

			// Mise à jour de la matrice de rotation actuelle
			rotation = rotation_matrix;
		}*/

		glm::mat4 Mat4()
		{
			const float c3 = glm::cos(rotation.z);
			const float s3 = glm::sin(rotation.z);
			const float c2 = glm::cos(rotation.x);
			const float s2 = glm::sin(rotation.x);
			const float c1 = glm::cos(rotation.y);
			const float s1 = glm::sin(rotation.y);
			return glm::mat4{
				{
					scale.x * (c1 * c3 + s1 * s2 * s3),
					scale.x * (c2 * s3),
					scale.x * (c1 * s2 * s3 - c3 * s1),
					0.0f,
				},
				{
					scale.y * (c3 * s1 * s2 - c1 * s3),
					scale.y * (c2 * c3),
					scale.y * (c1 * c3 * s2 + s1 * s3),
					0.0f,
				},
				{
					scale.z * (c2 * s1),
					scale.z * (-s2),
					scale.z * (c1 * c2),
					0.0f,
				},
				{position.x, position.y, position.z, 1.0f}
			};
		}

		glm::mat3 NormalMatrix() const
		{
			const float     c3 = glm::cos(rotation.z);
			const float     s3 = glm::sin(rotation.z);
			const float     c2 = glm::cos(rotation.x);
			const float     s2 = glm::sin(rotation.x);
			const float     c1 = glm::cos(rotation.y);
			const float     s1 = glm::sin(rotation.y);
			const glm::vec3 inv_scale = 1.0f / scale;

			return glm::mat3{
				{
					inv_scale.x * (c1 * c3 + s1 * s2 * s3),
					inv_scale.x * (c2 * s3),
					inv_scale.x * (c1 * s2 * s3 - c3 * s1),
				},
				{
					inv_scale.y * (c3 * s1 * s2 - c1 * s3),
					inv_scale.y * (c2 * c3),
					inv_scale.y * (c1 * c3 * s2 + s1 * s3),
				},
				{
					inv_scale.z * (c2 * s1),
					inv_scale.z * (-s2),
					inv_scale.z * (c1 * c2),
				}
			};
		}

		// Méthode pour afficher les propriétés du Transform
		void Display();

		// Implémentation de la méthode toJson() pour convertir en JSON
		json toJson() const override {
			json j = Component::toJson(); // Appeler la méthode de la classe de base pour obtenir les données communes
			j["position"] = { position.x, position.y, position.z };
			j["scale"] = { scale.x, scale.y, scale.z };
			j["rotation"] = { rotation.x, rotation.y, rotation.z };
			return j;
		}

		// Implémentation de la méthode fromJson() pour initialiser à partir de JSON
		void fromJson(const json& j) override {
			Component::fromJson(j); // Appeler la méthode de la classe de base pour initialiser les données communes
			position = glm::vec3(j["position"][0], j["position"][1], j["position"][2]);
			scale = glm::vec3(j["scale"][0], j["scale"][1], j["scale"][2]);
			rotation = glm::vec3(j["rotation"][0], j["rotation"][1], j["rotation"][2]);
		}
	private:
		glm::vec3 position{};
		glm::vec3 scale{ 1.0f };
	glm::vec3 rotation{};
};
