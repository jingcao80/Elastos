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

#include "elastos/droid/opengl/CGLU.h"
#include "elastos/droid/opengl/CMatrix.h"

#include <elastos/core/AutoLock.h>
#include <elastos/core/Math.h>

using Elastos::Core::AutoLock;

namespace Elastos {
namespace Droid {
namespace Opengl {

AutoPtr<ArrayOf<Float> > CGLU::sScratch = ArrayOf<Float>::Alloc(32);
Object CGLU::sLock;

CAR_INTERFACE_IMPL(CGLU, Singleton, IGLU)

CAR_SINGLETON_IMPL(CGLU)

ECode CGLU::GluErrorString(
    /* [in] */ Int32 error,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)

    switch (error) {
    case IGL10::_GL_NO_ERROR:
        *str = String("no error");
    case IGL10::_GL_INVALID_ENUM:
        *str = String("invalid enum");
    case IGL10::_GL_INVALID_VALUE:
        *str = String("invalid value");
    case IGL10::_GL_INVALID_OPERATION:
        *str = String("invalid operation");
    case IGL10::_GL_STACK_OVERFLOW:
        *str = String("stack overflow");
    case IGL10::_GL_STACK_UNDERFLOW:
        *str = String("stack underflow");
    case IGL10::_GL_OUT_OF_MEMORY:
        *str = String("out of memory");
    default:
        *str = String(NULL);
    }
    return NOERROR;
}

ECode CGLU::GluLookAt(
    /* [in] */ IGL10* gl,
    /* [in] */ Float eyeX,
    /* [in] */ Float eyeY,
    /* [in] */ Float eyeZ,
    /* [in] */ Float centerX,
    /* [in] */ Float centerY,
    /* [in] */ Float centerZ,
    /* [in] */ Float upX,
    /* [in] */ Float upY,
    /* [in] */ Float upZ)
{
    {
        AutoLock lock(sLock);
        AutoPtr<IMatrix> matrix;
        CMatrix::AcquireSingleton((IMatrix**)&matrix);
        matrix->SetLookAtM(sScratch, 0, eyeX, eyeY, eyeZ, centerX, centerY, centerZ,
                upX, upY, upZ);
        gl->GlMultMatrixf(sScratch, 0);
    }
    return NOERROR;
}

ECode CGLU::GluOrtho2D(
    /* [in] */ IGL10* gl,
    /* [in] */ Float left,
    /* [in] */ Float right,
    /* [in] */ Float bottom,
    /* [in] */ Float top)
{
    gl->GlOrthof(left, right, bottom, top, -1.0f, 1.0f);
    return NOERROR;
}

ECode CGLU::GluPerspective(
    /* [in] */ IGL10* gl,
    /* [in] */ Float fovy,
    /* [in] */ Float aspect,
    /* [in] */ Float zNear,
    /* [in] */ Float zFar)
{
    Float top = zNear * (Float) Elastos::Core::Math::Tan(fovy * (Elastos::Core::Math::PI / 360.0));
    Float bottom = -top;
    Float left = bottom * aspect;
    Float right = top * aspect;
    gl->GlFrustumf(left, right, bottom, top, zNear, zFar);
    return NOERROR;
}

ECode CGLU::GluProject(
    /* [in] */ Float objX,
    /* [in] */ Float objY,
    /* [in] */ Float objZ,
    /* [in] */ ArrayOf<Float>* model,
    /* [in] */ Int32 modelOffset,
    /* [in] */ ArrayOf<Float>* project,
    /* [in] */ Int32 projectOffset,
    /* [in] */ ArrayOf<Int32>* view,
    /* [in] */ Int32 viewOffset,
    /* [in] */ ArrayOf<Float>* win,
    /* [in] */ Int32 winOffset,
    /* [out] */ Int32* rst)
{
    VALIDATE_NOT_NULL(rst)

    {
        AutoLock lock(sLock);
        Int32 M_OFFSET = 0; // 0..15
        Int32 V_OFFSET = 16; // 16..19
        Int32 V2_OFFSET = 20; // 20..23
        AutoPtr<IMatrix> matrix;
        CMatrix::AcquireSingleton((IMatrix**)&matrix);
        matrix->MultiplyMM(sScratch, M_OFFSET, project, projectOffset,
                model, modelOffset);

        (*sScratch)[V_OFFSET + 0] = objX;
        (*sScratch)[V_OFFSET + 1] = objY;
        (*sScratch)[V_OFFSET + 2] = objZ;
        (*sScratch)[V_OFFSET + 3] = 1.0f;

        matrix->MultiplyMV(sScratch, V2_OFFSET,
                sScratch, M_OFFSET, sScratch, V_OFFSET);

        Float w = (*sScratch)[V2_OFFSET + 3];
        if (w == 0.0f) {
            *rst = IGL10::_GL_FALSE;
            return NOERROR;
        }

        Float rw = 1.0f / w;

        (*win)[winOffset] =
                (*view)[viewOffset] + (*view)[viewOffset + 2]
                        * ((*sScratch)[V2_OFFSET + 0] * rw + 1.0f)
                        * 0.5f;
        (*win)[winOffset + 1] =
                (*view)[viewOffset + 1] + (*view)[viewOffset + 3]
                        * ((*sScratch)[V2_OFFSET + 1] * rw + 1.0f) * 0.5f;
        (*win)[winOffset + 2] = ((*sScratch)[V2_OFFSET + 2] * rw + 1.0f) * 0.5f;
    }

    *rst = IGL10::_GL_TRUE;
    return NOERROR;
}


ECode CGLU::GluUnProject(
    /* [in] */ Float winX,
    /* [in] */ Float winY,
    /* [in] */ Float winZ,
    /* [in] */ ArrayOf<Float>* model,
    /* [in] */ Int32 modelOffset,
    /* [in] */ ArrayOf<Float>* project,
    /* [in] */ Int32 projectOffset,
    /* [in] */ ArrayOf<Int32>* view,
    /* [in] */ Int32 viewOffset,
    /* [in] */ ArrayOf<Float>* obj,
    /* [in] */ Int32 objOffset,
    /* [out] */ Int32* rst)
{
    VALIDATE_NOT_NULL(rst)

    {
        Int32 PM_OFFSET = 0; // 0..15
        Int32 INVPM_OFFSET = 16; // 16..31
        Int32 V_OFFSET = 0; // 0..3 Reuses PM_OFFSET space
        AutoPtr<IMatrix> matrix;
        CMatrix::AcquireSingleton((IMatrix**)&matrix);
        matrix->MultiplyMM(sScratch, PM_OFFSET, project, projectOffset,
                model, modelOffset);

        Boolean invert;
        matrix->InvertM(sScratch, INVPM_OFFSET, sScratch, PM_OFFSET, &invert);
        if (!invert) {
            *rst = IGL10::_GL_FALSE;
            return NOERROR;
        }

        (*sScratch)[V_OFFSET + 0] =
                2.0f * (winX - (*view)[viewOffset + 0]) / (*view)[viewOffset + 2]
                        - 1.0f;
        (*sScratch)[V_OFFSET + 1] =
                2.0f * (winY - (*view)[viewOffset + 1]) / (*view)[viewOffset + 3]
                        - 1.0f;
        (*sScratch)[V_OFFSET + 2] = 2.0f * winZ - 1.0f;
        (*sScratch)[V_OFFSET + 3] = 1.0f;

        matrix->MultiplyMV(obj, objOffset, sScratch, INVPM_OFFSET,
                sScratch, V_OFFSET);
    }

    *rst = IGL10::_GL_TRUE;
    return NOERROR;
}


} // namespace Opengl
} // namespace Droid
} // namespace Elastos
