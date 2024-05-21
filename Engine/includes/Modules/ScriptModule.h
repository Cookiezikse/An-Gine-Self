#pragma once

#include <queue>
#include <string>
#include <vector>

#include "Module.h"
#include "Script/IScript.h"

/**
 * @brief Classe ScriptModule.
 *
 * Cette classe gère l'ajout, le démarrage et la mise à jour des scripts dans le moteur de jeu.
 */
class ScriptModule : public Module {
public:
    /**
     * @brief Constructeur par défaut.
     */
    ScriptModule() = default;

    /**
     * @brief Destructeur par défaut.
     */
    ~ScriptModule() = default;

    /**
     * @brief Ajoute un script au module.
     * @tparam T Le type de script à ajouter.
     * @tparam U La classe du script à ajouter.
     * @return True si le script a été ajouté avec succès, sinon false.
     */
    template <typename T, class U>
    bool AddScript()
    {
        IScript* script = static_cast<IScript*>(new U);

        script->Initialize();

        scriptsToStart.push(script);
        scripts.push_back(script);

        return true;
    }

#pragma region Event

    void Update() override
    {
        UpdateScripts();
    }


#pragma endregion

    /**
     * @brief Démarre tous les scripts en attente.
     * @return True si tous les scripts ont été démarrés avec succès, sinon false.
     */
    bool StartScripts();

    /**
     * @brief Obtient un script par son nom.
     * @param _name Le nom du script à obtenir.
     * @return Un pointeur vers le script s'il est trouvé, sinon nullptr.
     */
    IScript* GetIScriptByName(const std::string& _name) const;

    /**
     * @brief Met à jour tous les scripts.
     */
    void UpdateScripts();

    /**
     * @brief Arrête tous les scripts et libère les ressources.
     */
    void Stop();

private:
    std::vector<IScript*> scripts;
    std::queue<IScript*> scriptsToStart;
};
