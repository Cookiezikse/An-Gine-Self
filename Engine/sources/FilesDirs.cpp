#include "FilesDirs.h"
#include <unordered_set>
#include <shlobj_core.h>
#include <commdlg.h>
#include <algorithm>

#pragma comment(lib, "Comdlg32.lib")

int FilesDirs::FilesInDir(std::string _filepath) 
{
    std::string directory_path = _filepath + "\\*";
    int file_count = 0;
    WIN32_FIND_DATAA file_data;
    HANDLE hFind = FindFirstFileA(directory_path.c_str(), &file_data);

    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (!(file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                ++file_count;
            }
        } while (FindNextFileA(hFind, &file_data) != 0);
        FindClose(hFind);
        //std::cout << "Number of files in directory: " << file_count << std::endl;
        return file_count;
    }
    else {
        //std::cerr << "Failed to open directory." << std::endl;
        return -1;
    }

    return 0;
}

std::vector<std::wstring> FilesDirs::GetFilesInDir(std::wstring _filepath)
{
    std::vector<std::wstring> file_names;
    std::wstring directory_path = _filepath + L"\\*"; 
    WIN32_FIND_DATAW file_data;
    HANDLE hFind = FindFirstFileW(directory_path.c_str(), &file_data);

    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (!(file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) { 
                file_names.push_back(file_data.cFileName);
            }
        } while (FindNextFileW(hFind, &file_data) != 0);
        FindClose(hFind);
    }
    else {
        DWORD error = GetLastError();
    }

    return file_names;
}

int FilesDirs::CountDirectories(const std::string& _directory_path) {
    int directory_count = 0;
    WIN32_FIND_DATAA find_data;
    HANDLE find_handle = FindFirstFileA((_directory_path + "\\*").c_str(), &find_data);
    if (find_handle != INVALID_HANDLE_VALUE) {
        do {
            if (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                if (strcmp(find_data.cFileName, ".") != 0 && strcmp(find_data.cFileName, "..") != 0) {
                    directory_count++;
                }
            }
        } while (FindNextFileA(find_handle, &find_data) != 0);
        FindClose(find_handle);
    }
    return directory_count;
}

std::vector<std::string> FilesDirs::GetDirectoryNames(const std::string& _directory_path) {
    std::vector<std::string> directory_names;
    WIN32_FIND_DATAA find_data;
    HANDLE find_handle = FindFirstFileA((_directory_path + "\\*").c_str(), &find_data);
    if (find_handle != INVALID_HANDLE_VALUE) {
        do {
            if (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                if (strcmp(find_data.cFileName, ".") != 0 && strcmp(find_data.cFileName, "..") != 0) {
                    directory_names.push_back(find_data.cFileName);
                }
            }
        } while (FindNextFileA(find_handle, &find_data) != 0);
        FindClose(find_handle);
    }
    return directory_names;
}

std::wstring FilesDirs::ConvertStringToWideString(const std::string& _narrow_string) {
    int wide_string_length = MultiByteToWideChar(CP_UTF8, 0, _narrow_string.c_str(), -1, nullptr, 0);
    wchar_t* wide_string_buffer = new wchar_t[wide_string_length];
    MultiByteToWideChar(CP_UTF8, 0, _narrow_string.c_str(), -1, wide_string_buffer, wide_string_length);
    std::wstring wide_string(wide_string_buffer);
    delete[] wide_string_buffer;

    return wide_string;
}

std::string FilesDirs::ConvertWideStringToString(const std::wstring& _wide_string) {
    int narrow_string_length = WideCharToMultiByte(CP_UTF8, 0, _wide_string.c_str(), -1, nullptr, 0, nullptr, nullptr);
    char* narrow_string_buffer = new char[narrow_string_length];
    WideCharToMultiByte(CP_UTF8, 0, _wide_string.c_str(), -1, narrow_string_buffer, narrow_string_length, nullptr, nullptr);
    std::string narrow_string(narrow_string_buffer);
    delete[] narrow_string_buffer;

    return narrow_string;
}

void FilesDirs::ExtractFilenameAndExtension(const std::string& _filePath, std::string& _filename, std::string& _extension) {
    // Find the position of the last '/' or '\' character in the path
    size_t lastSlashPos = _filePath.find_last_of("/\\");

    // Extract the filename from the path
    _filename = (lastSlashPos != std::string::npos) ? _filePath.substr(lastSlashPos + 1) : _filePath;

    // Find the position of the last '.' character in the filename
    size_t lastDotPos = _filename.find_last_of('.');

    // Extract the extension from the filename
    _extension = (lastDotPos != std::string::npos) ? _filename.substr(lastDotPos + 1) : "";
}

bool FilesDirs::ContainsSubstring(const std::string& _main_string, const std::string& _substring) {
    return _main_string.find(_substring) != std::string::npos;
}

bool FilesDirs::ContainsSubstringIns(const std::string& _main_string, const std::string& _substring) {
    auto it = std::search(
        _main_string.begin(), _main_string.end(),
        _substring.begin(), _substring.end(),
        [](char ch1, char ch2) { return std::tolower(ch1) == std::tolower(ch2); }
    );
    return it != _main_string.end();
}

bool FilesDirs::IsImageExtension(const std::string& _ext) {
    static const std::unordered_set<std::string> imageExtensions = { "png", "jpg", "gif", "tga", "bmp", "psd", "hdr", "pic" };
    return imageExtensions.count(_ext) > 0;
}

