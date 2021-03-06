#include <gmp.h>
#include <openssl/sha.h>
#include "byte_string.h"
#include <stdlib.h>
#pragma once


// LoginDlg 대화 상자

class LoginDlg : public CDialog
{
	DECLARE_DYNAMIC(LoginDlg)

public:
	LoginDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~LoginDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LOGINDLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLogin();
	void sha_hash(byte_string_t md_value, byte_string_t bs);
	void mympz_inp_raw(mpz_t z, const unsigned char* c, int n);
	CString password;
};
