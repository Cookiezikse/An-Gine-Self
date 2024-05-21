#pragma once


#include "Script.h"
#include "GameObject/GameObject.h"

/**
 * @brief Classe GameObjectScript.
 *
 * Cette classe représente un script attaché à un GameObject.
 * Elle hérite de la classe de base Script.
 */
//class ENGINEDLL_API GameObjectScript : public Script
class  GameObjectScript : public Script
{
public:
    /**
     * @brief Constructeur par défaut.
     */
    GameObjectScript();

    /**
     * @brief Destructeur par défaut.
     */
    ~GameObjectScript() = default;

protected:
    GameObject object;
};
