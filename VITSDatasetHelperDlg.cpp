
// VITSDatasetHelperDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "VITSDatasetHelper.h"
#include "VITSDatasetHelperDlg.h"
#include "afxdialogex.h"
#include <filesystem>
#include <fstream>
#include <locale>
#include <codecvt>
#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <thread>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fs = std::filesystem;
// CVITSDatasetHelperDlg 对话框



CVITSDatasetHelperDlg::CVITSDatasetHelperDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_VITSDATASETHELPER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CVITSDatasetHelperDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_MFCEDITBROWSE4, m_editBrowse);
    DDX_Control(pDX, IDC_EDIT3, m_editFileNames);
    DDX_Control(pDX, IDC_EDIT2, m_editNewFileNames);
    DDX_Control(pDX, IDC_BUTTON3, m_btnBrowse);
    DDX_Control(pDX, IDC_BUTTON4, m_btnRename);
    DDX_Control(pDX, IDC_BUTTON5, m_btnRestore);
    DDX_Control(pDX, IDC_MFCEDITBROWSE2, m_editBrowseWav);
    DDX_Control(pDX, IDC_MFCEDITBROWSE3, m_editBrowseSrt);
    DDX_Control(pDX, IDC_BUTTON2, m_btnProcess);
    DDX_Control(pDX, IDC_CHECK3, m_checkNewline);
    DDX_Control(pDX, IDC_MFCEDITBROWSE1, m_editBrowse2);
    DDX_Control(pDX, IDC_CHECK1, m_check1);
    DDX_Control(pDX, IDC_CHECK2, m_check2);
}


BEGIN_MESSAGE_MAP(CVITSDatasetHelperDlg, CDialogEx)
    ON_BN_CLICKED(IDC_BUTTON3, &CVITSDatasetHelperDlg::OnBnClickedButtonBrowse)
    ON_BN_CLICKED(IDC_BUTTON4, &CVITSDatasetHelperDlg::OnBnClickedButtonRename)
    ON_BN_CLICKED(IDC_BUTTON5, &CVITSDatasetHelperDlg::OnBnClickedButtonRestore)
    ON_BN_CLICKED(IDC_BUTTON2, &CVITSDatasetHelperDlg::OnBnClickedButtonProcess)
    ON_WM_DROPFILES()
    ON_BN_CLICKED(IDC_BUTTON1, &CVITSDatasetHelperDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


BOOL CVITSDatasetHelperDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    SetIcon(m_hIcon, TRUE);
    SetIcon(m_hIcon, FALSE);
    m_editBrowseWav.EnableFileBrowseButton();
    m_editBrowseSrt.EnableFileBrowseButton();
    m_checkNewline.SetCheck(BST_CHECKED);
    m_check1.SetCheck(BST_CHECKED); // 设置 IDC_CHECK3 默认被勾选
    m_check2.SetCheck(BST_CHECKED); // 设置 IDC_CHECK3 默认被勾选
    m_btnRename.EnableWindow(FALSE);
    m_btnRestore.EnableWindow(FALSE);
    DragAcceptFiles(TRUE);
    return TRUE;

}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CVITSDatasetHelperDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CVITSDatasetHelperDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CVITSDatasetHelperDlg::OnBnClickedButtonBrowse()
{
    BrowseFolder();
}

void CVITSDatasetHelperDlg::BrowseFolder()
{
    CString folderPath;
    m_editBrowse.GetWindowText(folderPath);

    if (folderPath.IsEmpty())
    {
        AfxMessageBox(_T("请选择一个文件夹。"));
        return;
    }

    m_originalFileNames.clear();
    m_newFileNames.clear();

    for (const auto& entry : fs::directory_iterator((LPCTSTR)folderPath))
    {
        if (entry.is_regular_file())
        {
            m_originalFileNames.push_back(entry.path().filename().c_str());
        }
    }

    DisplayFileNames();
    m_btnRename.EnableWindow(TRUE);
}

void CVITSDatasetHelperDlg::DisplayFileNames()
{
    m_editFileNames.SetWindowText(_T(""));
    for (const auto& fileName : m_originalFileNames)
    {
        CString currentText;
        m_editFileNames.GetWindowText(currentText);
        currentText += fileName + _T("\r\n");
        m_editFileNames.SetWindowText(currentText);
    }
}

