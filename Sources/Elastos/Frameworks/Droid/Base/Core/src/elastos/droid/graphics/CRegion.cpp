
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/graphics/CRegion.h"
#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/graphics/CPath.h"
#include "elastos/droid/graphics/GraphicsNative.h"
#include <binder/Parcel.h>
#include <skia/core/SkRegion.h>
#include <skia/core/SkPath.h>

namespace Elastos {
namespace Droid {
namespace Graphics {

const Int32 CRegion::MAX_POOL_SIZE = 10;
AutoPtr<Pools::SynchronizedPool<IRegion> > CRegion::sPool =
        new Pools::SynchronizedPool<IRegion>(MAX_POOL_SIZE);

CAR_OBJECT_IMPL(CRegion);
CAR_INTERFACE_IMPL_2(CRegion, Object, IRegion, IParcelable);
ECode CRegion::constructor()
{
    mNativeRegion = NativeConstructor();
    return NOERROR;
}

ECode CRegion::constructor(
    /* [in] */ IRegion* region)
{
    mNativeRegion = NativeConstructor();
    NativeSetRegion(mNativeRegion, ((CRegion*)region)->mNativeRegion);
    return NOERROR;
}

ECode CRegion::constructor(
    /* [in] */ IRect* r)
{
    mNativeRegion = NativeConstructor();
    CRect* rect = (CRect*)r;
    NativeSetRect(mNativeRegion, rect->mLeft, rect->mTop, rect->mRight, rect->mBottom);
    return NOERROR;
}

ECode CRegion::constructor(
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    mNativeRegion = NativeConstructor();
    NativeSetRect(mNativeRegion, left, top, right, bottom);
    return NOERROR;
}

ECode CRegion::constructor(
    /* [in] */ Handle64 ni)
{
    if (ni == 0) {
        //throw new RuntimeException();
        return E_RUNTIME_EXCEPTION;
    }

    mNativeRegion = ni;
    return NOERROR;
}

ECode CRegion::constructor(
    /* [in] */ Handle64 ni,
    /* [in] */ Int32 dummy)
{
    return constructor(ni);
}

CRegion::~CRegion()
{
    NativeDestructor(mNativeRegion);
}

ECode CRegion::SetEmpty()
{
    NativeSetRect(mNativeRegion, 0, 0, 0, 0);
    return NOERROR;
}

ECode CRegion::Set(
    /* [in] */ IRegion* src,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    NativeSetRegion(mNativeRegion, ((CRegion*)src)->mNativeRegion);
    *result = TRUE;
    return NOERROR;
}

ECode CRegion::Set(
    /* [in] */ IRect* r,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    CRect* rect = (CRect*)r;
    *result = NativeSetRect(mNativeRegion, rect->mLeft, rect->mTop, rect->mRight, rect->mBottom);
    return NOERROR;
}

ECode CRegion::Set(
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = NativeSetRect(mNativeRegion, left, top, right, bottom);
    return NOERROR;
}

ECode CRegion::SetPath(
    /* [in] */ IPath* path,
    /* [in] */ IRegion* clip,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = NativeSetPath(mNativeRegion, ((CPath*)path)->Ni(), ((CRegion*)clip)->mNativeRegion);
    return NOERROR;
}

ECode CRegion::IsEmpty(
    /* [out] */ Boolean* isEmpty)
{
    VALIDATE_NOT_NULL(isEmpty);

    *isEmpty = ((SkRegion*)mNativeRegion)->isEmpty();
    return NOERROR;
}

ECode CRegion::IsRect(
    /* [out] */ Boolean* isRect)
{
    VALIDATE_NOT_NULL(isRect);

    *isRect = ((SkRegion*)mNativeRegion)->isRect();
    return NOERROR;
}

ECode CRegion::IsComplex(
    /* [out] */ Boolean* isComplex)
{
    VALIDATE_NOT_NULL(isComplex);

    *isComplex = ((SkRegion*)mNativeRegion)->isComplex();
    return NOERROR;
}

ECode CRegion::GetBounds(
    /* [out] */ IRect** bounds)
{
    CRect::New(bounds);
    NativeGetBounds(mNativeRegion, *bounds);
    return NOERROR;
}

ECode CRegion::GetBounds(
    /* [in, out] */ IRect* r,
    /* [out] */ Boolean* result)
{
    if (r == NULL) {
        //throw new NullPointerException();
        return E_NULL_POINTER_EXCEPTION;
    }
    *result = NativeGetBounds(mNativeRegion, r);
    return NOERROR;
}

/**
 * Return the boundary of the region as a new Path. If the region is empty,
 * the path will also be empty.
 */
ECode CRegion::GetBoundaryPath(
    /* [out] */ IPath** path)
{
    VALIDATE_NOT_NULL(path);

    CPath::New(path);
    NativeGetBoundaryPath(mNativeRegion, ((CPath*)*path)->Ni());
    return NOERROR;
}

/**
 * Set the path to the boundary of the region. If the region is empty, the
 * path will also be empty.
 */
ECode CRegion::GetBoundaryPath(
    /* [in, out] */ IPath* path,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = NativeGetBoundaryPath(mNativeRegion, ((CPath*)path)->Ni());
    return NOERROR;
}

ECode CRegion::Contains(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = ((SkRegion*)mNativeRegion)->contains(x, y);
    return NOERROR;
}

ECode CRegion::QuickContains(
    /* [in] */ IRect* r,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    CRect* rect = (CRect*)r;
    return QuickContains(rect->mLeft, rect->mTop, rect->mRight, rect->mBottom, result);
}

ECode CRegion::QuickContains(
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = ((SkRegion*)mNativeRegion)->quickContains(left, top, right, bottom);
    return NOERROR;
}

ECode CRegion::QuickReject(
    /* [in] */ IRect* r,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    CRect* rect = (CRect*)r;
    return QuickReject(rect->mLeft, rect->mTop, rect->mRight, rect->mBottom, result);
}

ECode CRegion::QuickReject(
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    SkIRect ir;
    ir.set(left, top, right, bottom);
    *result = ((SkRegion*)mNativeRegion)->quickReject(ir);
    return NOERROR;
}

ECode CRegion::QuickReject(
    /* [in] */ IRegion* rgn,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = ((SkRegion*)mNativeRegion)->quickReject(*(SkRegion*)((CRegion*)rgn)->mNativeRegion);
    return NOERROR;
}

ECode CRegion::Translate(
    /* [in] */ Int32 dx,
    /* [in] */ Int32 dy)
{
    return Translate(dx, dy, NULL);
}

ECode CRegion::Translate(
    /* [in] */ Int32 dx,
    /* [in] */ Int32 dy,
    /* [in] */ IRegion* dst)
{
    if (dst) {
        ((SkRegion*)mNativeRegion)->translate(dx, dy, (SkRegion*)((CRegion*)dst)->mNativeRegion);
    }
    else {
        ((SkRegion*)mNativeRegion)->translate(dx, dy);
    }

    return NOERROR;
}

ECode CRegion::Scale(
    /* [in] */ Float scale)
{
    return Scale(scale, NULL);
}

// Scale the rectangle by given scale and set the reuslt to the dst.
static void ScaleRect(
    /* [in] */ SkIRect* dst,
    /* [in] */ const SkIRect& src,
    /* [in] */ Float scale)
{
   dst->fLeft = (Int32)::roundf(src.fLeft * scale);
   dst->fTop = (Int32)::roundf(src.fTop * scale);
   dst->fRight = (Int32)::roundf(src.fRight * scale);
   dst->fBottom = (Int32)::roundf(src.fBottom * scale);
}

// Scale the region by given scale and set the reuslt to the dst.
// dest and src can be the same region instance.
static void ScaleRgn(
    /* [in] */ SkRegion* dst,
    /* [in] */ const SkRegion& src,
    /* [in] */ Float scale)
{
   SkRegion tmp;
   SkRegion::Iterator iter(src);

   for (; !iter.done(); iter.next()) {
       SkIRect r;
       ScaleRect(&r, iter.rect(), scale);
       tmp.op(r, SkRegion::kUnion_Op);
   }
   dst->swap(tmp);
}

ECode CRegion::Scale(
    /* [in] */ Float scale,
    /* [in] */ IRegion* dst)
{
    if (dst) {
        ScaleRgn((SkRegion*)((CRegion*)dst)->mNativeRegion, *(SkRegion*)mNativeRegion, scale);
    }
    else {
        ScaleRgn((SkRegion*)mNativeRegion, *(SkRegion*)mNativeRegion, scale);
    }

    return NOERROR;
}

ECode CRegion::Union(
    /* [in] */ IRect* r,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    return Op(r, RegionOp_UNION, result);
}

ECode CRegion::Op(
    /* [in] */ IRect* r,
    /* [in] */ RegionOp op,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    CRect* rect = (CRect*)r;
    *result = NativeOp(mNativeRegion, rect->mLeft, rect->mTop, rect->mRight, rect->mBottom, op);
    return NOERROR;
}

ECode CRegion::Op(
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom,
    /* [in] */ RegionOp op,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = NativeOp(mNativeRegion, left, top, right, bottom, op);
    return NOERROR;
}

ECode CRegion::Op(
    /* [in] */ IRegion* region,
    /* [in] */ RegionOp op,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    return Op((IRegion*)this, region, op, result);
}

ECode CRegion::Op(
    /* [in] */ IRect* rect,
    /* [in] */ IRegion* region,
    /* [in] */ RegionOp op,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = NativeOp(mNativeRegion, rect, ((CRegion*)region)->mNativeRegion, op);
    return NOERROR;
}

ECode CRegion::Op(
    /* [in] */ IRegion* region1,
    /* [in] */ IRegion* region2,
    /* [in] */ RegionOp op,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = NativeOp(
                mNativeRegion,
                ((CRegion*)region1)->mNativeRegion,
                ((CRegion*)region2)->mNativeRegion,
                op);
    return NOERROR;
}

ECode CRegion::Equals(
    /* [in] */ IRegion* r,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    if (r != NULL) {
        *result = NativeEquals(mNativeRegion, ((CRegion*)r)->mNativeRegion);
    }
    return NOERROR;
}

ECode CRegion::Equals(
    /* [in] */ IInterface* region,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return Equals(IRegion::Probe(region), result);
}

ECode CRegion::GetHashCode(
    /* [out] */ Int32* hash)
{
    VALIDATE_NOT_NULL(hash);
    *hash = (Int32)mNativeRegion;
    return NOERROR;
}

ECode CRegion::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    *str = NativeToString(mNativeRegion);
    return NOERROR;
}

AutoPtr<IRegion> CRegion::Obtain()
{
    AutoPtr<IRegion> region = sPool->AcquireItem();
    if (region != NULL) return region;
    CRegion::New((IRegion**)&region);
    return region;
}

AutoPtr<IRegion> CRegion::Obtain(
    /* [in] */ IRegion* other)
{
    AutoPtr<IRegion> region = Obtain();
    Boolean result = FALSE;
    region->Set(other, &result);
    return region;
}

ECode CRegion::Recycle()
{
    SetEmpty();
    sPool->ReleaseItem(this);
    return NOERROR;
}

ECode CRegion::GetNativeRegion(
    /* [out] */ Handle64* region)
{
    VALIDATE_NOT_NULL(region);

    *region = mNativeRegion;
     return NOERROR;
}

ECode CRegion::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    NativeDestructor(mNativeRegion);
    mNativeRegion = NativeCreateFromParcel(source);
    return NOERROR;
}

ECode CRegion::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    if (!NativeWriteToParcel(mNativeRegion, dest)) {
        //throw new RuntimeException();
        return E_RUNTIME_EXCEPTION;
    }
    return NOERROR;
}

