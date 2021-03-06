// OpenDlg.cpp: 구현 파일
//

#include "stdafx.h"
#include "20142304.h"
#include "OpenDlg.h"
#include "afxdialogex.h"

#include <stdio.h>
#include <string.h>
#include <openssl/aes.h>
#include <openssl/md5.h>
#include <openssl/des.h>

CString path = _T("C:\\Control Panel.{21EC2020-3AEA-1069-A2DD-08002B30309D}\\*.*");
static char md5Hash[32] = { 0, };
#define BLOCK_SIZE 16
#define FREAD_COUNT 4096
#define KEY_BIT 256
#define IV_SIZE 16
#define RW_SIZE 1

static AES_KEY aes_ks3;
static unsigned char iv[IV_SIZE];

int fs_decrypt_aes(char *in_file, char *out_file);
static int fs_passwd_aes(char *plain);

static int fs_passwd_des(char *passwd);
static int fs_decrypt_des(char *infile, char *ckey, char *outfile);

static int fs_decrypt_3des(char *infile, char *ckey, char *outfile);
static int fs_passwd_3des(char *passwd);
static void passwd_shift(char *plain, char *shift);

static DES_cblock key1 = { 0, };
static DES_cblock key2 = { 0, };
static DES_cblock key3 = { 0, };
// OpenDlg 대화 상자

IMPLEMENT_DYNAMIC(OpenDlg, CDialog)

OpenDlg::OpenDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_OPENDLG, pParent)
	, Cpath(_T(""))
	, Cpasswd(_T(""))
{
}

OpenDlg::~OpenDlg()
{
}

void OpenDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_List);
	DDX_Text(pDX, IDC_EDIT2, Cpath);
	DDX_Text(pDX, IDC_EDIT1, Cpasswd);
	DDX_Control(pDX, IDC_RADIO1, AES);
	DDX_Control(pDX, IDC_RADIO2, DES);
	DDX_Control(pDX, IDC_RADIO3, Thr_DES);
}

BEGIN_MESSAGE_MAP(OpenDlg, CDialog)
	ON_LBN_SELCHANGE(IDC_LIST1, &OpenDlg::OnSelchangeList)
	ON_BN_CLICKED(IDC_BUTTON1, &OpenDlg::OnOpen)
END_MESSAGE_MAP()
BOOL OpenDlg::OnInitDialog() {
	CDialog::OnInitDialog();

	CFileFind finder;

	BOOL bWorking = finder.FindFile(path);


	while (bWorking) {
		bWorking = finder.FindNextFile();
		if (finder.IsArchived()) {
			fileName = finder.GetFileName();
			m_List.AddString(fileName);
		}
	}
	return TRUE;
}

void OpenDlg::OnSelchangeList() {
	UpdateData(TRUE);
	int index = m_List.GetCurSel();
	m_List.GetText(index, Cfile);
	SetDlgItemText(IDC_EDIT2, Cfile);
}


void OpenDlg::OnOpen() {
	UpdateData(TRUE);
	CString type = NULL;
	if (AES.GetCheck())
		if (en_AES()) {
			MessageBox("파일을 성공적으로 열었습니다.", "숨기기", NULL);
		}
		else
			AfxMessageBox("파일을 숨기는데 실패하였습니다.");
	if (DES.GetCheck())
		if (en_DES()) {
			MessageBox("파일을 성공적으로 열었습니다.", "숨기기", NULL);
		}
		else
			AfxMessageBox("파일을 숨기는데 실패하였습니다.");
	if (Thr_DES.GetCheck())
		if (en_Thr_DES()) {
			MessageBox("파일을 성공적으로 열었습니다.", "숨기기", NULL);
		}
		else
			AfxMessageBox("파일을 숨기는데 실패하였습니다.");
	UpdateData(FALSE);
}

BOOL OpenDlg::en_AES() {
	UpdateData(TRUE);
	if (!(Cpasswd.Compare(""))) {
		AfxMessageBox("비밀번호를 입력하지 않았습니다!");
		UpdateData(FALSE);
		return FALSE;
	}
	else {
		char *passwd = (char *)malloc(Cpasswd.GetLength());
		strcpy(passwd, (LPTSTR)(LPCTSTR)Cpasswd);
		fs_passwd_aes(passwd);
	}
	
	if (!(Cfile.Compare(""))) {
		AfxMessageBox("파일을 선택하지 않았습니다!");
		UpdateData(FALSE);
		return FALSE;
	}
	else {
		int del_num = path.GetLength() - 3;
		file_path = path.Left(del_num) + Cfile;

		char *infile = (char *)malloc(file_path.GetLength());
		strcpy(infile, (LPTSTR)(LPCTSTR)file_path);

		char *out_file = (char *)malloc(Cfile.GetLength() - 4);
		strcpy(out_file, (LPTSTR)(LPCTSTR)Cfile);
		for (int i = 0; i < file_path.GetLength(); i++) {
			if (out_file[i] == '.')
				if (out_file[i + 1] == 'a')
					if (out_file[i + 2] == 'e')
						if (out_file[i + 3] == 's') {
							out_file[i] = '\0';
						}

		}

		if (fs_decrypt_aes(infile, out_file)) {
			//AfxMessageBox("복호화에 성공하였습니다.");
			remove(infile);
			UpdateData(FALSE);
			return 1;
		}
		else {
			UpdateData(FALSE);
			return 0;
		}
	}
}

