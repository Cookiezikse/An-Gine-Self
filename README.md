🖥️ Moteur de jeu 3D avec Vulkan
============================


Notre moteur 3D en développement s'appuie sur la puissance de Vulkan et intègre plusieurs bibliothèques spécialisées pour améliorer le processus de développement. Actuellement, nous explorons l'utilisation de FMOD pour le rendu audio, GLFW pour la gestion des fenêtres, GLM pour les calculs mathématiques, JSON pour la configuration des données, TinyObjLoader pour le chargement des modèles 3D, STB pour divers utilitaires graphiques, et Winsock pour le support réseau. Combinaison qui permet un bon développement et des solutions plus rapides !

Our in-development 3D engine leverages the power of Vulkan and integrates several specialized libraries to enhance the development process. Currently, we are exploring the use of FMOD for audio rendering, GLFW for window management, GLM for mathematical calculations, JSON for data configuration, TinyObjLoader for loading 3D models, STB for various graphics utilities, and Winsock for network support. This combination enables efficient development and faster solutions!

🔍 Technologies utilisées / Technologies used
----------------------

[![](https://skillicons.dev/icons?i=cpp,git)](https://skillicons.dev)<img src="https://www.vulkan.org/user/themes/vulkan/images/logo/vulkan-logo.svg" width="150">


🔍 Librairies utilisées / Libraries used
----------------------

*   **[Vulkan](https://www.vulkan.org/)**: API graphique pour le rendu 3D. / Graphics API for 3D rendering.
*   **[Bullet Physics](https://github.com/bulletphysics/bullet3)**: Simulation de la physique en temps réel. / Real-time physics simulation.
*   **[Dear ImGui](https://github.com/ocornut/imgui)**: Création d'interfaces utilisateur dans les applications 3D. / Creation of user interfaces in 3D applications.
*   **[GLFW](https://github.com/glfw/glfw)**: Création de fenêtres et gestion des entrées. / Window creation and input management.
*   **[FMOD](https://www.fmod.com/)**: Gestion audio multiplateforme. / Cross-platform audio management.
*   **Winsock**: Programmation des sockets pour la communication réseau. / Socket programming for network communication.
*   **[tinyObjLoader](https://github.com/tinyobjloader/tinyobjloader)**: Chargement de fichiers obj pour les modèles 3D. / Loading OBJ files for 3D models.
*   **[JSON for Modern C++](https://github.com/nlohmann/json)**: Manipulation de fichiers JSON. / JSON file manipulation.
*   **[GLM](https://github.com/g-truc/glm)**: Bibliothèque mathématique pour les graphiques. / Mathematical library for graphics.

Comment utiliser / How to use
----------------

### ⚙️ Installation des dépendances / Installation of dependencies

*   Assurez-vous d'avoir le [Vulkan SDK](https://vulkan.lunarg.com/sdk/home#windows) installé sur votre système.
  *
*   Make sure you have the [Vulkan SDK](https://vulkan.lunarg.com/sdk/home#windows) installed on your system.

### ⚒️ Compilation du projet (si vous utilisez le repository) / Project compilation (if you are using the repository)

1.  Clonez ce dépôt sur votre machine.
2.  Lancez (les dépendences sont déjà faites normalement) Si vous avez vulkan installé autre part que sur le C **(C:\VulkanSDK)**, vous devrez refaire une dépendence [Tuto Vulkan](https://vulkan-tutorial.com/Development_environment) pour vous aider sur comment faire.
*

1.  Clone this repository on your machine.
2.  Run (dependencies should already be set up). If you have Vulkan installed in a location other than C **(C:\VulkanSDK)**, you may need to set up the dependencies again.
Here's a [Vulkan Tuto](https://vulkan-tutorial.com/Development_environment) to assist you in doing so.

### 🔧 Exécution du moteur / Engine execution

*   Lancez l'application compilée.
*   Utilisez l'interface utilisateur pour charger des modèles 3D, définir les paramètres de simulation physique et interagir avec le moteur.
*
*   Run the compiled application.
*   Use the user interface to load 3D models, set up physics simulation parameters, and interact with the engine.

### 🎹 Les commandes / The commands

*   Z Q S D Pour les déplacements de la caméra
*   A E Pour les déplacements en hauteur
*   ⬆️ ⬇️ ➡️ ⬅️ pour la rotation de la caméra
*
*   Z Q S D for camera movements
*   A E for vertical movements
*   ⬆️ ⬇️ ➡️ ⬅️ for camera rotation

Informations supplémentaires / Additional information
----------------------------

### 🔗 Équipe de développement / Development team

*   **[@Cookiezikse](https://www.github.com/cookiezikse)**
*   **[@Quentin](https://github.com/MtPoison)**
*   **[@PoloBongo](https://github.com/PoloBongo)**
*   **[@DemonXlegenD](https://github.com/DemonXlegenD)**
*   **[@Kais](https://github.com/KakaTheRat)**
*   **[@Euexia](https://github.com/Euexia)**
*   **[@Mathias](https://github.com/0nnen)**
*   **[@Maurad](https://github.com/PoloBongo)**