Handle64 CRegion::Ni()
{
    return mNativeRegion;
}

Boolean CRegion::NativeEquals(
    /* [in] */ Handle64 region1,
    /* [in] */ Handle64 region2)
{
    const SkRegion *r1 = reinterpret_cast<SkRegion*>(region1);
    const SkRegion *r2 = reinterpret_cast<SkRegion*>(region2);
    return (*r1 == *r2);
}

Handle64 CRegion::NativeConstructor()
{
    return reinterpret_cast<Handle64>(new SkRegion);
}

void CRegion::NativeDestructor(
    /* [in] */ Handle64 regionHandle)
{
    SkRegion* region = reinterpret_cast<SkRegion*>(regionHandle);
    SkASSERT(region);
    delete region;
}

void CRegion::NativeSetRegion(
    /* [in] */ Handle64 dstHandle,
    /* [in] */ Handle64 srcHandle)
{
    SkRegion* dst = reinterpret_cast<SkRegion*>(dstHandle);
    const SkRegion* src = reinterpret_cast<SkRegion*>(srcHandle);
    SkASSERT(dst && src);
    *dst = *src;
}

Boolean CRegion::NativeSetRect(
    /* [in] */ Handle64 dstHandle,
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    SkRegion* dst = reinterpret_cast<SkRegion*>(dstHandle);
    bool result = dst->setRect(left, top, right, bottom);
    return result;
}

