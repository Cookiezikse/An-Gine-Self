#pragma once

#include <string> 

/**
 * @brief Macro pour déclarer le nom et le type par défaut d'un script.
 *
 * Cette macro permet de définir le nom et le type par défaut d'un script.
 * Elle doit être utilisée dans la définition d'une classe de script dérivée d'IScript.
 * @param name Le nom par défaut du script.
 * @param subtype Le type par défaut du script.
 */
#define DECLARE_SCRIPT(name, subtype) \
		void SetDefaultScriptName() override { scriptName = name; } \
		void SetDefaultScriptType() override { id = subtype; }

#ifdef ENGINECORE_EXPORTS
#define ENGINEDLL_API __declspec(dllexport)
#else
#define ENGINEDLL_API __declspec(dllimport)
#endif

 /**
  * @brief Interface de base pour les scripts.
  *
  * Cette classe définit une interface de base pour tous les scripts utilisés dans le moteur de jeu.
  * Les scripts peuvent être initialisés, démarrés, mis à jour et détruits.
  * Ils peuvent également avoir un nom et un type par défaut.
  */
//class ENGINEDLL_API IScript
class  IScript
{
public:
    /**
     * @brief Destructeur virtuel par défaut.
     */
    virtual ~IScript() = default;

    /**
     * @brief Initialise le script.
     */
    virtual void Initialize() {}

    /**
     * @brief Démarre le script.
     */
    virtual void Start() {}

    /**
     * @brief Met à jour le script.
     */
    virtual void Update() {}

    /**
     * @brief Met à jour le script avec un delta de temps spécifié.
     * @param _delta Le delta de temps depuis la dernière mise à jour.
     */
    virtual void Update(const float _delta) {}

    /**
     * @brief Destructeur le script.
     */
    virtual void Destruct() {}

    /**
     * @brief Définit le nom par défaut du script.
     * @param _newName Le nouveau nom du script.
     */
    void SetScriptName(const std::string& _newName = "Unnamed script") { scriptName = _newName; }

    /**
     * @brief Obtient le nom du script.
     * @return Le nom du script.
     */
    std::string GetScriptName() const { return scriptName; }

protected:
    std::string scriptName = "Unnamed Script";
    int id;
};