bool FilesDirs::createEngineDataDirectory() {
    // Obtenir le chemin vers le dossier AppData local
    wchar_t* appDataPath = nullptr;
    if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, NULL, &appDataPath))) {
        std::wstring engineDataDir = std::wstring(appDataPath) + L"\\An-Gine";
        CoTaskMemFree(appDataPath); // Libérer la mémoire allouée par SHGetKnownFolderPath

        // Vérifier si le dossier existe déjà
        DWORD attributes = GetFileAttributesW(engineDataDir.c_str());
        if (attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_DIRECTORY)) {
            //std::wcout << L"Le dossier existe déjà." << std::endl;
        }
        else {
            // Créer le dossier
            if (!CreateDirectoryW(engineDataDir.c_str(), NULL)) {
                std::wcerr << L"Échec de la création du dossier." << std::endl;
                return false;
            }
            else {
                //std::wcout << L"Dossier créé avec succès." << std::endl;
            }
        }
    }
    else {
        std::wcerr << L"Impossible d'obtenir le chemin vers le dossier AppData local." << std::endl;
        return false;
    }

    return true;
}


bool FilesDirs::createANFileInDirectory() {
    wchar_t* appDataPath = nullptr;
    if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, NULL, &appDataPath))) {
        std::wstring engineDataDir = std::wstring(appDataPath) + L"\\An-Gine";
        CoTaskMemFree(appDataPath);

        DWORD attributes = GetFileAttributesW(engineDataDir.c_str());
        if (attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_DIRECTORY)) {
            std::wstring filePath = engineDataDir + L"\\projects.an";

            std::ifstream file(filePath);
            if (file.good()) {
                //std::wcout << L"Le fichier .an existe déjà." << std::endl;
                return true;
            }

            std::ofstream outFile(filePath);
            if (!outFile.is_open()) {
                std::wcerr << L"Échec de la création du fichier .an." << std::endl;
                return false;
            }
            else {
                //std::wcout << L"Fichier .an créé avec succès." << std::endl;
                return true;
            }
        }
        else {
            std::wcerr << L"Le dossier n'existe pas." << std::endl;
            return false;
        }
    }
    else {
        std::wcerr << L"Impossible d'obtenir le chemin vers le dossier AppData local." << std::endl;
        return false;
    }
}

std::vector<std::wstring> FilesDirs::readANFile() {
    std::vector<std::wstring> filePaths;

    wchar_t* appDataPath = nullptr;
    if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, NULL, &appDataPath))) {
        std::wstring engineDataDir = std::wstring(appDataPath) + L"\\An-Gine";
        CoTaskMemFree(appDataPath);

        std::wstring filePath = engineDataDir + L"\\projects.an";

        std::wifstream file(filePath);
        if (file.is_open()) {
            std::wstring line;
            while (std::getline(file, line)) {
                filePaths.push_back(line);
            }
            file.close();
        }
        else {
            std::cerr << "Failed to open the .an file." << std::endl;
        }
    }
    else {
        std::cerr << "Failed to get the path to the local AppData folder." << std::endl;
    }

    return filePaths;
}

bool FilesDirs::pathExistsInANFile(const std::wstring& _filePath) {
    wchar_t* appDataPath = nullptr;
    if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, NULL, &appDataPath))) {
        std::wstring engineDataDir = std::wstring(appDataPath) + L"\\An-Gine";
        CoTaskMemFree(appDataPath);

        std::wstring anFilePath = engineDataDir + L"\\projects.an";

        std::wifstream file(anFilePath);
        if (file.is_open()) {
            std::wstring line;
            while (std::getline(file, line)) {
                if (line == _filePath) {
                    file.close();
                    return true;
                }
            }
            file.close();
        }
        else {
            std::wcerr << L"Failed to open the .an file for reading." << std::endl;
            return true; 
        }
    }
    else {
        std::wcerr << L"Failed to get the path to the local AppData folder." << std::endl;
        return true; 
    }

    return false; 
}

bool FilesDirs::addPathToANFile(const std::wstring& _newPath) {
    if (pathExistsInANFile(_newPath)) {
        //std::wcout << L"The path already exists in the .an file." << std::endl;
        return false;
    }

    wchar_t* appDataPath = nullptr;
    if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, NULL, &appDataPath))) {
        std::wstring engineDataDir = std::wstring(appDataPath) + L"\\An-Gine";
        CoTaskMemFree(appDataPath);

        std::wstring anFilePath = engineDataDir + L"\\projects.an";

        std::wofstream file(anFilePath, std::ios_base::app);
        if (file.is_open()) {
            file << _newPath << std::endl;
            file.close();
            //std::wcout << L"Path added successfully." << std::endl;
            return true;
        }
        else {
            std::wcerr << L"Failed to open the .an file for writing." << std::endl;
            return false;
        }
    }
    else {
        std::wcerr << L"Failed to get the path to the local AppData folder." << std::endl;
        return false;
    }
}

std::wstring FilesDirs::openFileDialog() {
    OPENFILENAME ofn;
    wchar_t szFile[260];
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = L"JSON Files\0*.json\0All Files\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrInitialDir = NULL;
    ofn.lpstrTitle = L"Select a JSON file";
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

    if (GetOpenFileName(&ofn) == TRUE) {
        return ofn.lpstrFile; 
    }
    else {
        return L""; 
    }
}

std::wstring FilesDirs::openImageFileDialog() {
    OPENFILENAME ofn;
    wchar_t szFile[260];
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = L"Image Files\0*.png;*.jpg;*.gif;*.tga;*.bmp;*.psd;*.hdr;*.pic\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrInitialDir = NULL;
    ofn.lpstrTitle = L"Select an Image file";
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

    if (GetOpenFileName(&ofn) == TRUE) {
        return ofn.lpstrFile;
    }
    else {
        return L"";
    }
}