
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
    const SkMatrix* matrix = reinterpret_cast<SkMatrix*>(matrixHandle);
    if (shader) {
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