void CVITSDatasetHelperDlg::OnBnClickedButtonRename()
{
    if (!ValidateFileNames())
    {
        AfxMessageBox(_T("文件名列表不匹配或包含非法字符。"));
        return;
    }

    RenameFiles();
    m_editNewFileNames.SetReadOnly(TRUE);
    m_btnRename.EnableWindow(FALSE);
    m_btnRestore.EnableWindow(TRUE);
}

bool CVITSDatasetHelperDlg::ValidateFileNames()
{
    CString newFileNamesText;
    m_editNewFileNames.GetWindowText(newFileNamesText);

    m_newFileNames.clear(); // Clear previous entries

    int newFileCount = 0;
    int pos = 0;
    CString token = newFileNamesText.Tokenize(_T("\r\n"), pos);
    while (!token.IsEmpty())
    {
        if (ContainsIllegalCharacters(token))
        {
            return false;
        }
        m_newFileNames.push_back(token);
        newFileCount++;
        token = newFileNamesText.Tokenize(_T("\r\n"), pos);
    }

    return newFileCount == m_originalFileNames.size();
}

bool CVITSDatasetHelperDlg::ContainsIllegalCharacters(const CString& str)
{
    CString illegalChars = _T("\\/:*?\"<>|");
    for (int i = 0; i < str.GetLength(); ++i)
    {
        if (illegalChars.Find(str[i]) != -1)
        {
            return true;
        }
    }
    return false;
}

bool CVITSDatasetHelperDlg::FileExists(const CString& filePath)
{
    return fs::exists((LPCTSTR)filePath);
}

void CVITSDatasetHelperDlg::LogMessage(const CString& message)
{
    CStdioFile logFile;
    if (logFile.Open(_T("log.txt"), CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite | CFile::typeText))
    {
        logFile.SeekToEnd();
        logFile.WriteString(message + _T("\n"));
        logFile.Close();
    }
}

void CVITSDatasetHelperDlg::RenameFiles()
{
    CString folderPath;
    m_editBrowse.GetWindowText(folderPath);

    std::vector<CString> notFoundFiles;

    for (size_t i = 0; i < m_originalFileNames.size(); ++i)
    {
        CString oldFilePath = folderPath + _T("\\") + m_originalFileNames[i];
        CString newFilePath = folderPath + _T("\\") + m_newFileNames[i];

        if (FileExists(oldFilePath))
        {
            LogMessage(_T("Renaming: ") + oldFilePath + _T(" to ") + newFilePath);
            fs::rename((LPCTSTR)oldFilePath, (LPCTSTR)newFilePath);
        }
        else
        {
            notFoundFiles.push_back(oldFilePath);
            LogMessage(_T("File not found: ") + oldFilePath);
        }
    }

    if (!notFoundFiles.empty())
    {
        CString message = _T("以下文件不存在:\n");
        for (const auto& file : notFoundFiles)
        {
            message += file + _T("\n");
        }
        AfxMessageBox(message);
    }
}

void CVITSDatasetHelperDlg::OnBnClickedButtonRestore()
{
    RestoreFiles();
    m_editNewFileNames.SetReadOnly(FALSE);
    m_btnRename.EnableWindow(TRUE);
    m_btnRestore.EnableWindow(FALSE);
}

void CVITSDatasetHelperDlg::RestoreFiles()
{
    CString folderPath;
    m_editBrowse.GetWindowText(folderPath);

    std::vector<CString> notFoundFiles;

    for (size_t i = 0; i < m_newFileNames.size(); ++i)
    {
        CString newFilePath = folderPath + _T("\\") + m_newFileNames[i];
        CString oldFilePath = folderPath + _T("\\") + m_originalFileNames[i];

        if (FileExists(newFilePath))
        {
            LogMessage(_T("Restoring: ") + newFilePath + _T(" to ") + oldFilePath);
            fs::rename((LPCTSTR)newFilePath, (LPCTSTR)oldFilePath);
        }
        else
        {
            notFoundFiles.push_back(newFilePath);
            LogMessage(_T("File not found: ") + newFilePath);
        }
    }

    if (!notFoundFiles.empty())
    {
        CString message = _T("以下文件不存在:\n");
        for (const auto& file : notFoundFiles)
        {
            message += file + _T("\n");
        }
        AfxMessageBox(message);
    }
}
void CVITSDatasetHelperDlg::OnBnClickedButtonProcess()
{
    ProcessFiles();
}

