// HideDlg.cpp: 구현 파일
//

#include "stdafx.h"
#include "20142304.h"
#include "HideDlg.h"
#include "afxdialogex.h"

#include <stdio.h>
#include <string.h>
#include <openssl/aes.h>
#include <openssl/md5.h>
#include <openssl/des.h>

static char md5Hash[32] = { 0, };

#define BLOCK_SIZE 16
#define FREAD_COUNT 4096
#define KEY_BIT 256
#define IV_SIZE 16
#define RW_SIZE 1

static AES_KEY aes_ks3;
static unsigned char iv[IV_SIZE];

int fs_encrypt_aes(char *in_file, char *out_file);
static int fs_passwd_aes(char *plain);

BOOL fs_encrypt_des(char *infile, char *ckey, char *outfile);
BOOL fs_passwd_des(char *passwd);
BOOL fs_encrypt_3des(char *infile, char *ckey, char *outfile);
BOOL fs_passwd_3des(char *passwd);
void passwd_shift(char *plain, char *shift);

DES_cblock key1 = { 0, };
DES_cblock key2 = { 0, };
DES_cblock key3 = { 0, };
// HideDlg 대화 상자

IMPLEMENT_DYNAMIC(HideDlg, CDialog)

HideDlg::HideDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_HIDEDLG, pParent)
	, Cpasswd(_T(""))
{

}

HideDlg::~HideDlg()
{
}

void HideDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT2, pre);
	//  DDX_Text(pDX, IDC_EDIT1, Passwd);
	DDX_Control(pDX, IDC_RADIO1, AES);
	DDX_Control(pDX, IDC_RADIO2, DES);
	DDX_Control(pDX, IDC_RADIO3, Thr_DES);
	//  DDX_Text(pDX, IDC_EDIT3, print);
	DDX_Text(pDX, IDC_EDIT1, Cpasswd);
}


BEGIN_MESSAGE_MAP(HideDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &HideDlg::OnOpenFile)
	ON_BN_CLICKED(IDC_BUTTON2, &HideDlg::OnHiding)
END_MESSAGE_MAP()


// HideDlg 메시지 처리기


void HideDlg::OnOpenFile() {
	// 파일 선택창 열어서 경로 따기
	CString szFilter = _T("All Files(*.*)|*.*||");
	CString strPathName;
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, szFilter);
	if (IDOK == dlg.DoModal()) {
		strPathName = _T(dlg.GetPathName());
		CFile fp;
		CFileException e;
		if (!fp.Open(strPathName, CFile::modeRead, &e)) {
			e.ReportError();
			return;
		}
		else {
			file_path = strPathName;
			pre.SetWindowTextA(strPathName);
			file_name = fp.GetFileTitle();
				fp.Close();
		}
	}
}


void HideDlg::OnHiding() {
	UpdateData(TRUE);
	CString type = NULL;
	if (AES.GetCheck())
		if (en_AES()) {
			DeleteFile(file_path);
			MessageBox("파일을 성공적으로 숨겼습니다.", "숨기기", NULL);
		}
		else
			AfxMessageBox("파일을 숨기는데 실패하였습니다.");
	if (DES.GetCheck())
		if (en_DES()) {
			MessageBox("파일을 성공적으로 숨겼습니다.", "숨기기", NULL);
		}
		else
			AfxMessageBox("파일을 숨기는데 실패하였습니다.");
	if (Thr_DES.GetCheck())
		if (en_Thr_DES()) {
			MessageBox("파일을 성공적으로 숨겼습니다.", "숨기기", NULL);
		}
		else
			AfxMessageBox("파일을 숨기는데 실패하였습니다.");
	UpdateData(FALSE);
}

