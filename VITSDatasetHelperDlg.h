
// VITSDatasetHelperDlg.h: 头文件
//

#pragma once
#include "afxeditbrowsectrl.h"
#include <vector>
#include <string>
#include <sstream>
#include "sndfile.hh"
#include <Windows.h>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <Windows.h>
#include <shellapi.h>  // 包含拖放相关的函数声明
#include <afxwin.h> // MFC support
// CVITSDatasetHelperDlg 对话框
class CVITSDatasetHelperDlg : public CDialogEx
{
// 构造
public:
	CVITSDatasetHelperDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_VITSDATASETHELPER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
    afx_msg void OnDropFiles(HDROP hDropInfo);
private:
    CMFCEditBrowseCtrl m_editBrowse;
    CEdit m_editFileNames;
    CEdit m_editNewFileNames;
    CButton m_btnBrowse;
    CButton m_btnRename;
    CButton m_btnRestore;
    std::vector<CString> m_originalFileNames;
    std::vector<CString> m_newFileNames;

    CMFCEditBrowseCtrl m_editBrowseWav;
    CMFCEditBrowseCtrl m_editBrowseSrt;
    CButton m_btnProcess;
    CButton m_checkNewline;

    afx_msg void OnBnClickedButtonProcess();

    void ProcessFiles();
    std::wstring ReplaceIllegalCharacters(const std::wstring& input);

    void SplitAudio(const std::wstring& wavPath, const std::wstring& srtPath, bool useHashForNewline);
    void ProcessAudioSplitting(const std::wstring& wavPath, const std::wstring& srtPath, bool useHashForNewline);
    static DWORD WINAPI AudioSplittingThread(LPVOID param);

    static void RemoveCharacters(std::wstring& str, const wchar_t* charsToRemove);
    static std::wstring Utf8ToWstring(const std::string& str);

    void BrowseFolder();
    void DisplayFileNames();
    void RenameFiles();
    void RestoreFiles();
    bool ValidateFileNames();
    bool ContainsIllegalCharacters(const CString& str);
    bool FileExists(const CString& filePath);
    void LogMessage(const CString& message);
    void ClearFileNames();
    void OnButtonClicked();  // Handler for IDC_BUTTON6
    void MoveOrCopyFiles(const CString& folderPath, bool bMove);

    CMFCEditBrowseCtrl m_editBrowse5;  // Edit browse control
    CButton m_radio5;                  // Radio button IDC_RADIO5
    CButton m_radio6;                  // Radio button IDC_RADIO6
    CMFCEditBrowseCtrl m_editBrowse6;
    CEdit m_edit1;
    CEdit m_edit5;
    CButton m_radio1;
    CButton m_radio2;
    CButton m_radio3;
    CButton m_radio4;
    CButton m_button7;

    void CreateListFile(const CString& folderPath, const CString& roleName, const CString& language);
    CString ExtractTextFromFileName(const CString& fileName);
    std::string CStringToUTF8(const CString& str); // 声明 CStringToUTF8 函数


public:
    afx_msg void OnBnClickedButtonBrowse();
    afx_msg void OnBnClickedButtonRename();
    afx_msg void OnBnClickedButtonRestore();
    CMFCEditBrowseCtrl m_editBrowse2;
    CButton m_check1;
    CButton m_check2;
    afx_msg void OnBnClickedButton1();
    void ProcessFiles(CString folderPath, bool extendShortFiles, bool trimLongFiles);
    void ExtendAudio(CString filePath, double duration);
    void TrimAudio(CString filePath);
    double GetAudioDuration(CString filePath);
    void ExecuteFFmpegCommand(CString command, CString tempFilePath, CString originalFilePath);
    void ProcessFilesInBackground(CString folderPath, bool extendShortFiles, bool trimLongFiles);
    afx_msg void OnBnClickedButton6(); // Button click handler
// 实现
protected:
	HICON m_hIcon;
    afx_msg void OnBnClickedButton7();
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
};
