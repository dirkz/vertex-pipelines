#include "Strings.h"

namespace dxultra
{

std::string WideCharToUTF8(const std::wstring &wstring)
{
    size_t len = wstring.length();
    size_t size = len + 1; // optimistic guess
    std::vector<char> utf8chars(size);
    size_t cbWritten = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, wstring.c_str(), len,
                                           utf8chars.data(), size, NULL, NULL);

    while (cbWritten == 0 && GetLastError() == ERROR_INSUFFICIENT_BUFFER)
    {
        size *= 2;
        utf8chars.resize(size);
        cbWritten = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, wstring.c_str(), len,
                                        utf8chars.data(), size, NULL, NULL);
    }

    if (cbWritten == 0)
    {
        auto error_msg_fn = [](DWORD error) {
            switch (error)
            {
            case ERROR_INVALID_FLAGS:
                return "WideCharToMultiByte ERROR_INVALID_FLAGS";
            case ERROR_INVALID_PARAMETER:
                return "WideCharToMultiByte ERROR_INVALID_PARAMETER";
            case ERROR_NO_UNICODE_TRANSLATION:
                return "WideCharToMultiByte ERROR_NO_UNICODE_TRANSLATION";
            default:
                return std::format("WideCharToMultiByte unknown error: {}", error).c_str();
            }
        };
        std::string msg{error_msg_fn(GetLastError())};
        throw std::runtime_error{msg};
    }

    return std::string{utf8chars.data(), cbWritten};
}

} // namespace dxultra
