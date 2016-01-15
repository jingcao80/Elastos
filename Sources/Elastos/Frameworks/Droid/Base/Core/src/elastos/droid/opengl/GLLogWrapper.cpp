#include "GLLogWrapper.h"
#include <elastos/core/StringUtils.h>
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Core::StringUtils;
using Elastos::IO::IFlushable;
using Elastos::IO::ByteOrder;
using Elastos::IO::IByteBufferHelper;
using Elastos::IO::CByteBufferHelper;
using Elastos::IO::IByteOrderHelper;
using Elastos::IO::CByteOrderHelper;
using Elastos::IO::IByteBuffer;
using Elastos::IO::ICharBuffer;
using Elastos::IO::IInt16Buffer;
using Elastos::IO::IInt32Buffer;
using Elastos::IO::IInt64Buffer;
using Elastos::IO::IDoubleBuffer;

namespace Elastos {
namespace Droid {
namespace Opengl {

GLLogWrapper::PointerInfo::PointerInfo(
    /* [in] */ GLLogWrapper* host)
    : mSize(0)
    , mType(0)
    , mStride(0)
    , mHost(host)
{}

GLLogWrapper::PointerInfo::PointerInfo(
    /* [in] */ Int32 size,
    /* [in] */ Int32 type,
    /* [in] */ Int32 stride,
    /* [in] */ IBuffer* pointer,
    /* [in] */ GLLogWrapper* host)
{
    mSize = size;
    mType = type;
    mStride = stride;
    mPointer = pointer;
    mHost = host;
}

Int32 GLLogWrapper::PointerInfo::Sizeof(
    /* [in] */ Int32 type)
{
    switch (type) {
    case IGL10::_GL_UNSIGNED_BYTE:
        return 1;
    case IGL10::_GL_BYTE:
        return 1;
    case IGL10::_GL_SHORT:
        return 2;
    case IGL10::_GL_FIXED:
        return 4;
    case IGL10::_GL_FLOAT:
        return 4;
    default:
        return 0;
    }
}

Int32 GLLogWrapper::PointerInfo::GetStride()
{
    return mStride > 0 ? mStride : Sizeof(mType) * mSize;
}

ECode GLLogWrapper::PointerInfo::BindByteBuffer()
{
    if (mPointer == NULL) {
        mTempByteBuffer = NULL;
    } else {
        mHost->ToByteBuffer(-1, mPointer, (IByteBuffer**)&mTempByteBuffer);
    }
    return NOERROR;
}

ECode GLLogWrapper::PointerInfo::UnbindByteBuffer()
{
    mTempByteBuffer = NULL;
    return NOERROR;
}

GLLogWrapper::GLLogWrapper(
    /* [in] */ IGL* gl,
    /* [in] */ IWriter* log,
    /* [in] */ Boolean logArgumentNames)
    : GLWrapperBase(gl)
{
    mLog = log;
    mLogArgumentNames = logArgumentNames;
}

ECode GLLogWrapper::CheckError()
{
    Int32 glError;
    if ((mgl->GlGetError(&glError), glError) != 0) {
        String errorMessage = String("glError") + StringUtils::Int32ToString(glError);
        LogLine(errorMessage);
    }
    return NOERROR;
}

ECode GLLogWrapper::LogLine(
    /* [in] */ const String& message)
{
    return Log(message + "\n");
}

ECode GLLogWrapper::Log(
    /* [in] */ const String& message)
{
    mLog->WriteString(message);
    return NOERROR;
}

ECode GLLogWrapper::Begin(
    /* [in] */ const String& name)
{
    Log(name + "(");
    mArgCount = 0;
    return NOERROR;
}

ECode GLLogWrapper::Arg(
    /* [in] */ const String& name,
    /* [in] */ const String& value)
{
    if (mArgCount++ > 0) {
        Log(String(", "));
    }
    if (mLogArgumentNames) {
        Log(name + "=");
    }
    Log(value);
    return NOERROR;
}

ECode GLLogWrapper::End()
{
    Log(String(");\n"));
    Flush();
    return NOERROR;
}

ECode GLLogWrapper::Flush()
{
    // try {
        if(FAILED(IFlushable::Probe(mLog)->Flush())) {
            mLog = NULL;
        }
    // } catch (IOException e) {
        // mLog = null;
    // }
    return NOERROR;
}

ECode GLLogWrapper::Arg(
    /* [in] */ const String& name,
    /* [in] */ Boolean value)
{
    return Arg(name, StringUtils::BooleanToString(value));
}

ECode GLLogWrapper::Arg(
    /* [in] */ const String& name,
    /* [in] */ Int32 value)
{
    return Arg(name, StringUtils::Int32ToString(value));
}

ECode GLLogWrapper::Arg(
    /* [in] */ const String& name,
    /* [in] */ Float value)
{
    return Arg(name, StringUtils::FloatToString(value));
}

ECode GLLogWrapper::Returns(
    /* [in] */ const String& result)
{
    Log(String(") Returns ") + result + ");\n");
    Flush();
    return NOERROR;
}

ECode GLLogWrapper::Returns(
    /* [in] */ Int32 result)
{
    Returns(StringUtils::Int32ToString(result));
    return NOERROR;
}

ECode GLLogWrapper::Arg(
    /* [in] */ const String& name,
    /* [in] */ Int32 n,
    /* [in] */ ArrayOf<Int32>* arr,
    /* [in] */ Int32 offset)
{
    return Arg(name, ToString(n, FORMAT_INT, arr, offset));
}

ECode GLLogWrapper::Arg(
    /* [in] */ const String& name,
    /* [in] */ Int32 n,
    /* [in] */ ArrayOf<Int16>* arr,
    /* [in] */ Int32 offset)
{
    return Arg(name, ToString(n, arr, offset));
}

ECode GLLogWrapper::Arg(
    /* [in] */ const String& name,
    /* [in] */ Int32 n,
    /* [in] */ ArrayOf<Float>* arr,
    /* [in] */ Int32 offset)
{
    return Arg(name, ToString(n, arr, offset));
}

ECode GLLogWrapper::FormattedAppend(
    /* [in] */ StringBuilder& buf,
    /* [in] */ Int32 value,
    /* [in] */ Int32 format)
{
    switch (format) {
    case FORMAT_INT:
        buf += value;
        break;
    case FORMAT_FLOAT:
        buf += Elastos::Core::Math::FloatToInt32Bits(value);
        break;
    case FORMAT_FIXED:
        buf += value / 65536.0f;
        break;
    }
    return NOERROR;
}

String GLLogWrapper::ToString(
    /* [in] */ Int32 n,
    /* [in] */ Int32 format,
    /* [in] */ ArrayOf<Int32>* arr,
    /* [in] */ Int32 offset)
{
    StringBuilder buf;
    buf += "{\n";
    Int32 arrLen = arr->GetLength();
    for (Int32 i = 0; i < n; i++) {
        Int32 index = offset + i;
        buf += " [";
        buf += index;
        buf += "] = ";
        if (index < 0 || index >= arrLen) {
            buf += "out of bounds";
        } else {
            FormattedAppend(buf, (*arr)[index], format);
        }
        buf += '\n';
    }
    buf += "}";
    return buf.ToString();
}

String GLLogWrapper::ToString(
    /* [in] */ Int32 n,
    /* [in] */ ArrayOf<Int16>* arr,
    /* [in] */ Int32 offset)
{
    StringBuilder buf;
    buf += "{\n";
    Int32 arrLen = arr->GetLength();
    for (Int32 i = 0; i < n; i++) {
        Int32 index = offset + i;
        buf += " [";
        buf += index;
        buf += "] = ";
        if (index < 0 || index >= arrLen) {
            buf += "out of bounds";
        } else {
            buf += (*arr)[index];
        }
        buf += '\n';
    }
    buf += "}";
    return buf.ToString();
}

String GLLogWrapper::ToString(
    /* [in] */ Int32 n,
    /* [in] */ ArrayOf<Float>* arr,
    /* [in] */ Int32 offset)
{
    StringBuilder buf;
    buf += "{\n";
    Int32 arrLen = arr->GetLength();
    for (Int32 i = 0; i < n; i++) {
        Int32 index = offset + i;
        buf += " [";
        buf += index;
        buf += "] = ";
        if (index < 0 || index >= arrLen) {
            buf += "out of bounds";
        } else {
            buf += (*arr)[index];
        }
        buf += '\n';
    }
    buf += "}";
    return buf.ToString();
}

String GLLogWrapper::ToString(
    /* [in] */ Int32 n,
    /* [in] */ IFloatBuffer* buf)
{
    StringBuilder builder;
    builder += "{\n";
    for (Int32 i = 0; i < n; i++) {
        builder += " [";
        builder += i;
        builder += "] = ";
        Float f;
        buf->GetFloat(i, &f);
        builder += f;
        builder += '\n';
    }
    builder += "}";
    return builder.ToString();
}

String GLLogWrapper::ToString(
    /* [in] */ Int32 n,
    /* [in] */ Int32 format,
    /* [in] */ IInt32Buffer* buf)
{
    StringBuilder builder;
    builder += "{\n";
    for (Int32 i = 0; i < n; i++) {
        builder += " [";
        builder += i;
        builder += "] = ";
        Int32 f;
        buf->GetInt32(i, &f);
        builder += FormattedAppend(builder, f, format);
        builder += '\n';
    }
    builder += "}";
    return builder.ToString();
}

String GLLogWrapper::ToString(
    /* [in] */ Int32 n,
    /* [in] */ IInt16Buffer* buf)
{
    StringBuilder builder;
    builder += "{\n";
    for (Int32 i = 0; i < n; i++) {
        builder += " [";
        builder += i;
        builder += "] = ";
        Int16 f;
        buf->GetInt16(i, &f);
        builder += f;
        builder += '\n';
    }
    builder += "}";
    return builder.ToString();
}

ECode GLLogWrapper::Arg(
    /* [in] */ const String& name,
    /* [in] */ Int32 n,
    /* [in] */ IFloatBuffer* buf)
{
    return Arg(name, ToString(n, buf));
}

ECode GLLogWrapper::Arg(
    /* [in] */ const String& name,
    /* [in] */ Int32 n,
    /* [in] */ IInt32Buffer* buf)
{
    return Arg(name, ToString(n, FORMAT_INT, buf));
}

ECode GLLogWrapper::Arg(
    /* [in] */ const String& name,
    /* [in] */ Int32 n,
    /* [in] */ IInt16Buffer* buf)
{
    return Arg(name, ToString(n, buf));
}

ECode GLLogWrapper::ArgPointer(
    /* [in] */ Int32 size,
    /* [in] */ Int32 type,
    /* [in] */ Int32 stride,
    /* [in] */ IBuffer* pointer)
{
    Arg(String("size"), size);
    Arg(String("type"), GetPointerTypeName(type));
    Arg(String("stride"), stride);
    Arg(String("pointer"), ParamsToString(pointer));
    return NOERROR;
}

String GLLogWrapper::GetHex(
    /* [in] */ Int32 value)
{
    return String("0x") + StringUtils::Int32ToString(value, 16);
}

String GLLogWrapper::GetErrorString(
    /* [in] */ Int32 error)
{
    switch (error) {
    case IGL10::_GL_NO_ERROR:
        return String("GL_NO_ERROR");
    case IGL10::_GL_INVALID_ENUM:
        return String("GL_INVALID_ENUM");
    case IGL10::_GL_INVALID_VALUE:
        return String("GL_INVALID_VALUE");
    case IGL10::_GL_INVALID_OPERATION:
        return String("GL_INVALID_OPERATION");
    case IGL10::_GL_STACK_OVERFLOW:
        return String("GL_STACK_OVERFLOW");
    case IGL10::_GL_STACK_UNDERFLOW:
        return String("GL_STACK_UNDERFLOW");
    case IGL10::_GL_OUT_OF_MEMORY:
        return String("GL_OUT_OF_MEMORY");
    default:
        return GetHex(error);
    }
}

String GLLogWrapper::GetClearBufferMask(
    /* [in] */ Int32 mask)
{
    StringBuilder b;
    if ((mask & IGL10::_GL_DEPTH_BUFFER_BIT) != 0) {
        b += "GL_DEPTH_BUFFER_BIT";
        mask &= ~IGL10::_GL_DEPTH_BUFFER_BIT;
    }
    if ((mask & IGL10::_GL_STENCIL_BUFFER_BIT) != 0) {
        Int32 len;
        b.GetLength(&len);
        if (len > 0) {
            b += " | ";
        }
        b += "GL_STENCIL_BUFFER_BIT";
        mask &= ~IGL10::_GL_STENCIL_BUFFER_BIT;
    }
    if ((mask & IGL10::_GL_COLOR_BUFFER_BIT) != 0) {
        Int32 len;
        b.GetLength(&len);
        if (len > 0) {
           b += " | ";
        }
        b += "GL_COLOR_BUFFER_BIT";
        mask &= ~IGL10::_GL_COLOR_BUFFER_BIT;
    }
    if (mask != 0) {
        Int32 len;
        b.GetLength(&len);
        if (len > 0) {
            b += " | ";
        }
        b += GetHex(mask);
    }
    return b.ToString();
}

String GLLogWrapper::GetFactor(
    /* [in] */ Int32 factor)
{
    switch(factor) {
    case IGL10::_GL_ZERO:
        return String("GL_ZERO");
    case IGL10::_GL_ONE:
        return String("GL_ONE");
    case IGL10::_GL_SRC_COLOR:
        return String("GL_SRC_COLOR");
    case IGL10::_GL_ONE_MINUS_SRC_COLOR:
        return String("GL_ONE_MINUS_SRC_COLOR");
    case IGL10::_GL_DST_COLOR:
        return String("GL_DST_COLOR");
    case IGL10::_GL_ONE_MINUS_DST_COLOR:
        return String("GL_ONE_MINUS_DST_COLOR");
    case IGL10::_GL_SRC_ALPHA:
        return String("GL_SRC_ALPHA");
    case IGL10::_GL_ONE_MINUS_SRC_ALPHA:
        return String("GL_ONE_MINUS_SRC_ALPHA");
    case IGL10::_GL_DST_ALPHA:
        return String("GL_DST_ALPHA");
    case IGL10::_GL_ONE_MINUS_DST_ALPHA:
        return String("GL_ONE_MINUS_DST_ALPHA");
    case IGL10::_GL_SRC_ALPHA_SATURATE:
        return String("GL_SRC_ALPHA_SATURATE");

    default:
        return GetHex(factor);
    }
}

String GLLogWrapper::GetShadeModel(
    /* [in] */ Int32 model)
{
    switch(model) {
    case IGL10::_GL_FLAT:
        return String("GL_FLAT");
    case IGL10::_GL_SMOOTH:
        return String("GL_SMOOTH");
    default:
        return GetHex(model);
    }
}

String GLLogWrapper::GetTextureTarget(
    /* [in] */ Int32 target)
{
    switch (target) {
    case IGL10::_GL_TEXTURE_2D:
        return String("GL_TEXTURE_2D");
    default:
        return GetHex(target);
    }
}

String GLLogWrapper::GetTextureEnvTarget(
    /* [in] */ Int32 target)
{
    switch (target) {
    case IGL10::_GL_TEXTURE_ENV:
        return String("GL_TEXTURE_ENV");
    default:
        return GetHex(target);
    }
}

String GLLogWrapper::GetTextureEnvPName(
    /* [in] */ Int32 pname)
{
    switch (pname) {
    case IGL10::_GL_TEXTURE_ENV_MODE:
        return String("GL_TEXTURE_ENV_MODE");
    case IGL10::_GL_TEXTURE_ENV_COLOR:
        return String("GL_TEXTURE_ENV_COLOR");
    default:
        return GetHex(pname);
    }
}

Int32 GLLogWrapper::GetTextureEnvParamCount(
    /* [in] */ Int32 pname)
{
    switch (pname) {
    case IGL10::_GL_TEXTURE_ENV_MODE:
        return 1;
    case IGL10::_GL_TEXTURE_ENV_COLOR:
        return 4;
    default:
        return 0;
    }
}

String GLLogWrapper::GetTextureEnvParamName(
    /* [in] */ Float param)
{
    Int32 iparam = (Int32) param;
    if (param == (Float) iparam) {
        switch (iparam) {
        case IGL10::_GL_REPLACE:
            return String("GL_REPLACE");
        case IGL10::_GL_MODULATE:
            return String("GL_MODULATE");
        case IGL10::_GL_DECAL:
            return String("GL_DECAL");
        case IGL10::_GL_BLEND:
            return String("GL_BLEND");
        case IGL10::_GL_ADD:
            return String("GL_ADD");
        case IGL11::_GL_COMBINE:
            return String("GL_COMBINE");
        default:
            return GetHex(iparam);
        }
    }
    return StringUtils::FloatToString(param);;
}

String GLLogWrapper::GetMatrixMode(
    /* [in] */ Int32 matrixMode)
{
    switch (matrixMode) {
    case IGL10::_GL_MODELVIEW:
        return String("GL_MODELVIEW");
    case IGL10::_GL_PROJECTION:
        return String("GL_PROJECTION");
    case IGL10::_GL_TEXTURE:
        return String("GL_TEXTURE");
    default:
        return GetHex(matrixMode);
    }
}

String GLLogWrapper::GetClientState(
    /* [in] */ Int32 clientState)
{
    switch (clientState) {
    case IGL10::_GL_COLOR_ARRAY:
        return String("GL_COLOR_ARRAY");
    case IGL10::_GL_VERTEX_ARRAY:
        return String("GL_VERTEX_ARRAY");
    case IGL10::_GL_NORMAL_ARRAY:
        return String("GL_NORMAL_ARRAY");
    case IGL10::_GL_TEXTURE_COORD_ARRAY:
        return String("GL_TEXTURE_COORD_ARRAY");
    default:
        return GetHex(clientState);
    }
}

String GLLogWrapper::GetCap(
    /* [in] */ Int32 cap)
{
    switch (cap) {
    case IGL10::_GL_FOG:
        return String("GL_FOG");
    case IGL10::_GL_LIGHTING:
        return String("GL_LIGHTING");
    case IGL10::_GL_TEXTURE_2D:
        return String("GL_TEXTURE_2D");
    case IGL10::_GL_CULL_FACE:
        return String("GL_CULL_FACE");
    case IGL10::_GL_ALPHA_TEST:
        return String("GL_ALPHA_TEST");
    case IGL10::_GL_BLEND:
        return String("GL_BLEND");
    case IGL10::_GL_COLOR_LOGIC_OP:
        return String("GL_COLOR_LOGIC_OP");
    case IGL10::_GL_DITHER:
        return String("GL_DITHER");
    case IGL10::_GL_STENCIL_TEST:
        return String("GL_STENCIL_TEST");
    case IGL10::_GL_DEPTH_TEST:
        return String("GL_DEPTH_TEST");
    case IGL10::_GL_LIGHT0:
        return String("GL_LIGHT0");
    case IGL10::_GL_LIGHT1:
        return String("GL_LIGHT1");
    case IGL10::_GL_LIGHT2:
        return String("GL_LIGHT2");
    case IGL10::_GL_LIGHT3:
        return String("GL_LIGHT3");
    case IGL10::_GL_LIGHT4:
        return String("GL_LIGHT4");
    case IGL10::_GL_LIGHT5:
        return String("GL_LIGHT5");
    case IGL10::_GL_LIGHT6:
        return String("GL_LIGHT6");
    case IGL10::_GL_LIGHT7:
        return String("GL_LIGHT7");
    case IGL10::_GL_POINT_SMOOTH:
        return String("GL_POINT_SMOOTH");
    case IGL10::_GL_LINE_SMOOTH:
        return String("GL_LINE_SMOOTH");
    case IGL10::_GL_COLOR_MATERIAL:
        return String("GL_COLOR_MATERIAL");
    case IGL10::_GL_NORMALIZE:
        return String("GL_NORMALIZE");
    case IGL10::_GL_RESCALE_NORMAL:
        return String("GL_RESCALE_NORMAL");
    case IGL10::_GL_VERTEX_ARRAY:
        return String("GL_VERTEX_ARRAY");
    case IGL10::_GL_NORMAL_ARRAY:
        return String("GL_NORMAL_ARRAY");
    case IGL10::_GL_COLOR_ARRAY:
        return String("GL_COLOR_ARRAY");
    case IGL10::_GL_TEXTURE_COORD_ARRAY:
        return String("GL_TEXTURE_COORD_ARRAY");
    case IGL10::_GL_MULTISAMPLE:
        return String("GL_MULTISAMPLE");
    case IGL10::_GL_SAMPLE_ALPHA_TO_COVERAGE:
        return String("GL_SAMPLE_ALPHA_TO_COVERAGE");
    case IGL10::_GL_SAMPLE_ALPHA_TO_ONE:
        return String("GL_SAMPLE_ALPHA_TO_ONE");
    case IGL10::_GL_SAMPLE_COVERAGE:
        return String("GL_SAMPLE_COVERAGE");
    case IGL10::_GL_SCISSOR_TEST:
        return String("GL_SCISSOR_TEST");
    default:
        return GetHex(cap);
    }
}

String GLLogWrapper::GetTexturePName(
    /* [in] */ Int32 pname)
{
    switch (pname) {
    case IGL10::_GL_TEXTURE_MAG_FILTER:
        return String("GL_TEXTURE_MAG_FILTER");
    case IGL10::_GL_TEXTURE_MIN_FILTER:
        return String("GL_TEXTURE_MIN_FILTER");
    case IGL10::_GL_TEXTURE_WRAP_S:
        return String("GL_TEXTURE_WRAP_S");
    case IGL10::_GL_TEXTURE_WRAP_T:
        return String("GL_TEXTURE_WRAP_T");
    case IGL11::_GL_GENERATE_MIPMAP:
        return String("GL_GENERATE_MIPMAP");
    case IGL11Ext::_GL_TEXTURE_CROP_RECT_OES:
        return String("GL_TEXTURE_CROP_RECT_OES");
    default:
        return GetHex(pname);
    }
}

String GLLogWrapper::GetTextureParamName(
    /* [in] */ Float param)
{
    Int32 iparam = (Int32) param;
    if (param == (Float) iparam) {
        switch (iparam) {
        case IGL10::_GL_CLAMP_TO_EDGE:
            return String("GL_CLAMP_TO_EDGE");
        case IGL10::_GL_REPEAT:
            return String("GL_REPEAT");
        case IGL10::_GL_NEAREST:
            return String("GL_NEAREST");
        case IGL10::_GL_LINEAR:
            return String("GL_LINEAR");
        case IGL10::_GL_NEAREST_MIPMAP_NEAREST:
            return String("GL_NEAREST_MIPMAP_NEAREST");
        case IGL10::_GL_LINEAR_MIPMAP_NEAREST:
            return String("GL_LINEAR_MIPMAP_NEAREST");
        case IGL10::_GL_NEAREST_MIPMAP_LINEAR:
            return String("GL_NEAREST_MIPMAP_LINEAR");
        case IGL10::_GL_LINEAR_MIPMAP_LINEAR:
            return String("GL_LINEAR_MIPMAP_LINEAR");
        default:
            return GetHex(iparam);
        }
    }
    return StringUtils::FloatToString(param);
}

String GLLogWrapper::GetFogPName(
    /* [in] */ Int32 pname)
{
        switch (pname) {
        case IGL10::_GL_FOG_DENSITY:
            return String("GL_FOG_DENSITY");
        case IGL10::_GL_FOG_START:
            return String("GL_FOG_START");
        case IGL10::_GL_FOG_END:
            return String("GL_FOG_END");
        case IGL10::_GL_FOG_MODE:
            return String("GL_FOG_MODE");
        case IGL10::_GL_FOG_COLOR:
            return String("GL_FOG_COLOR");
        default:
            return GetHex(pname);
        }
}

Int32 GLLogWrapper::GetFogParamCount(
    /* [in] */ Int32 pname)
{
    switch (pname) {
    case IGL10::_GL_FOG_DENSITY:
        return 1;
    case IGL10::_GL_FOG_START:
        return 1;
    case IGL10::_GL_FOG_END:
        return 1;
    case IGL10::_GL_FOG_MODE:
        return 1;
    case IGL10::_GL_FOG_COLOR:
        return 4;
    default:
        return 0;
    }
}

String GLLogWrapper::GetBeginMode(
    /* [in] */ Int32 mode)
{
    switch (mode) {
    case IGL10::_GL_POINTS:
        return String("GL_POINTS");
    case IGL10::_GL_LINES:
        return String("GL_LINES");
    case IGL10::_GL_LINE_LOOP:
        return String("GL_LINE_LOOP");
    case IGL10::_GL_LINE_STRIP:
        return String("GL_LINE_STRIP");
    case IGL10::_GL_TRIANGLES:
        return String("GL_TRIANGLES");
    case IGL10::_GL_TRIANGLE_STRIP:
        return String("GL_TRIANGLE_STRIP");
    case IGL10::_GL_TRIANGLE_FAN:
        return String("GL_TRIANGLE_FAN");
    default:
        return GetHex(mode);
    }
}

String GLLogWrapper::GetIndexType(
    /* [in] */ Int32 type)
{
    switch (type) {
    case IGL10::_GL_UNSIGNED_SHORT:
        return String("GL_UNSIGNED_SHORT");
    case IGL10::_GL_UNSIGNED_BYTE:
        return String("GL_UNSIGNED_BYTE");
    default:
        return GetHex(type);
    }
}

String GLLogWrapper::GetIntegerStateName(
    /* [in] */ Int32 pname)
{
    switch (pname) {
    case IGL10::_GL_ALPHA_BITS:
        return String("GL_ALPHA_BITS");
    case IGL10::_GL_ALIASED_LINE_WIDTH_RANGE:
        return String("GL_ALIASED_LINE_WIDTH_RANGE");
    case IGL10::_GL_ALIASED_POINT_SIZE_RANGE:
        return String("GL_ALIASED_POINT_SIZE_RANGE");
    case IGL10::_GL_BLUE_BITS:
        return String("GL_BLUE_BITS");
    case IGL10::_GL_COMPRESSED_TEXTURE_FORMATS:
        return String("GL_COMPRESSED_TEXTURE_FORMATS");
    case IGL10::_GL_DEPTH_BITS:
        return String("GL_DEPTH_BITS");
    case IGL10::_GL_GREEN_BITS:
        return String("GL_GREEN_BITS");
    case IGL10::_GL_MAX_ELEMENTS_INDICES:
        return String("GL_MAX_ELEMENTS_INDICES");
    case IGL10::_GL_MAX_ELEMENTS_VERTICES:
        return String("GL_MAX_ELEMENTS_VERTICES");
    case IGL10::_GL_MAX_LIGHTS:
        return String("GL_MAX_LIGHTS");
    case IGL10::_GL_MAX_TEXTURE_SIZE:
        return String("GL_MAX_TEXTURE_SIZE");
    case IGL10::_GL_MAX_VIEWPORT_DIMS:
        return String("GL_MAX_VIEWPORT_DIMS");
    case IGL10::_GL_MAX_MODELVIEW_STACK_DEPTH:
        return String("GL_MAX_MODELVIEW_STACK_DEPTH");
    case IGL10::_GL_MAX_PROJECTION_STACK_DEPTH:
        return String("GL_MAX_PROJECTION_STACK_DEPTH");
    case IGL10::_GL_MAX_TEXTURE_STACK_DEPTH:
        return String("GL_MAX_TEXTURE_STACK_DEPTH");
    case IGL10::_GL_MAX_TEXTURE_UNITS:
        return String("GL_MAX_TEXTURE_UNITS");
    case IGL10::_GL_NUM_COMPRESSED_TEXTURE_FORMATS:
        return String("GL_NUM_COMPRESSED_TEXTURE_FORMATS");
    case IGL10::_GL_RED_BITS:
        return String("GL_RED_BITS");
    case IGL10::_GL_SMOOTH_LINE_WIDTH_RANGE:
        return String("GL_SMOOTH_LINE_WIDTH_RANGE");
    case IGL10::_GL_SMOOTH_POINT_SIZE_RANGE:
        return String("GL_SMOOTH_POINT_SIZE_RANGE");
    case IGL10::_GL_STENCIL_BITS:
        return String("GL_STENCIL_BITS");
    case IGL10::_GL_SUBPIXEL_BITS:
        return String("GL_SUBPIXEL_BITS");

    case IGL11::_GL_MODELVIEW_MATRIX_FLOAT_AS_INT_BITS_OES:
        return String("GL_MODELVIEW_MATRIX_FLOAT_AS_INT_BITS_OES");
    case IGL11::_GL_PROJECTION_MATRIX_FLOAT_AS_INT_BITS_OES:
        return String("GL_PROJECTION_MATRIX_FLOAT_AS_INT_BITS_OES");
    case IGL11::_GL_TEXTURE_MATRIX_FLOAT_AS_INT_BITS_OES:
        return String("GL_TEXTURE_MATRIX_FLOAT_AS_INT_BITS_OES");

    default:
        return GetHex(pname);
    }
}

Int32 GLLogWrapper::GetIntegerStateSize(
    /* [in] */ Int32 pname)
{
    switch (pname) {
    case IGL10::_GL_ALPHA_BITS:
        return 1;
    case IGL10::_GL_ALIASED_LINE_WIDTH_RANGE:
        return 2;
    case IGL10::_GL_ALIASED_POINT_SIZE_RANGE:
        return 2;
    case IGL10::_GL_BLUE_BITS:
        return 1;
    case IGL10::_GL_COMPRESSED_TEXTURE_FORMATS:
        // Have to ask the implementation for the size
    {
        AutoPtr<ArrayOf<Int32> > buffer = ArrayOf<Int32>::Alloc(1);
        mgl->GlGetIntegerv(IGL10::_GL_NUM_COMPRESSED_TEXTURE_FORMATS, buffer, 0);
        return (*buffer)[0];
    }
    case IGL10::_GL_DEPTH_BITS:
        return 1;
    case IGL10::_GL_GREEN_BITS:
        return 1;
    case IGL10::_GL_MAX_ELEMENTS_INDICES:
        return 1;
    case IGL10::_GL_MAX_ELEMENTS_VERTICES:
        return 1;
    case IGL10::_GL_MAX_LIGHTS:
        return 1;
    case IGL10::_GL_MAX_TEXTURE_SIZE:
        return 1;
    case IGL10::_GL_MAX_VIEWPORT_DIMS:
        return 2;
    case IGL10::_GL_MAX_MODELVIEW_STACK_DEPTH:
        return 1;
    case IGL10::_GL_MAX_PROJECTION_STACK_DEPTH:
        return 1;
    case IGL10::_GL_MAX_TEXTURE_STACK_DEPTH:
        return 1;
    case IGL10::_GL_MAX_TEXTURE_UNITS:
        return 1;
    case IGL10::_GL_NUM_COMPRESSED_TEXTURE_FORMATS:
        return 1;
    case IGL10::_GL_RED_BITS:
        return 1;
    case IGL10::_GL_SMOOTH_LINE_WIDTH_RANGE:
        return 2;
    case IGL10::_GL_SMOOTH_POINT_SIZE_RANGE:
        return 2;
    case IGL10::_GL_STENCIL_BITS:
        return 1;
    case IGL10::_GL_SUBPIXEL_BITS:
        return 1;

    case IGL11::_GL_MODELVIEW_MATRIX_FLOAT_AS_INT_BITS_OES:
    case IGL11::_GL_PROJECTION_MATRIX_FLOAT_AS_INT_BITS_OES:
    case IGL11::_GL_TEXTURE_MATRIX_FLOAT_AS_INT_BITS_OES:
        return 16;

    default:
        return 0;
    }
}

Int32 GLLogWrapper::GetIntegerStateFormat(
    /* [in] */ Int32 pname)
{
    switch (pname) {
    case IGL11::_GL_MODELVIEW_MATRIX_FLOAT_AS_INT_BITS_OES:
    case IGL11::_GL_PROJECTION_MATRIX_FLOAT_AS_INT_BITS_OES:
    case IGL11::_GL_TEXTURE_MATRIX_FLOAT_AS_INT_BITS_OES:
        return FORMAT_FLOAT;

    default:
        return FORMAT_INT;
    }
}

String GLLogWrapper::GetHintTarget(
    /* [in] */ Int32 target)
{
    switch (target) {
    case IGL10::_GL_FOG_HINT:
        return String("GL_FOG_HINT");
    case IGL10::_GL_LINE_SMOOTH_HINT:
        return String("GL_LINE_SMOOTH_HINT");
    case IGL10::_GL_PERSPECTIVE_CORRECTION_HINT:
        return String("GL_PERSPECTIVE_CORRECTION_HINT");
    case IGL10::_GL_POINT_SMOOTH_HINT:
        return String("GL_POINT_SMOOTH_HINT");
    case IGL10::_GL_POLYGON_SMOOTH_HINT:
        return String("GL_POLYGON_SMOOTH_HINT");
    case IGL11::_GL_GENERATE_MIPMAP_HINT:
        return String("GL_GENERATE_MIPMAP_HINT");
    default:
        return GetHex(target);
    }
}

String GLLogWrapper::GetHintMode(
    /* [in] */ Int32 mode)
{
    switch (mode) {
    case IGL10::_GL_FASTEST:
        return String("GL_FASTEST");
    case IGL10::_GL_NICEST:
        return String("GL_NICEST");
    case IGL10::_GL_DONT_CARE:
        return String("GL_DONT_CARE");
    default:
        return GetHex(mode);
    }
}

String GLLogWrapper::GetFaceName(
    /* [in] */ Int32 face)
{
    switch (face) {
    case IGL10::_GL_FRONT_AND_BACK:
        return String("GL_FRONT_AND_BACK");
    default:
        return GetHex(face);
    }
}

String GLLogWrapper::GetMaterialPName(
    /* [in] */ Int32 pname)
{
    switch (pname) {
    case IGL10::_GL_AMBIENT:
        return String("GL_AMBIENT");
    case IGL10::_GL_DIFFUSE:
        return String("GL_DIFFUSE");
    case IGL10::_GL_SPECULAR:
        return String("GL_SPECULAR");
    case IGL10::_GL_EMISSION:
        return String("GL_EMISSION");
    case IGL10::_GL_SHININESS:
        return String("GL_SHININESS");
    case IGL10::_GL_AMBIENT_AND_DIFFUSE:
        return String("GL_AMBIENT_AND_DIFFUSE");
    default:
        return GetHex(pname);
    }
}

Int32 GLLogWrapper::GetMaterialParamCount(
    /* [in] */ Int32 pname)
{
    switch (pname) {
    case IGL10::_GL_AMBIENT:
        return 4;
    case IGL10::_GL_DIFFUSE:
        return 4;
    case IGL10::_GL_SPECULAR:
        return 4;
    case IGL10::_GL_EMISSION:
        return 4;
    case IGL10::_GL_SHININESS:
        return 1;
    case IGL10::_GL_AMBIENT_AND_DIFFUSE:
        return 4;
    default:
        return 0;
    }
}

String GLLogWrapper::GetLightName(
    /* [in] */ Int32 light)
{
    if (light >= IGL10::_GL_LIGHT0 && light <= IGL10::_GL_LIGHT7) {
        return String("GL_LIGHT") + StringUtils::Int32ToString(light);
    }
    return GetHex(light);
}

String GLLogWrapper::GetLightPName(
    /* [in] */ Int32 pname)
{
    switch (pname) {
    case IGL10::_GL_AMBIENT:
        return String("GL_AMBIENT");
    case IGL10::_GL_DIFFUSE:
        return String("GL_DIFFUSE");
    case IGL10::_GL_SPECULAR:
        return String("GL_SPECULAR");
    case IGL10::_GL_POSITION:
        return String("GL_POSITION");
    case IGL10::_GL_SPOT_DIRECTION:
        return String("GL_SPOT_DIRECTION");
    case IGL10::_GL_SPOT_EXPONENT:
        return String("GL_SPOT_EXPONENT");
    case IGL10::_GL_SPOT_CUTOFF:
        return String("GL_SPOT_CUTOFF");
    case IGL10::_GL_CONSTANT_ATTENUATION:
        return String("GL_CONSTANT_ATTENUATION");
    case IGL10::_GL_LINEAR_ATTENUATION:
        return String("GL_LINEAR_ATTENUATION");
    case IGL10::_GL_QUADRATIC_ATTENUATION:
        return String("GL_QUADRATIC_ATTENUATION");
    default:
        return GetHex(pname);
    }
}

Int32 GLLogWrapper::GetLightParamCount(
    /* [in] */ Int32 pname)
{
    switch (pname) {
    case IGL10::_GL_AMBIENT:
        return 4;
    case IGL10::_GL_DIFFUSE:
        return 4;
    case IGL10::_GL_SPECULAR:
        return 4;
    case IGL10::_GL_POSITION:
        return 4;
    case IGL10::_GL_SPOT_DIRECTION:
        return 3;
    case IGL10::_GL_SPOT_EXPONENT:
        return 1;
    case IGL10::_GL_SPOT_CUTOFF:
        return 1;
    case IGL10::_GL_CONSTANT_ATTENUATION:
        return 1;
    case IGL10::_GL_LINEAR_ATTENUATION:
        return 1;
    case IGL10::_GL_QUADRATIC_ATTENUATION:
        return 1;
    default:
        return 0;
    }
}

String GLLogWrapper::GetLightModelPName(
    /* [in] */ Int32 pname)
{
    switch (pname) {
    case IGL10::_GL_LIGHT_MODEL_AMBIENT:
        return String("GL_LIGHT_MODEL_AMBIENT");
    case IGL10::_GL_LIGHT_MODEL_TWO_SIDE:
        return String("GL_LIGHT_MODEL_TWO_SIDE");
    default:
        return GetHex(pname);
    }
}

Int32 GLLogWrapper::GetLightModelParamCount(
    /* [in] */ Int32 pname)
{
    switch (pname) {
    case IGL10::_GL_LIGHT_MODEL_AMBIENT:
        return 4;
    case IGL10::_GL_LIGHT_MODEL_TWO_SIDE:
        return 1;
    default:
        return 0;
    }
}

String GLLogWrapper::GetPointerTypeName(
    /* [in] */ Int32 type)
{
    switch (type) {
    case IGL10::_GL_BYTE:
        return String("GL_BYTE");
    case IGL10::_GL_UNSIGNED_BYTE:
        return String("GL_UNSIGNED_BYTE");
    case IGL10::_GL_SHORT:
        return String("GL_SHORT");
    case IGL10::_GL_FIXED:
        return String("GL_FIXED");
    case IGL10::_GL_FLOAT:
        return String("GL_FLOAT");
    default:
        return GetHex(type);
    }
}

ECode GLLogWrapper::ToByteBuffer(
    /* [in] */ Int32 count,
    /* [in] */ IBuffer* input,
    /* [in] */ IByteBuffer** rBuf)
{
    AutoPtr<IByteBuffer> result;
    Int32 byteCount = count;
    Boolean convertWholeBuffer = (count < 0);
    AutoPtr<IByteBufferHelper> byteHelper;
    CByteBufferHelper::AcquireSingleton((IByteBufferHelper**)&byteHelper);
    if (IByteBuffer::Probe(input)) {
        AutoPtr<IByteBuffer> input2 = IByteBuffer::Probe(input);
        Int32 position;
        input2->GetPosition(&position);
        if (convertWholeBuffer) {
            Int32 limit;
            input2->GetLimit(&limit);
            byteCount = limit - position;
        }

        byteHelper->Allocate(byteCount, (IByteBuffer**)&result);
        ByteOrder order;
        input2->GetOrder(&order);
        result->SetOrder(order);
        for (Int32 i = 0; i < byteCount; i++) {
            Byte v;
            input2->GetByte(&v);
            result->PutByte(v);
        }
        input2->SetPosition(position);
    } else if (ICharBuffer::Probe(input)) {
        AutoPtr<ICharBuffer> input2 = ICharBuffer::Probe(input);
        Int32 position;
        input2->GetPosition(&position);
        if (convertWholeBuffer) {
            Int32 limit;
            input2->GetLimit(&limit);
            byteCount = (limit - position) * 4;
        }

        byteHelper->Allocate(byteCount, (IByteBuffer**)&result);
        ByteOrder order;
        input2->GetOrder(&order);
        result->SetOrder(order);
        AutoPtr<ICharBuffer> result2;
        result->AsCharBuffer((ICharBuffer**)&result2);
        for (Int32 i = 0; i < byteCount / 4; i++) {
            Char32 v;
            input2->GetChar(&v);
            result2->PutChar(v);
        }
        input2->SetPosition(position);
    } else if (IInt16Buffer::Probe(input)) {
        AutoPtr<IInt16Buffer> input2 = IInt16Buffer::Probe(input);
        Int32 position;
        input2->GetPosition(&position);
        if (convertWholeBuffer) {
            Int32 limit;
            input2->GetLimit(&limit);
            byteCount = (limit - position) * 2;
        }

        byteHelper->Allocate(byteCount, (IByteBuffer**)&result);
        ByteOrder order;
        input2->GetOrder(&order);
        result->SetOrder(order);
        AutoPtr<IInt16Buffer> result2;
        result->AsInt16Buffer((IInt16Buffer**)&result2);
        for (Int32 i = 0; i < byteCount / 2; i++) {
            Int16 v;
            input2->GetInt16(&v);
            result2->PutInt16(v);
        }
        input2->SetPosition(position);
    } else if (IInt32Buffer::Probe(input)) {
        AutoPtr<IInt32Buffer> input2 = IInt32Buffer::Probe(input);
        Int32 position;
        input2->GetPosition(&position);
        if (convertWholeBuffer) {
            Int32 limit;
            input2->GetLimit(&limit);
            byteCount = (limit - position) * 4;
        }

        byteHelper->Allocate(byteCount, (IByteBuffer**)&result);
        ByteOrder order;
        input2->GetOrder(&order);
        result->SetOrder(order);
        AutoPtr<IInt32Buffer> result2;
        result->AsInt32Buffer((IInt32Buffer**)&result2);
        for (Int32 i = 0; i < byteCount / 4; i++) {
            Int32 v;
            input2->GetInt32(&v);
            result2->PutInt32(v);
        }
        input2->SetPosition(position);
    } else if (IFloatBuffer::Probe(input)) {
        AutoPtr<IFloatBuffer> input2 = IFloatBuffer::Probe(input);
        Int32 position;
        input2->GetPosition(&position);
        if (convertWholeBuffer) {
            Int32 limit;
            input2->GetLimit(&limit);
            byteCount = (limit - position) * 4;
        }

        byteHelper->Allocate(byteCount, (IByteBuffer**)&result);
        ByteOrder order;
        input2->GetOrder(&order);
        result->SetOrder(order);
        AutoPtr<IFloatBuffer> result2;
        result->AsFloatBuffer((IFloatBuffer**)&result2);
        for (Int32 i = 0; i < byteCount / 4; i++) {
            Float v;
            input2->GetFloat(&v);
            result2->PutFloat(v);
        }
        input2->SetPosition(position);
    } else if (IDoubleBuffer::Probe(input)) {
        AutoPtr<IDoubleBuffer> input2 = IDoubleBuffer::Probe(input);
        Int32 position;
        input2->GetPosition(&position);
        if (convertWholeBuffer) {
            Int32 limit;
            input2->GetLimit(&limit);
            byteCount = (limit - position) * 8;
        }

        byteHelper->Allocate(byteCount, (IByteBuffer**)&result);
        ByteOrder order;
        input2->GetOrder(&order);
        result->SetOrder(order);
        AutoPtr<IDoubleBuffer> result2;
        result->AsDoubleBuffer((IDoubleBuffer**)&result2);
        for (Int32 i = 0; i < byteCount / 8; i++) {
            Double v;
            input2->GetDouble(&v);
            result2->PutDouble(v);
        }
        input2->SetPosition(position);
    } else if (IInt64Buffer::Probe(input)) {
        AutoPtr<IInt64Buffer> input2 = IInt64Buffer::Probe(input);
        Int32 position;
        input2->GetPosition(&position);
        if (convertWholeBuffer) {
            Int32 limit;
            input2->GetLimit(&limit);
            byteCount = (limit - position) * 8;
        }

        byteHelper->Allocate(byteCount, (IByteBuffer**)&result);
        ByteOrder order;
        input2->GetOrder(&order);
        result->SetOrder(order);
        AutoPtr<IInt64Buffer> result2;
        result->AsInt64Buffer((IInt64Buffer**)&result2);
        for (Int32 i = 0; i < byteCount / 8; i++) {
            Int64 v;
            input2->GetInt64(&v);
            result2->PutInt64(v);
        }
        input2->SetPosition(position);
    } else {
        SLOGGERE("GLLogWrapper", "Unimplemented Buffer subclass.")
        return E_RUNTIME_EXCEPTION;
    }
    result->Rewind();
    // The OpenGL API will interpret the result in hardware byte order,
    // so we better do that as well:
    AutoPtr<IByteOrderHelper> orderHelper;
    CByteOrderHelper::AcquireSingleton((IByteOrderHelper**)&orderHelper);
    ByteOrder nativeOrder;
    orderHelper->GetNativeOrder(&nativeOrder);
    result->SetOrder(nativeOrder);
    *rBuf = result;
    REFCOUNT_ADD(*rBuf)
    return NOERROR;
}

AutoPtr<ArrayOf<Char32> > GLLogWrapper::ToCharIndices(
    /* [in] */ Int32 count,
    /* [in] */ Int32 type,
    /* [in] */ IBuffer* indices)
{
    AutoPtr<ArrayOf<Char32> > result = ArrayOf<Char32>::Alloc(count);
    switch (type) {
    case IGL10::_GL_UNSIGNED_BYTE: {
        AutoPtr<IByteBuffer> byteBuffer;
        ToByteBuffer(count, indices, (IByteBuffer**)&byteBuffer);
        AutoPtr<ArrayOf<Byte> > array;
        byteBuffer->GetArray((ArrayOf<Byte>**)&array);
        Int32 offset;
        byteBuffer->GetArrayOffset(&offset);
        for (Int32 i = 0; i < count; i++) {
            (*result)[i] = (Char32) (0xff & (*array)[offset + i]);
        }
    }
        break;
    case IGL10::_GL_UNSIGNED_SHORT: {
        AutoPtr<ICharBuffer> charBuffer;
        if (ICharBuffer::Probe(indices) != NULL) {
            charBuffer = ICharBuffer::Probe(indices);
        } else {
        AutoPtr<IByteBuffer> byteBuffer;
        ToByteBuffer(count * 2, indices, (IByteBuffer**)&byteBuffer);
            byteBuffer->AsCharBuffer((ICharBuffer**)&charBuffer);
        }
        Int32 oldPosition;
        charBuffer->GetPosition(&oldPosition);
        charBuffer->SetPosition(0);
        charBuffer->GetChars(result);
        charBuffer->SetPosition(oldPosition);
    }
        break;
    default:
        // Don't throw an exception, because we don't want logging to
        // change the behavior.
        break;
    }
    return result;
}

ECode GLLogWrapper::DoArrayElement(
    /* [in] */ StringBuilder& builder,
    /* [in] */ Boolean enabled,
    /* [in] */ const String& name,
    /* [in] */ PointerInfo* pointer,
    /* [in] */ Int32 index)
{
    if (!enabled) {
        return NOERROR;
    }
    builder += " ";
    builder += name + ":{";
    if (pointer == NULL || pointer->mTempByteBuffer == NULL ) {
        builder += "undefined }";
        return NOERROR;
    }
    if (pointer->mStride < 0) {
        builder += "invalid stride";
        return NOERROR;
    }

    Int32 stride = pointer->GetStride();
    AutoPtr<IByteBuffer> byteBuffer = pointer->mTempByteBuffer;
    Int32 size = pointer->mSize;
    Int32 type = pointer->mType;
    Int32 sizeofType = pointer->Sizeof(type);
    Int32 byteOffset = stride * index;
    for (Int32 i = 0; i < size; i++) {
        if (i > 0) {
            builder += ", ";
        }
        switch (type) {
        case IGL10::_GL_BYTE: {
            Byte d;
            byteBuffer->GetByte(byteOffset, &d);
            builder += d;
        }
            break;
        case IGL10::_GL_UNSIGNED_BYTE: {
            Byte d;
            byteBuffer->GetByte(byteOffset, &d);
            builder += (0xff & d);
        }
            break;
        case IGL10::_GL_SHORT: {
            AutoPtr<IInt16Buffer> shortBuffer;
            byteBuffer->AsInt16Buffer((IInt16Buffer**)&shortBuffer);
            Int16 d;
            shortBuffer->GetInt16(byteOffset / 2, &d);
            builder += d;
        }
            break;
        case IGL10::_GL_FIXED: {
            AutoPtr<IInt32Buffer> intBuffer;
            byteBuffer->AsInt32Buffer((IInt32Buffer**)&intBuffer);
            Int32 d;
            intBuffer->GetInt32(byteOffset / 4, &d);
            builder += d;
        }
            break;
        case IGL10::_GL_FLOAT: {
            AutoPtr<IFloatBuffer> floatBuffer;
            byteBuffer->AsFloatBuffer((IFloatBuffer**)&floatBuffer);
            Float d;
            floatBuffer->GetFloat(byteOffset / 4, &d);
            builder += d;
        }
            break;
        default:
            builder += "?";
            break;
        }
        byteOffset += sizeofType;
    }
    builder += "}";
    return NOERROR;
}

ECode GLLogWrapper::DoElement(
    /* [in] */ StringBuilder& builder,
    /* [in] */ Int32 ordinal,
    /* [in] */ Int32 vertexIndex)
{
    builder += " [";
    builder += ordinal;
    builder += " : ";
    builder += vertexIndex;
    builder += "] =";
    DoArrayElement(builder, mVertexArrayEnabled, String("v"), mVertexPointer,
            vertexIndex);
    DoArrayElement(builder, mNormalArrayEnabled, String("n"), mNormalPointer,
            vertexIndex);
    DoArrayElement(builder, mColorArrayEnabled, String("c"), mColorPointer,
            vertexIndex);
    DoArrayElement(builder, mTextureCoordArrayEnabled, String("t"),
            mTexCoordPointer, vertexIndex);
    builder += "\n";
    // Vertex
    // Normal
    // Color
    // TexCoord
    return NOERROR;
}

ECode GLLogWrapper::BindArrays()
{
    if (mColorArrayEnabled)
        mColorPointer->BindByteBuffer();
    if (mNormalArrayEnabled)
        mNormalPointer->BindByteBuffer();
    if (mTextureCoordArrayEnabled)
        mTexCoordPointer->BindByteBuffer();
    if (mVertexArrayEnabled)
        mVertexPointer->BindByteBuffer();
    return NOERROR;
}

ECode GLLogWrapper::UnbindArrays()
{
    if (mColorArrayEnabled)
        mColorPointer->UnbindByteBuffer();
    if (mNormalArrayEnabled)
        mNormalPointer->UnbindByteBuffer();
    if (mTextureCoordArrayEnabled)
        mTexCoordPointer->UnbindByteBuffer();
    if (mVertexArrayEnabled)
        mVertexPointer->UnbindByteBuffer();
    return NOERROR;
}

ECode GLLogWrapper::StartLogIndices()
{
    mStringBuilder += "\n";
    BindArrays();
    return NOERROR;
}

ECode GLLogWrapper::EndLogIndices()
{
    Log(mStringBuilder.ToString());
    UnbindArrays();
    return NOERROR;
}

ECode GLLogWrapper::glActiveTexture(
    /* [in] */ Int32 texture)
{
    Begin(String("glActiveTexture"));
    Arg(String("texture"), texture);
    End();
    ECode ec = mgl->GlActiveTexture(texture);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glAlphaFunc(
    /* [in] */ Int32 func,
    /* [in] */ Float ref)
{
    Begin(String("glAlphaFunc"));
    Arg(String("func"), func);
    Arg(String("ref"), ref);
    End();
    ECode ec = mgl->GlAlphaFunc(func, ref);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glAlphaFuncx(
    /* [in] */ Int32 func,
    /* [in] */ Int32 ref)
{
    Begin(String("glAlphaFuncx"));
    Arg(String("func"), func);
    Arg(String("ref"), ref);
    End();
    ECode ec = mgl->GlAlphaFuncx(func, ref);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glBlendFunc(
    /* [in] */ Int32 sfactor,
    /* [in] */ Int32 dfactor)
{
    Begin(String("GlBlendFunc"));
    Arg(String("sfactor"), GetFactor(sfactor));
    Arg(String("dfactor"), GetFactor(dfactor));
    End();
    ECode ec = mgl->GlBlendFunc(sfactor, dfactor);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glClear(
    /* [in] */ Int32 mask)
{
    Begin(String("glClear"));
    Arg(String("mask"), GetClearBufferMask(mask));
    End();

    ECode ec = mgl->GlClear(mask);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glClearColor(
    /* [in] */ Float red,
    /* [in] */ Float green,
    /* [in] */ Float blue,
    /* [in] */ Float alpha)
{
    Begin(String("glClearColor"));
    Arg(String("red"), red);
    Arg(String("green"), green);
    Arg(String("blue"), blue);
    Arg(String("alpha"), alpha);
    End();

    ECode ec = mgl->GlClearColor(red, green, blue, alpha);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glClearColorx(
    /* [in] */ Int32 red,
    /* [in] */ Int32 green,
    /* [in] */ Int32 blue,
    /* [in] */ Int32 alpha)
{
    Begin(String("glClearColor"));
    Arg(String("red"), red);
    Arg(String("green"), green);
    Arg(String("blue"), blue);
    Arg(String("alpha"), alpha);
    End();

    ECode ec = mgl->GlClearColorx(red, green, blue, alpha);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glClearDepthf(
    /* [in] */ Float depth)
{
    Begin(String("glClearDepthf"));
    Arg(String("depth"), depth);
    End();

    ECode ec = mgl->GlClearDepthf(depth);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glClearDepthx(
    /* [in] */ Int32 depth)
{
    Begin(String("glClearDepthx"));
    Arg(String("depth"), depth);
    End();

    ECode ec = mgl->GlClearDepthx(depth);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glClearStencil(
    /* [in] */ Int32 s)
{
    Begin(String("glClearStencil"));
    Arg(String("s"), s);
    End();

    ECode ec = mgl->GlClearStencil(s);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glClientActiveTexture(
    /* [in] */ Int32 texture)
{
    Begin(String("glClientActiveTexture"));
    Arg(String("texture"), texture);
    End();

    ECode ec = mgl->GlClientActiveTexture(texture);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glColor4f(
    /* [in] */ Float red,
    /* [in] */ Float green,
    /* [in] */ Float blue,
    /* [in] */ Float alpha)
{
    Begin(String("glColor4f"));
    Arg(String("red"), red);
    Arg(String("green"), green);
    Arg(String("blue"), blue);
    Arg(String("alpha"), alpha);
    End();

    ECode ec = mgl->GlColor4f(red, green, blue, alpha);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glColor4x(
    /* [in] */ Int32 red,
    /* [in] */ Int32 green,
    /* [in] */ Int32 blue,
    /* [in] */ Int32 alpha)
{
    Begin(String("glColor4x"));
    Arg(String("red"), red);
    Arg(String("green"), green);
    Arg(String("blue"), blue);
    Arg(String("alpha"), alpha);
    End();

    ECode ec = mgl->GlColor4x(red, green, blue, alpha);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glColorMask(
    /* [in] */ Boolean red,
    /* [in] */ Boolean green,
    /* [in] */ Boolean blue,
    /* [in] */ Boolean alpha)
{
    Begin(String("glColorMask"));
    Arg(String("red"), red);
    Arg(String("green"), green);
    Arg(String("blue"), blue);
    Arg(String("alpha"), alpha);
    End();

    ECode ec = mgl->GlColorMask(red, green, blue, alpha);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glColorPointer(
    /* [in] */ Int32 size,
    /* [in] */ Int32 type,
    /* [in] */ Int32 stride,
    /* [in] */ IBuffer* pointer)
{
    Begin(String("glColorPointer"));
    ArgPointer(size, type, stride, pointer);
    End();
    mColorPointer = new PointerInfo(size, type, stride, pointer, this);

    ECode ec = mgl->GlColorPointer(size, type, stride, pointer);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glCompressedTexSubImage2D(
    /* [in] */ Int32 target,
    /* [in] */ Int32 level,
    /* [in] */ Int32 xoffset,
    /* [in] */ Int32 yoffset,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 format,
    /* [in] */ Int32 imageSize,
    /* [in] */ IBuffer* data)
{
    Begin(String("glCompressedTexSubImage2D"));
    Arg(String("target"), GetTextureTarget(target));
    Arg(String("level"), level);
    Arg(String("xoffset"), xoffset);
    Arg(String("yoffset"), yoffset);
    Arg(String("width"), width);
    Arg(String("height"), height);
    Arg(String("format"), format);
    Arg(String("imageSize"), imageSize);
    Arg(String("data"), ParamsToString(data));
    End();

    ECode ec = mgl->GlCompressedTexSubImage2D(target, level, xoffset, yoffset, width,
        height, format, imageSize, data);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glCopyTexSubImage2D(
    /* [in] */ Int32 target,
    /* [in] */ Int32 level,
    /* [in] */ Int32 xoffset,
    /* [in] */ Int32 yoffset,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    Begin(String("glCopyTexSubImage2D"));
    Arg(String("target"), GetTextureTarget(target));
    Arg(String("level"), level);
    Arg(String("xoffset"), xoffset);
    Arg(String("yoffset"), yoffset);
    Arg(String("x"), x);
    Arg(String("y"), y);
    Arg(String("width"), width);
    Arg(String("height"), height);
    End();

    ECode ec = mgl->GlCopyTexSubImage2D(target, level, xoffset, yoffset, x, y, width,
        height);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glCullFace(
    /* [in] */ Int32 mode)
{
    Begin(String("glCullFace"));
    Arg(String("mode"), mode);
    End();

    ECode ec = mgl->GlCullFace(mode);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glDeleteTextures(
    /* [in] */ Int32 n,
    /* [in] */ ArrayOf<Int32>* textures,
    /* [in] */ Int32 offset)
{
    Begin(String("glDeleteTextures"));
    Arg(String("n"), n);
    Arg(String("textures"), n, textures, offset);
    Arg(String("offset"), offset);
    End();

    ECode ec = mgl->GlDeleteTextures(n, textures, offset);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glDeleteTextures(
    /* [in] */ Int32 n,
    /* [in] */ IInt32Buffer* textures)
{
    Begin(String("GlDeleteTexturesEx"));
    Arg(String("n"), n);
    Arg(String("textures"), n, textures);
    End();

    ECode ec = mgl->GlDeleteTextures(n, textures);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glDepthFunc(
    /* [in] */ Int32 func)
{
    Begin(String("glDepthFunc"));
    Arg(String("func"), func);
    End();

    ECode ec = mgl->GlDepthFunc(func);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glDepthMask(
    /* [in] */ Boolean flag)
{
    Begin(String("glDepthMask"));
    Arg(String("flag"), flag);
    End();

    ECode ec = mgl->GlDepthMask(flag);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glDepthRangef(
    /* [in] */ Float near,
    /* [in] */ Float far)
{
    Begin(String("glDepthRangef"));
    Arg(String("near"), near);
    Arg(String("far"), far);
    End();

    ECode ec = mgl->GlDepthRangef(near, far);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glDepthRangex(
    /* [in] */ Int32 near,
    /* [in] */ Int32 far)
{
    Begin(String("glDepthRangex"));
    Arg(String("near"), near);
    Arg(String("far"), far);
    End();

    ECode ec = mgl->GlDepthRangex(near, far);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glDisable(
    /* [in] */ Int32 cap)
{
    Begin(String("glDisable"));
    Arg(String("cap"), GetCap(cap));
    End();

    ECode ec = mgl->GlDisable(cap);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glDisableClientState(
    /* [in] */ Int32 array)
{
    Begin(String("glDisableClientState"));
    Arg(String("array"), GetClientState(array));
    End();

    switch (array) {
    case IGL10::_GL_COLOR_ARRAY:
        mColorArrayEnabled = FALSE;
        break;
    case IGL10::_GL_NORMAL_ARRAY:
        mNormalArrayEnabled = FALSE;
        break;
    case IGL10::_GL_TEXTURE_COORD_ARRAY:
        mTextureCoordArrayEnabled = FALSE;
        break;
    case IGL10::_GL_VERTEX_ARRAY:
        mVertexArrayEnabled = FALSE;
        break;
    }
    ECode ec = mgl->GlDisableClientState(array);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glDrawArrays(
    /* [in] */ Int32 mode,
    /* [in] */ Int32 first,
    /* [in] */ Int32 count)
{
    Begin(String("glDrawArrays"));
    Arg(String("mode"), mode);
    Arg(String("first"), first);
    Arg(String("count"), count);
    StartLogIndices();
    for (int i = 0; i < count; i++) {
        DoElement(mStringBuilder, i, first + i);
    }
    EndLogIndices();
    End();

    ECode ec = mgl->GlDrawArrays(mode, first, count);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glDrawElements(
    /* [in] */ Int32 mode,
    /* [in] */ Int32 count,
    /* [in] */ Int32 type,
    /* [in] */ IBuffer* indices)
{
    Begin(String("glDrawElements"));
    Arg(String("mode"), GetBeginMode(mode));
    Arg(String("count"), count);
    Arg(String("type"), GetIndexType(type));
    AutoPtr<ArrayOf<Char32> > indexArray = ToCharIndices(count, type, indices);
    Int32 indexArrayLength = indexArray->GetLength();
    StartLogIndices();
    for (int i = 0; i < indexArrayLength; i++) {
        DoElement(mStringBuilder, i, (*indexArray)[i]);
    }
    EndLogIndices();
    End();

    ECode ec = mgl->GlDrawElements(mode, count, type, indices);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glFinish()
{
    Begin(String("glFinish"));
    End();

    ECode ec = mgl->GlFinish();
    CheckError();
    return ec;
}

ECode GLLogWrapper::glFlush()
{
    Begin(String("glFlush"));
    End();

    ECode ec = mgl->GlFlush();
    CheckError();
    return ec;
}

ECode GLLogWrapper::glFogf(
    /* [in] */ Int32 pname,
    /* [in] */ Float param)
{
    Begin(String("glFogf"));
    Arg(String("pname"), pname);
    Arg(String("param"), param);
    End();

    ECode ec = mgl->GlFogf(pname, param);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glFogfv(
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Float>* params,
    /* [in] */ Int32 offset)
{
    Begin(String("glFogfv"));
    Arg(String("pname"), GetFogPName(pname));
    Arg(String("params"), GetFogParamCount(pname), params, offset);
    Arg(String("offset"), offset);
    End();

    ECode ec = mgl->GlFogfv(pname, params, offset);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glFogfv(
    /* [in] */ Int32 pname,
    /* [in] */ IFloatBuffer* params)
{
    Begin(String("GlFogfvEx"));
    Arg(String("pname"), GetFogPName(pname));
    Arg(String("params"), GetFogParamCount(pname), params);
    End();

    ECode ec = mgl->GlFogfv(pname, params);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glFogx(
    /* [in] */ Int32 pname,
    /* [in] */ Int32 param)
{
    Begin(String("glFogx"));
    Arg(String("pname"), GetFogPName(pname));
    Arg(String("param"), param);
    End();

    ECode ec = mgl->GlFogx(pname, param);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glFogxv(
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params,
    /* [in] */ Int32 offset)
{
    Begin(String("glFogxv"));
    Arg(String("pname"), GetFogPName(pname));
    Arg(String("params"), GetFogParamCount(pname), params, offset);
    Arg(String("offset"), offset);
    End();

    ECode ec = mgl->GlFogxv(pname, params, offset);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glFogxv(
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params)
{
    Begin(String("GlFogxvEx"));
    Arg(String("pname"), GetFogPName(pname));
    Arg(String("params"), GetFogParamCount(pname), params);
    End();

    ECode ec = mgl->GlFogxv(pname, params);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glFrontFace(
    /* [in] */ Int32 mode)
{
    Begin(String("glFrontFace"));
    Arg(String("mode"), mode);
    End();

    ECode ec = mgl->GlFrontFace(mode);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glFrustumf(
    /* [in] */ Float left,
    /* [in] */ Float right,
    /* [in] */ Float bottom,
    /* [in] */ Float top,
    /* [in] */ Float near,
    /* [in] */ Float far)
{
    Begin(String("glFrustumf"));
    Arg(String("left"), left);
    Arg(String("right"), right);
    Arg(String("bottom"), bottom);
    Arg(String("top"), top);
    Arg(String("near"), near);
    Arg(String("far"), far);
    End();

    ECode ec = mgl->GlFrustumf(left, right, bottom, top, near, far);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glFrustumx(
    /* [in] */ Int32 left,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom,
    /* [in] */ Int32 top,
    /* [in] */ Int32 near,
    /* [in] */ Int32 far)
{
    Begin(String("glFrustumx"));
    Arg(String("left"), left);
    Arg(String("right"), right);
    Arg(String("bottom"), bottom);
    Arg(String("top"), top);
    Arg(String("near"), near);
    Arg(String("far"), far);
    End();

    ECode ec = mgl->GlFrustumx(left, right, bottom, top, near, far);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glGenTextures(
    /* [in] */ Int32 n,
    /* [in] */ ArrayOf<Int32>* textures,
    /* [in] */ Int32 offset)
{
    Begin(String("glGenTextures"));
    Arg(String("n"), n);
    Arg(String("textures"), TranArrayToString(textures));
    Arg(String("offset"), offset);

    ECode ec = mgl->GlGenTextures(n, textures, offset);

    Returns(ToString(n, FORMAT_INT, textures, offset));

    CheckError();
    return ec;
}

ECode GLLogWrapper::glGenTextures(
    /* [in] */ Int32 n,
    /* [in] */ IInt32Buffer* textures)
{
    Begin(String("GlGenTexturesEx"));
    Arg(String("n"), n);
    Arg(String("textures"), ParamsToString(textures));

    ECode ec = mgl->GlGenTextures(n, textures);

    Returns(ToString(n, FORMAT_INT, textures));

    CheckError();
    return ec;
}

ECode GLLogWrapper::glGetError(
    /* [out] */ Int32* error)
{
    Begin(String("glGetError"));

    ECode ec = mgl->GlGetError(error);

    Returns(*error);

    return ec;
}

ECode GLLogWrapper::glGetString(
    /* [in] */ Int32 name,
    /* [out] */ String* str)
{
    Begin(String("glGetString"));
    Arg(String("name"), name);

    ECode ec = mgl->GlGetString(name, str);

    Returns(*str);

    CheckError();
    return ec;
}

ECode GLLogWrapper::glHint(
    /* [in] */ Int32 target,
    /* [in] */ Int32 mode)
{
    Begin(String("glHint"));
    Arg(String("target"), GetHintTarget(target));
    Arg(String("mode"), GetHintMode(mode));
    End();

    ECode ec = mgl->GlHint(target, mode);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glLightModelf(
    /* [in] */ Int32 pname,
    /* [in] */ Float param)
{
    Begin(String("glLightModelf"));
    Arg(String("pname"), GetLightModelPName(pname));
    Arg(String("param"), param);
    End();

    ECode ec = mgl->GlLightModelf(pname, param);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glLightModelfv(
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Float>* params,
    /* [in] */ Int32 offset)
{
    Begin(String("glLightModelfv"));
    Arg(String("pname"), GetLightModelPName(pname));
    Arg(String("params"), GetLightModelParamCount(pname), params, offset);
    Arg(String("offset"), offset);
    End();

    ECode ec = mgl->GlLightModelfv(pname, params, offset);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glLightModelfv(
    /* [in] */ Int32 pname,
    /* [in] */ IFloatBuffer* params)
{
    Begin(String("GlLightModelfvEx"));
    Arg(String("pname"), GetLightModelPName(pname));
    Arg(String("params"), GetLightModelParamCount(pname), params);
    End();

    ECode ec = mgl->GlLightModelfv(pname, params);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glLightModelx(
    /* [in] */ Int32 pname,
    /* [in] */ Int32 param)
{
    Begin(String("glLightModelx"));
    Arg(String("pname"), GetLightModelPName(pname));
    Arg(String("param"), param);
    End();

    ECode ec = mgl->GlLightModelx(pname, param);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glLightModelxv(
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params,
    /* [in] */ Int32 offset)
{
    Begin(String("glLightModelxv"));
    Arg(String("pname"), GetLightModelPName(pname));
    Arg(String("params"), GetLightModelParamCount(pname), params, offset);
    Arg(String("offset"), offset);
    End();

    ECode ec = mgl->GlLightModelxv(pname, params, offset);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glLightModelxv(
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params)
{
    Begin(String("GlLightModelxvEx"));
    Arg(String("pname"), GetLightModelPName(pname));
    Arg(String("params"), GetLightModelParamCount(pname), params);
    End();

    ECode ec = mgl->GlLightModelxv(pname, params);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glLightf(
    /* [in] */ Int32 light,
    /* [in] */ Int32 pname,
    /* [in] */ Float param)
{
    Begin(String("glLightf"));
    Arg(String("light"), GetLightName(light));
    Arg(String("pname"), GetLightPName(pname));
    Arg(String("param"), param);
    End();

    ECode ec = mgl->GlLightf(light, pname, param);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glLightfv(
    /* [in] */ Int32 light,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Float>* params,
    /* [in] */ Int32 offset)
{
    Begin(String("glLightfv"));
    Arg(String("light"), GetLightName(light));
    Arg(String("pname"), GetLightPName(pname));
    Arg(String("params"), GetLightParamCount(pname), params, offset);
    Arg(String("offset"), offset);
    End();

    ECode ec = mgl->GlLightfv(light, pname, params, offset);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glLightfv(
    /* [in] */ Int32 light,
    /* [in] */ Int32 pname,
    /* [in] */ IFloatBuffer* params)
{
    Begin(String("GlLightfvEx"));
    Arg(String("light"), GetLightName(light));
    Arg(String("pname"), GetLightPName(pname));
    Arg(String("params"), GetLightParamCount(pname), params);
    End();

    ECode ec = mgl->GlLightfv(light, pname, params);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glLightx(
    /* [in] */ Int32 light,
    /* [in] */ Int32 pname,
    /* [in] */ Int32 param)
{
    Begin(String("glLightx"));
    Arg(String("light"), GetLightName(light));
    Arg(String("pname"), GetLightPName(pname));
    Arg(String("param"), param);
    End();

    ECode ec = mgl->GlLightx(light, pname, param);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glLightxv(
    /* [in] */ Int32 light,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params,
    /* [in] */ Int32 offset)
{
    Begin(String("glLightxv"));
    Arg(String("light"), GetLightName(light));
    Arg(String("pname"), GetLightPName(pname));
    Arg(String("params"), GetLightParamCount(pname), params, offset);
    Arg(String("offset"), offset);
    End();

    ECode ec = mgl->GlLightxv(light, pname, params, offset);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glLightxv(
    /* [in] */ Int32 light,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params)
{
    Begin(String("GlLightxvEx"));
    Arg(String("light"), GetLightName(light));
    Arg(String("pname"), GetLightPName(pname));
    Arg(String("params"), GetLightParamCount(pname), params);
    End();

    ECode ec = mgl->GlLightxv(light, pname, params);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glLineWidth(
    /* [in] */ Float width)
{
    Begin(String("glLineWidth"));
    Arg(String("width"), width);
    End();

    ECode ec = mgl->GlLineWidth(width);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glLineWidthx(
    /* [in] */ Int32 width)
{
    Begin(String("glLineWidthx"));
    Arg(String("width"), width);
    End();

    ECode ec = mgl->GlLineWidthx(width);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glLoadIdentity()
{
    Begin(String("glLoadIdentity"));
    End();

    ECode ec = mgl->GlLoadIdentity();
    CheckError();
    return ec;
}

ECode GLLogWrapper::glLoadMatrixf(
    /* [in] */ ArrayOf<Float>* m,
    /* [in] */ Int32 offset)
{
    Begin(String("glLoadMatrixf"));
    Arg(String("m"), 16, m, offset);
    Arg(String("offset"), offset);
    End();

    ECode ec = mgl->GlLoadMatrixf(m, offset);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glLoadMatrixf(
    /* [in] */ IFloatBuffer* m)
{
    Begin(String("GlLoadMatrixfEx"));
    Arg(String("m"), 16, m);
    End();

    ECode ec = mgl->GlLoadMatrixf(m);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glLoadMatrixx(
    /* [in] */ ArrayOf<Int32>* m,
    /* [in] */ Int32 offset)
{
    Begin(String("glLoadMatrixx"));
    Arg(String("m"), 16, m, offset);
    Arg(String("offset"), offset);
    End();

    ECode ec = mgl->GlLoadMatrixx(m, offset);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glLoadMatrixx(
    /* [in] */ IInt32Buffer* m)
{
    Begin(String("GlLoadMatrixxEx"));
    Arg(String("m"), 16, m);
    End();

    ECode ec = mgl->GlLoadMatrixx(m);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glLogicOp(
    /* [in] */ Int32 opcode)
{
    Begin(String("glLogicOp"));
    Arg(String("opcode"), opcode);
    End();

    ECode ec = mgl->GlLogicOp(opcode);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glMaterialf(
    /* [in] */ Int32 face,
    /* [in] */ Int32 pname,
    /* [in] */ Float param)
{
    Begin(String("glMaterialf"));
    Arg(String("face"), GetFaceName(face));
    Arg(String("pname"), GetMaterialPName(pname));
    Arg(String("param"), param);
    End();

    ECode ec = mgl->GlMaterialf(face, pname, param);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glMaterialfv(
    /* [in] */ Int32 face,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Float>* params,
    /* [in] */ Int32 offset)
{
    Begin(String("glMaterialfv"));
    Arg(String("face"), GetFaceName(face));
    Arg(String("pname"), GetMaterialPName(pname));
    Arg(String("params"), GetMaterialParamCount(pname), params, offset);
    Arg(String("offset"), offset);
    End();

    ECode ec = mgl->GlMaterialfv(face, pname, params, offset);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glMaterialfv(
    /* [in] */ Int32 face,
    /* [in] */ Int32 pname,
    /* [in] */ IFloatBuffer* params)
{
    Begin(String("GlMaterialfvEx"));
    Arg(String("face"), GetFaceName(face));
    Arg(String("pname"), GetMaterialPName(pname));
    Arg(String("params"), GetMaterialParamCount(pname), params);
    End();

    ECode ec = mgl->GlMaterialfv(face, pname, params);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glMaterialx(
    /* [in] */ Int32 face,
    /* [in] */ Int32 pname,
    /* [in] */ Int32 param)
{
    Begin(String("glMaterialx"));
    Arg(String("face"), GetFaceName(face));
    Arg(String("pname"), GetMaterialPName(pname));
    Arg(String("param"), param);
    End();

    ECode ec = mgl->GlMaterialx(face, pname, param);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glMaterialxv(
    /* [in] */ Int32 face,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params,
    /* [in] */ Int32 offset)
{
    Begin(String("glMaterialxv"));
    Arg(String("face"), GetFaceName(face));
    Arg(String("pname"), GetMaterialPName(pname));
    Arg(String("params"), GetMaterialParamCount(pname), params, offset);
    Arg(String("offset"), offset);
    End();

    ECode ec = mgl->GlMaterialxv(face, pname, params, offset);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glMaterialxv(
    /* [in] */ Int32 face,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params)
{
    Begin(String("GlMaterialxvEx"));
    Arg(String("face"), GetFaceName(face));
    Arg(String("pname"), GetMaterialPName(pname));
    Arg(String("params"), GetMaterialParamCount(pname), params);
    End();

    ECode ec = mgl->GlMaterialxv(face, pname, params);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glMatrixMode(
    /* [in] */ Int32 mode)
{
    Begin(String("glMatrixMode"));
    Arg(String("mode"), GetMatrixMode(mode));
    End();

    ECode ec = mgl->GlMatrixMode(mode);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glMultMatrixf(
    /* [in] */ ArrayOf<Float>* m,
    /* [in] */ Int32 offset)
{
    Begin(String("glMultMatrixf"));
    Arg(String("m"), 16, m, offset);
    Arg(String("offset"), offset);
    End();

    ECode ec = mgl->GlMultMatrixf(m, offset);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glMultMatrixf(
    /* [in] */ IFloatBuffer* m)
{
    Begin(String("GlMultMatrixfEx"));
    Arg(String("m"), 16, m);
    End();

    ECode ec = mgl->GlMultMatrixf(m);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glMultMatrixx(
    /* [in] */ ArrayOf<Int32>* m,
    /* [in] */ Int32 offset)
{
    Begin(String("glMultMatrixx"));
    Arg(String("m"), 16, m, offset);
    Arg(String("offset"), offset);
    End();

    ECode ec = mgl->GlMultMatrixx(m, offset);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glMultMatrixx(
    /* [in] */ IInt32Buffer* m)
{
    Begin(String("GlMultMatrixxEx"));
    Arg(String("m"), 16, m);
    End();

    ECode ec = mgl->GlMultMatrixx(m);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glMultiTexCoord4f(
    /* [in] */ Int32 target,
    /* [in] */ Float s,
    /* [in] */ Float t,
    /* [in] */ Float r,
    /* [in] */ Float q)
{
    Begin(String("glMultiTexCoord4f"));
    Arg(String("target"), target);
    Arg(String("s"), s);
    Arg(String("t"), t);
    Arg(String("r"), r);
    Arg(String("q"), q);
    End();

    ECode ec = mgl->GlMultiTexCoord4f(target, s, t, r, q);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glMultiTexCoord4x(
    /* [in] */ Int32 target,
    /* [in] */ Int32 s,
    /* [in] */ Int32 t,
    /* [in] */ Int32 r,
    /* [in] */ Int32 q)
{
    Begin(String("glMultiTexCoord4x"));
    Arg(String("target"), target);
    Arg(String("s"), s);
    Arg(String("t"), t);
    Arg(String("r"), r);
    Arg(String("q"), q);
    End();

    ECode ec = mgl->GlMultiTexCoord4x(target, s, t, r, q);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glNormal3f(
    /* [in] */ Float nx,
    /* [in] */ Float ny,
    /* [in] */ Float nz)
{
    Begin(String("glNormal3f"));
    Arg(String("nx"), nx);
    Arg(String("ny"), ny);
    Arg(String("nz"), nz);
    End();

    ECode ec = mgl->GlNormal3f(nx, ny, nz);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glNormal3x(
    /* [in] */ Int32 nx,
    /* [in] */ Int32 ny,
    /* [in] */ Int32 nz)
{
    Begin(String("glNormal3x"));
    Arg(String("nx"), nx);
    Arg(String("ny"), ny);
    Arg(String("nz"), nz);
    End();

    ECode ec = mgl->GlNormal3x(nx, ny, nz);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glNormalPointer(
    /* [in] */ Int32 type,
    /* [in] */ Int32 stride,
    /* [in] */ IBuffer* pointer)
{
    Begin(String("glNormalPointer"));
    Arg(String("type"), type);
    Arg(String("stride"), stride);
    Arg(String("pointer"), ParamsToString(pointer));
    End();
    mNormalPointer = new PointerInfo(3, type, stride, pointer, this);

    ECode ec = mgl->GlNormalPointer(type, stride, pointer);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glOrthof(
    /* [in] */ Float left,
    /* [in] */ Float right,
    /* [in] */ Float bottom,
    /* [in] */ Float top,
    /* [in] */ Float near,
    /* [in] */ Float far)
{
    Begin(String("glOrthof"));
    Arg(String("left"), left);
    Arg(String("right"), right);
    Arg(String("bottom"), bottom);
    Arg(String("top"), top);
    Arg(String("near"), near);
    Arg(String("far"), far);
    End();

    ECode ec = mgl->GlOrthof(left, right, bottom, top, near, far);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glOrthox(
    /* [in] */ Int32 left,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom,
    /* [in] */ Int32 top,
    /* [in] */ Int32 near,
    /* [in] */ Int32 far)
{
    Begin(String("glOrthox"));
    Arg(String("left"), left);
    Arg(String("right"), right);
    Arg(String("bottom"), bottom);
    Arg(String("top"), top);
    Arg(String("near"), near);
    Arg(String("far"), far);
    End();

    ECode ec = mgl->GlOrthox(left, right, bottom, top, near, far);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glPixelStorei(
    /* [in] */ Int32 pname,
    /* [in] */ Int32 param)
{
    Begin(String("glPixelStorei"));
    Arg(String("pname"), pname);
    Arg(String("param"), param);
    End();

    ECode ec = mgl->GlPixelStorei(pname, param);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glPointSize(
    /* [in] */ Float size)
{
    Begin(String("glPointSize"));
    Arg(String("size"), size);
    End();

    ECode ec = mgl->GlPointSize(size);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glPointSizex(
    /* [in] */ Int32 size)
{
    Begin(String("glPointSizex"));
    Arg(String("size"), size);
    End();

    ECode ec = mgl->GlPointSizex(size);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glPolygonOffset(
    /* [in] */ Float factor,
    /* [in] */ Float units)
{
    Begin(String("glPolygonOffset"));
    Arg(String("factor"), factor);
    Arg(String("units"), units);
    End();
    ECode ec = mgl->GlPolygonOffset(factor, units);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glPolygonOffsetx(
    /* [in] */ Int32 factor,
    /* [in] */ Int32 units)
{
    Begin(String("glPolygonOffsetx"));
    Arg(String("factor"), factor);
    Arg(String("units"), units);
    End();

    ECode ec = mgl->GlPolygonOffsetx(factor, units);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glPopMatrix()
{
    Begin(String("glPopMatrix"));
    End();

    ECode ec = mgl->GlPopMatrix();
    CheckError();
    return ec;
}

ECode GLLogWrapper::glPushMatrix()
{
    Begin(String("glPushMatrix"));
    End();

    ECode ec = mgl->GlPushMatrix();
    CheckError();
    return ec;
}

ECode GLLogWrapper::glReadPixels(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 format,
    /* [in] */ Int32 type,
    /* [in] */ IBuffer* pixels)
{
    Begin(String("glReadPixels"));
    Arg(String("x"), x);
    Arg(String("y"), y);
    Arg(String("width"), width);
    Arg(String("height"), height);
    Arg(String("format"), format);
    Arg(String("type"), type);
    Arg(String("pixels"), ParamsToString(pixels));
    End();

    ECode ec = mgl->GlReadPixels(x, y, width, height, format, type, pixels);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glRotatef(
    /* [in] */ Float angle,
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Float z)
{
    Begin(String("glRotatef"));
    Arg(String("angle"), angle);
    Arg(String("x"), x);
    Arg(String("y"), y);
    Arg(String("z"), z);
    End();

    ECode ec = mgl->GlRotatef(angle, x, y, z);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glRotatex(
    /* [in] */ Int32 angle,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 z)
{
    Begin(String("glRotatex"));
    Arg(String("angle"), angle);
    Arg(String("x"), x);
    Arg(String("y"), y);
    Arg(String("z"), z);
    End();

    ECode ec = mgl->GlRotatex(angle, x, y, z);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glSampleCoverage(
    /* [in] */ Float value,
    /* [in] */ Boolean invert)
{
    Begin(String("glSampleCoveragex"));
    Arg(String("value"), value);
    Arg(String("invert"), invert);
    End();

    ECode ec = mgl->GlSampleCoverage(value, invert);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glSampleCoveragex(
    /* [in] */ Int32 value,
    /* [in] */ Boolean invert)
{
    Begin(String("glSampleCoveragex"));
    Arg(String("value"), value);
    Arg(String("invert"), invert);
    End();

    ECode ec = mgl->GlSampleCoveragex(value, invert);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glScalef(
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Float z)
{
    Begin(String("glScalef"));
    Arg(String("x"), x);
    Arg(String("y"), y);
    Arg(String("z"), z);
    End();

    ECode ec = mgl->GlScalef(x, y, z);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glScalex(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 z)
{
    Begin(String("glScalex"));
    Arg(String("x"), x);
    Arg(String("y"), y);
    Arg(String("z"), z);
    End();

    ECode ec = mgl->GlScalex(x, y, z);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glScissor(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    Begin(String("glScissor"));
    Arg(String("x"), x);
    Arg(String("y"), y);
    Arg(String("width"), width);
    Arg(String("height"), height);
    End();

    ECode ec = mgl->GlScissor(x, y, width, height);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glShadeModel(
    /* [in] */ Int32 mode)
{
    Begin(String("glShadeModel"));
    Arg(String("mode"), GetShadeModel(mode));
    End();

    ECode ec = mgl->GlShadeModel(mode);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glStencilFunc(
    /* [in] */ Int32 func,
    /* [in] */ Int32 ref,
    /* [in] */ Int32 mask)
{
    Begin(String("glStencilFunc"));
    Arg(String("func"), func);
    Arg(String("ref"), ref);
    Arg(String("mask"), mask);
    End();

    ECode ec = mgl->GlStencilFunc(func, ref, mask);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glStencilMask(
    /* [in] */ Int32 mask)
{
    Begin(String("glStencilMask"));
    Arg(String("mask"), mask);
    End();

    ECode ec = mgl->GlStencilMask(mask);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glTexCoordPointer(
    /* [in] */ Int32 size,
    /* [in] */ Int32 type,
    /* [in] */ Int32 stride,
    /* [in] */ IBuffer* pointer)
{
    Begin(String("glTexCoordPointer"));
    ArgPointer(size, type, stride, pointer);
    End();
    mTexCoordPointer = new PointerInfo(size, type, stride, pointer, this);

    ECode ec = mgl->GlTexCoordPointer(size, type, stride, pointer);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glTexImage2D(
    /* [in] */ Int32 target,
    /* [in] */ Int32 level,
    /* [in] */ Int32 internalformat,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 border,
    /* [in] */ Int32 format,
    /* [in] */ Int32 type,
    /* [in] */ IBuffer* pixels)
{
    Begin(String("glTexImage2D"));
    Arg(String("target"), target);
    Arg(String("level"), level);
    Arg(String("internalformat"), internalformat);
    Arg(String("width"), width);
    Arg(String("height"), height);
    Arg(String("border"), border);
    Arg(String("format"), format);
    Arg(String("type"), type);
    Arg(String("pixels"), ParamsToString(pixels));
    End();

    ECode ec = mgl->GlTexImage2D(target, level, internalformat, width, height, border,
        format, type, pixels);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glTexParameterx(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ Int32 param)
{
    Begin(String("glTexParameterx"));
    Arg(String("target"), GetTextureTarget(target));
    Arg(String("pname"), GetTexturePName(pname));
    Arg(String("param"), param);
    End();

    ECode ec = mgl->GlTexParameterx(target, pname, param);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glTexSubImage2D(
    /* [in] */ Int32 target,
    /* [in] */ Int32 level,
    /* [in] */ Int32 xoffset,
    /* [in] */ Int32 yoffset,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 format,
    /* [in] */ Int32 type,
    /* [in] */ IBuffer* pixels)
{
    Begin(String("glTexSubImage2D"));
    Arg(String("target"), GetTextureTarget(target));
    Arg(String("level"), level);
    Arg(String("xoffset"), xoffset);
    Arg(String("yoffset"), yoffset);
    Arg(String("width"), width);
    Arg(String("height"), height);
    Arg(String("format"), format);
    Arg(String("type"), type);
    Arg(String("pixels"), ParamsToString(pixels));
    End();
    ECode ec = mgl->GlTexSubImage2D(target, level, xoffset, yoffset, width, height,
        format, type, pixels);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glTranslatef(
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Float z)
{
    Begin(String("glTranslatef"));
    Arg(String("x"), x);
    Arg(String("y"), y);
    Arg(String("z"), z);
    End();
    ECode ec = mgl->GlTranslatef(x, y, z);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glTranslatex(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 z)
{
    Begin(String("glTranslatex"));
    Arg(String("x"), x);
    Arg(String("y"), y);
    Arg(String("z"), z);
    End();
    ECode ec = mgl->GlTranslatex(x, y, z);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glVertexPointer(
    /* [in] */ Int32 size,
    /* [in] */ Int32 type,
    /* [in] */ Int32 stride,
    /* [in] */ IBuffer* pointer)
{
    Begin(String("glVertexPointer"));
    ArgPointer(size, type, stride, pointer);
    End();
    mVertexPointer = new PointerInfo(size, type, stride, pointer, this);
    ECode ec = mgl->GlVertexPointer(size, type, stride, pointer);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glViewport(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    Begin(String("glViewport"));
    Arg(String("x"), x);
    Arg(String("y"), y);
    Arg(String("width"), width);
    Arg(String("height"), height);
    End();
    ECode ec = mgl->GlViewport(x, y, width, height);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glEnable(
    /* [in] */ Int32 cap)
{
    Begin(String("glEnable"));
    Arg(String("cap"), GetCap(cap));
    End();

    ECode ec = mgl->GlEnable(cap);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glEnableClientState(
    /* [in] */ Int32 array)
{
    Begin(String("glEnableClientState"));
    Arg(String("array"), GetClientState(array));
    End();

    switch (array) {
    case IGL10::_GL_COLOR_ARRAY:
        mColorArrayEnabled = true;
        break;
    case IGL10::_GL_NORMAL_ARRAY:
        mNormalArrayEnabled = true;
        break;
    case IGL10::_GL_TEXTURE_COORD_ARRAY:
        mTextureCoordArrayEnabled = true;
        break;
    case IGL10::_GL_VERTEX_ARRAY:
        mVertexArrayEnabled = true;
        break;
    }
    ECode ec = mgl->GlEnableClientState(array);
    CheckError();
    return ec;
}


ECode GLLogWrapper::glTexParameterf(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ Float param)
{
    Begin(String("glTexParameterf"));
    Arg(String("target"), GetTextureTarget(target));
    Arg(String("pname"), GetTexturePName(pname));
    Arg(String("param"), GetTextureParamName(param));
    End();

    ECode ec = mgl->GlTexParameterf(target, pname, param);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glTexParameterfv(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Float>* params,
    /* [in] */ Int32 offset)
{
    Begin(String("glTexParameterfv"));
    Arg(String("target"), target);
    Arg(String("pname"), pname);
    Arg(String("params"), TranArrayToString(params));
    Arg(String("offset"), offset);
    End();
    ECode ec = mgl11->GlTexParameterfv( target, pname, params, offset);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glTexParameterfv(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ IFloatBuffer* params)
{
    Begin(String("GlTexParameterfvEx"));
    Arg(String("target"), target);
    Arg(String("pname"), pname);
    String str;
    params->ToString(&str);
    Arg(String("params"), str);
    End();
    ECode ec = mgl11->GlTexParameterfv(target, pname, params);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glBindTexture(
    /* [in] */ Int32 target,
    /* [in] */ Int32 texture)
{
    Begin(String("glBindTexture"));
    Arg(String("target"), GetTextureTarget(target));
    Arg(String("texture"), texture);
    End();
    ECode ec = mgl->GlBindTexture(target, texture);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glCompressedTexImage2D(
    /* [in] */ Int32 target,
    /* [in] */ Int32 level,
    /* [in] */ Int32 internalformat,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 border,
    /* [in] */ Int32 imageSize,
    /* [in] */ IBuffer* data)
{
    Begin(String("glCompressedTexImage2D"));
    Arg(String("target"), GetTextureTarget(target));
    Arg(String("level"), level);
    Arg(String("internalformat"), internalformat);
    Arg(String("width"), width);
    Arg(String("height"), height);
    Arg(String("border"), border);
    Arg(String("imageSize"), imageSize);
    Arg(String("data"), ParamsToString(data));
    End();

    ECode ec = mgl->GlCompressedTexImage2D(target, level, internalformat, width,
        height, border, imageSize, data);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glCopyTexImage2D(
    /* [in] */ Int32 target,
    /* [in] */ Int32 level,
    /* [in] */ Int32 internalformat,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 border)
{
    Begin(String("glCopyTexImage2D"));
    Arg(String("target"), GetTextureTarget(target));
    Arg(String("level"), level);
    Arg(String("internalformat"), internalformat);
    Arg(String("x"), x);
    Arg(String("y"), y);
    Arg(String("width"), width);
    Arg(String("height"), height);
    Arg(String("border"), border);
    End();

    ECode ec = mgl->GlCopyTexImage2D(target, level, internalformat, x, y, width,
        height, border);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glGetIntegerv(
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params,
    /* [in] */ Int32 offset)
{
    Begin(String("glGetIntegerv"));
    Arg(String("pname"), GetIntegerStateName(pname));
    Arg(String("params"), ParamsToString(params));
    Arg(String("offset"), offset);

    ECode ec = mgl->GlGetIntegerv(pname, params, offset);

    Returns(ToString(GetIntegerStateSize(pname),
        GetIntegerStateFormat(pname), params, offset));

    CheckError();
    return ec;
}

ECode GLLogWrapper::glGetIntegerv(
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params)
{
    Begin(String("GlGetIntegervEx"));
    Arg(String("pname"), GetIntegerStateName(pname));
    Arg(String("params"), ParamsToString(params));

    ECode ec = mgl->GlGetIntegerv(pname, params);

    Returns(ToString(GetIntegerStateSize(pname),
        GetIntegerStateFormat(pname), params));

    CheckError();
    return ec;
}

ECode GLLogWrapper::glStencilOp(
    /* [in] */ Int32 fail,
    /* [in] */ Int32 zfail,
    /* [in] */ Int32 zpass)
{
    Begin(String("glStencilOp"));
    Arg(String("fail"), fail);
    Arg(String("zfail"), zfail);
    Arg(String("zpass"), zpass);
    End();

    ECode ec = mgl->GlStencilOp(fail, zfail, zpass);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glTexEnvf(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ Float param)
{
    Begin(String("glTexEnvf"));
    Arg(String("target"), GetTextureEnvTarget(target));
    Arg(String("pname"), GetTextureEnvPName(pname));
    Arg(String("param"), GetTextureEnvParamName(param));
    End();

    ECode ec = mgl->GlTexEnvf(target, pname, param);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glTexEnvfv(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Float>* params,
    /* [in] */ Int32 offset)
{
    Begin(String("glTexEnvfv"));
    Arg(String("target"), GetTextureEnvTarget(target));
    Arg(String("pname"), GetTextureEnvPName(pname));
    Arg(String("params"), GetTextureEnvParamCount(pname), params, offset);
    Arg(String("offset"), offset);
    End();

    ECode ec = mgl->GlTexEnvfv(target, pname, params, offset);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glTexEnvfv(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ IFloatBuffer* params)
{
    Begin(String("GlTexEnvfvEx"));
    Arg(String("target"), GetTextureEnvTarget(target));
    Arg(String("pname"), GetTextureEnvPName(pname));
    Arg(String("params"), GetTextureEnvParamCount(pname), params);
    End();

    ECode ec = mgl->GlTexEnvfv(target, pname, params);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glTexEnvx(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ Int32 param)
{
    Begin(String("glTexEnvx"));
    Arg(String("target"), GetTextureEnvTarget(target));
    Arg(String("pname"), GetTextureEnvPName(pname));
    Arg(String("param"), param);
    End();

    ECode ec = mgl->GlTexEnvx(target, pname, param);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glTexEnvxv(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params,
    /* [in] */ Int32 offset)
{
    Begin(String("glTexEnvxv"));
    Arg(String("target"), GetTextureEnvTarget(target));
    Arg(String("pname"), GetTextureEnvPName(pname));
    Arg(String("params"), GetTextureEnvParamCount(pname), params, offset);
    Arg(String("offset"), offset);
    End();

    ECode ec = mgl->GlTexEnvxv(target, pname, params, offset);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glTexEnvxv(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params)
{
    Begin(String("GlTexEnvxvEx"));
    Arg(String("target"), GetTextureEnvTarget(target));
    Arg(String("pname"), GetTextureEnvPName(pname));
    Arg(String("params"), GetTextureEnvParamCount(pname), params);
    End();

    ECode ec = mgl->GlTexEnvxv(target, pname, params);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glQueryMatrixxOES(
    /* [in] */ ArrayOf<Int32>* mantissa,
    /* [in] */ Int32 mantissaOffset,
    /* [in] */ ArrayOf<Int32>* exponent,
    /* [in] */ Int32 exponentOffset,
    /* [out] */ Int32* matrixxOES)
{
    Begin(String("glQueryMatrixxOES"));
    Arg(String("mantissa"), ParamsToString(mantissa));
    Arg(String("exponent"), ParamsToString(exponent));
    End();
    ECode ec = mgl10Ext->GlQueryMatrixxOES(mantissa, mantissaOffset,
        exponent, exponentOffset, matrixxOES);
    Returns(ToString(16, FORMAT_FIXED, mantissa, mantissaOffset));
    Returns(ToString(16, FORMAT_INT, exponent, exponentOffset));
    CheckError();
    return ec;
}

ECode GLLogWrapper::glQueryMatrixxOES(
    /* [in] */ IInt32Buffer* mantissa,
    /* [in] */ IInt32Buffer* exponent,
    /* [out] */ Int32* matrixxOES)
{
    Begin(String("GlQueryMatrixxOESEx"));
    Arg(String("mantissa"), ParamsToString(mantissa));
    Arg(String("exponent"), ParamsToString(exponent));
    End();
    ECode ec = mgl10Ext->GlQueryMatrixxOES(mantissa, exponent, matrixxOES);
    Returns(ToString(16, FORMAT_FIXED, mantissa));
    Returns(ToString(16, FORMAT_INT, exponent));
    CheckError();
    return ec;
}

ECode GLLogWrapper::glGetPointerv(
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<IBuffer*>* params)
{
    Begin(String("glGetPointerv"));
    Arg(String("pname"), pname);
    Arg(String("params"), ParamsToString(params));
    End();
    ECode ec = mgl11->GlGetPointerv(pname, params);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glBindBuffer(
    /* [in] */ Int32 target,
    /* [in] */ Int32 buffer)
{
    Begin(String("glBindBuffer"));
    Arg(String("target"), target);
    Arg(String("buffer"), buffer);
    End();
    ECode ec = mgl11->GlBindBuffer(target, buffer);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glBufferData(
    /* [in] */ Int32 target,
    /* [in] */ Int32 size,
    /* [in] */ IBuffer* data,
    /* [in] */ Int32 usage)
{
    Begin(String("glBufferData"));
    Arg(String("target"), target);
    Arg(String("size"), size);
    Arg(String("data"), ParamsToString(data));
    Arg(String("usage"), usage);
    End();
    ECode ec = mgl11->GlBufferData(target, size, data, usage);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glBufferSubData(
    /* [in] */ Int32 target,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 size,
    /* [in] */ IBuffer* data)
{
    Begin(String("glBufferSubData"));
    Arg(String("target"), target);
    Arg(String("offset"), offset);
    Arg(String("size"), size);
    Arg(String("data"), ParamsToString(data));
    End();
    ECode ec = mgl11->GlBufferSubData(target, offset, size, data);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glClipPlanef(
    /* [in] */ Int32 plane,
    /* [in] */ ArrayOf<Float>* equation,
    /* [in] */ Int32 offset)
{
    Begin(String("glClipPlanef"));
    Arg(String("plane"), plane);
    Arg(String("equation"), 4, equation, offset);
    Arg(String("offset"), offset);
    End();
    ECode ec = mgl11->GlClipPlanef(plane, equation, offset);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glClipPlanef(
    /* [in] */ Int32 plane,
    /* [in] */ IFloatBuffer* equation)
{
    Begin(String("GlClipPlanefEx"));
    Arg(String("plane"), plane);
    Arg(String("equation"), 4, equation);
    End();
    ECode ec = mgl11->GlClipPlanef(plane, equation);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glClipPlanex(
    /* [in] */ Int32 plane,
    /* [in] */ ArrayOf<Int32>* equation,
    /* [in] */ Int32 offset)
{
    Begin(String("glClipPlanex"));
    Arg(String("plane"), plane);
    Arg(String("equation"), 4, equation, offset);
    Arg(String("offset"), offset);
    End();
    ECode ec = mgl11->GlClipPlanex(plane, equation, offset);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glClipPlanex(
    /* [in] */ Int32 plane,
    /* [in] */ IInt32Buffer* equation)
{
    Begin(String("GlClipPlanexEx"));
    Arg(String("plane"), plane);
    Arg(String("equation"), 4, equation);
    End();
    ECode ec = mgl11->GlClipPlanex(plane, equation);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glColor4ub(
    /* [in] */ Byte red,
    /* [in] */ Byte green,
    /* [in] */ Byte blue,
    /* [in] */ Byte alpha)
{
    Begin(String("glColor4ub"));
    Arg(String("red"), red);
    Arg(String("green"), green);
    Arg(String("blue"), blue);
    Arg(String("alpha"), alpha);
    End();
    ECode ec = mgl11->GlColor4ub(red, green, blue, alpha);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glColorPointer(
    /* [in] */ Int32 size,
    /* [in] */ Int32 type,
    /* [in] */ Int32 stride,
    /* [in] */ Int32 offset)
{
    Begin(String("GlColorPointerEx"));
    Arg(String("size"), size);
    Arg(String("type"), type);
    Arg(String("stride"), stride);
    Arg(String("offset"), offset);
    End();
    ECode ec = mgl11->GlColorPointer(size, type, stride, offset);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glDeleteBuffers(
    /* [in] */ Int32 n,
    /* [in] */ ArrayOf<Int32>* buffers,
    /* [in] */ Int32 offset)
{
    Begin(String("glDeleteBuffers"));
    Arg(String("n"), n);
    Arg(String("buffers"), ParamsToString(buffers));
    Arg(String("offset"), offset);
    End();
    ECode ec = mgl11->GlDeleteBuffers(n, buffers, offset);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glDeleteBuffers(
    /* [in] */ Int32 n,
    /* [in] */ IInt32Buffer* buffers)
{
    Begin(String("GlDeleteBuffersEx"));
    Arg(String("n"), n);
    Arg(String("buffers"), ParamsToString(buffers));
    End();
    ECode ec = mgl11->GlDeleteBuffers(n, buffers);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glDrawElements(
    /* [in] */ Int32 mode,
    /* [in] */ Int32 count,
    /* [in] */ Int32 type,
    /* [in] */ Int32 offset)
{
    Begin(String("GlDrawElementsEx"));
    Arg(String("mode"), mode);
    Arg(String("count"), count);
    Arg(String("type"), type);
    Arg(String("offset"), offset);
    End();
    ECode ec = mgl11->GlDrawElements(mode, count, type, offset);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glGenBuffers(
    /* [in] */ Int32 n,
    /* [in] */ ArrayOf<Int32>* buffers,
    /* [in] */ Int32 offset)
{
    Begin(String("glGenBuffers"));
    Arg(String("n"), n);
    Arg(String("buffers"), ParamsToString(buffers));
    Arg(String("offset"), offset);
    End();
    ECode ec = mgl11->GlGenBuffers(n, buffers, offset);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glGenBuffers(
    /* [in] */ Int32 n,
    /* [in] */ IInt32Buffer* buffers)
{
    Begin(String("GlGenBuffersEx"));
    Arg(String("n"), n);
    Arg(String("buffers"), ParamsToString(buffers));
    End();
    ECode ec = mgl11->GlGenBuffers(n, buffers);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glGetBooleanv(
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Boolean>* params,
    /* [in] */ Int32 offset)
{
    Begin(String("glGetBooleanv"));
    Arg(String("pname"), pname);
    Arg(String("params"), ParamsToString(params));
    Arg(String("offset"), offset);
    End();
    ECode ec = mgl11->GlGetBooleanv(pname, params, offset);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glGetBooleanv(
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params)
{
    Begin(String("GlGetBooleanvEx"));
    Arg(String("pname"), pname);
    Arg(String("params"), ParamsToString(params));
    End();
    ECode ec = mgl11->GlGetBooleanv(pname, params);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glGetBufferParameteriv(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params,
    /* [in] */ Int32 offset)
{
    Begin(String("glGetBufferParameteriv"));
    Arg(String("target"), target);
    Arg(String("pname"), pname);
    Arg(String("params"), ParamsToString(params));
    Arg(String("offset"), offset);
    End();
    ECode ec = mgl11->GlGetBufferParameteriv(target, pname, params, offset);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glGetBufferParameteriv(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params)
{
    Begin(String("GlGetBufferParameterivEx"));
    Arg(String("target"), target);
    Arg(String("pname"), pname);
    Arg(String("params"), ParamsToString(params));
    End();
    ECode ec = mgl11->GlGetBufferParameteriv(target, pname, params);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glGetClipPlanef(
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Float>* eqn,
    /* [in] */ Int32 offset)
{
    Begin(String("glGetClipPlanef"));
    Arg(String("pname"), pname);
    Arg(String("eqn"), ParamsToString(eqn));
    Arg(String("offset"), offset);
    End();
    ECode ec = mgl11->GlGetClipPlanef(pname, eqn, offset);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glGetClipPlanef(
    /* [in] */ Int32 pname,
    /* [in] */ IFloatBuffer* eqn)
{
    Begin(String("GlGetClipPlanefEx"));
    Arg(String("pname"), pname);
    Arg(String("eqn"), ParamsToString(eqn));
    End();
    ECode ec = mgl11->GlGetClipPlanef(pname, eqn);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glGetClipPlanex(
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* eqn,
    /* [in] */ Int32 offset)
{
    Begin(String("glGetClipPlanex"));
    Arg(String("pname"), pname);
    Arg(String("eqn"), ParamsToString(eqn));
    Arg(String("offset"), offset);
    End();
    return mgl11->GlGetClipPlanex(pname, eqn, offset);
}

ECode GLLogWrapper::glGetClipPlanex(
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* eqn)
{
    Begin(String("GlGetClipPlanexEx"));
    Arg(String("pname"), pname);
    Arg(String("eqn"), ParamsToString(eqn));
    End();
    ECode ec = mgl11->GlGetClipPlanex(pname, eqn);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glGetFixedv(
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params,
    /* [in] */ Int32 offset)
{
    Begin(String("glGetFixedv"));
    Arg(String("pname"), pname);
    Arg(String("params"), ParamsToString(params));
    Arg(String("offset"), offset);
    End();
    return mgl11->GlGetFixedv(pname, params, offset);
}

ECode GLLogWrapper::glGetFixedv(
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params)
{
    Begin(String("GlGetFixedvEx"));
    Arg(String("pname"), pname);
    Arg(String("params"), ParamsToString(params));
    End();
    ECode ec = mgl11->GlGetFixedv(pname, params);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glGetFloatv(
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Float>* params,
    /* [in] */ Int32 offset)
{
    Begin(String("glGetFloatv"));
    Arg(String("pname"), pname);
    Arg(String("params"), ParamsToString(params));
    Arg(String("offset"), offset);
    End();
    return mgl11->GlGetFloatv(pname, params, offset);
}

ECode GLLogWrapper::glGetFloatv(
    /* [in] */ Int32 pname,
    /* [in] */ IFloatBuffer* params)
{
    Begin(String("GlGetFloatvEx"));
    Arg(String("pname"), pname);
    Arg(String("params"), ParamsToString(params));
    End();
    ECode ec = mgl11->GlGetFloatv(pname, params);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glGetLightfv(
    /* [in] */ Int32 light,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Float>* params,
    /* [in] */ Int32 offset)
{
    Begin(String("glGetLightfv"));
    Arg(String("light"), light);
    Arg(String("pname"), pname);
    Arg(String("params"), ParamsToString(params));
    Arg(String("offset"), offset);
    End();
    ECode ec = mgl11->GlGetLightfv(light, pname, params, offset);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glGetLightfv(
    /* [in] */ Int32 light,
    /* [in] */ Int32 pname,
    /* [in] */ IFloatBuffer* params)
{
    Begin(String("GlGetLightfvEx"));
    Arg(String("light"), light);
    Arg(String("pname"), pname);
    Arg(String("params"), ParamsToString(params));
    End();
    ECode ec = mgl11->GlGetLightfv(light, pname, params);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glGetLightxv(
    /* [in] */ Int32 light,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params,
    /* [in] */ Int32 offset)
{
    Begin(String("glGetLightxv"));
    Arg(String("light"), light);
    Arg(String("pname"), pname);
    Arg(String("params"), ParamsToString(params));
    Arg(String("offset"), offset);
    End();
    ECode ec = mgl11->GlGetLightxv(light, pname, params, offset);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glGetLightxv(
    /* [in] */ Int32 light,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params)
{
    Begin(String("GlGetLightxvEx"));
    Arg(String("light"), light);
    Arg(String("pname"), pname);
    Arg(String("params"), ParamsToString(params));
    End();
    ECode ec = mgl11->GlGetLightxv(light, pname, params);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glGetMaterialfv(
    /* [in] */ Int32 face,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Float>* params,
    /* [in] */ Int32 offset)
{
    Begin(String("glGetMaterialfv"));
    Arg(String("face"), face);
    Arg(String("pname"), pname);
    Arg(String("params"), ParamsToString(params));
    Arg(String("offset"), offset);
    End();
    ECode ec = mgl11->GlGetMaterialfv(face, pname, params, offset);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glGetMaterialfv(
    /* [in] */ Int32 face,
    /* [in] */ Int32 pname,
    /* [in] */ IFloatBuffer* params)
{
    Begin(String("GlGetMaterialfvEx"));
    Arg(String("face"), face);
    Arg(String("pname"), pname);
    Arg(String("params"), ParamsToString(params));
    End();
    ECode ec = mgl11->GlGetMaterialfv(face, pname, params);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glGetMaterialxv(
    /* [in] */ Int32 face,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params,
    /* [in] */ Int32 offset)
{
    Begin(String("glGetMaterialxv"));
    Arg(String("face"), face);
    Arg(String("pname"), pname);
    Arg(String("params"), ParamsToString(params));
    Arg(String("offset"), offset);
    End();
    ECode ec = mgl11->GlGetMaterialxv(face, pname, params, offset);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glGetMaterialxv(
    /* [in] */ Int32 face,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params)
{
    Begin(String("GlGetMaterialxvEx"));
    Arg(String("face"), face);
    Arg(String("pname"), pname);
    Arg(String("params"), ParamsToString(params));
    End();
    ECode ec = mgl11->GlGetMaterialxv(face, pname, params);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glGetTexEnviv(
    /* [in] */ Int32 env,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params,
    /* [in] */ Int32 offset)
{
    Begin(String("glGetTexEnviv"));
    Arg(String("env"), env);
    Arg(String("pname"), pname);
    Arg(String("params"), ParamsToString(params));
    Arg(String("offset"), offset);
    End();
    ECode ec = mgl11->GlGetTexEnviv(env, pname, params, offset);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glGetTexEnviv(
    /* [in] */ Int32 env,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params)
{
    Begin(String("GlGetTexEnvivEx"));
    Arg(String("env"), env);
    Arg(String("pname"), pname);
    Arg(String("params"), ParamsToString(params));
    End();
    ECode ec = mgl11->GlGetTexEnviv(env, pname, params);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glGetTexEnvxv(
    /* [in] */ Int32 env,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params,
    /* [in] */ Int32 offset)
{
    Begin(String("glGetTexEnviv"));
    Arg(String("env"), env);
    Arg(String("pname"), pname);
    Arg(String("params"), ParamsToString(params));
    Arg(String("offset"), offset);
    End();
    ECode ec = mgl11->GlGetTexEnviv(env, pname, params, offset);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glGetTexEnvxv(
    /* [in] */ Int32 env,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params)
{
    Begin(String("GlGetTexEnvxvEx"));
    Arg(String("env"), env);
    Arg(String("pname"), pname);
    Arg(String("params"), ParamsToString(params));
    End();
    ECode ec = mgl11->GlGetTexEnvxv(env, pname, params);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glGetTexParameterfv(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Float>* params,
    /* [in] */ Int32 offset)
{
    Begin(String("glGetTexParameterfv"));
    Arg(String("target"), target);
    Arg(String("pname"), pname);
    Arg(String("params"), ParamsToString(params));
    Arg(String("offset"), offset);
    End();
    ECode ec = mgl11->GlGetTexParameterfv(target, pname, params, offset);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glGetTexParameterfv(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ IFloatBuffer* params)
{
    Begin(String("GlGetTexParameterfvEx"));
    Arg(String("target"), target);
    Arg(String("pname"), pname);
    Arg(String("params"), ParamsToString(params));
    End();
    ECode ec = mgl11->GlGetTexParameterfv(target, pname, params);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glGetTexParameteriv(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params,
    /* [in] */ Int32 offset)
{
    Begin(String("glGetTexParameteriv"));
    Arg(String("target"), target);
    Arg(String("pname"), pname);
    Arg(String("params"), ParamsToString(params));
    Arg(String("offset"), offset);
    End();
    ECode ec = mgl11->GlGetTexEnviv(target, pname, params, offset);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glGetTexParameteriv(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params)
{
    Begin(String("GlGetTexParameterivEx"));
    Arg(String("target"), target);
    Arg(String("pname"), pname);
    Arg(String("params"), ParamsToString(params));
    End();
    ECode ec = mgl11->GlGetTexParameteriv(target, pname, params);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glGetTexParameterxv(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params,
    /* [in] */ Int32 offset)
{
    Begin(String("glGetTexParameterxv"));
    Arg(String("target"), target);
    Arg(String("pname"), pname);
    Arg(String("params"), ParamsToString(params));
    Arg(String("offset"), offset);
    End();
    ECode ec = mgl11->GlGetTexParameterxv(target, pname, params, offset);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glGetTexParameterxv(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params)
{
    Begin(String("GlGetTexParameterxvEx"));
    Arg(String("target"), target);
    Arg(String("pname"), pname);
    Arg(String("params"), ParamsToString(params));
    End();
    ECode ec = mgl11->GlGetTexParameterxv(target, pname, params);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glIsBuffer(
    /* [in] */ Int32 buffer,
    /* [out] */ Boolean* result)
{
    Begin(String("glIsBuffer"));
    Arg(String("buffer"), buffer);
    End();
    ECode ec = mgl11->GlIsBuffer(buffer, result);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glIsEnabled(
    /* [in] */ Int32 cap,
    /* [out] */ Boolean* result)
{
    Begin(String("glIsEnabled"));
    Arg(String("cap"), cap);
    End();
    ECode ec = mgl11->GlIsEnabled(cap, result);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glIsTexture(
    /* [in] */ Int32 texture,
    /* [out] */ Boolean* result)
{
    Begin(String("glIsTexture"));
    Arg(String("texture"), texture);
    End();
    ECode ec = mgl11->GlIsTexture(texture, result);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glNormalPointer(
    /* [in] */ Int32 type,
    /* [in] */ Int32 stride,
    /* [in] */ Int32 offset)
{
    Begin(String("GlNormalPointerEx"));
    Arg(String("type"), type);
    Arg(String("stride"), stride);
    Arg(String("offset"), offset);
    End();
    return mgl11->GlNormalPointer(type, stride, offset);
}

ECode GLLogWrapper::glPointParameterf(
    /* [in] */ Int32 pname,
    /* [in] */ Float param)
{
    Begin(String("glPointParameterf"));
    Arg(String("pname"), pname);
    Arg(String("param"), param);
    End();
    ECode ec = mgl11->GlPointParameterf( pname, param);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glPointParameterfv(
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Float>* params,
    /* [in] */ Int32 offset)
{
    Begin(String("glPointParameterfv"));
    Arg(String("pname"), pname);
    Arg(String("params"), ParamsToString(params));
    Arg(String("offset"), offset);
    End();
    ECode ec = mgl11->GlPointParameterfv(pname, params, offset);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glPointParameterfv(
    /* [in] */ Int32 pname,
    /* [in] */ IFloatBuffer* params)
{
    Begin(String("GlPointParameterfvEx"));
    Arg(String("pname"), pname);
    Arg(String("params"), ParamsToString(params));
    End();
    ECode ec = mgl11->GlPointParameterfv(pname, params);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glPointParameterx(
    /* [in] */ Int32 pname,
    /* [in] */ Int32 param)
{
    Begin(String("glPointParameterfv"));
    Arg(String("pname"), pname);
    Arg(String("param"), param);
    End();
    ECode ec = mgl11->GlPointParameterx( pname, param);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glPointParameterxv(
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params,
    /* [in] */ Int32 offset)
{
    Begin(String("glPointParameterxv"));
    Arg(String("pname"), pname);
    Arg(String("params"), ParamsToString(params));
    Arg(String("offset"), offset);
    End();
    ECode ec = mgl11->GlPointParameterxv(pname, params, offset);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glPointParameterxv(
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params)
{
    Begin(String("GlPointParameterxvEx"));
    Arg(String("pname"), pname);
    Arg(String("params"), ParamsToString(params));
    End();
    ECode ec = mgl11->GlPointParameterxv( pname, params);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glPointSizePointerOES(
    /* [in] */ Int32 type,
    /* [in] */ Int32 stride,
    /* [in] */ IBuffer* pointer)
{
    Begin(String("glPointSizePointerOES"));
    Arg(String("type"), type);
    Arg(String("stride"), stride);
    Arg(String("params"), ParamsToString(pointer));
    End();
    ECode ec = mgl11->GlPointSizePointerOES( type, stride, pointer);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glTexCoordPointer(
    /* [in] */ Int32 size,
    /* [in] */ Int32 type,
    /* [in] */ Int32 stride,
    /* [in] */ Int32 offset)
{
    Begin(String("GlTexCoordPointerEx"));
    Arg(String("size"), size);
    Arg(String("type"), type);
    Arg(String("stride"), stride);
    Arg(String("offset"), offset);
    End();
    return mgl11->GlTexCoordPointer(size, type, stride, offset);
}

ECode GLLogWrapper::glTexEnvi(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ Int32 param)
{
    Begin(String("glTexEnvi"));
    Arg(String("target"), target);
    Arg(String("pname"), pname);
    Arg(String("param"), param);
    End();
    ECode ec = mgl11->GlTexEnvi(target, pname, param);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glTexEnviv(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params,
    /* [in] */ Int32 offset)
{
    Begin(String("glTexEnviv"));
    Arg(String("target"), target);
    Arg(String("pname"), pname);
    Arg(String("params"), ParamsToString(params));
    Arg(String("offset"), offset);
    End();
    ECode ec = mgl11->GlTexEnviv(target, pname, params, offset);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glTexEnviv(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params)
{
    Begin(String("GlTexEnvivEx"));
    Arg(String("target"), target);
    Arg(String("pname"), pname);
    Arg(String("params"), ParamsToString(params));
    End();
    ECode ec = mgl11->GlTexEnviv( target, pname, params);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glTexParameteri(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ Int32 param)
{
    Begin(String("glTexParameterxv"));
    Arg(String("target"), target);
    Arg(String("pname"), pname);
    Arg(String("param"), param);
    End();
    ECode ec = mgl11->GlTexParameteri(target, pname, param);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glTexParameteriv(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params,
    /* [in] */ Int32 offset)
{
    Begin(String("glTexParameteriv"));
    Arg(String("target"), GetTextureTarget(target));
    Arg(String("pname"), GetTexturePName(pname));
    Arg(String("params"), 4, params, offset);
    End();

    ECode ec = mgl11->GlTexParameteriv(target, pname, params, offset);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glTexParameteriv(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params)
{
    Begin(String("GlTexParameterivEx"));
    Arg(String("target"), GetTextureTarget(target));
    Arg(String("pname"), GetTexturePName(pname));
    Arg(String("params"), 4, params);
    End();

    ECode ec = mgl11->GlTexParameteriv(target, pname, params);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glTexParameterxv(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params,
    /* [in] */ Int32 offset)
{
    Begin(String("glTexParameterxv"));
    Arg(String("target"), target);
    Arg(String("pname"), pname);
    Arg(String("params"), ParamsToString(params));
    Arg(String("offset"), offset);
    End();
    ECode ec = mgl11->GlTexParameterxv(target, pname, params, offset);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glTexParameterxv(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params)
{
    Begin(String("GlTexParameterxvEx"));
    Arg(String("target"), target);
    Arg(String("pname"), pname);
    Arg(String("params"), ParamsToString(params));
    End();
    ECode ec = mgl11->GlTexParameterxv(target, pname, params);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glVertexPointer(
    /* [in] */ Int32 size,
    /* [in] */ Int32 type,
    /* [in] */ Int32 stride,
    /* [in] */ Int32 offset)
{
    Begin(String("GlVertexPointerEx"));
    Arg(String("size"), size);
    Arg(String("type"), type);
    Arg(String("stride"), stride);
    Arg(String("offset"), offset);
    End();
    return mgl11->GlVertexPointer(size, type, stride, offset);
}

ECode GLLogWrapper::glCurrentPaletteMatrixOES(
    /* [in] */ Int32 matrixpaletteindex)
{
    Begin(String("glCurrentPaletteMatrixOES"));
    Arg(String("matrixpaletteindex"), matrixpaletteindex);
    End();
    ECode ec = mgl11Ext->GlCurrentPaletteMatrixOES(matrixpaletteindex);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glDrawTexfOES(
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Float z,
    /* [in] */ Float width,
    /* [in] */ Float height)
{
    Begin(String("glDrawTexfOES"));
    Arg(String("x"), x);
    Arg(String("y"), y);
    Arg(String("z"), z);
    Arg(String("width"), width);
    Arg(String("height"), height);
    End();
    ECode ec = mgl11Ext->GlDrawTexfOES(x, y, z, width, height);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glDrawTexfvOES(
    /* [in] */ ArrayOf<Float>* coords,
    /* [in] */ Int32 offset)
{
    Begin(String("glDrawTexfvOES"));
    Arg(String("coords"), 5, coords, offset);
    Arg(String("offset"), offset);
    End();
    ECode ec = mgl11Ext->GlDrawTexfvOES(coords, offset);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glDrawTexfvOES(
    /* [in] */ IFloatBuffer* coords)
{
    Begin(String("GlDrawTexfvOESEx"));
    Arg(String("coords"), 5, coords);
    End();
    ECode ec = mgl11Ext->GlDrawTexfvOES(coords);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glDrawTexiOES(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 z,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    Begin(String("glDrawTexiOES"));
    Arg(String("x"), x);
    Arg(String("y"), y);
    Arg(String("z"), z);
    Arg(String("width"), width);
    Arg(String("height"), height);
    End();
    ECode ec = mgl11Ext->GlDrawTexiOES(x, y, z, width, height);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glDrawTexivOES(
    /* [in] */ ArrayOf<Int32>* coords,
    /* [in] */ Int32 offset)
{
    Begin(String("glDrawTexivOES"));
    Arg(String("coords"), 5, coords, offset);
    Arg(String("offset"), offset);
    End();
    ECode ec = mgl11Ext->GlDrawTexivOES(coords, offset);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glDrawTexivOES(
    /* [in] */ IInt32Buffer* coords)
{
    Begin(String("GlDrawTexivOESEx"));
    Arg(String("coords"), 5, coords);
    End();
    ECode ec = mgl11Ext->GlDrawTexivOES(coords);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glDrawTexsOES(
    /* [in] */ Int16 x,
    /* [in] */ Int16 y,
    /* [in] */ Int16 z,
    /* [in] */ Int16 width,
    /* [in] */ Int16 height)
{
    Begin(String("glDrawTexsOES"));
    Arg(String("x"), x);
    Arg(String("y"), y);
    Arg(String("z"), z);
    Arg(String("width"), width);
    Arg(String("height"), height);
    End();
    ECode ec = mgl11Ext->GlDrawTexsOES(x, y, z, width, height);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glDrawTexsvOES(
    /* [in] */ ArrayOf<Int16>* coords,
    /* [in] */ Int32 offset)
{
    Begin(String("glDrawTexsvOES"));
    Arg(String("coords"), 5, coords, offset);
    Arg(String("offset"), offset);
    End();
    ECode ec = mgl11Ext->GlDrawTexsvOES(coords, offset);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glDrawTexsvOES(
    /* [in] */ IInt16Buffer* coords)
{
    Begin(String("GlDrawTexsvOESEx"));
    Arg(String("coords"), 5, coords);
    End();
    ECode ec = mgl11Ext->GlDrawTexsvOES(coords);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glDrawTexxOES(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 z,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    Begin(String("glDrawTexxOES"));
    Arg(String("x"), x);
    Arg(String("y"), y);
    Arg(String("z"), z);
    Arg(String("width"), width);
    Arg(String("height"), height);
    End();
    ECode ec = mgl11Ext->GlDrawTexxOES(x, y, z, width, height);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glDrawTexxvOES(
    /* [in] */ ArrayOf<Int32>* coords,
    /* [in] */ Int32 offset)
{
    Begin(String("glDrawTexxvOES"));
    Arg(String("coords"), 5, coords, offset);
    Arg(String("offset"), offset);
    End();
    ECode ec = mgl11Ext->GlDrawTexxvOES(coords, offset);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glDrawTexxvOES(
    /* [in] */ IInt32Buffer* coords)
{
    Begin(String("GlDrawTexxvOESEx"));
    Arg(String("coords"), 5, coords);
    End();
    ECode ec = mgl11Ext->GlDrawTexxvOES(coords);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glLoadPaletteFromModelViewMatrixOES()
{
    Begin(String("glLoadPaletteFromModelViewMatrixOES"));
    End();
    ECode ec = mgl11Ext->GlLoadPaletteFromModelViewMatrixOES();
    CheckError();
    return ec;
}

ECode GLLogWrapper::glMatrixIndexPointerOES(
    /* [in] */ Int32 size,
    /* [in] */ Int32 type,
    /* [in] */ Int32 stride,
    /* [in] */ IBuffer* pointer)
{
    Begin(String("glMatrixIndexPointerOES"));
    ArgPointer(size, type, stride, pointer);
    End();
    ECode ec = mgl11Ext->GlMatrixIndexPointerOES(size, type, stride, pointer);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glMatrixIndexPointerOES(
    /* [in] */ Int32 size,
    /* [in] */ Int32 type,
    /* [in] */ Int32 stride,
    /* [in] */ Int32 offset)
{
    Begin(String("GlMatrixIndexPointerOESEx"));
    Arg(String("size"), size);
    Arg(String("type"), type);
    Arg(String("stride"), stride);
    Arg(String("offset"), offset);
    End();
    ECode ec = mgl11Ext->GlMatrixIndexPointerOES(size, type, stride, offset);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glWeightPointerOES(
    /* [in] */ Int32 size,
    /* [in] */ Int32 type,
    /* [in] */ Int32 stride,
    /* [in] */ IBuffer* pointer)
{
    Begin(String("glWeightPointerOES"));
    ArgPointer(size, type, stride, pointer);
    End();
    ECode ec = mgl11Ext->GlWeightPointerOES(size, type, stride, pointer);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glWeightPointerOES(
    /* [in] */ Int32 size,
    /* [in] */ Int32 type,
    /* [in] */ Int32 stride,
    /* [in] */ Int32 offset)
{
    Begin(String("GlWeightPointerOESEx"));
    Arg(String("size"), size);
    Arg(String("type"), type);
    Arg(String("stride"), stride);
    Arg(String("offset"), offset);
    End();
    ECode ec = mgl11Ext->GlWeightPointerOES(size, type, stride, offset);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glBindFramebufferOES(
    /* [in] */ Int32 target,
    /* [in] */ Int32 framebuffer)
{
    Begin(String("glBindFramebufferOES"));
    Arg(String("target"), target);
    Arg(String("framebuffer"), framebuffer);
    End();
    ECode ec = mgl11ExtensionPack->GlBindFramebufferOES(target, framebuffer);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glBindRenderbufferOES(
    /* [in] */ Int32 target,
    /* [in] */ Int32 renderbuffer)
{
    Begin(String("glBindRenderbufferOES"));
    Arg(String("target"), target);
    Arg(String("renderbuffer"), renderbuffer);
    End();
    ECode ec = mgl11ExtensionPack->GlBindRenderbufferOES(target, renderbuffer);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glBlendEquation(
    /* [in] */ Int32 mode)
{
    Begin(String("glBlendEquation"));
    Arg(String("mode"), mode);
    End();
    ECode ec = mgl11ExtensionPack->GlBlendEquation(mode);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glBlendEquationSeparate(
    /* [in] */ Int32 modeRGB,
    /* [in] */ Int32 modeAlpha)
{
    Begin(String("glBlendEquationSeparate"));
    Arg(String("modeRGB"), modeRGB);
    Arg(String("modeAlpha"), modeAlpha);
    End();
    ECode ec = mgl11ExtensionPack->GlBlendEquationSeparate(modeRGB, modeAlpha);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glBlendFuncSeparate(
    /* [in] */ Int32 srcRGB,
    /* [in] */ Int32 dstRGB,
    /* [in] */ Int32 srcAlpha,
    /* [in] */ Int32 dstAlpha)
{
    Begin(String("glBlendFuncSeparate"));
    Arg(String("srcRGB"), srcRGB);
    Arg(String("dstRGB"), dstRGB);
    Arg(String("srcAlpha"), srcAlpha);
    Arg(String("dstAlpha"), dstAlpha);
    End();
    ECode ec = mgl11ExtensionPack->GlBlendFuncSeparate(srcRGB, dstRGB, srcAlpha, dstAlpha);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glCheckFramebufferStatusOES(
    /* [in] */ Int32 target,
    /* [out] */ Int32* status)
{
    Begin(String("glCheckFramebufferStatusOES"));
    Arg(String("target"), target);
    End();
    ECode ec = mgl11ExtensionPack->GlCheckFramebufferStatusOES(target, status);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glDeleteFramebuffersOES(
    /* [in] */ Int32 n,
    /* [in] */ ArrayOf<Int32>* framebuffers,
    /* [in] */ Int32 offset)
{
    Begin(String("glDeleteFramebuffersOES"));
    Arg(String("n"), n);
    Arg(String("framebuffers"), ParamsToString(framebuffers));
    Arg(String("offset"), offset);
    End();
    ECode ec = mgl11ExtensionPack->GlDeleteFramebuffersOES(n, framebuffers, offset);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glDeleteFramebuffersOES(
    /* [in] */ Int32 n,
    /* [in] */ IInt32Buffer* framebuffers)
{
    Begin(String("GlDeleteFramebuffersOESEx"));
    Arg(String("n"), n);
    Arg(String("framebuffers"), ParamsToString(framebuffers));
    End();
    ECode ec = mgl11ExtensionPack->GlDeleteFramebuffersOES(n, framebuffers);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glDeleteRenderbuffersOES(
    /* [in] */ Int32 n,
    /* [in] */ ArrayOf<Int32>* renderbuffers,
    /* [in] */ Int32 offset)
{
    Begin(String("glDeleteRenderbuffersOES"));
    Arg(String("n"), n);
    Arg(String("renderbuffers"), ParamsToString(renderbuffers));
    Arg(String("offset"), offset);
    End();
    ECode ec = mgl11ExtensionPack->GlDeleteRenderbuffersOES(n, renderbuffers, offset);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glDeleteRenderbuffersOES(
    /* [in] */ Int32 n,
    /* [in] */ IInt32Buffer* renderbuffers)
{
    Begin(String("GlDeleteRenderbuffersOESEx"));
    Arg(String("n"), n);
    Arg(String("renderbuffers"), ParamsToString(renderbuffers));
    End();
    ECode ec = mgl11ExtensionPack->GlDeleteRenderbuffersOES(n, renderbuffers);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glFramebufferRenderbufferOES(
    /* [in] */ Int32 target,
    /* [in] */ Int32 attachment,
    /* [in] */ Int32 renderbuffertarget,
    /* [in] */ Int32 renderbuffer)
{
    Begin(String("glFramebufferRenderbufferOES"));
    Arg(String("target"), target);
    Arg(String("attachment"), attachment);
    Arg(String("renderbuffertarget"), renderbuffertarget);
    Arg(String("renderbuffer"), renderbuffer);
    End();
    ECode ec = mgl11ExtensionPack->GlFramebufferRenderbufferOES(target, attachment, renderbuffertarget, renderbuffer);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glFramebufferTexture2DOES(
    /* [in] */ Int32 target,
    /* [in] */ Int32 attachment,
    /* [in] */ Int32 textarget,
    /* [in] */ Int32 texture,
    /* [in] */ Int32 level)
{
    Begin(String("glFramebufferTexture2DOES"));
    Arg(String("target"), target);
    Arg(String("attachment"), attachment);
    Arg(String("textarget"), textarget);
    Arg(String("texture"), texture);
    Arg(String("level"), level);
    End();
    ECode ec = mgl11ExtensionPack->GlFramebufferTexture2DOES(target, attachment, textarget, texture, level);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glGenerateMipmapOES(
    /* [in] */ Int32 target)
{
    Begin(String("glGenerateMipmapOES"));
    Arg(String("target"), target);
    End();
    ECode ec = mgl11ExtensionPack->GlGenerateMipmapOES(target);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glGenFramebuffersOES(
    /* [in] */ Int32 n,
    /* [in] */ ArrayOf<Int32>* framebuffers,
    /* [in] */ Int32 offset)
{
    Begin(String("glGenFramebuffersOES"));
    Arg(String("n"), n);
    Arg(String("framebuffers"), ParamsToString(framebuffers));
    Arg(String("offset"), offset);
    End();
    ECode ec = mgl11ExtensionPack->GlGenFramebuffersOES(n, framebuffers, offset);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glGenFramebuffersOES(
    /* [in] */ Int32 n,
    /* [in] */ IInt32Buffer* framebuffers)
{
    Begin(String("GlGenFramebuffersOESEx"));
    Arg(String("n"), n);
    Arg(String("framebuffers"), ParamsToString(framebuffers));
    End();
    ECode ec = mgl11ExtensionPack->GlGenFramebuffersOES(n, framebuffers);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glGenRenderbuffersOES(
    /* [in] */ Int32 n,
    /* [in] */ ArrayOf<Int32>* renderbuffers,
    /* [in] */ Int32 offset)
{
    Begin(String("glGenRenderbuffersOES"));
    Arg(String("n"), n);
    Arg(String("renderbuffers"), ParamsToString(renderbuffers));
    Arg(String("offset"), offset);
    End();
    ECode ec = mgl11ExtensionPack->GlGenRenderbuffersOES(n, renderbuffers, offset);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glGenRenderbuffersOES(
    /* [in] */ Int32 n,
    /* [in] */ IInt32Buffer* renderbuffers)
{
    Begin(String("GlGenRenderbuffersOESEx"));
    Arg(String("n"), n);
    Arg(String("renderbuffers"), ParamsToString(renderbuffers));
    End();
    ECode ec = mgl11ExtensionPack->GlGenRenderbuffersOES(n, renderbuffers);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glGetFramebufferAttachmentParameterivOES(
    /* [in] */ Int32 target,
    /* [in] */ Int32 attachment,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params,
    /* [in] */ Int32 offset)
{
    Begin(String("glGetFramebufferAttachmentParameterivOES"));
    Arg(String("target"), target);
    Arg(String("attachment"), attachment);
    Arg(String("pname"), pname);
    Arg(String("params"), ParamsToString(params));
    Arg(String("offset"), offset);
    End();
    ECode ec = mgl11ExtensionPack->GlGetFramebufferAttachmentParameterivOES(target, attachment, pname, params, offset);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glGetFramebufferAttachmentParameterivOES(
    /* [in] */ Int32 target,
    /* [in] */ Int32 attachment,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params)
{
    Begin(String("GlGetFramebufferAttachmentParameterivOESEx"));
    Arg(String("target"), target);
    Arg(String("attachment"), attachment);
    Arg(String("pname"), pname);
    Arg(String("params"), ParamsToString(params));
    End();
    ECode ec = mgl11ExtensionPack->GlGetFramebufferAttachmentParameterivOES(target, attachment, pname, params);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glGetRenderbufferParameterivOES(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params,
    /* [in] */ Int32 offset)
{
    Begin(String("glGetRenderbufferParameterivOES"));
    Arg(String("target"), target);
    Arg(String("pname"), pname);
    Arg(String("params"), ParamsToString(params));
    Arg(String("offset"), offset);
    End();
    ECode ec = mgl11ExtensionPack->GlGetRenderbufferParameterivOES(target, pname, params, offset);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glGetRenderbufferParameterivOES(
    /* [in] */ Int32 target,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params)
{
    Begin(String("GlGetRenderbufferParameterivOESEx"));
    Arg(String("target"), target);
    Arg(String("pname"), pname);
    Arg(String("params"), ParamsToString(params));
    End();
    ECode ec = mgl11ExtensionPack->GlGetRenderbufferParameterivOES(target, pname, params);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glGetTexGenfv(
    /* [in] */ Int32 coord,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Float>* params,
    /* [in] */ Int32 offset)
{
    Begin(String("glGetTexGenfv"));
    Arg(String("coord"), coord);
    Arg(String("pname"), pname);
    Arg(String("params"), ParamsToString(params));
    Arg(String("offset"), offset);
    End();
    ECode ec = mgl11ExtensionPack->GlGetTexGenfv(coord, pname, params, offset);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glGetTexGenfv(
    /* [in] */ Int32 coord,
    /* [in] */ Int32 pname,
    /* [in] */ IFloatBuffer* params)
{
    Begin(String("GlGetTexGenfvEx"));
    Arg(String("coord"), coord);
    Arg(String("pname"), pname);
    Arg(String("params"), ParamsToString(params));
    End();
    ECode ec = mgl11ExtensionPack->GlGetTexGenfv(coord, pname, params);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glGetTexGeniv(
    /* [in] */ Int32 coord,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params,
    /* [in] */ Int32 offset)
{
    Begin(String("glGetTexGeniv"));
    Arg(String("coord"), coord);
    Arg(String("pname"), pname);
    Arg(String("params"), ParamsToString(params));
    Arg(String("offset"), offset);
    End();
    ECode ec = mgl11ExtensionPack->GlGetTexGeniv(coord, pname, params, offset);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glGetTexGeniv(
    /* [in] */ Int32 coord,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params)
{
    Begin(String("GlGetTexGenivEx"));
    Arg(String("coord"), coord);
    Arg(String("pname"), pname);
    Arg(String("params"), ParamsToString(params));
    End();
    ECode ec = mgl11ExtensionPack->GlGetTexGeniv(coord, pname, params);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glGetTexGenxv(
    /* [in] */ Int32 coord,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params,
    /* [in] */ Int32 offset)
{
    Begin(String("glGetTexGenxv"));
    Arg(String("coord"), coord);
    Arg(String("pname"), pname);
    Arg(String("params"), ParamsToString(params));
    Arg(String("offset"), offset);
    End();
    ECode ec = mgl11ExtensionPack->GlGetTexGenxv(coord, pname, params, offset);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glGetTexGenxv(
    /* [in] */ Int32 coord,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params)
{
    Begin(String("GlGetTexGenxvEx"));
    Arg(String("coord"), coord);
    Arg(String("pname"), pname);
    Arg(String("params"), ParamsToString(params));
    End();
    ECode ec = mgl11ExtensionPack->GlGetTexGenxv(coord, pname, params);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glIsFramebufferOES(
    /* [in] */ Int32 framebuffer,
    /* [out] */ Boolean* result)
{
    Begin(String("glIsFramebufferOES"));
    Arg(String("framebuffer"), framebuffer);
    End();
    ECode ec = mgl11ExtensionPack->GlIsFramebufferOES(framebuffer, result);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glIsRenderbufferOES(
    /* [in] */ Int32 renderbuffer,
    /* [out] */ Boolean* result)
{
    Begin(String("glIsRenderbufferOES"));
    Arg(String("renderbuffer"), renderbuffer);
    End();
    ECode ec = mgl11ExtensionPack->GlIsRenderbufferOES(renderbuffer, result);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glRenderbufferStorageOES(
    /* [in] */ Int32 target,
    /* [in] */ Int32 internalformat,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    Begin(String("glRenderbufferStorageOES"));
    Arg(String("target"), target);
    Arg(String("internalformat"), internalformat);
    Arg(String("width"), width);
    Arg(String("height"), height);
    End();
    ECode ec = mgl11ExtensionPack->GlRenderbufferStorageOES(target, internalformat, width, height);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glTexGenf(
    /* [in] */ Int32 coord,
    /* [in] */ Int32 pname,
    /* [in] */ Float param)
{
    Begin(String("glTexGenf"));
    Arg(String("coord"), coord);
    Arg(String("pname"), pname);
    Arg(String("param"), param);
    End();
    ECode ec = mgl11ExtensionPack->GlTexGenf(coord, pname, param);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glTexGenfv(
    /* [in] */ Int32 coord,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Float>* params,
    /* [in] */ Int32 offset)
{
    Begin(String("glTexGenfv"));
    Arg(String("coord"), coord);
    Arg(String("pname"), pname);
    Arg(String("params"), ParamsToString(params));
    Arg(String("offset"), offset);
    End();
    ECode ec = mgl11ExtensionPack->GlTexGenfv(coord, pname, params, offset);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glTexGenfv(
    /* [in] */ Int32 coord,
    /* [in] */ Int32 pname,
    /* [in] */ IFloatBuffer* params)
{
    Begin(String("GlTexGenfvEx"));
    Arg(String("coord"), coord);
    Arg(String("pname"), pname);
    Arg(String("params"), ParamsToString(params));
    End();
    ECode ec = mgl11ExtensionPack->GlTexGenfv(coord, pname, params);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glTexGeni(
    /* [in] */ Int32 coord,
    /* [in] */ Int32 pname,
    /* [in] */ Int32 param)
{
    Begin(String("glTexGeni"));
    Arg(String("coord"), coord);
    Arg(String("pname"), pname);
    Arg(String("param"), param);
    End();
    ECode ec = mgl11ExtensionPack->GlTexGeni(coord, pname, param);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glTexGeniv(
    /* [in] */ Int32 coord,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params,
    /* [in] */ Int32 offset)
{
    Begin(String("glTexGeniv"));
    Arg(String("coord"), coord);
    Arg(String("pname"), pname);
    Arg(String("params"), ParamsToString(params));
    Arg(String("offset"), offset);
    End();
    ECode ec = mgl11ExtensionPack->GlTexGeniv(coord, pname, params, offset);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glTexGeniv(
    /* [in] */ Int32 coord,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params)
{
    Begin(String("GlTexGenivEx"));
    Arg(String("coord"), coord);
    Arg(String("pname"), pname);
    Arg(String("params"), ParamsToString(params));
    End();
    ECode ec = mgl11ExtensionPack->GlTexGeniv(coord, pname, params);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glTexGenx(
    /* [in] */ Int32 coord,
    /* [in] */ Int32 pname,
    /* [in] */ Int32 param)
{
    Begin(String("glTexGenx"));
    Arg(String("coord"), coord);
    Arg(String("pname"), pname);
    Arg(String("param"), param);
    End();
    ECode ec = mgl11ExtensionPack->GlTexGenx(coord, pname, param);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glTexGenxv(
    /* [in] */ Int32 coord,
    /* [in] */ Int32 pname,
    /* [in] */ ArrayOf<Int32>* params,
    /* [in] */ Int32 offset)
{
    Begin(String("glTexGenxv"));
    Arg(String("coord"), coord);
    Arg(String("pname"), pname);
    Arg(String("params"), ParamsToString(params));
    Arg(String("offset"), offset);
    End();
    ECode ec = mgl11ExtensionPack->GlTexGenxv(coord, pname, params, offset);
    CheckError();
    return ec;
}

ECode GLLogWrapper::glTexGenxv(
    /* [in] */ Int32 coord,
    /* [in] */ Int32 pname,
    /* [in] */ IInt32Buffer* params)
{
    Begin(String("GlTexGenxvEx"));
    Arg(String("coord"), coord);
    Arg(String("pname"), pname);
    Arg(String("params"), ParamsToString(params));
    End();
    ECode ec = mgl11ExtensionPack->GlTexGenxv(coord, pname, params);
    CheckError();
    return ec;
}

template<typename T>
String GLLogWrapper::TranArrayToString(
    /* [in] */ ArrayOf<T>* array)
{
    StringBuilder bd;
    Int32 len = array->GetLength();
    for(Int32 i = 0; i < len; i++)
    {
        bd += (*array)[i];
        bd += ' ';
    }
    return bd.ToString();
}

template<typename T>
String GLLogWrapper::ParamsToString(
    /* [in] */ ArrayOf<T>* array)
{
    return TranArrayToString(array);
}

String GLLogWrapper::ParamsToString(
    /* [in] */ IBuffer* buf)
{
    String str;
    buf->ToString(&str);
    return str;
}

} // namespace Opengl
} // namespace Droid
} // namespace Elastos
