#include <windows.h>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

int main() {
    wstring subkeyPath;
    HKEY hKey;

    cout << "========================================" << endl;
    cout << "   EXPLORATOR REGISTRY (HKCU)           " << endl;
    cout << "========================================" << endl;
    cout << "Introduceti subcheia (ex: Software\\Microsoft\\Windows\\CurrentVersion\\Run): " << endl;

    getline(wcin, subkeyPath);

    LONG openStatus = RegOpenKeyExW(HKEY_CURRENT_USER, subkeyPath.c_str(), 0, KEY_READ, &hKey);

    if (openStatus != ERROR_SUCCESS) {
        wcerr << L"\n[EROARE] Nu am putut deschide subcheia. Verificati daca exista." << endl;
        return 1;
    }

    cout << "\nSe listeaza valorile din subcheia specificata...\n" << endl;

    DWORD index = 0;
    WCHAR valueName[16383];
    BYTE dataBuffer[4096];

    while (true) {
        DWORD nameSize = 16383;
        DWORD dataSize = sizeof(dataBuffer);
        DWORD type = 0;

        LONG result = RegEnumValueW(
            hKey,
            index,
            valueName,
            &nameSize,
            NULL,
            &type,
            dataBuffer,
            &dataSize
        );

        if (result == ERROR_NO_MORE_ITEMS) {
            break;
        }

        if (result == ERROR_SUCCESS) {
            wcout << L"[" << index << L"] Nume: " << valueName;

            if (type == REG_SZ) {
                wcout << L" | Tip: TEXT | Date: " << (wchar_t*)dataBuffer << endl;
            }
            else if (type == REG_DWORD) {
                DWORD num = *((DWORD*)dataBuffer);
                wcout << L" | Tip: DWORD | Date: " << num << endl;
            }
            else {
                wcout << L" | Tip: Altul (Binar/Multi-String) | Date: [Complex]" << endl;
            }
        }

        index++;
    }

    RegCloseKey(hKey);

    cout << "\nEnumerare finalizata. Gasite: " << index << " valori." << endl;

    return 0;
}