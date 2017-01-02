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

#include "elastos/droid/renderscript/CMatrix4f.h"

namespace Elastos {
namespace Droid {
namespace RenderScript {

CAR_INTERFACE_IMPL(CMatrix4f, Object, IMatrix4f)

CAR_OBJECT_IMPL(CMatrix4f)

CMatrix4f::CMatrix4f()
{}

ECode CMatrix4f::constructor()
{
    mMat = ArrayOf<Float>::Alloc(16);
    return LoadIdentity();
}

ECode CMatrix4f::constructor(
    /* [in] */ ArrayOf<Float>* dataArray)
{
    mMat = ArrayOf<Float>::Alloc(16);
    mMat->Copy(dataArray, 0, mMat->GetLength());
    return NOERROR;
}

ECode CMatrix4f::LoadOrtho(
    /* [in] */ Float l,
    /* [in] */ Float r,
    /* [in] */ Float b,
    /* [in] */ Float t,
    /* [in] */ Float n,
    /* [in] */ Float f)
{
    LoadIdentity();
    (*mMat)[0] = 2 / (r - l);
    (*mMat)[5] = 2 / (t - b);
    (*mMat)[10]= -2 / (f - n);
    (*mMat)[12]= -(r + l) / (r - l);
    (*mMat)[13]= -(t + b) / (t - b);
    (*mMat)[14]= -(f + n) / (f - n);
    return NOERROR;
}

ECode CMatrix4f::GetArray(
    /* [out, callee] */ ArrayOf<Float>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mMat;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CMatrix4f::LoadIdentity()
{
    (*mMat)[0] = 1;
    (*mMat)[1] = 0;
    (*mMat)[2] = 0;
    (*mMat)[3] = 0;

    (*mMat)[4] = 0;
    (*mMat)[5] = 1;
    (*mMat)[6] = 0;
    (*mMat)[7] = 0;

    (*mMat)[8] = 0;
    (*mMat)[9] = 0;
    (*mMat)[10] = 1;
    (*mMat)[11] = 0;

    (*mMat)[12] = 0;
    (*mMat)[13] = 0;
    (*mMat)[14] = 0;
    (*mMat)[15] = 1;
    return NOERROR;
}


} // namespace RenderScript
} // namespace Droid
} // namespace Elastos