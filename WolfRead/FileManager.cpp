
#include <Windows.h>
#include <ShObjIdl.h>

#include <atlbase.h>
#include <string>

#include <fstream>

#include "FileManager.h"

FileManager::FileManager() {
    succeed = false;
    openDialog();
    if (succeed) {
        file.open(filePath, std::ios::in | std::ios::binary);
        if (!file.is_open()) {
            succeed = false;
        }
    }
}

FileManager::~FileManager() {
    file.close();
}

void FileManager::openDialog() {
    succeed = false;
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (SUCCEEDED(hr)) {
        IFileOpenDialog *pFileOpen;

        // Create the FileOpenDialog object
        hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
            IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));
        if (SUCCEEDED(hr)) {
            LPCWSTR title = L"Select file";
            pFileOpen->SetTitle(title);

            // Show the Open dialog box
            hr = pFileOpen->Show(NULL);

            // Get the file name from the dialog box
            if (SUCCEEDED(hr)) {
                IShellItem *pItem;
                hr = pFileOpen->GetResult(&pItem);
                if (SUCCEEDED(hr)) {
                    //PWSTR pszFilePath;
                    LPWSTR pszFilePath;
                    hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
                    // Display the file name to the user
                    if (SUCCEEDED(hr)) {
                        filePath = pszFilePath;

                        // Convert wchar to string
                        char *szTo = new char[filePath.length() + 1];
                        szTo[filePath.size()] = '\0';
                        WideCharToMultiByte(CP_ACP, NULL, filePath.c_str(), -1, szTo, (int)filePath.length(), NULL, NULL);
                        filename = szTo;
                        delete szTo;

                        filename = filename.substr(filename.rfind("\\") + 1);

                        CoTaskMemFree(pszFilePath);
                        succeed = true;
                    }
                    pItem->Release();
                }
            }
            pFileOpen->Release();
        }
        CoUninitialize();
    }
}
