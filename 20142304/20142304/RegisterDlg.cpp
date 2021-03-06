// RegisterDlg.cpp: 구현 파일
//

#include "stdafx.h"
#include "20142304.h"
#include "RegisterDlg.h"
#include "afxdialogex.h"

#include "LoginDlg.h"

#include <gmp.h>
#include <openssl/sha.h>
#include "byte_string.h"


// RegisterDlg 대화 상자

IMPLEMENT_DYNAMIC(RegisterDlg, CDialog)

RegisterDlg::RegisterDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_REGISTERDLG, pParent)
	, password(_T(""))
	, repassword(_T(""))
{

}

RegisterDlg::~RegisterDlg()
{
}

void RegisterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, password);
	DDX_Text(pDX, IDC_EDIT2, repassword);
}


BEGIN_MESSAGE_MAP(RegisterDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &RegisterDlg::OnRegister)
END_MESSAGE_MAP()


// RegisterDlg 메시지 처리기


void RegisterDlg::OnRegister() {
	UpdateData(TRUE);
	// 1. 비밀번호 중복확인
	BOOL com = password.Compare(repassword);
	if (com) {
		MessageBox((CString)"비밀번호가 일치하지 않습니다.", (CString)"경고", MB_ICONSTOP);
		password.SetString(_T(""));
		repassword.SetString(_T(""));
	}
	else {
		// 2. 중복이 아니면 SHA1-256으로 해시화
		byte_string_t bs, md_value;

		LPTSTR lpsz = new TCHAR[password.GetLength() + 1];
		_tcscpy(lpsz, password);

		byte_string_set(bs, lpsz);

		sha_hash(md_value, bs);

		char d[100] = "";
		memcpy(d, md_value->data, md_value->len);



		mpz_t z;
		char str_mpz[1024];
		mpz_init(z);
		mympz_inp_raw(z, md_value->data, md_value->len);

		mpz_get_str(str_mpz, 16, z);

		CString hash_pw = str_mpz;
		mpz_clear(z);
		
		CFile file;
		file.Open("Init.ini", CFile::modeCreate | CFile::modeWrite);
		file.Write(hash_pw, (hash_pw.GetLength()-1)*(sizeof(CString)));
		file.Close();


		SendMessage(WM_CLOSE, 0, 0);
		LoginDlg login;
		login.DoModal();

	}
	UpdateData(FALSE);
}

void RegisterDlg::sha_hash(byte_string_t md_value, byte_string_t bs) {
	SHA_CTX SHA;
	byte_string_init(md_value, 20);
	SHA_Init(&SHA);
	SHA_Update(&SHA, bs->data, bs->len);
	SHA_Final(md_value->data, &SHA);
}

void RegisterDlg::mympz_inp_raw(mpz_t z, const unsigned char* c, int n) {
	int i = 0;
	mpz_set_ui(z, 0);
	while (i < n) {
		mpz_mul_2exp(z, z, 0);
		mpz_add_ui(z, z, c[i]);
		i++;
	}
}