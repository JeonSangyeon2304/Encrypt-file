#include <gmp.h>
#include <openssl/sha.h>
#include "byte_string.h"
#include <stdlib.h>
#pragma once


// RegisterDlg 대화 상자

class RegisterDlg : public CDialog
{
	DECLARE_DYNAMIC(RegisterDlg)

public:
	RegisterDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~RegisterDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_REGISTERDLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnRegister();
	void sha_hash(byte_string_t md_value, byte_string_t bs);
	void mympz_inp_raw(mpz_t z, const unsigned char* c, int n);
	CString password;
	CString repassword;
	CString pre1;
};
