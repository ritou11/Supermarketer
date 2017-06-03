#pragma once
#include "stdafx.h"
#include <windows.h>

string convGBK(string& src) {
    const char* GBK_LOCALE_NAME = ".936"; //GBKÔÚwindowsÏÂµÄlocale name
    wstring_convert<codecvt_byname<wchar_t, char, mbstate_t>> cvGBK(new codecvt_byname<wchar_t, char, mbstate_t>(GBK_LOCALE_NAME));
    wstring_convert<codecvt_utf8<wchar_t>> cvUTF8;

    wstring utf8_str = cvUTF8.from_bytes(src);
    string gbk_str = cvGBK.to_bytes(utf8_str);
    return gbk_str;
}

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
