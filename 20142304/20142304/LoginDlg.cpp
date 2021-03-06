// LoginDlg.cpp: 구현 파일
//

#include "stdafx.h"
#include "20142304.h"
#include "LoginDlg.h"
#include "afxdialogex.h"

#include "20142304Dlg.h"
#include "OpenDlg.h"
#include "HideDlg.h"

#include <gmp.h>
#include <openssl/sha.h>
#include "byte_string.h"


// LoginDlg 대화 상자

IMPLEMENT_DYNAMIC(LoginDlg, CDialog)

LoginDlg::LoginDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_LOGINDLG, pParent)
	, password(_T(""))
{

}

LoginDlg::~LoginDlg()
{
}

void LoginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, password);
}


BEGIN_MESSAGE_MAP(LoginDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &LoginDlg::OnLogin)
END_MESSAGE_MAP()


// LoginDlg 메시지 처리기
void LoginDlg::OnLogin() {
	UpdateData(TRUE);
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

	CFile sfile;
	CString str;
	sfile.Open("Init.ini", CFile::modeRead);
	CArchive ar(&sfile, CArchive::load);
	while (ar.ReadString(str)) {
		if (!(str.GetLength() == 0)) {
			break;
		}
	}
	BOOL test = hash_pw.Compare(str);
	ar.Close();
	sfile.Close();
	
	if (!(test)) {
		CMy20142304Dlg* pDlg = (CMy20142304Dlg *)GetParent();

		if (pDlg->rule == 0) {
			SendMessage(WM_CLOSE, 0, 0);
			OpenDlg open;
			open.DoModal();
		}
		else {
			SendMessage(WM_CLOSE, 0, 0);
			HideDlg hide;
			hide.DoModal();
		}
	}
	else {
		MessageBox((CString)"비밀번호가 일치하지 않습니다.", (CString)"경고", MB_ICONSTOP);
		password.SetString(_T(""));
	}
	UpdateData(FALSE);
}

void LoginDlg::sha_hash(byte_string_t md_value, byte_string_t bs) {
	SHA_CTX SHA;
	byte_string_init(md_value, 20);
	SHA_Init(&SHA);
	SHA_Update(&SHA, bs->data, bs->len);
	SHA_Final(md_value->data, &SHA);
}

void LoginDlg::mympz_inp_raw(mpz_t z, const unsigned char* c, int n) {
	int i = 0;
	mpz_set_ui(z, 0);
	while (i < n) {
		mpz_mul_2exp(z, z, 0);
		mpz_add_ui(z, z, c[i]);
		i++;
	}
}