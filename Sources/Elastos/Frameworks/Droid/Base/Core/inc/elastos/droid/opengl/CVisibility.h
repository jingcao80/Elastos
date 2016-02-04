
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
