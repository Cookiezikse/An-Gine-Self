#pragma once
#include <imgui.h>

/**
 * @brief Classe repr�sentant une interface utilisateur avec ImGUI.
 */
class ImGUIInterface {
public:
    /**
     * @brief Couleur principale utilis�e dans le th�me de l'interface utilisateur.
     */
    static ImVec4 mainColor;

    /**
     * @brief Couleur d'accentuation, utilis�e pour mettre en �vidence certains �l�ments de l'interface utilisateur.
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
     * @brief Couleur secondaire utilis�e pour compl�ter la couleur principale.
     */
    static ImVec4 secondaryColor;

    /**
     * @brief Applique le th�me personnalis� aux �l�ments de l'interface utilisateur d'ImGUI.
     */
    static void ApplyCustomTheme();

    /**
     * @brief Affiche les contr�les pour �diter les couleurs du th�me de l'interface utilisateur.
     */
    static void EditTheme();
};
