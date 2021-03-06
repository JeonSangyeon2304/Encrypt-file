#pragma once


// OpenDlg 대화 상자

class OpenDlg : public CDialog
{
	DECLARE_DYNAMIC(OpenDlg)

public:
	OpenDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~OpenDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_OPENDLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	DECLARE_MESSAGE_MAP()
public:
	CListBox m_List;
	CString Cfile;
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeList();
	CString Cpath;
	CString Cpasswd;
	CButton AES;
	CButton DES;
	CButton Thr_DES;
	afx_msg void OnOpen();

	BOOL en_AES();
	BOOL en_DES();
	BOOL en_Thr_DES();


	CString file_path;
	CString fileName;
};
