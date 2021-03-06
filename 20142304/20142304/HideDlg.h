#include <Windows.h>
#include <wincrypt.h>
#pragma once


// HideDlg 대화 상자

class HideDlg : public CDialog
{
	DECLARE_DYNAMIC(HideDlg)

public:
	HideDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~HideDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_HIDEDLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CString file_path;
	afx_msg void OnOpenFile();
	afx_msg void OnHiding();

	BOOL en_AES();
	BOOL en_DES();
	BOOL en_Thr_DES();

	CEdit pre;

	CButton AES;
	CButton DES;
	CButton Thr_DES;
	CString Cpasswd;
	CString file_name;
};