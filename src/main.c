#include "main.h"

int _tmain(int argc, _TCHAR *argv[]) {
    const _TCHAR *targetExecutable = _T("ssh");
    int cmdLineCount = 0;
    const _TCHAR** cmdLine = NULL;
    BOOL cmdLinePending = FALSE;
    for (int i = 0; i < argc; ++i) {
        if (!_tcscmp(EXECUTABLE_INDICATOR_ARG, argv[i])) {
            if (cmdLinePending) {
                cmdLinePending = FALSE;
                cmdLineCount -= argc - i;
            }
            if (i + 1 >= argc) {
                break;
            }
            targetExecutable = argv[i + 1];
            continue;
        }
        if (!_tcscmp(CMDLINE_INDICATOR_ARG, argv[i])) {
            if (i + 1 >= argc) {
                break;
            }
            cmdLine = argv + i + 1;
            cmdLineCount = argc - i - 1;
            cmdLinePending = TRUE;
            continue;
        }
    }

    size_t cmdLineLength = 3 + _tcslen(targetExecutable);
    for (int i = 0; i < cmdLineCount; ++i) {
        cmdLineLength += 3 + _tcslen(cmdLine[i]);
    }

    _TCHAR* cmdLineString = malloc(cmdLineLength * sizeof(_TCHAR));
    cmdLineString[0] = 0;
    cmdLineString[cmdLineLength - 1] = 0;
    _tcscat_s(cmdLineString, cmdLineLength, _T("\""));
    _tcscat_s(cmdLineString, cmdLineLength, targetExecutable);
    _tcscat_s(cmdLineString, cmdLineLength, _T("\""));
    for (int i = 0; i < cmdLineCount; ++i) {
        _tcscat_s(cmdLineString, cmdLineLength, _T(" \""));
        _tcscat_s(cmdLineString, cmdLineLength, cmdLine[i]);
        _tcscat_s(cmdLineString, cmdLineLength, _T("\""));
    }
    _tprintf(_T("%s\n"), cmdLineString);

    STARTUPINFO startupInfo;
    PROCESS_INFORMATION processInfo;
    while (TRUE) {
        ZeroMemory(&startupInfo, sizeof(startupInfo));
        startupInfo.cb = sizeof(startupInfo);
        ZeroMemory(&processInfo, sizeof(processInfo));

        if (!CreateProcess(NULL, cmdLineString, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &startupInfo,
                &processInfo)) {
            const DWORD errorCode = GetLastError();
            _TCHAR* errorDesc = NULL;
            FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                    NULL, errorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) &errorDesc, 0, NULL);
            _tprintf(_T("[%d] %s"), errorCode, errorDesc);
            Sleep(1000);
        }

        WaitForSingleObject(processInfo.hProcess, INFINITE);

        CloseHandle(processInfo.hProcess);
        CloseHandle(processInfo.hThread);
    }
    return 0;
}
