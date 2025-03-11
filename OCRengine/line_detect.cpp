#include <vector>
#include <iostream>
#include <sstream>

#include "util_func.h"

double ruby_cutoff = 0.5;
double rubybase_cutoff = 0.5;
double space_cutoff = 0.5;
double emphasis_cutoff = 0.5;
float line_valueth = 0.4;
float sep_valueth = 0.1;
float sep_valueth2 = 0.15;
double allowwidth_next = 1.0;

#include <Windows.h>
#include <strsafe.h>

#define BUFSIZE 4096 

HANDLE g_hChildStd_IN_Rd = NULL;
HANDLE g_hChildStd_IN_Wr = NULL;
HANDLE g_hChildStd_OUT_Rd = NULL;
HANDLE g_hChildStd_OUT_Wr = NULL;
HANDLE g_hChildStd_ERR_Rd = NULL;
HANDLE g_hChildStd_ERR_Wr = NULL;

void ErrorExit(PCTSTR lpszFunction)

// Format a readable error message, display a message box, 
// and exit from the application.
{
    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD dw = GetLastError();

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf,
        0, NULL);

    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
        (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));
    StringCchPrintf((LPTSTR)lpDisplayBuf,
        LocalSize(lpDisplayBuf) / sizeof(TCHAR),
        TEXT("%s failed with error %d: %s"),
        lpszFunction, dw, lpMsgBuf);
    MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);
    ExitProcess(1);
}

void CreateChildProcess()
{
    // Create a child process that uses the previously created pipes for STDIN and STDOUT.

    std::stringstream ss;
    ss << "textline_detect.exe";
    ss << " --ruby_cutoff=" << ruby_cutoff;
    ss << " --rubybase_cutoff=" << rubybase_cutoff;
    ss << " --space_cutoff=" << space_cutoff;
    ss << " --emphasis_cutoff=" << emphasis_cutoff;
    ss << " --line_valueth=" << line_valueth;
    ss << " --sep_valueth=" << sep_valueth;
    ss << " --sep_valueth2=" << sep_valueth2;
    ss << " --allowwidth_next_block=" << allowwidth_next;

    std::wstring cmd = ToUnicodeStr(ss.str());
    std::vector<wchar_t> szCmdline(cmd.size() + 1);
    wcsncat_s(szCmdline.data(), szCmdline.size(), cmd.c_str(), cmd.size());
    PROCESS_INFORMATION piProcInfo;
    STARTUPINFO siStartInfo;
    BOOL bSuccess = FALSE;

    // Set up members of the PROCESS_INFORMATION structure. 

    ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));

    // Set up members of the STARTUPINFO structure. 
    // This structure specifies the STDIN and STDOUT handles for redirection.

    ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
    siStartInfo.cb = sizeof(STARTUPINFO);
    siStartInfo.hStdError = g_hChildStd_ERR_Wr;
    siStartInfo.hStdOutput = g_hChildStd_OUT_Wr;
    siStartInfo.hStdInput = g_hChildStd_IN_Rd;
    siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

    // Create the child process. 

    bSuccess = CreateProcessW(NULL,
        szCmdline.data(), // command line 
        NULL,          // process security attributes 
        NULL,          // primary thread security attributes 
        TRUE,          // handles are inherited 
        0,             // creation flags 
        NULL,          // use parent's environment 
        NULL,          // use parent's current directory 
        &siStartInfo,  // STARTUPINFO pointer 
        &piProcInfo);  // receives PROCESS_INFORMATION 

    // If an error occurs, exit the application. 
    if (!bSuccess)
        ErrorExit(TEXT("CreateProcess"));
    else
    {
        // Close handles to the child process and its primary thread.
        // Some applications might keep these handles to monitor the status
        // of the child process, for example. 

        CloseHandle(piProcInfo.hProcess);
        CloseHandle(piProcInfo.hThread);

        // Close handles to the stdin and stdout pipes no longer needed by the child process.
        // If they are not explicitly closed, there is no way to recognize that the child process has ended.

        CloseHandle(g_hChildStd_OUT_Wr);
        CloseHandle(g_hChildStd_ERR_Wr);
        CloseHandle(g_hChildStd_IN_Rd);
    }
}