Boolean CRegion::NativeSetPath(
    /* [in] */ Handle64 dstHandle,
    /* [in] */ Handle64 pathHandle,
    /* [in] */ Handle64 clipHandle)
{
    SkRegion*       dst  = reinterpret_cast<SkRegion*>(dstHandle);
    const SkPath*   path = reinterpret_cast<SkPath*>(pathHandle);
    const SkRegion* clip = reinterpret_cast<SkRegion*>(clipHandle);
    SkASSERT(dst && path && clip);
    bool result = dst->setPath(*path, *clip);
    return result;
}

Boolean CRegion::NativeGetBounds(
    /* [in] */ Handle64 regionHandle,
    /* [out] */ IRect* rectBounds)
{
    SkRegion* region = reinterpret_cast<SkRegion*>(regionHandle);
    GraphicsNative::SkIRect2IRect(region->getBounds(), rectBounds);
    bool result = !region->isEmpty();
    return result;
}

Boolean CRegion::NativeGetBoundaryPath(
    /* [in] */ Handle64 regionHandle,
    /* [out] */ Handle64 pathHandle)
{
    const SkRegion* region = reinterpret_cast<SkRegion*>(regionHandle);
    SkPath*   path = reinterpret_cast<SkPath*>(pathHandle);
    bool result = region->getBoundaryPath(path);
    return result;
}

