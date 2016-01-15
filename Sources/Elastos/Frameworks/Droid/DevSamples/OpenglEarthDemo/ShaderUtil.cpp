#include "ShaderUtil.h"
#include <elastos/utility/logging/Slogger.h>
#include "elastos/droid/ext/frameworkext.h"

using Elastos::IO::IInputStream;
using Elastos::IO::CByteArrayOutputStream;
using Elastos::IO::IByteArrayOutputStream;
using Elastos::Droid::Opengl::IGLES20;
using Elastos::Droid::Opengl::CGLES20;
using Elastos::Droid::Content::Res::IAssetManager;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace OpenglEarthDemo {

Int32 ShaderUtil::LoadShader(
    /* [in] */ Int32 shaderType,
    /* [in] */ const String& source)
{
    AutoPtr<IGLES20> gl;
    CGLES20::AcquireSingleton((IGLES20**)&gl);
    Int32 shader;
    ASSERT_SUCCEEDED(gl->GlCreateShader(shaderType, &shader));
    if (shader != 0)
    {
        ASSERT_SUCCEEDED(gl->GlShaderSource(shader, source));
        ASSERT_SUCCEEDED(gl->GlCompileShader(shader));
        AutoPtr<ArrayOf<Int32> > compiled = ArrayOf<Int32>::Alloc(1);
        ASSERT_SUCCEEDED(gl->GlGetShaderiv(shader, IGLES20::_GL_COMPILE_STATUS, compiled, 0));
        if ((*compiled)[0] == 0) {
            SLOGGERE("zhangjingcheng", "shader error")
            gl->GlDeleteShader(shader);
            shader = 0;
        }
    }
    return shader;
}

Int32 ShaderUtil::CreateProgram(
    /* [in] */ const String& vSource,
    /* [in] */ const String& fSource)
{
    AutoPtr<IGLES20> gl;
    CGLES20::AcquireSingleton((IGLES20**)&gl);
    Int32 vertexShader = LoadShader(IGLES20::_GL_VERTEX_SHADER, vSource);
    if (vertexShader == 0)
    {
        return 0;
    }

    Int32 pixelShader = LoadShader(IGLES20::_GL_FRAGMENT_SHADER, fSource);
    if (pixelShader == 0)
    {
        return 0;
    }

    Int32 program;
    ASSERT_SUCCEEDED(gl->GlCreateProgram(&program));
    if (program != 0)
    {
        ASSERT_SUCCEEDED(gl->GlAttachShader(program, vertexShader));
        ASSERT_SUCCEEDED(gl->GlAttachShader(program, pixelShader));
        ASSERT_SUCCEEDED(gl->GlLinkProgram(program));
        AutoPtr<ArrayOf<Int32> > linkStatus = ArrayOf<Int32>::Alloc(1);
        ASSERT_SUCCEEDED(gl->GlGetProgramiv(program, IGLES20::_GL_LINK_STATUS, linkStatus, 0));
        if ((*linkStatus)[0] != IGLES20::_GL_TRUE)
        {
            SLOGGERE("ES20_ERROR", "Could not link program")
            // Log.e("ES20_ERROR", "Could not link program: ");
            // Log.e("ES20_ERROR", GLES20.glGetProgramInfoLog(program));
            gl->GlDeleteProgram(program);
            program = 0;
        } else {
            SLOGGERE("ES20", "link program succees")
        }
    }
    return program;
}

String ShaderUtil::LoadFromAssetsFile(
    /* [in] */ const String& fName,
    /* [in] */ IResources* r)
{
    AutoPtr<IAssetManager> mgr;
    r->GetAssets((IAssetManager**)&mgr);
    AutoPtr<IInputStream> in;
    mgr->Open(fName, (IInputStream**)&in);
    Int32 ch = 0;
    AutoPtr<IByteArrayOutputStream> baos;
    CByteArrayOutputStream::New((IByteArrayOutputStream**)&baos);
    while((in->Read(&ch), ch) != -1)
    {
        baos->Write(ch);
    }
    AutoPtr<ArrayOf<Byte> > buf;
    baos->ToByteArray((ArrayOf<Byte>**)&buf);
    baos->Close();
    in->Close();
    return String((char const*)buf->GetPayload(), buf->GetLength());
}

} // namespace OpenglEarthDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
