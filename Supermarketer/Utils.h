#pragma once
using namespace System;
using namespace System::Security::Cryptography;
using namespace System::Text;
static class Utils
{
public:
    static String^ getMD5String(String^ inputString)
    {
        array<Byte>^ byteArray = Encoding::ASCII->GetBytes(inputString);
        MD5CryptoServiceProvider^ md5provider = gcnew MD5CryptoServiceProvider();
        array<Byte>^ byteArrayHash = md5provider->ComputeHash(byteArray);
        return BitConverter::ToString(byteArrayHash);
    }
};

