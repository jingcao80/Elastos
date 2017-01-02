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

#ifndef __ELASTOS_DROID_RENDERSCRIPT_CMATRIX4F_H__
#define __ELASTOS_DROID_RENDERSCRIPT_CMATRIX4F_H__

#include "_Elastos_Droid_RenderScript_CMatrix4f.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace RenderScript {

class CMatrix4f
    : public Object
    , public IMatrix4f
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CMatrix4f();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ ArrayOf<Float>* dataArray);
    /**
    * Set current values to be an orthographic projection matrix
    *
    * @param l location of the left vertical clipping plane
    * @param r location of the right vertical clipping plane
    * @param b location of the bottom horizontal clipping plane
    * @param t location of the top horizontal clipping plane
    * @param n location of the near clipping plane
    * @param f location of the far clipping plane
    */
    CARAPI LoadOrtho(
        /* [in] */ Float l,
        /* [in] */ Float r,
        /* [in] */ Float b,
        /* [in] */ Float t,
        /* [in] */ Float n,
        /* [in] */ Float f);

    /**
    * Return a reference to the internal array representing matrix
    * values. Modifying this array will also change the matrix
    *
    * @return internal array representing the matrix
    */
    CARAPI GetArray(
        /* [out, callee] */ ArrayOf<Float>** result);

    /**
    * Sets the matrix values to identity
    */
    CARAPI LoadIdentity();

private:
    AutoPtr<ArrayOf<Float> > mMat;
};

} // namespace RenderScript
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_RENDERSCRIPT_CMATRIX4F_H__
