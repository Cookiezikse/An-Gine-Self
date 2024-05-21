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
		 * @brief Compte le nombre de r�pertoires dans un chemin de r�pertoire donn�.
		 * @param directory_path Le chemin du r�pertoire.
		 * @return Le nombre de r�pertoires dans le chemin sp�cifi�.
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
		 * @brief Cr�e le r�pertoire des donn�es du moteur.
		 * @return Vrai si la cr�ation du r�pertoire est r�ussie, sinon faux.
		 */
		bool createEngineDataDirectory();

		/**
		 * @brief Cr�e un fichier AN dans le r�pertoire.
		 * @return Vrai si la cr�ation du fichier AN est r�ussie, sinon faux.
		 */
		bool createANFileInDirectory();

		/**
		 * @brief Lit le fichier AN et retourne son contenu.
		 * @return Le contenu du fichier AN sous forme de vecteur de wstrings.
		 */
		std::vector<std::wstring> readANFile();

		/**
		 * @brief Ouvre une bo�te de dialogue pour s�lectionner un fichier.
		 * @return Le chemin du fichier s�lectionn�.
		 */
		std::wstring openFileDialog();

		/**
		 * @brief Ajoute un chemin au fichier AN.
		 * @param newPath Le nouveau chemin � ajouter.
		 * @return Vrai si l'ajout du chemin est r�ussi, sinon faux.
		 */
		bool addPathToANFile(const std::wstring& _newPath);

		/**
		 * @brief V�rifie si un chemin existe dans le fichier AN.
		 * @param filePath Le chemin � v�rifier.
		 * @return Vrai si le chemin existe dans le fichier AN, sinon faux.
		 */
		bool pathExistsInANFile(const std::wstring& _filePath);

		/**
		 * @brief Ouvre une bo�te de dialogue pour s�lectionner une image.
		 * @return Le chemin de l'image s�lectionn�e.
		 */
		std::wstring openImageFileDialog();

		/**
		 * @brief Extrait le nom de fichier et son extension � partir du chemin du fichier.
		 * @param filePath Le chemin du fichier.
		 * @param filename R�f�rence pour stocker le nom de fichier extrait.
		 * @param extension R�f�rence pour stocker l'extension de fichier extrait.
		 */
		void ExtractFilenameAndExtension(const std::string& _filePath, std::string& _filename, std::string& _extension);

		/**
		 * @brief V�rifie si une cha�ne de caract�res contient une sous-cha�ne.
		 * @param main_string La cha�ne principale.
		 * @param substring La sous-cha�ne � rechercher.
		 * @return Vrai si la cha�ne principale contient la sous-cha�ne, sinon faux.
		 */
		bool ContainsSubstring(const std::string& _main_string, const std::string& _substring);

		bool ContainsSubstringIns(const std::string& _main_string, const std::string& _substring);

		/**
		 * @brief V�rifie si une extension de fichier est une image.
		 * @param ext L'extension de fichier � v�rifier.
		 * @return Vrai si l'extension est une extension d'image valide, sinon faux.
		 */
		bool IsImageExtension(const std::string& _ext);
};