BOOL HideDlg::en_AES() {
	UpdateData(TRUE);
	if (!(Cpasswd.Compare(""))) {
		AfxMessageBox("비밀번호를 입력하지 않았습니다.");
		return FALSE;
	}
	else {
		char *passwd = (char *)malloc(Cpasswd.GetLength());
		strcpy(passwd, (LPTSTR)(LPCTSTR)Cpasswd);
		fs_passwd_aes(passwd);

		char *infile = (char *)malloc(file_path.GetLength());
		strcpy(infile, (LPTSTR)(LPCTSTR)file_path);

		CString tmp;

		char *out_file = (char *)malloc(file_name.GetLength() + 62);
		strcpy(out_file, "C:\\Control Panel.{21EC2020-3AEA-1069-A2DD-08002B30309D}\\");
		strcat(out_file, (LPTSTR)(LPCTSTR)file_name);
		strcat(out_file, ".aes");

		if (fs_encrypt_aes(infile, out_file)) {
			AfxMessageBox("암호화에 성공하였습니다.");
			remove(file_path);
			return 1;
		}
		else
			return 0;
	}
	UpdateData(FALSE);
}

BOOL HideDlg::en_DES() {
	UpdateData(TRUE);
	char *passwd = (char *)malloc(9 * sizeof(char));
	strcpy(passwd, (LPTSTR)(LPCTSTR)Cpasswd);
	if (!(fs_passwd_des(passwd)))
		AfxMessageBox("비밀번호를 입력하지 않았습니다.");
	else {
		char *infile = (char *)malloc(file_path.GetLength());
		strcpy(infile, (LPTSTR)(LPCTSTR)file_path);

		CString tmp;

		char *out_file = (char *)malloc(file_name.GetLength() + 62);
		strcpy(out_file, "C:\\Control Panel.{21EC2020-3AEA-1069-A2DD-08002B30309D}\\");
		strcat(out_file, (LPTSTR)(LPCTSTR)file_name);
		strcat(out_file, ".des");

		if (fs_encrypt_des(infile, passwd, out_file)) {
			AfxMessageBox("암호화에 성공하였습니다.");
			remove(file_path);
			return 1;
		}
		else
			return 0;
	}
}

BOOL HideDlg::en_Thr_DES() {
	UpdateData(TRUE);
	char *passwd = (char *)malloc(Cpasswd.GetLength() + 1);
	strcpy(passwd, (LPTSTR)(LPCTSTR)Cpasswd);
	if (!(fs_passwd_3des(passwd)))
		AfxMessageBox("비밀번호 설정 실패.");
	else {
		char *infile = (char *)malloc(file_path.GetLength());
		strcpy(infile, (LPTSTR)(LPCTSTR)file_path);

		char *out_file = (char *)malloc(file_name.GetLength() + 62);
		strcpy(out_file, "C:\\Control Panel.{21EC2020-3AEA-1069-A2DD-08002B30309D}\\");
		strcat(out_file, (LPTSTR)(LPCTSTR)file_name);
		strcat(out_file, ".3es");

		if (fs_encrypt_3des(infile, passwd, out_file)) {
			AfxMessageBox("암호화에 성공하였습니다.");
			remove(file_path);
			return 1;
		}
		else
			return 0;
	}
}

int fs_encrypt_aes(char *in_file, char *out_file) {
	int i = 0;
	int len = 0;
	int padding_len = 0;
	int size;

	FILE *fp = fopen(in_file, "rb");
	if (fp == NULL) {
		CString err = "[ERROR] can not fopen in_file";
		AfxMessageBox(err);
		return FALSE;
	}
	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char *buf = (char *)malloc(size + BLOCK_SIZE);

	FILE *wfp = fopen(out_file, "wb");
	if (wfp == NULL) {
		CString err = "[ERROR] can not fopen out_file";
		AfxMessageBox(err);
		return FALSE;
	}

	memset(iv, 0, sizeof(iv));
	AES_set_encrypt_key((const unsigned char *)md5Hash, KEY_BIT, &aes_ks3);
	while (len = fread(buf, RW_SIZE, size, fp)) {
		if (size != len) {
			break;
		}
		AES_cbc_encrypt((const unsigned char*)buf, (unsigned char*)buf, len, &aes_ks3, iv, AES_ENCRYPT);
		fwrite(buf, RW_SIZE, len, wfp);

		fclose(wfp);
		fclose(fp);

		return TRUE;
	}
}