Boolean CRegion::NativeOp(
    /* [in] */ Handle64 dstHandle,
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom,
    /* [in] */ RegionOp op)
{
    SkRegion* dst = reinterpret_cast<SkRegion*>(dstHandle);
    SkIRect ir;

    ir.set(left, top, right, bottom);
    bool result = dst->op(ir, (SkRegion::Op)op);
    return result;
}

Boolean CRegion::NativeOp(
    /* [in] */ Handle64 dstHandle,
    /* [in] */ IRect* rect,
    /* [in] */ Handle64 regionHandle,
    /* [in] */ RegionOp op)
{
    SkRegion* dst = reinterpret_cast<SkRegion*>(dstHandle);
    const SkRegion* region = reinterpret_cast<SkRegion*>(regionHandle);
    SkIRect    ir;
    GraphicsNative::IRect2SkIRect(rect, &ir);
    bool result = dst->op(ir, *region, (SkRegion::Op)op);
    return result;
}

Boolean CRegion::NativeOp(
    /* [in] */ Handle64 dstHandle,
    /* [in] */ Handle64 region1Handle,
    /* [in] */ Handle64 region2Handle,
    /* [in] */ RegionOp op)
{
    SkRegion* dst = reinterpret_cast<SkRegion*>(dstHandle);
    const SkRegion* region1 = reinterpret_cast<SkRegion*>(region1Handle);
    const SkRegion* region2 = reinterpret_cast<SkRegion*>(region2Handle);
    bool result = dst->op(*region1, *region2, (SkRegion::Op)op);
    return result;
}

Handle64 CRegion::NativeCreateFromParcel(
    /* [in] */ IParcel* parcel)
{
    if (parcel == NULL) {
        return 0;
    }

    android::Parcel* p;
    parcel->GetElementPayload((Handle32*)&p);

    SkRegion* region = new SkRegion;
    size_t size = p->readInt32();
    region->readFromMemory(p->readInplace(size), size);

    return reinterpret_cast<Handle64>(region);
}

Boolean CRegion::NativeWriteToParcel(
    /* [in] */ Handle64 regionHandle,
    /* [out] */ IParcel* parcel)
{
    const SkRegion* region = reinterpret_cast<SkRegion*>(regionHandle);
    if (parcel == NULL) {
        return FALSE;
    }

    android::Parcel* p;
    parcel->GetElementPayload((Handle32*)&p);

    size_t size = region->writeToMemory(NULL);
    p->writeInt32(size);
    region->writeToMemory(p->writeInplace(size));

    return TRUE;
}

String CRegion::NativeToString(
    /* [in] */ Handle64 regionHandle)
{
    SkRegion* region = reinterpret_cast<SkRegion*>(regionHandle);
    char* str = region->toString();
    if (str == NULL) {
        return String(NULL);
    }
    String result(str);
    free(str);
    return result;
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
