
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/graphics/CPathMeasure.h"
#include <skia/core/SkPath.h>
#include <skia/core/SkPathMeasure.h>

namespace Elastos {
namespace Droid {
namespace Graphics {

struct PathMeasurePair
{
    PathMeasurePair() {}
    PathMeasurePair(const SkPath& path, Boolean forceClosed)
        : fPath(path), fMeasure(fPath, forceClosed) {}

    SkPath fPath;      // copy of the user's path
    SkPathMeasure fMeasure;   // this guy points to fPath
};

CAR_OBJECT_IMPL(CPathMeasure);

CAR_INTERFACE_IMPL(CPathMeasure, Object, IPathMeasure);

CPathMeasure::~CPathMeasure()
{
    NativeDestroy(mNativeInstance);
}

ECode CPathMeasure::constructor()
{
    mPath = NULL;
    mNativeInstance = NativeCreate(0, FALSE);
    return NOERROR;
}

ECode CPathMeasure::constructor(
    /* [in] */ IPath* path,
    /* [in] */ Boolean forceClosed)
{
    // The native implementation does not copy the path, prevent it from being GC'd
    mPath = path;
    mNativeInstance = NativeCreate(
                        path != NULL ? ((CPath*)path)->mNativePath : 0,
                        forceClosed);
    return NOERROR;
}

ECode CPathMeasure::SetPath(
    /* [in] */ IPath* path,
    /* [in] */ Boolean forceClosed)
{
    mPath = path;
    NativeSetPath(
            mNativeInstance,
            path != NULL ? ((CPath*)path)->mNativePath : 0,
            forceClosed);
    return NOERROR;
}

ECode CPathMeasure::GetLength(
    /* [out] */ Float* length)
{
    VALIDATE_NOT_NULL(length);

    *length = NativeGetLength(mNativeInstance);
    return NOERROR;
}

ECode CPathMeasure::GetPosTan(
    /* [in] */ Float distance,
    /* [out] */ ArrayOf<Float>* pos,
    /* [out] */ ArrayOf<Float>* tan,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if ((pos != NULL && pos->GetLength() < 2) ||
        (tan != NULL && tan->GetLength() < 2)) {
        // throw new ArrayIndexOutOfBoundsException();
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    *result = NativeGetPosTan(mNativeInstance, distance, pos, tan);
    return NOERROR;
}

ECode CPathMeasure::GetMatrix(
    /* [in] */ Float distance,
    /* [in] */ IMatrix* matrix,
    /* [in] */ Int32 flags,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = NativeGetMatrix(mNativeInstance, distance, ((Matrix*)matrix)->mNativeMatrix, flags);
    return NOERROR;
}

ECode CPathMeasure::GetSegment(
    /* [in] */ Float startD,
    /* [in] */ Float stopD,
    /* [in] */ IPath* dst,
    /* [in] */ Boolean startWithMoveTo,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    ((CPath*)dst)->mIsSimplePath = FALSE;
    *result = NativeGetSegment(mNativeInstance, startD, stopD,
            ((CPath*)dst)->mNativePath, startWithMoveTo);
    return NOERROR;
}

ECode CPathMeasure::IsClosed(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = NativeIsClosed(mNativeInstance);
    return NOERROR;
}

ECode CPathMeasure::NextContour(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = NativeNextContour(mNativeInstance);
    return NOERROR;
}

Int64 CPathMeasure::NativeCreate(
    /* [in] */ Int64 pathHandle,
    /* [in] */ Boolean forceClosedHandle)
{
    const SkPath* path = reinterpret_cast<SkPath*>(pathHandle);
    bool forceClosed = (forceClosedHandle == TRUE);
    PathMeasurePair* pair;
    if (path) {
        pair = new PathMeasurePair(*path, forceClosed);
    }
    else {
        pair = new PathMeasurePair;
    }
    return reinterpret_cast<Int64>(pair);
}

void CPathMeasure::NativeSetPath(
    /* [in] */ Int64 nativeInstance,
    /* [in] */ Int64 nativePath,
    /* [in] */ Boolean forceClosedHandle)
{
    PathMeasurePair* pair = reinterpret_cast<PathMeasurePair*>(nativeInstance);
    const SkPath* path = reinterpret_cast<SkPath*>(nativePath);
    bool forceClosed = (forceClosedHandle == TRUE);
    if (NULL == path) {
        pair->fPath.reset();
    }
    else {
        pair->fPath = *path;
    }
    pair->fMeasure.setPath(&pair->fPath, forceClosed);
}

Float CPathMeasure::NativeGetLength(
    /* [in] */ Int64 nativeInstance)
{
    PathMeasurePair* pair = reinterpret_cast<PathMeasurePair*>(nativeInstance);
    return SkScalarToFloat(pair->fMeasure.getLength());;
}

static void ConvertTwoElemFloatArray(
    /* [out] */ ArrayOf<Float>*  array,
    /* [in] */ SkScalar* src)
{
    assert(array != NULL);

    if (array->GetLength() < 2) {
        sk_throw();
    }
    (*array)[0] = SkScalarToFloat(src[0]);
    (*array)[1] = SkScalarToFloat(src[1]);
}

Boolean CPathMeasure::NativeGetPosTan(
    /* [in] */ Int64 nativeInstance,
    /* [in] */ Float distance,
    /* [out] */ ArrayOf<Float>* pos,
    /* [out] */ ArrayOf<Float>* tan)
{
    PathMeasurePair* pair = reinterpret_cast<PathMeasurePair*>(nativeInstance);
    SkScalar tmpPos[2], tmpTan[2];
    SkScalar* posPtr = pos ? tmpPos : NULL;
    SkScalar* tanPtr = tan ? tmpTan : NULL;

    if (!pair->fMeasure.getPosTan(distance, (SkPoint*)posPtr,
            (SkVector*)tanPtr)) {
        return FALSE;
    }

    if (pos) {
        ConvertTwoElemFloatArray(pos, tmpPos);
    }
    if (tan) {
        ConvertTwoElemFloatArray(tan, tmpTan);
    }
    return TRUE;
}

Boolean CPathMeasure::NativeGetMatrix(
    /* [in] */ Int64 nativeInstance,
    /* [in] */ Float dist,
    /* [out] */ Int64 matrixHandle,
    /* [in] */ Int32 flags)
{
    PathMeasurePair* pair = reinterpret_cast<PathMeasurePair*>(nativeInstance);
    SkMatrix* matrix = reinterpret_cast<SkMatrix*>(matrixHandle);
    bool result = pair->fMeasure.getMatrix(dist, matrix, (SkPathMeasure::MatrixFlags)flags);
    return result ? TRUE : FALSE;
}

Boolean CPathMeasure::NativeGetSegment(
    /* [in] */ Int64 nativeInstance,
    /* [in] */ Float startF,
    /* [in] */ Float stopF,
    /* [out] */ Int64 dstHandle,
    /* [in] */ Boolean startWithMoveTo)
{
    PathMeasurePair* pair = reinterpret_cast<PathMeasurePair*>(nativeInstance);
    SkPath* dst = reinterpret_cast<SkPath*>(dstHandle);
    bool result = pair->fMeasure.getSegment(startF, stopF, dst, startWithMoveTo);
    return result ? TRUE : FALSE;
}

Boolean CPathMeasure::NativeIsClosed(
    /* [in] */ Int64 nativeInstance)
{
    PathMeasurePair* pair = reinterpret_cast<PathMeasurePair*>(nativeInstance);
    bool result = pair->fMeasure.isClosed();
    return result ? TRUE : FALSE;
}

Boolean CPathMeasure::NativeNextContour(
    /* [in] */ Int64 nativeInstance)
{
    PathMeasurePair* pair = reinterpret_cast<PathMeasurePair*>(nativeInstance);
    bool result = pair->fMeasure.nextContour();
    return result ? TRUE : FALSE;
}

void CPathMeasure::NativeDestroy(
    /* [in] */ Int64 nativeInstance)
{
    PathMeasurePair* pair = reinterpret_cast<PathMeasurePair*>(nativeInstance);
    delete pair;
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
