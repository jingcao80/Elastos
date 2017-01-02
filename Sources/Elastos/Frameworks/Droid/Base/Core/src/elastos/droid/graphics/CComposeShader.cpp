//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/graphics/CComposeShader.h"
#include "elastos/droid/graphics/Xfermode.h"
#include <skia/core/SkComposeShader.h>
#include <skia/effects/SkPorterDuff.h>

namespace Elastos {
namespace Droid {
namespace Graphics {

const Int32 CComposeShader::TYPE_XFERMODE = 1;
const Int32 CComposeShader::TYPE_PORTERDUFFMODE = 2;

CAR_OBJECT_IMPL(CComposeShader);
CAR_INTERFACE_IMPL(CComposeShader, Shader, IComposeShader);
CComposeShader::CComposeShader()
    : mType(0)
    , mPorterDuffMode(-1)
{}

ECode CComposeShader::constructor(
    /* [in] */ IShader* shaderA,
    /* [in] */ IShader* shaderB,
    /* [in] */ IXfermode* mode)
{
    mType = TYPE_XFERMODE;
    mShaderA = shaderA;
    mShaderB = shaderB;
    mXferMode = mode;
    Init(NativeCreate1(((Shader*)shaderA)->mNativeInstance,
            ((Shader*)shaderB)->mNativeInstance,
            (mode != NULL) ? ((Xfermode*)mode)->mNativeInstance : 0));
    return NOERROR;
}

ECode CComposeShader::constructor(
    /* [in] */ IShader* shaderA,
    /* [in] */ IShader* shaderB,
    /* [in] */ PorterDuffMode mode)
{
    mType = TYPE_PORTERDUFFMODE;
    mShaderA = shaderA;
    mShaderB = shaderB;
    mPorterDuffMode = mode;
    Init(NativeCreate2(((Shader*)shaderA)->mNativeInstance,
            ((Shader*)shaderB)->mNativeInstance,
            mode));
    return NOERROR;
}

ECode CComposeShader::Copy(
    /* [out] */ IShader** shader)
{
    VALIDATE_NOT_NULL(shader);
    AutoPtr<IShader> copy;
    switch (mType) {
        case TYPE_XFERMODE: {
            AutoPtr<IShader> c1;
            AutoPtr<IShader> c2;
            ((Shader*)mShaderA.Get())->Copy((IShader**)&c1);
            ((Shader*)mShaderB.Get())->Copy((IShader**)&c2);
            CComposeShader::New(c1, c2, mXferMode, (IShader**)&copy);
            }
            break;
        case TYPE_PORTERDUFFMODE: {
            AutoPtr<IShader> c1;
            AutoPtr<IShader> c2;
            ((Shader*)mShaderA.Get())->Copy((IShader**)&c1);
            ((Shader*)mShaderB.Get())->Copy((IShader**)&c2);
            CComposeShader::New(c1, c2, mPorterDuffMode, (IShader**)&copy);
            }
            break;
        default:
            // throw new IllegalArgumentException(
            //         "ComposeShader should be created with either Xfermode or PorterDuffMode");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    CopyLocalMatrix(copy);
    *shader = copy;
    REFCOUNT_ADD(*shader);
    return NOERROR;
}

Int64 CComposeShader::NativeCreate1(
    /* [in] */ Int64 shaderAHandle,
    /* [in] */ Int64 shaderBHandle,
    /* [in] */ Int64 modeHandle)
{
    SkShader* shaderA = reinterpret_cast<SkShader *>(shaderAHandle);
    SkShader* shaderB = reinterpret_cast<SkShader *>(shaderBHandle);
    SkXfermode* mode = reinterpret_cast<SkXfermode *>(modeHandle);
    SkShader* shader = new SkComposeShader(shaderA, shaderB, mode);
    return reinterpret_cast<Int64>(shader);
}

Int64 CComposeShader::NativeCreate2(
    /* [in] */ Int64 shaderAHandle,
    /* [in] */ Int64 shaderBHandle,
    /* [in] */ Int32 porterDuffModeHandle)
{
    SkShader* shaderA = reinterpret_cast<SkShader *>(shaderAHandle);
    SkShader* shaderB = reinterpret_cast<SkShader *>(shaderBHandle);
    SkPorterDuff::Mode porterDuffMode = static_cast<SkPorterDuff::Mode>(porterDuffModeHandle);
    SkAutoUnref au(SkPorterDuff::CreateXfermode(porterDuffMode));
    SkXfermode* mode = (SkXfermode*) au.get();
    SkShader* shader = new SkComposeShader(shaderA, shaderB, mode);
    return reinterpret_cast<Int64>(shader);
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
