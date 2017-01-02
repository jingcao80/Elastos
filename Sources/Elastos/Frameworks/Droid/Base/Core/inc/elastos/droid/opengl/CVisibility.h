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

#ifndef __ELASTOS_DROID_OPENGL_CVISIBILITY_H__
#define __ELASTOS_DROID_OPENGL_CVISIBILITY_H__

#include "Elastos.Droid.Opengl.h"
#include "_Elastos_Droid_Opengl_CVisibility.h"
#include "elastos/droid/opengl/CMatrix.h"
#include <stdio.h>
#include <stdlib.h>

namespace Elastos {
namespace Droid {
namespace Opengl {

class MallocHelper {
public:
    MallocHelper() {
        mData = 0;
    }

    ~MallocHelper() {
        if (mData != 0) {
            free(mData);
        }
    }

    void* alloc(size_t size) {
        mData = malloc(size);
        return mData;
    }

private:
    void* mData;
};

CarClass(CVisibility)
     , public Singleton
     , public IVisibility
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI VisibilityTest(
        /* [in] */ ArrayOf<Float>* ws,
        /* [in] */ Int32 wsOffset,
        /* [in] */ ArrayOf<Float>* positions,
        /* [in] */ Int32 positionsOffset,
        /* [in] */ ArrayOf<Char32>* indices,
        /* [in] */ Int32 indicesOffset,
        /* [in] */ Int32 indexCount,
        /* [out] */ Int32* rst);

    CARAPI FrustumCullSpheres(
        /* [in] */ ArrayOf<Float>* mvp,
        /* [in] */ Int32 mvpOffset,
        /* [in] */ ArrayOf<Float>* spheres,
        /* [in] */ Int32 spheresOffset,
        /* [in] */ Int32 spheresCount,
        /* [in] */ ArrayOf<Int32>* results,
        /* [in] */ Int32 resultsOffset,
        /* [in] */ Int32 resultsCapacity,
        /* [out] */ Int32* rst);

    CARAPI ComputeBoundingSphere(
        /* [in] */ ArrayOf<Float>* positions,
        /* [in] */ Int32 positionsOffset,
        /* [in] */ Int32 positionsCount,
        /* [in] */ ArrayOf<Float>* sphere,
        /* [in] */ Int32 sphereOffset);

private:
    typedef ArrayHelper<Float> FloatArrayHelper;
    typedef ArrayHelper<Int32> Int32ArrayHelper;
    typedef ArrayHelper<Char32> Char32ArrayHelper;
};

} // namespace Opengl
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_OPENGL_CVISIBILITY_H__
