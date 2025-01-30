#include "Shader.h"

#include "Error.h"

namespace zdx
{

Shader::Shader(const std::wstring &filenameFragment, const std::string &entryPoint,
               const std::string &target, bool compileFromSource)
    : m_entryPoint{entryPoint}, m_target{target}
{
    if (compileFromSource)
    {
        CompileFromSource(filenameFragment, entryPoint, target);
    }
    else
    {
        std::wstring filename = ModulePath() + filenameFragment;
        m_codeBlob = ReadBlob(filename);
    }
}

ComPtr<ID3DBlob> Shader::Code()
{
    UINT size = m_codeBlob->GetBufferSize();
    void *pCode = m_codeBlob->GetBufferPointer();
    return m_codeBlob;
}

std::wstring Shader::ShaderSourceBasePath()
{
#ifdef DXULTRA_SHADER_PATH
#define WSTRING1(x) L#x
#define WSTRING(x) WSTRING1(x)
    const wchar_t *pShaderBasePath = WSTRING(DXULTRA_SHADER_PATH);
    if (pShaderBasePath && wcslen(pShaderBasePath) > 0)
    {
        std::wstring shaderPathOrig{pShaderBasePath};
        std::wstring shaderPath;
        for (wchar_t ch : shaderPathOrig)
        {
            if (ch == L'/')
            {
                shaderPath += L'\\';
            }
            else
            {
                shaderPath += ch;
            }
        }
        if (shaderPath.find_last_of('\\') != shaderPath.length())
        {
            shaderPath += '\\';
        }
        return shaderPath;
    }
#endif

    return ModulePath();
}

std::wstring Shader::ModuleFileName()
{
    DWORD capacity = MAX_PATH;

    std::vector<wchar_t> characters(capacity);

    DWORD size = GetModuleFileName(NULL, characters.data(), capacity);

    while (size == capacity && GetLastError() == ERROR_INSUFFICIENT_BUFFER)
    {
        capacity *= 2;
        characters.resize(capacity);
        size = GetModuleFileName(NULL, characters.data(), capacity);
    }

    return std::wstring{characters.data()};
}

std::wstring Shader::ModulePath()
{
    std::wstring filename = ModuleFileName();

    size_t pos = filename.find_last_of('\\');
    if (pos != std::wstring::npos)
    {
        filename = filename.substr(0, pos + 1);
        return filename;
    }

    return filename;
}

ComPtr<ID3DBlob> Shader::ReadBlob(const std::wstring &filename)
{
    UINT64 fileSize = std::filesystem::file_size(filename);

    std::fstream input{filename, std::ios_base::binary | std::ios_base::in};

    ComPtr<ID3DBlob> blob;
    ThrowIfFailed(D3DCreateBlob(fileSize, blob.GetAddressOf()));
    input.read(reinterpret_cast<char *>(blob->GetBufferPointer()), fileSize);

    if (!input)
    {
        std::streamsize numReadBytes = input.gcount();
        std::wstring output{std::format(L"*** read only {} bytes\n", numReadBytes)};
        OutputDebugString(output.c_str());
        // TODO: WideCharToMultiByte and throw
    }

    return blob;
}

ComPtr<ID3DBlob> Shader::CompileFromSource(const std::wstring &filenameFragment,
                                           const std::string &entryPoint, const std::string &target)
{
    static const std::wstring shaderSourceBasePath = ShaderSourceBasePath();

    const std::wstring filename = shaderSourceBasePath + filenameFragment;

    constexpr D3D_SHADER_MACRO *pDefines = nullptr;
    constexpr ID3DInclude *pInclude = nullptr;
    UINT flags1 = 0;
#ifdef _DEBUG
    flags1 |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
    constexpr UINT flags2 = 0;
    ComPtr<ID3DBlob> code;
    ComPtr<ID3DBlob> error;
    HRESULT hResult =
        D3DCompileFromFile(filename.c_str(), pDefines, pInclude, entryPoint.c_str(), target.c_str(),
                           flags1, flags2, code.GetAddressOf(), error.GetAddressOf());

    if (!SUCCEEDED(hResult) && error.Get())
    {
        SIZE_T errorBufferSize = error->GetBufferSize();
        if (errorBufferSize > 0)
        {
            OutputDebugStringA(reinterpret_cast<const char *>(error->GetBufferPointer()));
        }
    }

    ThrowIfFailed(hResult);

    return code;
}

} // namespace dxultra