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
#include "elastos/droid/graphics/Shader.h"
#include "elastos/droid/graphics/CShader.h"
#include "elastos/droid/graphics/CMatrix.h"
#include <skia/core/SkShader.h>

namespace Elastos {
namespace Droid {
namespace Graphics {

CAR_INTERFACE_IMPL(Shader, Object, IShader);

Shader::Shader()
    : mNativeInstance(0)
{}

Shader::~Shader()
{
    NativeDestructor(mNativeInstance);
}

ECode Shader::GetLocalMatrix(
    /* [in, out] */ IMatrix* localM,
    /* [out] */ Boolean* has)
{
    VALIDATE_NOT_NULL(has);
    if (mLocalMatrix != NULL) {
        localM->Set(mLocalMatrix);
        Boolean result;
        mLocalMatrix->IsIdentity(&result);
        *has = !result;
        return NOERROR;
    }
    *has = FALSE;
    return NOERROR;
}

ECode Shader::SetLocalMatrix(
    /* [in] */ IMatrix* localM)
{
    mLocalMatrix = localM;
    NativeSetLocalMatrix(mNativeInstance,
        localM == NULL ? 0 : ((Matrix*)localM)->mNativeMatrix);
    return NOERROR;
}

void Shader::Init(
    /* [in] */ Int64 ni)
{
    mNativeInstance = ni;
}

ECode Shader::Copy(
    /* [out] */ IShader** shader)
{
    VALIDATE_NOT_NULL(shader);
    AutoPtr<IShader> copy;
    CShader::New((IShader**)&copy);
    CopyLocalMatrix(copy);
    *shader = copy;
    REFCOUNT_ADD(*shader);
    return NOERROR;
}

void Shader::CopyLocalMatrix(
    /* [in] */ IShader* dest)
{
    if (mLocalMatrix != NULL) {
        AutoPtr<IMatrix> lm;
        CMatrix::New((IMatrix**)&lm);
        Boolean has = FALSE;
        GetLocalMatrix(lm, &has);
        dest->SetLocalMatrix(lm);
    } else {
        dest->SetLocalMatrix(NULL);
    }
}

Int64 Shader::GetNativeInstance()
{
    return mNativeInstance;
}

void Shader::NativeDestructor(
    /* [in] */ Int64 shaderHandle)
{
    SkShader* shader = reinterpret_cast<SkShader*>(shaderHandle);
    SkSafeUnref(shader);
}

void Shader::NativeSetLocalMatrix(
    /* [in] */ Int64 shaderHandle,
    /* [in] */ Int64 matrixHandle)
{
    SkShader* shader       = reinterpret_cast<SkShader*>(shaderHandle);
    if (shader) {
        SkMatrix* matrix = reinterpret_cast<SkMatrix*>(matrixHandle);
        if (matrix) {
            shader->setLocalMatrix(*matrix);
        } else {
            shader->resetLocalMatrix();
        }
        shader->setGenerationID(shader->getGenerationID() + 1);
    }
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
