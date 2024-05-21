#pragma once

#include <iostream>
#include <string>
#include <windows.h>
#include <fstream>
#include <vector>
#include <string>

class FilesDirs
{
	public:

		/**
		* @brief Return the numbers of files in a directory.
		*/
		int FilesInDir(std::string _filepath);

		/**
		 * @brief Compte le nombre de répertoires dans un chemin de répertoire donné.
		 * @param directory_path Le chemin du répertoire.
		 * @return Le nombre de répertoires dans le chemin spécifié.
		 */
		int CountDirectories(const std::string& directory_path);

		std::vector<std::string> GetDirectoryNames(const std::string& directory_path);

		/**
		* @brief Return the list of the files in a directory.
		*/
		std::vector<std::wstring> GetFilesInDir(std::wstring _filepath);
		
		/**
		* @brief Convert the string to a wide string.
		*/
		std::wstring ConvertStringToWideString(const std::string& narrow_string);

		/**
		* @brief Convert the wide string to a string.
		*/
		std::string ConvertWideStringToString(const std::wstring& wide_string);

		/**
		 * @brief Crée le répertoire des données du moteur.
		 * @return Vrai si la création du répertoire est réussie, sinon faux.
		 */
		bool createEngineDataDirectory();

		/**
		 * @brief Crée un fichier AN dans le répertoire.
		 * @return Vrai si la création du fichier AN est réussie, sinon faux.
		 */
		bool createANFileInDirectory();

		/**
		 * @brief Lit le fichier AN et retourne son contenu.
		 * @return Le contenu du fichier AN sous forme de vecteur de wstrings.
		 */
		std::vector<std::wstring> readANFile();

		/**
		 * @brief Ouvre une boîte de dialogue pour sélectionner un fichier.
		 * @return Le chemin du fichier sélectionné.
		 */
		std::wstring openFileDialog();

		/**
		 * @brief Ajoute un chemin au fichier AN.
		 * @param newPath Le nouveau chemin à ajouter.
		 * @return Vrai si l'ajout du chemin est réussi, sinon faux.
		 */
		bool addPathToANFile(const std::wstring& _newPath);

		/**
		 * @brief Vérifie si un chemin existe dans le fichier AN.
		 * @param filePath Le chemin à vérifier.
		 * @return Vrai si le chemin existe dans le fichier AN, sinon faux.
		 */
		bool pathExistsInANFile(const std::wstring& _filePath);

		/**
		 * @brief Ouvre une boîte de dialogue pour sélectionner une image.
		 * @return Le chemin de l'image sélectionnée.
		 */
		std::wstring openImageFileDialog();

		/**
		 * @brief Extrait le nom de fichier et son extension à partir du chemin du fichier.
		 * @param filePath Le chemin du fichier.
		 * @param filename Référence pour stocker le nom de fichier extrait.
		 * @param extension Référence pour stocker l'extension de fichier extrait.
		 */
		void ExtractFilenameAndExtension(const std::string& _filePath, std::string& _filename, std::string& _extension);

		/**
		 * @brief Vérifie si une chaîne de caractères contient une sous-chaîne.
		 * @param main_string La chaîne principale.
		 * @param substring La sous-chaîne à rechercher.
		 * @return Vrai si la chaîne principale contient la sous-chaîne, sinon faux.
		 */
		bool ContainsSubstring(const std::string& _main_string, const std::string& _substring);

		bool ContainsSubstringIns(const std::string& _main_string, const std::string& _substring);

		/**
		 * @brief Vérifie si une extension de fichier est une image.
		 * @param ext L'extension de fichier à vérifier.
		 * @return Vrai si l'extension est une extension d'image valide, sinon faux.
		 */
		bool IsImageExtension(const std::string& _ext);
};

