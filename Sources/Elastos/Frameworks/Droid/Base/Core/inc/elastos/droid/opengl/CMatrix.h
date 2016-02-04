
#ifndef __ELASTOS_DROID_OPENGL_CMATRIX_H__
#define __ELASTOS_DROID_OPENGL_CMATRIX_H__

#include "Elastos.Droid.Opengl.h"
#include "_Elastos_Droid_Opengl_CMatrix.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Opengl {

template<class T>
class ArrayHelper {
public:
    ArrayHelper(
        /* [in] */ ArrayOf<T>* ref,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 minSize)
    {
        mRef = ref;
        mOffset = offset;
        mMinSize = minSize;
        mBase = 0;
    }

    // We seperate the bounds check from the initialization because we want to
    // be able to bounds-check multiple arrays, and we can't throw an exception
    // after we've called GetPrimitiveArrayCritical.

    // Return true if the bounds check succeeded
    // Else instruct the runtime to throw an exception

    CARAPI Check(
        /* [in] */ Boolean* rst)
    {
        if ( ! mRef) {
            *rst = FALSE;
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        if ( mOffset < 0) {
            *rst = FALSE;
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        mLength = mRef->GetLength() - mOffset;
        if (mLength < mMinSize ) {
            *rst = FALSE;
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        return TRUE;
    }

    // Bind the array.

    CARAPI_(void) Bind()
    {
        mBase = (T*) mRef->GetPayload();
        mData = mBase + mOffset;
    }

    T* mData;
    Int32 mLength;

private:
    T* mBase;
    AutoPtr<ArrayOf<T> > mRef;
    Int32 mOffset;
    Int32 mMinSize;
};

CarClass(CMatrix)
    , public Singleton
    , public IMatrix
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI MultiplyMM(
        /* [in] */ ArrayOf<Float>* result,
        /* [in] */ Int32 resultOffset,
        /* [in] */ ArrayOf<Float>* lhs,
        /* [in] */ Int32 lhsOffset,
        /* [in] */ ArrayOf<Float>* rhs,
        /* [in] */ Int32 rhsOffset);

    CARAPI MultiplyMV(
        /* [in] */ ArrayOf<Float>* resultVec,
        /* [in] */ Int32 resultVecOffset,
        /* [in] */ ArrayOf<Float>* lhsMat,
        /* [in] */ Int32 lhsMatOffset,
        /* [in] */ ArrayOf<Float>* rhsVec,
        /* [in] */ Int32 rhsVecOffset);

    CARAPI TransposeM(
        /* [in] */ ArrayOf<Float>* mTrans,
        /* [in] */ Int32 mTransOffset,
        /* [in] */ ArrayOf<Float>* m,
        /* [in] */ Int32 mOffset);

    CARAPI InvertM(
        /* [in] */ ArrayOf<Float>* mInv,
        /* [in] */ Int32 mInvOffset,
        /* [in] */ ArrayOf<Float>* m,
        /* [in] */ Int32 mOffset,
        /* [out] */ Boolean* rst);

    CARAPI OrthoM(
        /* [in] */ ArrayOf<Float>* m,
        /* [in] */ Int32 mOffset,
        /* [in] */ Float left,
        /* [in] */ Float right,
        /* [in] */ Float bottom,
        /* [in] */ Float top,
        /* [in] */ Float near,
        /* [in] */ Float far);

    CARAPI FrustumM(
        /* [in] */ ArrayOf<Float>* m,
        /* [in] */ Int32 offset,
        /* [in] */ Float left,
        /* [in] */ Float right,
        /* [in] */ Float bottom,
        /* [in] */ Float top,
        /* [in] */ Float near,
        /* [in] */ Float far);

    CARAPI PerspectiveM(
        /* [in] */ ArrayOf<Float>* m,
        /* [in] */ Int32 offset,
        /* [in] */ Float fovy,
        /* [in] */ Float aspect,
        /* [in] */ Float zNear,
        /* [in] */ Float zFar);

    CARAPI Length(
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ Float z,
        /* [out] */ Float* len);

    CARAPI SetIdentityM(
        /* [in] */ ArrayOf<Float>* sm,
        /* [in] */ Int32 smOffset);

    CARAPI ScaleM(
        /* [in] */ ArrayOf<Float>* sm,
        /* [in] */ Int32 smOffset,
        /* [in] */ ArrayOf<Float>* m,
        /* [in] */ Int32 mOffset,
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ Float z);

    CARAPI ScaleM(
        /* [in] */ ArrayOf<Float>* m,
        /* [in] */ Int32 mOffset,
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ Float z);

    CARAPI TranslateM(
        /* [in] */ ArrayOf<Float>* tm,
        /* [in] */ Int32 tmOffset,
        /* [in] */ ArrayOf<Float>* m,
        /* [in] */ Int32 mOffset,
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ Float z);

    CARAPI TranslateM(
        /* [in] */ ArrayOf<Float>* m,
        /* [in] */ Int32 mOffset,
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ Float z);

    CARAPI RotateM(
        /* [in] */ ArrayOf<Float>* rm,
        /* [in] */ Int32 rmOffset,
        /* [in] */ ArrayOf<Float>* m,
        /* [in] */ Int32 mOffset,
        /* [in] */ Float a,
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ Float z);

    CARAPI RotateM(
        /* [in] */ ArrayOf<Float>* m,
        /* [in] */ Int32 mOffset,
        /* [in] */ Float a,
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ Float z);

    CARAPI SetRotateM(
        /* [in] */ ArrayOf<Float>* rm,
        /* [in] */ Int32 rmOffset,
        /* [in] */ Float a,
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ Float z);

    CARAPI SetRotateEulerM(
        /* [in] */ ArrayOf<Float>* rm,
        /* [in] */ Int32 rmOffset,
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ Float z);

    CARAPI SetLookAtM(
        /* [in] */ ArrayOf<Float>* rm,
        /* [in] */ Int32 rmOffset,
        /* [in] */ Float  eyeX,
        /* [in] */ Float eyeY,
        /* [in] */ Float eyeZ,
        /* [in] */ Float centerX,
        /* [in] */ Float centerY,
        /* [in] */ Float centerZ,
        /* [in] */ Float upX,
        /* [in] */ Float upY,
        /* [in] */ Float upZ);

private:
    CARAPI_(Float) Length(
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ Float z);

    typedef ArrayHelper<Float> FloatArrayHelper;
    static AutoPtr<ArrayOf<Float> > sTemp;
    static Object sLock;
};

} // namespace Opengl
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_OPENGL_CMATRIX_H__
