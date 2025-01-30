#pragma once

#include "stdafx.h"

namespace dxultra
{

struct Shader
{
    Shader(const std::wstring &filenameFragment, const std::string &entryPoint,
           const std::string &target, bool compileFromSource = false);

    ComPtr<ID3DBlob> Code();

    static std::wstring ShaderSourceBasePath();
    static std::wstring ModuleFileName();
    static std::wstring ModulePath();

    static ComPtr<ID3DBlob> ReadBlob(const std::wstring &filename);
    static ComPtr<ID3DBlob> CompileFromSource(const std::wstring &filenameFragment,
                                              const std::string &entryPoint,
                                              const std::string &target);

  private:
    ComPtr<ID3DBlob> m_codeBlob;
    std::string m_entryPoint;
    std::string m_target;
};

} // namespace dxultra
