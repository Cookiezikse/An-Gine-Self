#pragma once

#include "GameObject/GameObject.h"
#include "LveEngine/lve_window.h"

namespace lve
{
	class KeyboardMovementController
	{
		public:
			struct KeyMappings
			{
				int moveLeft     = GLFW_KEY_A;
				int moveRight    = GLFW_KEY_D;
				int moveForward  = GLFW_KEY_W;
				int moveBackward = GLFW_KEY_S;
				int moveUp       = GLFW_KEY_E;
				int moveDown     = GLFW_KEY_Q;
				int lookLeft     = GLFW_KEY_LEFT;
				int lookRight    = GLFW_KEY_RIGHT;
				int lookUp       = GLFW_KEY_UP;
				int lookDown     = GLFW_KEY_DOWN;
			};

			/**
			 * @brief Déplace l'objet dans le plan XZ en réponse aux entrées clavier.
			 *
			 * Cette fonction ajuste la rotation et la translation d'un objet en fonction des entrées clavier, pour lui permettre de se déplacer dans le plan XZ.
			 *
			 * @param _window La fenêtre GLFW où les entrées clavier sont détectées.
			 * @param _deltaTime Le temps écoulé depuis la dernière mise à jour.
			 * @param _gameObject L'objet à déplacer.
			 *
			 * @note Les touches du clavier utilisées pour contrôler le mouvement et la rotation sont configurées dans l'attribut "keys" de cet objet.
			 * @note L'objet est déplacé vers l'avant et vers l'arrière le long de l'axe Z, et vers la gauche et vers la droite le long de l'axe X. Il peut également monter et descendre le long de l'axe Y.
			 */
			void MoveInPlaneXZ(GLFWwindow* _window, float _deltaTime, GameObject& _gameObject) const;

			KeyMappings keys{};
			float       moveSpeed{3.f};
			float       lookSpeed{1.5f};
	};
} // namespace lve
