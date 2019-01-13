# Encrypt-file
Explanation
---
It is a program that reads a desired file as binary value and encrypts the entire file using the encryption method that you want.
Supported encryption methods include AES, DES, and 3DES, and CDC mode. 3DES uses the Vee method.
Passwords to be used for login are hashed using SHA-1 method and stored in init.ini file.
The extension name of an encrypted file is appended with the extension name in each way, and if the extension and the decoding method do not match, the code does not become redundant.
Once encrypted, the original file is deleted and the encrypted files are hidden in a system-type hidden folder.
A compounded file deletes files in the hidden folder and creates the files in the location where the executable file is located.
The folder where the encryption file will be stored cannot be located with hidden view settings or dir commands.

Dependency
---
Developed in C++/MFC for Visual Studio 2017 in windows 10 environment, used OPENSSL

Generate Keys
---
1. AES: There is no limit to the password entered by the user and converts it to 128bit via MD5 hash.
2. DES: Users are entered with a limited number of passwords.
3. 3DES: User-input keys are limited, and keys to be used in each method are obtained through shift operations of the input keys.

How to Use
---
1. Login
2. Enter the encryption key
3. Encrypt via the encryption button and select the desired encryption method.
4. Decode it using the ventriloquize button. Select the desired compounding method.

파일 암호화
---

설명
---
원하는 파일을 바이너리값으로 읽어서 파일전체를 원하는 암호화 방식으로 암호화 하고 복호화 하는 프로그램이다.
지원하는 암호화 방식에는 AES, DES, 3DES가 있으며, CDC mode이다. 3DES는 ebe방식을 사용한다.
로그인에 사용할 비밀번호는 SHA-1방식으로 해시화하며, init.ini파일에 저장한다.
암호화된 파일의 확장자명은 각각의 방식으로 확장자 명이 붙으며, 복호화 시에 확장자명과 복호화 방식이 일치하지 않으면 복호화가 되지 않는다.
암호화가 되면 원본파일은 삭제가 되며, 암호화가 된 파일은 시스템형식으로 숨겨진 폴더에 숨겨진다.
복호화가 되면 숨겨진 폴더에 있는 파일은 삭제가 되며, 실행파일이 있는 위치에 복호화가 된 파일이 생긴다.
암호화 파일이 저장될 폴더는 숨김보기 설정이나 dir 명령어로는 위치를 찾을 수 없다.

의존성
---
windows 10환경, Visual Studio 2017에서 C++/MFC 및 OPENSSL을 사용했다.

키 생성
---
1. AES: 사용자에게 입력받은 비밀번호에 제한이 없으며, MD5해시를 통해 128bit로 변환한다.
2. DES: 사용자에게 비밀번호를 제한적으로 입력받는다.
3. 3DES: 사용자가 입력한 키를 제한적으로 받으며, 각각의 방식마다 사용할 키는 입력받은 키의 shift 연산을 통해 얻는다.

사용법
---
1. 로그인
2. 암호키 입력
3. 암호화 버튼을 통해 암호화하며, 원하는 암호화 방식을 선택한다.
4. 복호화 버튼을 통해 복호화하며, 원하는 복호화 방식을 선택한다.