std::vector<int32_t> line_detect(int im_width, int im_height, const std::vector<float>& loc, const std::vector<float> &lineimage, const std::vector<float>& sepimage)
{
    int boxcount = loc.size() / 9;

    if (boxcount <= 0) {
        return std::vector<int32_t>();
    }

    std::cerr << boxcount << " boxes" << std::endl;

    std::cerr << im_width << " x " << im_height << std::endl;

    SECURITY_ATTRIBUTES saAttr;
    // Set the bInheritHandle flag so pipe handles are inherited. 
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;

    // Create a pipe for the child process's STDOUT. 

    if (!CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &saAttr, 0)) {
        ErrorExit(TEXT("StdoutRd CreatePipe"));
    }

    // Ensure the read handle to the pipe for STDOUT is not inherited.

    if (!SetHandleInformation(g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0)) {
        ErrorExit(TEXT("Stdout SetHandleInformation"));
    }

    // Create a pipe for the child process's STDERR. 

    if (!CreatePipe(&g_hChildStd_ERR_Rd, &g_hChildStd_ERR_Wr, &saAttr, 0)) {
        ErrorExit(TEXT("StderrRd CreatePipe"));
    }

    // Ensure the read handle to the pipe for STDOUT is not inherited.

    if (!SetHandleInformation(g_hChildStd_ERR_Rd, HANDLE_FLAG_INHERIT, 0)) {
        ErrorExit(TEXT("Stderr SetHandleInformation"));
    }

    // Create a pipe for the child process's STDIN. 

    if (!CreatePipe(&g_hChildStd_IN_Rd, &g_hChildStd_IN_Wr, &saAttr, 0)) {
        ErrorExit(TEXT("Stdin CreatePipe"));
    }

    // Ensure the write handle to the pipe for STDIN is not inherited. 

    if (!SetHandleInformation(g_hChildStd_IN_Wr, HANDLE_FLAG_INHERIT, 0)) {
        ErrorExit(TEXT("Stdin SetHandleInformation"));
    }

    // Create the child process. 

    CreateChildProcess();

    DWORD dwRead, dwWritten;
    INT32 intbuf = 0;
    if (!WriteFile(g_hChildStd_IN_Wr, &intbuf, sizeof(intbuf), &dwWritten, NULL)) {
        ErrorExit(TEXT("write stdin runmode"));
    }
    intbuf = im_width;
    if (!WriteFile(g_hChildStd_IN_Wr, &intbuf, sizeof(intbuf), &dwWritten, NULL)) {
        ErrorExit(TEXT("write stdin width"));
    }
    intbuf = im_height;
    if (!WriteFile(g_hChildStd_IN_Wr, &intbuf, sizeof(intbuf), &dwWritten, NULL)) {
        ErrorExit(TEXT("write stdin height"));
    }

    if (!WriteFile(g_hChildStd_IN_Wr, lineimage.data(), sizeof(float)*lineimage.size(), &dwWritten, NULL)) {
        ErrorExit(TEXT("write stdin lineimage"));
    }
    if (!WriteFile(g_hChildStd_IN_Wr, sepimage.data(), sizeof(float) * sepimage.size(), &dwWritten, NULL)) {
        ErrorExit(TEXT("write stdin sepimage"));
    }

    intbuf = boxcount;
    if (!WriteFile(g_hChildStd_IN_Wr, &intbuf, sizeof(intbuf), &dwWritten, NULL)) {
        ErrorExit(TEXT("write stdin boxcount"));
    }

    for (int i = 0; i < boxcount; i++) {
        for (int j = 1; j < 9; j++) {
            float buf = loc[i * 9 + j];
            if (!WriteFile(g_hChildStd_IN_Wr, &buf, sizeof(float), &dwWritten, NULL)) {
                ErrorExit(TEXT("write stdin boxinfo"));
            }
        }
    }

    if (!CloseHandle(g_hChildStd_IN_Wr)) {
        ErrorExit(TEXT("StdInWr CloseHandle"));
    }

    if (!ReadFile(g_hChildStd_OUT_Rd, &intbuf, sizeof(intbuf), &dwRead, NULL)) {
        intbuf = 0;
    }
    if (dwRead == 0) {
        intbuf = 0;
    }
    int outcount = intbuf;

    std::cerr << outcount << " boxes" << std::endl;

    std::vector<int32_t> result;
    for (int i = 0; i < outcount; i++) {
        for (int j = 0; j < 5; j++) {
            int32_t buf;
            if (!ReadFile(g_hChildStd_OUT_Rd, &buf, sizeof(int32_t), &dwRead, NULL)) {
                ErrorExit(TEXT("read stdout boxinfo"));
            }
            if (dwRead == 0) {
                continue;
            }
            result.push_back(buf);
        }
    }

    BOOL bSuccess = FALSE;
    CHAR chBuf[BUFSIZE];
    HANDLE hParentStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    for (;;)
    {
        bSuccess = ReadFile(g_hChildStd_ERR_Rd, chBuf, BUFSIZE, &dwRead, NULL);
        if (!bSuccess || dwRead == 0) break;

        bSuccess = WriteFile(hParentStdOut, chBuf, dwRead, &dwWritten, NULL);
        if (!bSuccess) break;
    }

    return result;
}