void CVITSDatasetHelperDlg::ProcessFiles()
{
    CString wavPathCStr, srtPathCStr;
    m_editBrowseWav.GetWindowText(wavPathCStr);
    m_editBrowseSrt.GetWindowText(srtPathCStr);

    std::wstring wavPath(wavPathCStr.GetString());
    std::wstring srtPath(srtPathCStr.GetString());

    if (wavPath.empty() || srtPath.empty() || !std::filesystem::exists(wavPath) || !std::filesystem::exists(srtPath)) {
        AfxMessageBox(L"无效文件路径。");
        return;
    }

    bool useHashForNewline = (m_checkNewline.GetCheck() == BST_CHECKED);
    SplitAudio(wavPath, srtPath, useHashForNewline);
}

std::wstring CVITSDatasetHelperDlg::ReplaceIllegalCharacters(const std::wstring& input)
{
    std::wstring output = input;
    std::wstring illegalChars = L"?*:\"";
    std::wstring replacementChars = L"？＊：＂";

    for (size_t i = 0; i < illegalChars.length(); ++i) {
        std::replace(output.begin(), output.end(), illegalChars[i], replacementChars[i]);
    }

    return output;
}


void CVITSDatasetHelperDlg::RemoveCharacters(std::wstring& str, const wchar_t* charsToRemove) {
    for (unsigned int i = 0; charsToRemove[i] != L'\0'; ++i) {
        str.erase(std::remove(str.begin(), str.end(), charsToRemove[i]), str.end());
    }
}

