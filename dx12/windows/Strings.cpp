#include "Strings.h"

namespace zdx
{

std::string WideCharToUTF8(const std::wstring &wstring)
{
    size_t len = wstring.length();
    size_t size = len + 1; // optimistic guess
    std::vector<char> utf8chars(size);
    size_t cbWritten =
        WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, wstring.c_str(), static_cast<int>(len),
                            utf8chars.data(), static_cast<int>(size), NULL, NULL);

    while (cbWritten == 0 && GetLastError() == ERROR_INSUFFICIENT_BUFFER)
    {
        size *= 2;
        utf8chars.resize(size);
        cbWritten = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, wstring.c_str(),
                                        static_cast<int>(len), utf8chars.data(),
                                        static_cast<int>(size), NULL, NULL);
    }

    if (cbWritten == 0)
    {
        auto error_msg_fn = [](DWORD error) {
            switch (error)
            {
            case ERROR_INVALID_FLAGS:
                return std::string{"WideCharToMultiByte ERROR_INVALID_FLAGS"};
            case ERROR_INVALID_PARAMETER:
                return std::string{"WideCharToMultiByte ERROR_INVALID_PARAMETER"};
            case ERROR_NO_UNICODE_TRANSLATION:
                return std::string{"WideCharToMultiByte ERROR_NO_UNICODE_TRANSLATION"};
            default:
                return std::format("WideCharToMultiByte unknown error: {}", error);
            }
        };
        throw std::runtime_error{error_msg_fn(GetLastError())};
    }

    return std::string{utf8chars.data(), cbWritten};
}

} // namespace zdx
