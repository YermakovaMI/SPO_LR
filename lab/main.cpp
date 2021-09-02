#define WINVER 0x0A00
#define FILESYSNAMEBUFSIZE MAX_PATH
#define INFO_BUFFER_SIZE 32767
#define MAX_KEY_LENGTH 255
#define INFO_BUFFER_SIZE 32767
#include <iostream>
#include <windows.h>
#include <locale>

using namespace std;

double PCFreq = 0.0;
__int64 CounterStart = 0;

void StartCounter()
{
    LARGE_INTEGER li;
    if (!QueryPerformanceFrequency(&li))
        cout << "Function QueryPerformanceFrequency() failed!" << endl;

    PCFreq = double(li.QuadPart);

    printf("\n2.1.  CPU frequency: %u  Hz\n", li);
    QueryPerformanceCounter(&li);
    CounterStart = li.QuadPart;
}

// 2
double GetCounter()
{
    LARGE_INTEGER li;
    QueryPerformanceCounter(&li);
    return double ((li.QuadPart - CounterStart)* 1000000) / PCFreq;
}

int main()
{
    setlocale(LC_ALL, "Rus");

    OSVERSIONINFO os_version;
    GetVersionEx(&os_version);

    // 1.1
    cout << "Major Version: " << os_version.dwMajorVersion
         << " Minor Version: " << os_version.dwMinorVersion
         << " Build Number: " << os_version.dwBuildNumber << endl;

    // 1.2
    TCHAR infoBuffer[INFO_BUFFER_SIZE];
    DWORD bufferCharCount = INFO_BUFFER_SIZE;

    GetSystemDirectory(infoBuffer, INFO_BUFFER_SIZE);

    cout << "System Directory: " << infoBuffer << endl;

    // 1.3
    bufferCharCount = INFO_BUFFER_SIZE;
    GetComputerName(infoBuffer, &bufferCharCount);

    cout << "Computer Name: " << infoBuffer << endl;

    bufferCharCount = INFO_BUFFER_SIZE;
    GetUserName(infoBuffer, &bufferCharCount);

    cout << "User Name: " << infoBuffer << endl;

    // 1.4
    char buffer[MAX_PATH + 1];
    DWORD buffer_size = MAX_PATH;
    char names[MAX_PATH + 1];
    CHAR volume_name[MAX_PATH];
    __int64 total, available, free;

    HANDLE search_handle = FindFirstVolume(volume_name, ARRAYSIZE(volume_name));
    cout << "Volume Name: " << volume_name << endl;

    do {
        cout << names << endl;
        GetVolumePathNamesForVolumeName(names, buffer, buffer_size, &buffer_size);
        char* path = buffer;
        cout << path << endl;

        if (GetDiskFreeSpaceEx(buffer, (PULARGE_INTEGER)&available, (PULARGE_INTEGER)&total, (PULARGE_INTEGER)&free) == 0) {
            cout << "No info" << endl;
        }
        else {
            cout << "Total size (bytes): " << total << endl;
            cout << "Free space (bytes): " << available << endl;
        }
    }
    while (FindNextVolume(search_handle, names, ARRAYSIZE(volume_name)));
    FindVolumeClose(search_handle);

    // 1.5
    DWORD dwSize;
    TCHAR szName[MAX_KEY_LENGTH];
    HKEY hKey;
    DWORD dwIndex = 0;
    DWORD retCode;
    DWORD BufferSize = 8192;
    PPERF_DATA_BLOCK PerfData = (PPERF_DATA_BLOCK)malloc(BufferSize);
    DWORD cbData = BufferSize;

    if (RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run",
        0, KEY_ALL_ACCESS, &hKey) == !ERROR_SUCCESS)
    {
        cout << "Function RegOpenKeyEx() failed!" << endl;
    }
    else cout << "\n1.5.  Startup commands:" << endl;

    while (1) {
        dwSize = sizeof(szName);
        retCode = RegEnumValue(hKey, dwIndex, szName, &dwSize, NULL, NULL, NULL, NULL);
        if (retCode == ERROR_SUCCESS)
        {
            RegQueryValueEx(hKey, szName, NULL, NULL, (LPBYTE)PerfData, &cbData);
            printf("      %d: %s:  %s\n", dwIndex + 1, szName, PerfData);
            dwIndex++;
        }
        else break;
    }

    RegCloseKey(hKey);

    StartCounter();
    cout << "2.2.  CPU clock count: " << GetCounter()  << "  us \n";


    return 0;
}