std::wstring CVITSDatasetHelperDlg::Utf8ToWstring(const std::string& str) {
    if (str.empty()) return std::wstring();
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

void CVITSDatasetHelperDlg::SplitAudio(const std::wstring& wavPath, const std::wstring& srtPath, bool useHashForNewline)
{
    // Start a new thread to handle audio splitting
    CreateThread(NULL, 0, AudioSplittingThread, new std::tuple<std::wstring, std::wstring, bool>(wavPath, srtPath, useHashForNewline), 0, NULL);
}

bool FileExists(const std::wstring& path)
{
    return std::filesystem::exists(path);
}

std::wstring FindFFmpeg()
{
    // Check if ffmpeg is in the system path
    if (FileExists(L"ffmpeg"))
    {
        return L"ffmpeg";
    }

    // Check if ffmpeg is in the same directory as the executable
    wchar_t exePath[MAX_PATH];
    GetModuleFileName(NULL, exePath, MAX_PATH);
    std::wstring exeDir = std::filesystem::path(exePath).parent_path();
    std::wstring ffmpegPath = exeDir + L"\\ffmpeg.exe";

    if (FileExists(ffmpegPath))
    {
        return ffmpegPath;
    }

    return L"";
}
void FreeRes_ShowError(char* pszText)
{
    LPTSTR szBuffer = new TCHAR[8192];
    ::wsprintf(szBuffer, L"%hs Error[%d]\n", (pszText), ::GetLastError());//宽字符
#ifdef _DEBUG
    ::MessageBox(NULL, szBuffer, _T("ERROR"), MB_OK | MB_ICONERROR);
#endif
}

BOOL FreeMyResource(UINT uiResouceName, char* lpszResourceType, char* lpszSaveFileName)
{
    // 获取程序所在的路径
    TCHAR szModulePath[MAX_PATH];
    GetModuleFileName(NULL, szModulePath, MAX_PATH);
    PathRemoveFileSpec(szModulePath); // 移除文件名，保留目录路径

    // 组合保存文件的完整路径
    CString strSaveFilePath;
    strSaveFilePath.Format(_T("%s\\%s"), szModulePath, CStringW(lpszSaveFileName));

    HRSRC hRsrc = ::FindResource(GetModuleHandle(NULL), MAKEINTRESOURCE(uiResouceName), CStringW(lpszResourceType));
    if (hRsrc == NULL)
    {
        FreeRes_ShowError("FindResource");
        return FALSE;
    }
    DWORD dwSize = ::SizeofResource(NULL, hRsrc);
    if (0 >= dwSize)
    {
        FreeRes_ShowError("SizeofResource");
        return FALSE;
    }
    HGLOBAL hGlobal = ::LoadResource(NULL, hRsrc);
    if (NULL == hGlobal)
    {
        FreeRes_ShowError("LoadResource");
        return FALSE;
    }
    LPVOID lpVoid = ::LockResource(hGlobal);
    if (NULL == lpVoid)
    {
        FreeRes_ShowError("LockResource");
        return FALSE;
    }
    FILE* fp = NULL;
    fopen_s(&fp, CStringA(strSaveFilePath), "wb+"); // CStringA 将 Unicode 字符串转换为 ANSI 字符串
    if (NULL == fp)
    {
        FreeRes_ShowError("fopen");
        return FALSE;
    }
    fwrite(lpVoid, sizeof(char), dwSize, fp);
    fclose(fp);

    return TRUE;
}
bool ExtractFFmpeg()
{

    // 获取程序所在目录路径
    CString strExePath;
    GetModuleFileName(NULL, strExePath.GetBuffer(MAX_PATH), MAX_PATH);
    strExePath.ReleaseBuffer();
    strExePath = strExePath.Left(strExePath.ReverseFind('\\')); // 去掉文件名，只保留目录路径
    // 构建目标文件路径
    CString strDestFilePath;

    char szSaveName1[MAX_PATH] = "ffmpeg.exe";
    char szSaveName2[MAX_PATH] = "ffprobe.exe";
    if (FreeMyResource(IDR_MYRES2, "MYRES", szSaveName1)&& FreeMyResource(IDR_MYRES1, "MYRES", szSaveName2)) return true;
    else return false;
}

DWORD CVITSDatasetHelperDlg::AudioSplittingThread(LPVOID param)
{
    auto parameters = *(std::tuple<std::wstring, std::wstring, bool>*)param;
    std::wstring wavPath = std::get<0>(parameters);
    std::wstring srtPath = std::get<1>(parameters);
    bool useHashForNewline = std::get<2>(parameters);
    delete (std::tuple<std::wstring, std::wstring, bool>*)param;

    std::ifstream srtFile(srtPath);
    std::string line, delimiter = " --> ", startTime, endTime, text;
    int subtitleIndex = 1;
    std::wstring basePath = wavPath.substr(0, wavPath.find_last_of(L"\\"));
    std::wstring baseFilename = wavPath.substr(wavPath.find_last_of(L"\\") + 1);
    baseFilename = baseFilename.substr(0, baseFilename.length() - 4); // Remove .wav extension

    std::wstring ffmpegPath = FindFFmpeg();
    if (ffmpegPath.empty())
    {
        if (!ExtractFFmpeg())
        {
            AfxMessageBox(L"ffmpeg在系统和目录中找不到，而且无法释放。", MB_ICONERROR);
            return 1;
        }

        ffmpegPath = FindFFmpeg();
        if (ffmpegPath.empty())
        {
            AfxMessageBox(L"ffmpeg在系统和目录中找不到，而且无法释放。", MB_ICONERROR);
            return 1;
        }
    }

    while (std::getline(srtFile, line)) {
        if (line.find(delimiter) != std::string::npos) {
            size_t pos = line.find(delimiter);
            startTime = line.substr(0, pos);
            endTime = line.substr(pos + delimiter.length());
            std::wstring wStartTime = Utf8ToWstring(startTime);
            std::wstring wEndTime = Utf8ToWstring(endTime);
            RemoveCharacters(wStartTime, L":,");
            RemoveCharacters(wEndTime, L":,");

            text.clear();
            while (std::getline(srtFile, line) && !line.empty()) {
                if (!text.empty()) {
                    text += useHashForNewline ? "#" : " ";
                }
                text += line;
            }

            std::wstring wText = Utf8ToWstring(text);

            std::wstringstream outputFilename;
            outputFilename << basePath << L"\\" << baseFilename << L"_" << subtitleIndex++ << L"_" << wStartTime << L"_" << wEndTime << L"_" << wText << L".wav";
            std::wstring command = L"\"" + ffmpegPath + L"\" -y -i \"" + wavPath + L"\" -ss " + wStartTime.substr(0, 2) + L":" + wStartTime.substr(2, 2) + L":" + wStartTime.substr(4, 2) + L"." + wStartTime.substr(6, 3) + L" -to " + wEndTime.substr(0, 2) + L":" + wEndTime.substr(2, 2) + L":" + wEndTime.substr(4, 2) + L"." + wEndTime.substr(6, 3) + L" -c copy \"" + outputFilename.str() + L"\"";

            // Execute ffmpeg without console window
            STARTUPINFOW si = { sizeof(si) };
            PROCESS_INFORMATION pi;
            si.dwFlags = STARTF_USESHOWWINDOW;
            si.wShowWindow = SW_HIDE;
            if (!CreateProcessW(NULL, &command[0], NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
                std::wcerr << L"Failed to execute ffmpeg command." << std::endl;
            }
            else {
                WaitForSingleObject(pi.hProcess, INFINITE);
                CloseHandle(pi.hProcess);
                CloseHandle(pi.hThread);
            }
        }
    }
    srtFile.close();

    CString strExePath;
    GetModuleFileName(NULL, strExePath.GetBuffer(MAX_PATH), MAX_PATH);
    strExePath.ReleaseBuffer();
    strExePath = strExePath.Left(strExePath.ReverseFind('\\')); // 去掉文件名，只保留目录路径
    // 构建目标文件路径
    CString strDestFilePath;
    // 删除原文件 d3d9.dll（如果存在）
    strDestFilePath = strExePath + _T("\\ffmpeg.exe");
    if (PathFileExists(strDestFilePath)) DeleteFile(strDestFilePath);
    strDestFilePath = strExePath + _T("\\ffprobe.exe");
    if (PathFileExists(strDestFilePath)) DeleteFile(strDestFilePath);

    AfxMessageBox(_T("切分完成。"));

    return 0;
}

void CVITSDatasetHelperDlg::OnDropFiles(HDROP hDropInfo)
{
    UINT fileCount = DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);
    if (fileCount > 0)
    {
        wchar_t filePath[MAX_PATH];
        DragQueryFile(hDropInfo, 0, filePath, MAX_PATH);

        // Check if the dropped item is a directory
        DWORD attributes = GetFileAttributes(filePath);
        bool isDirectory = (attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_DIRECTORY));

        // Get the point where the file was dropped
        POINT pt;
        DragQueryPoint(hDropInfo, &pt);
        ClientToScreen(&pt);

        // Get the window under the drop point
        CWnd* pWnd = WindowFromPoint(pt);
        if (pWnd != nullptr)
        {
            // Check if the window is one of the edit controls
            if (pWnd->GetSafeHwnd() == m_editBrowseSrt.GetSafeHwnd())
            {
                m_editBrowseSrt.SetWindowTextW(filePath);
            }
            else if (pWnd->GetSafeHwnd() == m_editBrowseWav.GetSafeHwnd())
            {
                m_editBrowseWav.SetWindowTextW(filePath);
            }
            else if (pWnd->GetSafeHwnd() == m_editBrowse.GetSafeHwnd() && isDirectory)
            {
                m_editBrowse.SetWindowTextW(filePath);
            }
            else if (pWnd->GetSafeHwnd() == m_editBrowse2.GetSafeHwnd() && isDirectory)
            {
                m_editBrowse2.SetWindowTextW(filePath);
            }
        }
    }
    DragFinish(hDropInfo);
}

