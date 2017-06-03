#pragma once
#include "stdafx.h"
#include <windows.h>

/*
** 将UTF-8字符串转化为GBK字符串
** 参考了知乎用户Gomo Psivarh的答案思路
** Why？ JSON文件一般为UTF-8储存，json库也只支持UTF-8文件；
**       而在Windows下控制台的编码时GBK的，这导致了UTF-8编
**       码的中文字符无法在控制台上显示，需要转换为GBK。
*/
string convGBK(string& src) {
    const char* GBK_LOCALE_NAME = ".936"; //GBK在windows下的locale name
    wstring_convert<codecvt_byname<wchar_t, char, mbstate_t>> cvGBK(new codecvt_byname<wchar_t, char, mbstate_t>(GBK_LOCALE_NAME));
    wstring_convert<codecvt_utf8<wchar_t>> cvUTF8;

    wstring utf8_str = cvUTF8.from_bytes(src);
    string gbk_str = cvGBK.to_bytes(utf8_str);
    return gbk_str;
}
/*
** 以掩码形式获取密码
** 参考如下链接：http://www.cplusplus.com/forum/general/88853/
*/
string getpass(const char *prompt, bool show_asterisk = true)
{
    const char BACKSPACE = 8;
    const char RETURN = 13;

    string password;
    unsigned char ch = 0;

    cout << prompt;

    DWORD con_mode;
    DWORD dwRead;

    HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);

    GetConsoleMode(hIn, &con_mode);
    SetConsoleMode(hIn, con_mode & ~(ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT));

    while (ReadConsoleA(hIn, &ch, 1, &dwRead, NULL) && ch != RETURN)
    {
        if (ch == BACKSPACE)
        {
            if (password.length() != 0)
            {
                if (show_asterisk)
                    cout << "\b \b";
                password.resize(password.length() - 1);
            }
        }
        else
        {
            password += ch;
            if (show_asterisk)
                cout << '*';
        }
    }
    cout << endl;
    SetConsoleMode(hIn, con_mode);
    
    return password;
}
