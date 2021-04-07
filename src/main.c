#include "main.h"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
int _tmain(int argc, _TCHAR *argv[]) {
    const _TCHAR *executionIntervalStr = _T("60000");
    const _TCHAR *targetExecutable = _T("ssh");
    int cmdLineCount = 0;
    const _TCHAR** cmdLine = NULL;
    BOOL cmdLinePending = FALSE;
    for (int i = 0; i < argc; ++i) {
        if (!_tcscmp(EXECUTION_INTERVAL_ARG, argv[i])) {
            if (cmdLinePending) {
                cmdLinePending = FALSE;
                cmdLineCount -= argc - i;
            }
            if (i + 1 >= argc) {
                break;
            }
            executionIntervalStr = argv[i + 1];
            continue;
        }
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

    _TCHAR *executionIntervalEnd = NULL;
    DWORD executionInterval = _tcstoul(executionIntervalStr, &executionIntervalEnd, 10);
    if (executionIntervalEnd - executionIntervalStr != _tcslen(executionIntervalStr)) {
        _tprintf(_T("Invalid execution interval: '%s'\n"), executionIntervalStr);
        return 1;
    }
    // _tprintf(_T("Execution interval (ms): %d\n"), executionInterval);

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
        /*
        startupInfo.dwFlags = STARTF_USESTDHANDLES;
        startupInfo.hStdError = GetStdHandle(STD_ERROR_HANDLE);
        startupInfo.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
         */
        ZeroMemory(&processInfo, sizeof(processInfo));

        ULONGLONG sshStartTime = GetTickCount64();
        if (!CreateProcess(NULL, cmdLineString, NULL, NULL, /*TRUE*/FALSE, CREATE_NO_WINDOW, NULL, NULL, &startupInfo,
                &processInfo)) {
            const DWORD errorCode = GetLastError();
            _TCHAR* errorDesc = NULL;
            FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                    NULL, errorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) &errorDesc, 0, NULL);
            _tprintf(_T("[%d] %s"), errorCode, errorDesc);
            if (LocalFree(errorDesc)) {
                _tprintf(_T("Memory leak!\n"));
            }
            Sleep(executionInterval);
        }

        // _tprintf(_T("Started\n"));
        WaitForSingleObject(processInfo.hProcess, INFINITE);
        // _tprintf(_T("Stopped\n"));
        ULONGLONG elapsedMillis = GetTickCount64() - sshStartTime;
        if (elapsedMillis < executionInterval) {
            Sleep(executionInterval - elapsedMillis);
        }

        CloseHandle(processInfo.hProcess);
        CloseHandle(processInfo.hThread);
    }
    // return 0;
}
#pragma clang diagnostic pop