void CVITSDatasetHelperDlg::OnBnClickedButton1()
{
    CString folderPath;
    m_editBrowse2.GetWindowText(folderPath);

    if (folderPath.IsEmpty())
    {
        AfxMessageBox(_T("请选择一个文件夹。"));
        return;
    }

    BOOL extendShortFiles = m_check1.GetCheck();
    BOOL trimLongFiles = m_check2.GetCheck();

    if (!extendShortFiles && !trimLongFiles)
    {
        AfxMessageBox(_T("请至少勾选一个选项。"));
        return;
    }

    // 启动后台线程处理文件
    std::thread(&CVITSDatasetHelperDlg::ProcessFilesInBackground, this, folderPath, extendShortFiles, trimLongFiles).detach();
}

void CVITSDatasetHelperDlg::ProcessFilesInBackground(CString folderPath, bool extendShortFiles, bool trimLongFiles)
{
    ProcessFiles(folderPath, extendShortFiles, trimLongFiles);

    CString strExePath;
    GetModuleFileName(NULL, strExePath.GetBuffer(MAX_PATH), MAX_PATH);
    strExePath.ReleaseBuffer();
    strExePath = strExePath.Left(strExePath.ReverseFind('\\')); // 去掉文件名，只保留目录路径
    // 构建目标文件路径
    CString strDestFilePath;
    // 删除原文件 d3d9.dll（如果存在）
    strDestFilePath = strExePath + _T("\\ffmpeg.exe");
    if (PathFileExists(strDestFilePath)) 
        if(!DeleteFile(strDestFilePath))  AfxMessageBox(_T("删除ffmpeg失败。"));;
    strDestFilePath = strExePath + _T("\\ffprobe.exe");
    if (PathFileExists(strDestFilePath)) 
        if (!DeleteFile(strDestFilePath))  AfxMessageBox(_T("删除ffprobe失败。"));;

    AfxMessageBox(_T("修改成功。"));

}

