#ifndef PROCESS_HPP
#define PROCESS_HPP

#include <windows.h>
#include <iostream>

class Process {
    bool finished;
    int terminate_status;
    public:
    Process() {}
    Process(const std::wstring &command) : finished{false} {
        STARTUPINFOW si;
        PROCESS_INFORMATION pi;

        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));

        std::vector<wchar_t> cmd(command.begin(), command.end());
        cmd.push_back(L'\0');
        terminate_status = CreateProcessW(
            NULL,
            cmd.data(),
            NULL,
            NULL,
            FALSE,
            0,
            NULL,
            NULL,
            &si,
            &pi
        );

        if (!terminate_status) {
            std::cout << "Failed to create process. Error code: " << GetLastError() << std::endl;
            terminate_status = 1;
        } else {
            WaitForSingleObject(pi.hProcess, INFINITE);

            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);

            terminate_status = 0;
            finished = true;
        }
    }
    bool isFinished() { return finished; }
    int getTerminateStatus() { return terminate_status; }
};

#endif