BOOL OpenDlg::en_DES() {
	UpdateData(TRUE);
	char *passwd;
	if (!(Cpasswd.Compare(""))) {
		AfxMessageBox("비밀번호를 입력하지 않았습니다!");
		UpdateData(FALSE);
		return FALSE;
	}
	else {
		passwd = (char *)malloc(Cpasswd.GetLength());
		strcpy(passwd, (LPTSTR)(LPCTSTR)Cpasswd);
		fs_passwd_des(passwd);
	}
	if (!(Cfile.Compare(""))) {
		AfxMessageBox("파일을 선택하지 않았습니다!");
		UpdateData(FALSE);
		return FALSE;
	}
	else {
		int del_num = path.GetLength() - 3;
		file_path = path.Left(del_num) + Cfile;

		char *infile = (char *)malloc(file_path.GetLength());
		strcpy(infile, (LPTSTR)(LPCTSTR)file_path);

		char *out_file = (char *)malloc(Cfile.GetLength() - 4);
		strcpy(out_file, (LPTSTR)(LPCTSTR)Cfile);
		for (int i = 0; i < file_path.GetLength(); i++) {
			if (out_file[i] == '.')
				if (out_file[i + 1] == 'd')
					if (out_file[i + 2] == 'e')
						if (out_file[i + 3] == 's') {
							out_file[i] = '\0';
						}

		}
		if (fs_decrypt_des(infile, passwd, out_file)) {
			//AfxMessageBox("복호화에 성공하였습니다.");
			remove(infile);
			UpdateData(FALSE);
			return 1;
		}
		else {
			UpdateData(FALSE);
			return 0;
		}
		UpdateData(FALSE);
	}
}

BOOL OpenDlg::en_Thr_DES() {
	UpdateData(TRUE);
	char *passwd;
	if (!(Cpasswd.Compare(""))) {
		AfxMessageBox("비밀번호를 입력하지 않았습니다!");
		UpdateData(FALSE);
		return FALSE;
	}
	else {
		passwd = (char *)malloc(Cpasswd.GetLength());
		strcpy(passwd, (LPTSTR)(LPCTSTR)Cpasswd);
		fs_passwd_3des(passwd);
	}
	if (!(Cfile.Compare(""))) {
		AfxMessageBox("파일을 선택하지 않았습니다!");
		UpdateData(FALSE);
		return FALSE;
	}
	else {
		int del_num = path.GetLength() - 3;
		file_path = path.Left(del_num) + Cfile;

		char *infile = (char *)malloc(file_path.GetLength());
		strcpy(infile, (LPTSTR)(LPCTSTR)file_path);

		char *out_file = (char *)malloc(Cfile.GetLength() - 4);
		strcpy(out_file, (LPTSTR)(LPCTSTR)Cfile);
		for (int i = 0; i < file_path.GetLength(); i++) {
			if (out_file[i] == '.')
				if (out_file[i + 1] == '3')
					if (out_file[i + 2] == 'e')
						if (out_file[i + 3] == 's') {
							out_file[i] = '\0';
						}

		}
		if (fs_decrypt_3des(infile, passwd, out_file)) {
			//AfxMessageBox("복호화에 성공하였습니다.");
			remove(infile);
			UpdateData(FALSE);
			return 1;
		}
		else {
			UpdateData(FALSE);
			return 0;
		}
		UpdateData(FALSE);
	}
}

int fs_decrypt_aes(char *in_file, char *out_file) {
	int len = 0;
	int total_size = 0;
	int save_len = 0;
	int w_len = 0;
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

	memset(iv, 0, sizeof(iv)); // the same iv
	AES_set_decrypt_key((const unsigned char *)md5Hash, KEY_BIT, &aes_ks3);

	fseek(fp, 0, SEEK_END);
	total_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	while (len = fread(buf, RW_SIZE, size, fp)) {
		if (size == 0) {
			break;
		}
		save_len += len;
		w_len = len;

		AES_cbc_encrypt((const unsigned char*)buf, (unsigned char*)buf, len, &aes_ks3, iv, AES_DECRYPT);
		if (save_len == total_size) { // check last block
			buf[len] = EOF;
		}

		fwrite(buf, RW_SIZE, w_len, wfp);
	}
	fclose(wfp);
	fclose(fp);
	return TRUE;
}


int fs_decrypt_des(char *infile, char *ckey, char *outfile) {
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
		DES_cfb64_encrypt((unsigned char *)buf, (unsigned char *)Res, file_size, &schedule, &key, &n, DES_DECRYPT);
		fwrite(Res, 1, len, wfp);

		fclose(wfp);
		fclose(fp);

		return 1;
	}
}


int fs_decrypt_3des(char *infile, char *ckey, char *outfile) {
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
		DES_ede3_cbc_encrypt((unsigned char *)buf, (unsigned char *)Res, file_size, &schedule1, &schedule2, &schedule3, &key1, DES_DECRYPT);
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


int fs_passwd_des(char *passwd) {
	int padding_len = 8 - strlen(passwd);
	char padding[8];
	int padding_num = passwd[strlen(passwd) - 1] - 1;
	memset(padding, padding_num, padding_len);
	padding[padding_len] = '\0';
	strcat(passwd, padding);
	return 1;
}


int fs_passwd_3des(char *passwd) {
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