void CVITSDatasetHelperDlg::ProcessFiles(CString folderPath, bool extendShortFiles, bool trimLongFiles)
{
    for (const auto& entry : std::filesystem::directory_iterator((LPCTSTR)folderPath))
    {
        if (entry.path().extension() == ".wav")
        {
            CString filePath = entry.path().c_str();
            double duration = GetAudioDuration(filePath);

            if (extendShortFiles && duration < 3.0)
            {
                ExtendAudio(filePath, duration);
            }
            if (trimLongFiles && duration > 10.0)
            {
                TrimAudio(filePath);
            }
        }
    }
}

void CVITSDatasetHelperDlg::ExtendAudio(CString filePath, double duration)
{
    int loopCount = static_cast<int>(ceil(3.0 / duration));
    CString tempFilePath = filePath + _T(".temp.wav");
    CString command;
    command.Format(_T("ffmpeg -i \"%s\" -af \"aloop=loop=%d:size=2e+09\" -y \"%s\""), filePath, loopCount - 1, tempFilePath);
    ExecuteFFmpegCommand(command, tempFilePath, filePath);
}

void CVITSDatasetHelperDlg::TrimAudio(CString filePath)
{
    CString tempFilePath = filePath + _T(".temp.wav");
    CString command;
    command.Format(_T("ffmpeg -i \"%s\" -t 10 -c copy -y \"%s\""), filePath, tempFilePath);
    ExecuteFFmpegCommand(command, tempFilePath, filePath);
}

double CVITSDatasetHelperDlg::GetAudioDuration(CString filePath)
{
    CString command;
    command.Format(_T("ffprobe -v error -show_entries format=duration -of default=noprint_wrappers=1:nokey=1 \"%s\""), filePath);

    FILE* pipe = _tpopen(command, _T("r"));
    if (!pipe) return -1.0;

    char buffer[128];
    std::string result = "";
    while (fgets(buffer, 128, pipe) != NULL)
    {
        result += buffer;
    }
    _pclose(pipe);

    return atof(result.c_str());
}

void CVITSDatasetHelperDlg::ExecuteFFmpegCommand(CString command, CString tempFilePath, CString originalFilePath)
{
    std::wstring ffmpegPath = FindFFmpeg();
    if (ffmpegPath.empty())
    {
        if (!ExtractFFmpeg())
        {
            AfxMessageBox(L"ffmpeg在系统和目录中找不到，而且无法释放。", MB_ICONERROR);
        }

        ffmpegPath = FindFFmpeg();
        if (ffmpegPath.empty())
        {
            AfxMessageBox(L"ffmpeg在系统和目录中找不到，而且无法释放。", MB_ICONERROR);
        }
    }
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;
    ZeroMemory(&pi, sizeof(pi));

    // 使用 CreateProcess 来执行命令并隐藏窗口
    if (CreateProcess(NULL, command.GetBuffer(), NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi))
    {
        WaitForSingleObject(pi.hProcess, INFINITE);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);

        // 覆盖原始文件
        std::filesystem::remove((LPCTSTR)originalFilePath);
        std::filesystem::rename((LPCTSTR)tempFilePath, (LPCTSTR)originalFilePath);
    }
    else
    {
        AfxMessageBox(_T("FFmpeg命令执行失败。"));
    }
}