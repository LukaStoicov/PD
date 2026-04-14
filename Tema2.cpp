#include <windows.h>
#include <setupapi.h>
#include <devguid.h>
#include <regstr.h>
#include <iostream>
#include <vector>
#include <string>

#pragma comment(lib, "setupapi.lib")

struct DeviceProp {
    DWORD id;
    std::wstring name;
};

std::vector<DeviceProp> allProperties = {
    {SPDRP_DEVICEDESC, L"Device Description"},
    {SPDRP_HARDWAREID, L"Hardware IDs"},
    {SPDRP_COMPATIBLEIDS, L"Compatible IDs"},
    {SPDRP_SERVICE, L"Service"},
    {SPDRP_CLASS, L"Class"},
    {SPDRP_CLASSGUID, L"Class GUID"},
    {SPDRP_DRIVER, L"Driver Key"},
    {SPDRP_CONFIGFLAGS, L"Config Flags"},
    {SPDRP_MFG, L"Manufacturer"},
    {SPDRP_FRIENDLYNAME, L"Friendly Name"},
    {SPDRP_LOCATION_INFORMATION, L"Location Information"},
    {SPDRP_PHYSICAL_DEVICE_OBJECT_NAME, L"PDO Name"},
    {SPDRP_CAPABILITIES, L"Capabilities"},
    {SPDRP_UI_NUMBER, L"UI Number"},
    {SPDRP_UPPERFILTERS, L"Upper Filters"},
    {SPDRP_LOWERFILTERS, L"Lower Filters"},
    {SPDRP_BUSNUMBER, L"Bus Number"},
    {SPDRP_ENUMERATOR_NAME, L"Enumerator Name"},
    {SPDRP_INSTALL_STATE, L"Install State"},
    {SPDRP_LOCATION_PATHS, L"Location Paths"},
    {SPDRP_BASE_CONTAINERID, L"Base Container ID"}
};

void FetchAndPrintProperty(HDEVINFO deviceInfoSet, PSP_DEVINFO_DATA deviceInfoData, const DeviceProp& prop)
{
    DWORD dataType;
    DWORD bufferSize = 0;

    SetupDiGetDeviceRegistryPropertyW(deviceInfoSet, deviceInfoData, prop.id,
        &dataType, NULL, 0, &bufferSize);

    if (bufferSize > 0) {
        std::vector<BYTE> buffer(bufferSize);
        if (SetupDiGetDeviceRegistryPropertyW(deviceInfoSet, deviceInfoData, prop.id,
            &dataType, buffer.data(), bufferSize, NULL))
        {
            std::wcout << prop.name << L": ";

            if (dataType == REG_SZ || dataType == REG_EXPAND_SZ) 
            {
                std::wcout << (wchar_t*)buffer.data() << std::endl;
            }
            else if (dataType == REG_MULTI_SZ)
            {
                wchar_t* ptr = (wchar_t*)buffer.data();
                while (*ptr)
                {
                    std::wcout << ptr << L" ";
                    ptr += wcslen(ptr) + 1;
                }
                std::wcout << std::endl;
            }
            else if (dataType == REG_DWORD)
            {
                std::wcout << *(DWORD*)buffer.data() << std::endl;
            }
            else
            {
                std::wcout << L"[Binary Data]" << std::endl;
            }
        }
    }
}

int main()
{
    HDEVINFO deviceInfoSet = SetupDiGetClassDevsW(NULL, NULL, NULL,
        DIGCF_ALLCLASSES | DIGCF_PRESENT);

    if (deviceInfoSet == INVALID_HANDLE_VALUE)
    {
        std::cerr << "Eroare la accesarea dispozitivelor." << std::endl;
        return 1;
    }

    SP_DEVINFO_DATA deviceInfoData;
    deviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

    for (DWORD i = 0; SetupDiEnumDeviceInfo(deviceInfoSet, i, &deviceInfoData); i++)
    {
        std::wcout << L"\n====================================================" << std::endl;
        std::wcout << L"DISPOZITIV INDEX: " << i << std::endl;
        std::wcout << L"\n====================================================" << std::endl;

        for (const auto& prop : allProperties)
        {
            FetchAndPrintProperty(deviceInfoSet, &deviceInfoData, prop);
        }
    }

    SetupDiDestroyDeviceInfoList(deviceInfoSet);
    return 0;
}