#pragma once
#include <imgui.h>

/**
 * @brief Classe représentant une interface utilisateur avec ImGUI.
 */
class ImGUIInterface {
public:
    /**
     * @brief Couleur principale utilisée dans le thème de l'interface utilisateur.
     */
    static ImVec4 mainColor;

    /**
     * @brief Couleur d'accentuation, utilisée pour mettre en évidence certains éléments de l'interface utilisateur.
     */
    static ImVec4 accentColor;

    /**
     * @brief Couleur du texte standard dans toute l'interface utilisateur.
     */
    static ImVec4 textColor;

    /**
     * @brief Couleur de fond des zones principales de l'interface utilisateur.
     */
    static ImVec4 areaBgColor;

    /**
     * @brief Couleur secondaire utilisée pour compléter la couleur principale.
     */
    static ImVec4 secondaryColor;

    /**
     * @brief Applique le thème personnalisé aux éléments de l'interface utilisateur d'ImGUI.
     */
    static void ApplyCustomTheme();

    /**
     * @brief Affiche les contrôles pour éditer les couleurs du thème de l'interface utilisateur.
     */
    static void EditTheme();
};
