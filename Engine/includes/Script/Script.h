#pragma once

#include "IScript.h"

#include "Modules/InputModule.h"
#include "Modules/rhi.h"
#include "Modules/TimeModule.h"
#include "Modules/WindowModule.h"

/**
 * @brief Classe Script.
 *
 * Cette classe représente un script de base utilisé dans le moteur de jeu.
 * Elle fournit des méthodes pour accéder aux différents modules du moteur, tels que le module de temps, le module de fenêtre, le module d'interface de rendu et le module d'entrée.
 * Elle hérite de la classe de base IScript.
 */
class Script : public IScript
{
public:
    /**
     * @brief Constructeur par défaut.
     */
    Script();

    /**
     * @brief Destructeur par défaut.
     */
    ~Script() = default;

    /**
     * @brief Obtient une référence vers le module de temps.
     * @return Un pointeur vers le module de temps.
     */
    TimeModule* GetTimeModule() const;

    /**
     * @brief Obtient une référence vers le module de fenêtre.
     * @return Un pointeur vers le module de fenêtre.
     */
    WindowModule* GetWindowModule() const;

    /**
     * @brief Obtient une référence vers le module d'interface de rendu.
     * @return Un pointeur vers le module d'interface de rendu.
     */
    RHIModule* GetRenderInterfaceModule() const;

    /**
     * @brief Obtient une référence vers le module d'entrée.
     * @return Un pointeur vers le module d'entrée.
     */
    InputModule* GetInputModule() const;

private:

    TimeModule* timeModuleReference = nullptr; /**< Référence au module de gestion du temps. */
    WindowModule* windowModuleReference = nullptr; /**< Référence au module de fenêtre. */
    RHIModule* renderInterfaceModuleReference = nullptr; /**< Référence au module d'interface de rendu. */
    InputModule* inputModuleReference = nullptr; /**< Référence au module d'entrée. */
};
