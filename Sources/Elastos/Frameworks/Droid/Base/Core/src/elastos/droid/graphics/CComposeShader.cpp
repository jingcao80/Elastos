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
#include "elastos/droid/graphics/Matrix.h"
#include "elastos/droid/graphics/Xfermode.h"
#include <skia/core/SkMatrix.h>
#include <skia/core/SkShader.h>

namespace Elastos {
namespace Droid {
namespace Graphics {

CAR_OBJECT_IMPL(CComposeShader);
CAR_INTERFACE_IMPL(CComposeShader, Shader, IComposeShader);

CComposeShader::CComposeShader()
    : mPorterDuffMode(-1)
{}

ECode CComposeShader::constructor(
    /* [in] */ IShader* shaderA,
    /* [in] */ IShader* shaderB,
    /* [in] */ IXfermode* mode)
{
    Int32 modeVal = ((Xfermode*)mode)->mPorterDuffMode;
    return constructor(shaderA, shaderB, modeVal);
}

ECode CComposeShader::constructor(
    /* [in] */ IShader* shaderA,
    /* [in] */ IShader* shaderB,
    /* [in] */ PorterDuffMode mode)
{
    if (shaderA == NULL || shaderB == NULL) {
        // throw new IllegalArgumentException("Shader parameters must not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mShaderA = shaderA;
    mShaderB = shaderB;
    mPorterDuffMode = mode;
    Init(CreateNativeInstance(mLocalMatrix));
    return NOERROR;
}

ECode CComposeShader::Copy(
    /* [out] */ IShader** shader)
{
    VALIDATE_NOT_NULL(shader);
    AutoPtr<IShader> c1;
    AutoPtr<IShader> c2;
    ((Shader*)mShaderA.Get())->Copy((IShader**)&c1);
    ((Shader*)mShaderB.Get())->Copy((IShader**)&c2);
    AutoPtr<IShader> copy;
    CComposeShader::New(c1, c2, mPorterDuffMode, (IShader**)&copy);
    CopyLocalMatrix(copy);
    *shader = copy;
    REFCOUNT_ADD(*shader);
    return NOERROR;
}

Int64 CComposeShader::CreateNativeInstance(
    /* [in] */ IMatrix* matrix)
{
    Int64 nativeMatrix = matrix == NULL ? 0 :
            ((Matrix*)matrix)->mNativeMatrix;
    return NativeCreate(nativeMatrix,
            ((Shader*)mShaderA.Get())->mNativeInstance,
            ((Shader*)mShaderB.Get())->mNativeInstance,
            mPorterDuffMode);
}

Int64 CComposeShader::NativeCreate(
    /* [in] */ Int64 matrixHandle,
    /* [in] */ Int64 shaderAHandle,
    /* [in] */ Int64 shaderBHandle,
    /* [in] */ Int64 modeHandle)
{
    const SkMatrix* matrix = reinterpret_cast<const SkMatrix*>(matrixHandle);
    SkShader* shaderA = reinterpret_cast<SkShader *>(shaderAHandle);
    SkShader* shaderB = reinterpret_cast<SkShader *>(shaderBHandle);
    SkBlendMode mode = static_cast<SkBlendMode>(modeHandle);
    sk_sp<SkShader> baseShader(SkShader::MakeComposeShader(
            sk_ref_sp(shaderA), sk_ref_sp(shaderB), mode));

    SkShader* shader;

    if (matrix) {
        shader = baseShader->makeWithLocalMatrix(*matrix).release();
    } else {
        shader = baseShader.release();
    }
    return reinterpret_cast<Int64>(shader);
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