BOOL fs_encrypt_des(char *infile, char *ckey, char *outfile) {
	int file_size;
	static char* Res;
	int n = 0;
	DES_cblock key;
	DES_key_schedule schedule;
	int len = 0;


	FILE *fp = fopen(infile, "rb");
	if (fp == NULL) {
		CString err = "[ERROR] can not fopen in_file";
		AfxMessageBox(err);
		return 0;
	}
	fseek(fp, 0, SEEK_END);
	file_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	char *buf = (char *)malloc(file_size + 2);
	Res = (char *)malloc(file_size);

	memcpy(key, ckey, 8);
	DES_set_odd_parity(&key);
	DES_set_key_checked(&key, &schedule);

	FILE *wfp = fopen(outfile, "wb");
	if (wfp == NULL) {
		CString err = "[ERROR] can not fopen out_file";
		AfxMessageBox(err);
		return 0;
	}
	while (len = fread(buf, 1, file_size, fp)) {
		if (file_size != len) {
			break;
		}
		DES_cfb64_encrypt((unsigned char *)buf, (unsigned char *)Res, file_size, &schedule, &key, &n, DES_ENCRYPT);
		fwrite(Res, 1, len, wfp);

		fclose(wfp);
		fclose(fp);

		return 1;
	}
}


BOOL fs_encrypt_3des(char *infile, char *ckey, char *outfile) {
	int file_size;
	static char* Res;
	int n = 0;
	int len = 0;

	DES_key_schedule schedule1, schedule2, schedule3;

	FILE *fp = fopen(infile, "rb");
	if (fp == NULL) {
		CString err = "[ERROR] can not fopen in_file";
		AfxMessageBox(err);
		return 0;
	}
	fseek(fp, 0, SEEK_END);
	file_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	char *buf = (char *)malloc(file_size + 2);
	Res = (char *)malloc(file_size);

	DES_set_odd_parity(&key1);
	DES_set_key_checked(&key1, &schedule1);
	DES_set_key_checked(&key2, &schedule2);
	DES_set_key_checked(&key3, &schedule3);

	FILE *wfp = fopen(outfile, "wb");
	if (wfp == NULL) {
		CString err = "[ERROR] can not fopen out_file";
		AfxMessageBox(err);
		return 0;
	}
	while (len = fread(buf, 1, file_size, fp)) {
		if (file_size != len) {
			break;
		}
		DES_ede3_cbc_encrypt((unsigned char *)buf, (unsigned char *)Res,file_size, &schedule1, &schedule2, &schedule3, &key1, DES_ENCRYPT);
		fwrite(Res, 1, len, wfp);

		fclose(wfp);
		fclose(fp);

		return 1;
	}
}	

int fs_passwd_aes(char *plain) {
	unsigned char digest[MD5_DIGEST_LENGTH];

	MD5_CTX context;
	MD5_Init(&context);
	MD5_Update(&context, plain, strlen(plain));
	MD5_Final(digest, &context);

	for (int i = 0; i < MD5_DIGEST_LENGTH; ++i) {
		sprintf(md5Hash + (i * 2), "%02x", digest[i]);
	}

	return TRUE;
}

BOOL fs_passwd_des(char *passwd) {
	int padding_len = 8 - strlen(passwd);
	char padding[8];
	int padding_num = passwd[strlen(passwd) - 1] - 1;
	memset(padding, padding_num, padding_len);
	padding[padding_len] = '\0';
	strcat(passwd, padding);
	return 1;
}

BOOL fs_passwd_3des(char *passwd) {
	int padding_len = 8 - strlen(passwd);
	char padding[8];
	int padding_num = passwd[strlen(passwd) - 1] - 1;
	memset(padding, padding_num, padding_len);
	padding[padding_len] = '\0';
	strcat(passwd, padding);
	passwd_shift(passwd, (char *)key1);
	passwd_shift((char *)key1, (char *)key2);
	passwd_shift((char *)key2, (char *)key3);


	return 1;
}

void passwd_shift(char *plain, char *shift) {
	char tmp = plain[strlen(plain) - 1];
	char key_tmp[9] = { 'a', };
	for (int i = 0; i < strlen(plain) - 1; i++) {
		key_tmp[i + 1] = plain[i];
	}
	key_tmp[0] = tmp;
	key_tmp[8] = '\0';
	memcpy(shift, key_tmp, 9);
}