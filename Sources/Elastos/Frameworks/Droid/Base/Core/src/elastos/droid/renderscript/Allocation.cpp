
#include "elastos/droid/RenderScript/Allocation.h"
#include "elastos/droid/RenderScript/CTypeBuilder.h"
#include "elastos/droid/graphics/CBitmapFactory.h"
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Graphics::BitmapConfig_ALPHA_8;
using Elastos::Droid::Graphics::BitmapConfig_RGB_565;
using Elastos::Droid::Graphics::BitmapConfig_ARGB_4444;
using Elastos::Droid::Graphics::BitmapConfig_ARGB_8888;
using Elastos::Droid::Graphics::IBitmapHelper;
using Elastos::Droid::Graphics::CBitmapHelper;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::CCanvas;
using Elastos::Droid::Graphics::CBitmapFactoryOptions;
using Elastos::Droid::Graphics::IBitmapFactory;
using Elastos::Droid::Graphics::CBitmapFactory;
using Elastos::Droid::RenderScript::EIID_IAllocation;
using Elastos::Core::IArrayOf;
using Elastos::Core::CArrayOf;
using Elastos::Core::IInteger32;
using Elastos::Core::CInteger32;
using Elastos::Core::EIID_IInteger32;
using Elastos::Core::IInteger16;
using Elastos::Core::CInteger16;
using Elastos::Core::EIID_IInteger16;
using Elastos::Core::IByte;
using Elastos::Core::CByte;
using Elastos::Core::EIID_IByte;
using Elastos::Core::IFloat;
using Elastos::Core::CFloat;
using Elastos::Core::EIID_IFloat;
using Elastos::Core::EIID_IInteger64;
using Elastos::Core::EIID_IDouble;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace RenderScript {

HashMap<Int64, AutoPtr<Allocation> > Allocation::sAllocationMap;
Object Allocation::sAllocationMapLock;

AutoPtr<IBitmapFactoryOptions> InitBitmapOptions()
{
    AutoPtr<IBitmapFactoryOptions> options;
    CBitmapFactoryOptions::New((IBitmapFactoryOptions**)&options);
    options->SetInScaled(FALSE);
    return options
}
AutoPtr<IBitmapFactoryOptions> Allocation::sBitmapOptions = InitBitmapOptions();

Allocation::Allocation()
    : mUsage(0)
    , mSize(0)
    , mConstrainedLOD(FALSE)
    , mConstrainedFace(FALSE)
    , mConstrainedY(FALSE)
    , mConstrainedZ(FALSE)
    , mReadAllowed(TRUE)
    , mWriteAllowed(TRUE)
    , mSelectedY(0)
    , mSelectedZ(0)
    , mSelectedLOD(0)
    , mSelectedFace(TypeCubemapFace_POSITIVE_X)
    , mCurrentDimX(0)
    , mCurrentDimY(0)
    , mCurrentDimZ(0)
    , mCurrentCount(0)
{}

CAR_INTERFACE_IMPL(Allocation, BaseObj, IAllocation)

ECode Allocation::constructor(
    /* [in] */ Int64 id,
    /* [in] */ RenderScript* rs,
    /* [in] */ Type* t,
    /* [in] */ Int32 usage)
{
    BaseObj::constructor(id, rs);
    if ((usage & ~(USAGE_SCRIPT |
                   USAGE_GRAPHICS_TEXTURE |
                   USAGE_GRAPHICS_VERTEX |
                   USAGE_GRAPHICS_CONSTANTS |
                   USAGE_GRAPHICS_RENDER_TARGET |
                   USAGE_IO_INPUT |
                   USAGE_IO_OUTPUT |
                   USAGE_SHARED)) != 0) {
        Slogger::E("Allocation", "Unknown usage specified.");
        return E_RS_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if ((usage & USAGE_IO_INPUT) != 0) {
        mWriteAllowed = FALSE;

        if ((usage & ~(USAGE_IO_INPUT |
                       USAGE_GRAPHICS_TEXTURE |
                       USAGE_SCRIPT)) != 0) {
            Slogger::E("Allocation", "Invalid usage combination.");
        return E_RS_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }

    mType = t;
    mUsage = usage;

    if (t != NULL) {
        // TODO: A3D doesn't have Type info during creation, so we can't
        // calculate the size ahead of time. We can possibly add a method
        // to update the size in the future if it seems reasonable.
        Int32 count;
        mType->GetCount(&count);
        AutoPtr<IElement> e;
        mType->GetElement((IElement**)&e);
        Int32 size;
        e->GetBytesSize(&size);
        mSize = count * size;
        UpdateCacheInfo(t);
    }
    // try {
    // TODO: how to free?????????????????????????????????
    assert(0);
    Int32* buf = (Int32*)malloc(mSize);
    // RenderScript.registerNativeAllocation.invoke(RenderScript.sRuntime, mSize);
    // } catch (Exception e) {
    //     Log.e(RenderScript.LOG_TAG, "Couldn't invoke registerNativeAllocation:" + e);
    //     throw new RSRuntimeException("Couldn't invoke registerNativeAllocation:" + e);
    // }
    return NOERROR;
}

ECode Allocation::ValidateObjectIsPrimitiveArray(
    /* [in] */ IInterface* d,
    /* [in] */ Boolean checkType,
    /* [out] */ Element::DataType** type)
{
    VALIDATE_NOT_NULL(type)
    *type = NULL;

    // Class c = d.getClass();
    // if (!c.isArray()) {
    //     throw new RSIllegalArgumentException("Object passed is not an array of primitives.");
    // }
    AutoPtr<IArrayOf> array = IArrayOf::Probe(d);
    if (array == NULL) {
        Slogger:::E("Allocation", "Object passed is not an array of primitives.");
        return E_RS_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    // final Class cmp = c.getComponentType();
    // if (!cmp.isPrimitive()) {
    //     throw new RSIllegalArgumentException("Object passed is not an Array of primitives.");
    // }
    InterfaceID cmp;
    array->GetTypeId(&cmp);
    if (cmp != EIID_IInteger32 || cmp != EIID_IInteger16 || cmp != EIID_IFloat ||
            cmp != EIID_IByte || cmp != EIID_IInteger64 || cmp != EIID_IDouble) {
        Slogger:::E("Allocation", "Object passed is not an array of primitives.");
        return E_RS_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (cmp == EIID_IInteger64) {
        if (checkType) {
            FAIL_RETURN(ValidateIsInt64())
            *type = mType->mElement->mType;
            return NOERROR;
        }
        *type = Element::DataType::SIGNED_64;
        return NOERROR;
    }

    if (cmp == EIID_IInteger32) {
        if (checkType) {
            FAIL_RETURN(ValidateIsInt32())
            *type = mType->mElement->mType;
            return NOERROR;
        }
        *type = Element::DataType::SIGNED_32;
        return NOERROR;
    }

    if (cmp == EIID_IInteger16) {
        if (checkType) {
            FAIL_RETURN(ValidateIsInt16())
            *type = mType->mElement->mType;
            return NOERROR;
        }
        *type = Element::DataType::SIGNED_16;
        return NOERROR;
    }

    if (cmp == EIID_IByte) {
        if (checkType) {
            FAIL_RETURN(ValidateIsInt8())
            *type = mType->mElement->mType;
            return NOERROR;
        }
        *type = Element::DataType::SIGNED_8;
        return NOERROR;
    }

    if (cmp == EIID_IFloat) {
        if (checkType) {
            FAIL_RETURN(ValidateIsFloat32())
            *type = mType->mElement->mType;
            return NOERROR;
        }
        *type = Element::DataType::FLOAT_32;
        return NOERROR;
    }

    if (cmp == EIID_IDouble) {
        if (checkType) {
            FAIL_RETURN(ValidateIsFloat64())
            *type = mType->mElement->mType;
            return NOERROR;
        }
        *type = Element::DataType::FLOAT_64;
        return NOERROR;
    }
    return NOERROR;
}

ECode Allocation::GetIDSafe(
    /* [out] */ Int64* id)
{
    VALIDATE_NOT_NULL(id)
    if (mAdaptedAllocation != NULL) {
        return mAdaptedAllocation->GetID(mRS, id);
    }
    return GetID(mRS, id);
}

ECode Allocation::GetElement(
    /* [out] */ IElement** element)
{
    VALIDATE_NOT_NULL(element)
    return mType->GetElement(element);
}

ECode Allocation::GetUsage(
    /* [out] */ Int32* usage)
{
    VALIDATE_NOT_NULL(usage)
    *usage = mUsage;
    return NOERROR;
}

ECode Allocation::GetBytesSize(
    /* [out] */ Int32* bytesSize)
{
    VALIDATE_NOT_NULL(bytesSize)
    Int32 count;
    mType->GetCount(&count);
    AutoPtr<IElement> e;
    mType->GetElement((IElement**)&e);
    Int32 size;
    e->GetBytesSize(&size);
    if (mType->mDimYuv != 0) {
        *bytesSize = (Int32)Elastos::Core::Math::Ceil(count * size * 1.5);
        return NOERROR;
    }
    *bytesSize = count * size;
    return NOERROR;
}

void Allocation::UpdateCacheInfo(
    /* [in] */ IType* t)
{
    t->GetX(&mCurrentDimX);
    t->GetY(&mCurrentDimY);
    t->GetZ(&mCurrentDimZ);
    mCurrentCount = mCurrentDimX;
    if (mCurrentDimY > 1) {
        mCurrentCount *= mCurrentDimY;
    }
    if (mCurrentDimZ > 1) {
        mCurrentCount *= mCurrentDimZ;
    }
}

void Allocation::SetBitmap(
    /* [in] */ IBitmap* b)
{
    mBitmap = b;
}

Allocation::~Allocation()
{
    assert(0);
    // RenderScript.registerNativeFree.invoke(RenderScript.sRuntime, mSize);
}

ECode Allocation::ValidateIsInt64()
{
    AutoPtr<IElement> e;
    mType->GetElement((IElement**)&e);
    AutoPtr<Element> eObj = (Element*)e.Get();
    if ((eObj->mType->mID == ElementDataType_SIGNED_64) ||
        (eObj->mType->mID == ElementDataType_UNSIGNED_64)) {
        return NOERROR;
    }
    Slogger::E("Allocation", "64 bit integer source does not match allocation type %d", eObj->mType);
    return E_RS_ILLEGAL_ARGUMENT_EXCEPTION;
}

ECode Allocation::ValidateIsInt32()
{
    AutoPtr<IElement> e;
    mType->GetElement((IElement**)&e);
    AutoPtr<Element> eObj = (Element*)e.Get();
    if ((eObj->mType->mID == ElementDataType_SIGNED_32) ||
        (eObj->mType->mID == ElementDataType_UNSIGNED_32)) {
        return NOERROR;
    }
    Slogger::E("Allocation", "32 bit integer source does not match allocation type %d", eObj->mType);
    return E_RS_ILLEGAL_ARGUMENT_EXCEPTION;
}

ECode Allocation::ValidateIsInt16()
{
    AutoPtr<IElement> e;
    mType->GetElement((IElement**)&e);
    AutoPtr<Element> eObj = (Element*)e.Get();
    if ((eObj->mType->mID == ElementDataType_SIGNED_16) ||
        (eObj->mType->mID == ElementDataType_UNSIGNED_16)) {
        return NOERROR;
    }
    Slogger::E("Allocation", "16 bit integer source does not match allocation type %d", eObj->mType);
    return E_RS_ILLEGAL_ARGUMENT_EXCEPTION;
}

ECode Allocation::ValidateIsInt8()
{
    AutoPtr<IElement> e;
    mType->GetElement((IElement**)&e);
    AutoPtr<Element> eObj = (Element*)e.Get();
    if ((eObj->mType->mID == ElementDataType_SIGNED_8) ||
        (eObj->mType->mID == ElementDataType_UNSIGNED_8)) {
        return NOERROR;
    }
    Slogger::E("Allocation", "8 bit integer source does not match allocation type %d", eObj->mType);
    return E_RS_ILLEGAL_ARGUMENT_EXCEPTION;
}

ECode Allocation::ValidateIsFloat32()
{
    AutoPtr<IElement> e;
    mType->GetElement((IElement**)&e);
    AutoPtr<Element> eObj = (Element*)e.Get();
    if (eObj->mType->mID == ElementDataType_FLOAT_32) {
        return NOERROR;
    }
    Slogger::E("Allocation", "32 bit float source does not match allocation type %d", eObj->mType);
    return E_RS_ILLEGAL_ARGUMENT_EXCEPTION;
}

ECode Allocation::ValidateIsFloat64()
{
    AutoPtr<IElement> e;
    mType->GetElement((IElement**)&e);
    AutoPtr<Element> eObj = (Element*)e.Get();
    if (eObj->mType->mID == ElementDataType_FLOAT_64) {
        return NOERROR;
    }
    Slogger::E("Allocation", "64 bit float source does not match allocation type %d", eObj->mType);
    return E_RS_ILLEGAL_ARGUMENT_EXCEPTION;
}

ECode Allocation::ValidateIsObject()
{
    AutoPtr<IElement> e;
    mType->GetElement((IElement**)&e);
    AutoPtr<Element> eObj = (Element*)e.Get();
    if ((eObj->mType->mID == ElementDataType_RS_ELEMENT) ||
        (eObj->mType->mID == ElementDataType_RS_TYPE) ||
        (eObj->mType->mID == ElementDataType_RS_ALLOCATION) ||
        (eObj->mType->mID == ElementDataType_RS_SAMPLER) ||
        (eObj->mType->mID == ElementDataType_RS_SCRIPT) ||
        (eObj->mType->mID == ElementDataType_RS_MESH) ||
        (eObj->mType->mID == ElementDataType_RS_PROGRAM_FRAGMENT) ||
        (eObj->mType->mID == ElementDataType_RS_PROGRAM_VERTEX) ||
        (eObj->mType->mID == ElementDataType_RS_PROGRAM_RASTER) ||
        (eObj->mType->mID == ElementDataType_RS_PROGRAM_STORE)) {
        return NOERROR;
    }
    Slogger::E("Allocation", "Object source does not match allocation type %d", eObj->mType);
    return E_RS_ILLEGAL_ARGUMENT_EXCEPTION;
}

void Allocation::UpdateFromNative()
{
    BaseObj::UpdateFromNative();
    Int64 id;
    GetID(mRS, &id);
    Int64 typeID = mRS->NAllocationGetType(id);
    if(typeID != 0) {
        mType = new Type(typeID, mRS);
        mType=>UpdateFromNative();
        UpdateCacheInfo(mType);
    }
}

ECode Allocation::GetType(
    /* [out] */ IType** type)
{
    VALIDATE_NOT_NULL(type)
    *type = IType::Probe(mType)
    REFCOUNT_ADD(*type)
    return NOERROR;
}

ECode Allocation::SyncAll(
    /* [in] */ Int32 srcLocation)
{
    // Trace.traceBegin(RenderScript::TRACE_TAG, "syncAll");
    switch (srcLocation) {
        case USAGE_GRAPHICS_TEXTURE:
        case USAGE_SCRIPT:
            if ((mUsage & USAGE_SHARED) != 0) {
                CopyFrom(mBitmap);
            }
            break;
        case USAGE_GRAPHICS_CONSTANTS:
        case USAGE_GRAPHICS_VERTEX:
            break;
        case USAGE_SHARED:
            if ((mUsage & USAGE_SHARED) != 0) {
                copyTo(mBitmap);
            }
            break;
        default:
            Slogger::E("Allocation", "Source must be exactly one usage type.");
            return E_RS_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    FAIL_RETURN(mRS->Validate())
    Int64 id;
    GetIDSafe(&id);
    mRS->NAllocationSyncAll(id, srcLocation);
    // Trace.traceEnd(RenderScript.TRACE_TAG);
    return NOERROR;
}

ECode Allocation::IoSend()
{
    // Trace.traceBegin(RenderScript.TRACE_TAG, "ioSend");
    if ((mUsage & USAGE_IO_OUTPUT) == 0) {
        Slogger::E("Allocation", "Can only send buffer if IO_OUTPUT usage specified.");
        return E_RS_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    FAIL_RETURN(mRS->Validate())
    Int64 id;
    getID(mRS, &id);
    mRS->NAllocationIoSend(getID(mRS));
    // Trace.traceEnd(RenderScript.TRACE_TAG);
    return NOERROR;
}

ECode Allocation::IoReceive()
{
    // Trace.traceBegin(RenderScript.TRACE_TAG, "ioReceive");
    if ((mUsage & USAGE_IO_INPUT) == 0) {
        Slogger::E("Allocation", "Can only receive if IO_INPUT usage specified.");
        return E_RS_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    FAIL_RETURN(mRS->Validate())
    Int64 id;
    getID(mRS, &id);
    mRS->NAllocationIoSend(getID(mRS));
    // Trace.traceEnd(RenderScript.TRACE_TAG);
    return NOERROR;
}

ECode Allocation::CopyFrom(
    /* [in] */ ArrayOf<IBaseObj*>* d)
{
    // Trace.traceBegin(RenderScript.TRACE_TAG, "copyFrom");
    FAIL_RETURN(mRS->Validate())
    FAIL_RETURN(ValidateIsObject())
    Int32 len = d->GetLength();
    if (len != mCurrentCount) {
        Slogger::E("Allocation", "Array size mismatch, allocation sizeX = %d, array length = %d",
                mCurrentCount, len);
        return E_RS_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (RenderScript::sPointerSize == 8) {
        AutoPtr<ArrayOf<Int64> > i = ArrayOf<Int64>::Alloc(len * 4);
        for (Int32 ct = 0; ct < len; ct++) {
            AutoPtr<BaseObj> obj = (BaseObj*)(*d)[ct];
            obj->GetID(mRS, &(*i)[ct * 4]);
        }
        return Copy1DRangeFromUnchecked(0, mCurrentCount, i);
    }
    else {
        AutoPtr<ArrayOf<Int32> > i = ArrayOf<Int32>::Alloc(len);
        for (Int32 ct = 0; ct < len; ct++) {
            AutoPtr<BaseObj> obj = (BaseObj*)(*d)[ct];
            Int64 id;
            obj->GetID(mRS, &id);
            (*i)[ct] = (Int32)id;
        }
        return Copy1DRangeFromUnchecked(0, mCurrentCount, i);
    }
    // Trace.traceEnd(RenderScript.TRACE_TAG);
}

ECode Allocation::ValidateBitmapFormat(
    /* [in] */ IBitmap* b)
{
    BitmapConfig bc;
    b->GetConfig(&bc);
    if (bc == BitmapConfig_NONE) {
        Slogger::E("Allocation", "Bitmap has an unsupported format for this operation");
        return E_RS_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IElement> e;
    AutoPtr<Element> eObj;
    switch (bc) {
        case BitmapConfig_ALPHA_8:
            AutoPtr<IElement> e;
            mType->GetElement((IElement**)&e);
            AutoPtr<Element> eObj = (Element*)e.Get();
            if (eObj->mKind != ElementDataKind_PIXEL_A) {
                Int32 size;
                e->GetBytesSize(&size);
                Slogger::E("Allocation", "Allocation kind is %d, type %d of %d bytes, passed bitmap was %d",
                        eObj->mKind, eObj->mType, size, bc);
                return E_RS_ILLEGAL_ARGUMENT_EXCEPTION;
            }
            break;
        case BitmapConfig_ARGB_8888:
            AutoPtr<IElement> e;
            mType->GetElement((IElement**)&e);
            AutoPtr<Element> eObj = (Element*)e.Get();
            Int32 size;
            if ((eObj->mKind != ElementDataKind_PIXEL_RGBA) ||
                (e->GetBytesSize(&size), size != 4)) {
                e->GetBytesSize(&size);
                Slogger::E("Allocation", "Allocation kind is %d, type %d bytes, passed bitmap was %d",
                        eObj->mKind, eObj->mType, size, bc);
                return E_RS_ILLEGAL_ARGUMENT_EXCEPTION;
            }
            break;
        case BitmapConfig_RGB_565:
            AutoPtr<IElement> e;
            mType->GetElement((IElement**)&e);
            AutoPtr<Element> eObj = (Element*)e.Get();
            Int32 size;
            if ((eObj->mKind != ElementDataKind_PIXEL_RGB) ||
                (e->GetBytesSize(&size), size != 2)) {
                e->GetBytesSize(&size);
                Slogger::E("Allocation", "Allocation kind is %d, type %d bytes, passed bitmap was %d",
                        eObj->mKind, eObj->mType, size, bc);
                return E_RS_ILLEGAL_ARGUMENT_EXCEPTION;
            }
            break;
        case BitmapConfig_ARGB_4444:
            AutoPtr<IElement> e;
            mType->GetElement((IElement**)&e);
            AutoPtr<Element> eObj = (Element*)e.Get();
            Int32 size;
            if ((eObj->mKind != ElementDataKind_PIXEL_RGBA) ||
                (e->GetBytesSize(&size), size != 2)) {
                e->GetBytesSize(&size);
                Slogger::E("Allocation", "Allocation kind is %d, type %d bytes, passed bitmap was %d",
                        eObj->mKind, eObj->mType, size, bc);
                return E_RS_ILLEGAL_ARGUMENT_EXCEPTION;
            }
            break;
    }
    return NOERROR;
}

ECode Allocation::ValidateBitmapSize(
    /* [in] */ IBitmap* b)
{
    Int32 width, height;
    if((b->GetWidth(&width), mCurrentDimX != width) || (b->GetHeight(&height), mCurrentDimY != height)) {
        Slogger::E("Allocation", "Cannot update allocation from bitmap, sizes mismatch");
        return E_RS_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode Allocation::CopyFromUnchecked(
    /* [in] */ IInterface* array,
    /* [in] */ Element::DataType dt,
    /* [in] */ Int32 arrayLen)
{
    // Trace.traceBegin(RenderScript.TRACE_TAG, "copyFromUnchecked");
    FAIL_RETURN(mRS->Validate())
    if (mCurrentDimZ > 0) {
        return Copy3DRangeFromUnchecked(0, 0, 0, mCurrentDimX, mCurrentDimY, mCurrentDimZ, array, dt, arrayLen);
    }
    else if (mCurrentDimY > 0) {
        return Copy2DRangeFromUnchecked(0, 0, mCurrentDimX, mCurrentDimY, array, dt, arrayLen);
    }
    else {
        return Copy1DRangeFromUnchecked(0, mCurrentCount, array, dt, arrayLen);
    }
    // Trace.traceEnd(RenderScript.TRACE_TAG);
}

ECode Allocation::CopyFromUnchecked(
    /* [in] */ IInterface* array)
{
    // Trace.traceBegin(RenderScript.TRACE_TAG, "copyFromUnchecked");
    AutoPtr<Element::DataType> type;
    FAIL_RETURN(ValidateObjectIsPrimitiveArray(array, FALSE, (Element::DataType**)&type))
    Int32 len;
    IArrayOf::Probe(array)->GetLength(&len);
    return CopyFromUnchecked(array, type, len);
    // Trace.traceEnd(RenderScript.TRACE_TAG);
}

ECode Allocation::CopyFromUnchecked(
    /* [in] */ ArrayOf<Int32>* d)
{
    return CopyFromUnchecked(d, Element::DataType::SIGNED_32, d->GetLength());
}

ECode Allocation::CopyFromUnchecked(
    /* [in] */ ArrayOf<Int16>* d)
{
    return CopyFromUnchecked(d, Element::DataType::SIGNED_16, d->GetLength());
}

ECode Allocation::CopyFromUnchecked(
    /* [in] */ ArrayOf<Byte>* d)
{
    return CopyFromUnchecked(d, Element::DataType::SIGNED_8, d->GetLength());
}

ECode Allocation::CopyFromUnchecked(
    /* [in] */ ArrayOf<Float>* d)
{
    return CopyFromUnchecked(d, Element::DataType::FLOAT_32, d->GetLength());
}

ECode Allocation::CopyFrom(
    /* [in] */ IInterface* array)
{
    // Trace.traceBegin(RenderScript.TRACE_TAG, "copyFrom");
    AutoPtr<Element::DataType> type;
    FAIL_RETURN(ValidateObjectIsPrimitiveArray(array, TRUE, (Element::DataType**)&type))
    Int32 len;
    IArrayOf::Probe(array)->GetLength(&len);
    return CopyFromUnchecked(array, type, len);
    // Trace.traceEnd(RenderScript.TRACE_TAG);
}

ECode Allocation::CopyFrom(
    /* [in] */ ArrayOf<Int32>* d)
{
    FAIL_RETURN(ValidateIsInt32())
    return CopyFromUnchecked(d, Element::DataType::SIGNED_32, d->GetLength());
}

ECode Allocation::CopyFrom(
    /* [in] */ ArrayOf<Int16>* d)
{
    FAIL_RETURN(ValidateIsInt16())
    return CopyFromUnchecked(d, Element::DataType::SIGNED_16, d->GetLength());
}

ECode Allocation::CopyFrom(
    /* [in] */ ArrayOf<Byte>* d)
{
    FAIL_RETURN(ValidateIsInt8())
    return CopyFromUnchecked(d, Element::DataType::SIGNED_8, d->GetLength());
}

ECode Allocation::CopyFrom(
    /* [in] */ ArrayOf<Float>* d)
{
    FAIL_RETURN(ValidateIsFloat32())
    return CopyFromUnchecked(d, Element::DataType::FLOAT_32, d->GetLength());
}

ECode Allocation::CopyFrom(
    /* [in] */ IBitmap* b)
{
    // Trace.traceBegin(RenderScript.TRACE_TAG, "copyFrom");
    FAIL_RETURN(mRS->Validate())
    BitmapConfig config;
    if (b->GetConfig(&config), config == BitmapConfig_NONE) {
        Int32 width, height;
        b->GetWidth(&width);
        b->GetHeight(&height);
        AutoPtr<IBitmapHelper> helper;
        CBitmapHelper::AcquireSingleton((IBitmapHelper**)&helper);
        AutoPtr<IBitmap> newBitmap;
        helper->CreateBitmap(width, height, BitmapConfig_ARGB_8888, (IBitmap**)&newBitmap);
        AutoPtr<ICanvas> c;
        CCanvas::New(newBitmap, (ICanvas**)&c);
        Canvas c = new Canvas(newBitmap);
        c->DrawBitmap(b, 0, 0, NULL);
        return CopyFrom(newBitmap);
    }
    FAIL_RETURN(validateBitmapSize(b))
    FAIL_RETURN(validateBitmapFormat(b))
    Int64 id;
    GetID(mRS, &id);
    mRS->NAllocationCopyFromBitmap(id, b);
    return NOERROR;
    // Trace.traceEnd(RenderScript.TRACE_TAG);
}

ECode Allocation::CopyFrom(
    /* [in] */ IAllocation* a)
{
    // Trace.traceBegin(RenderScript.TRACE_TAG, "copyFrom");
    FAIL_RETURN(mRS->Validate())
    AutoPtr<IType> t;
    a->GetType((IType**)&t);
    Boolean equals;
    if (IObject::Probe(mType)->Equals(t, &equals), !equals) {
        Slogger::E("Allocation", "Types of allocations must match.");
        return E_RS_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return Copy2DRangeFrom(0, 0, mCurrentDimX, mCurrentDimY, a, 0, 0);
    // Trace.traceEnd(RenderScript.TRACE_TAG);
}

ECode Allocation::SetFromFieldPacker(
    /* [in] */ Int32 xoff,
    /* [in] */ IFieldPacker* fp)
{
    FAIL_RETURN(mRS->Validate())
    Int32 eSize;
    mType->mElement->GetBytesSize(&eSize);
    AutoPtr<ArrayOf<Byte> > data;
    fp->GetData((ArrayOf<Byte>**)&data);
    Int32 data_length;
    fp->GetPos(&data_length);

    Int32 count = data_length / eSize;
    if ((eSize * count) != data_length) {
        Slogger::E("Allocation", "Field packer length %d not divisible by element size %d.",
                data_length, eSize);
        return E_RS_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return Copy1DRangeFromUnchecked(xoff, count, data);
}

ECode Allocation::SetFromFieldPacker(
    /* [in] */ Int32 xoff,
    /* [in] */ Int32 component_number,
    /* [in] */ IFieldPacker* fp)
{
    FAIL_RETURN(mRS->Validate())
    if (component_number >= mType->mElement->mElements->GetLength()) {
        Slogger::E("Allocation", "Component_number %d out of range.", component_number);
        return E_RS_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if(xoff < 0) {
        Slogger::E("Allocation", "Offset must be >= 0.");
        return E_RS_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<ArrayOf<Byte> > data;
    fp->GetData((ArrayOf<Byte>**)&data);
    Int32 data_length;
    fp->GetPos(&data_length);
    Int32 eSize;
    (*mType->mElement->mElements)[component_number]->GetBytesSize(&eSize);
    eSize *= (*mType->mElement->mArraySizes)[component_number];

    if (data_length != eSize) {
        Slogger::E("Allocation", "Field packer sizelength %d does not match component size %d.",
                data_length, eSize);
        return E_RS_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int64 id;
    GetIDSafe(&id);
    mRS->NAllocationElementData1D(id, xoff, mSelectedLOD,
            component_number, data, data_length);
    return NOERROR;
}

ECode Allocation::Data1DChecks(
    /* [in] */ Int32 off,
    /* [in] */ Int32 count,
    /* [in] */ Int32 len,
    /* [in] */ Int32 dataSize)
{
    FAIL_RETURN(mRS->Validate())
    if(off < 0) {
        Slogger::E("Allocation", "Offset must be >= 0.");
        return E_RS_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if(count < 1) {
        Slogger::E("Allocation", "Count must be >= 1.");
        return E_RS_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if((off + count) > mCurrentCount) {
        Slogger::E("Allocation", "Overflow, Available count %d, got %d at offset %d.",
                mCurrentCount, count, off);
        return E_RS_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if(len < dataSize) {
        Slogger::E("Allocation", "Array too small for allocation type.");
        return E_RS_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode Allocation::GenerateMipmaps()
{
    mRS->NAllocationGenerateMipmaps(getID(mRS));
    return NOERROR;
}

ECode Allocation::Copy1DRangeFromUnchecked(
    /* [in] */ Int32 off,
    /* [in] */ Int32 count,
    /* [in] */ IInterface* array,
    /* [in] */ Element::DataType* dt,
    /* [in] */ Int32 arrayLen)
{
    // Trace.traceBegin(RenderScript.TRACE_TAG, "copy1DRangeFromUnchecked");
    Int32 dataSize;
    mType->mElement->GetBytesSize(&dataSize)
    dataSize = dataSize * count;
    FAIL_RETURN(Data1DChecks(off, count, arrayLen * dt->mSize, dataSize))
    mRS->NAllocationData1D(getIDSafe(), off, mSelectedLOD, count, array, dataSize, dt);
    // Trace.traceEnd(RenderScript.TRACE_TAG);
    return NOERROR;
}

ECode Allocation::Copy1DRangeFromUnchecked(
    /* [in] */ Int32 off,
    /* [in] */ Int32 count,
    /* [in] */ Interface* array)
{
    AutoPtr<Element::DataType> type;
    FAIL_RETURN(ValidateObjectIsPrimitiveArray(array, FALSE, (Element::DataType**)&type))
    Int32 len;
    IArrayOf::Probe(array)->GetLength(&len);
    return Copy1DRangeFromUnchecked(off, count, array, len);
}

ECode Allocation::Copy1DRangeFromUnchecked(
    /* [in] */ Int32 off,
    /* [in] */ Int32 count,
    /* [in] */ ArrayOf<Int32>* d)
{
    AutoPtr<IArrayOf> array;
    CArrayOf::New(EIID_IInteger32, d->GetLength(), (IArrayOf**)&array);
    for (Int32 i = 0; i < d->GetLength(); ++i) {
        AutoPtr<IInteger32> v;
        CInteger32::New((*d)[i], (IInteger32**)&v);
        array->Set(i, v);
    }
    return Copy1DRangeFromUnchecked(off, count, array, Element::DataType::SIGNED_32, d->GetLength());
}

ECode Allocation::Copy1DRangeFromUnchecked(
    /* [in] */ Int32 off,
    /* [in] */ Int32 count,
    /* [in] */ ArrayOf<Int16>* d)
{
    AutoPtr<IArrayOf> array;
    CArrayOf::New(EIID_IInteger16, d->GetLength(), (IArrayOf**)&array);
    for (Int32 i = 0; i < d->GetLength(); ++i) {
        AutoPtr<IInteger16> v;
        CInteger16::New((*d)[i], (IInteger16**)&v);
        array->Set(i, v);
    }
    return Copy1DRangeFromUnchecked(off, count, array, Element::DataType::SIGNED_16, d->GetLength());
}

ECode Allocation::Copy1DRangeFromUnchecked(
    /* [in] */ Int32 off,
    /* [in] */ Int32 count,
    /* [in] */ ArrayOf<Byte>* d)
{
    AutoPtr<IArrayOf> array;
    CArrayOf::New(EIID_IByte, d->GetLength(), (IArrayOf**)&array);
    for (Int32 i = 0; i < d->GetLength(); ++i) {
        AutoPtr<IByte> v;
        CByte::New((*d)[i], (IByte**)&v);
        array->Set(i, v);
    }
    return Copy1DRangeFromUnchecked(off, count, array, Element::DataType::SIGNED_8, d->GetLength());
}

ECode Allocation::Copy1DRangeFromUnchecked(
    /* [in] */ Int32 off,
    /* [in] */ Int32 count,
    /* [in] */ ArrayOf<Float>* d)
{
    AutoPtr<IArrayOf> array;
    CArrayOf::New(EIID_IFloat, d->GetLength(), (IArrayOf**)&array);
    for (Int32 i = 0; i < d->GetLength(); ++i) {
        AutoPtr<IFloat> v;
        CFloat::New((*d)[i], (IFloat**)&v);
        array->Set(i, v);
    }
    return Copy1DRangeFromUnchecked(off, count, array, Element::DataType::FLOAT_32, d->GetLength());
}

ECode Allocation::Copy1DRangeFrom(
    /* [in] */ Int32 off,
    /* [in] */ Int32 count,
    /* [in] */ Interface* array)
{
    AutoPtr<Element::DataType> t;
    FAIL_RETURN(ValidateObjectIsPrimitiveArray(array, TRUE, (Element::DataType**)&t))
    Int32 len;
    IArrayOf::Probe(array)->GetLength(&len);
    return Copy1DRangeFromUnchecked(off, count, array, t, len);
}

ECode Allocation::Copy1DRangeFrom(
    /* [in] */ Int32 off,
    /* [in] */ Int32 count,
    /* [in] */ ArrayOf<Int32>* d)
{
    FAIL_RETURN(ValidateIsInt32())
    return Copy1DRangeFromUnchecked(off, count, d, Element::DataType::SIGNED_32, d->GetLength());
}

ECode Allocation::Copy1DRangeFrom(
    /* [in] */ Int32 off,
    /* [in] */ Int32 count,
    /* [in] */ ArrayOf<Int16>* d)
{
    FAIL_RETURN(ValidateIsInt16())
    return Copy1DRangeFromUnchecked(off, count, d, Element::DataType::SIGNED_16, d->GetLength());
}

ECode Allocation::Copy1DRangeFrom(
    /* [in] */ Int32 off,
    /* [in] */ Int32 count,
    /* [in] */ ArrayOf<Byte>* d)
{
    FAIL_RETURN(ValidateIsInt8())
    return Copy1DRangeFromUnchecked(off, count, d, Element::DataType::SIGNED_8, d->GetLength());
}

ECode Allocation::Copy1DRangeFrom(
    /* [in] */ Int32 off,
    /* [in] */ Int32 count,
    /* [in] */ ArrayOf<Float>* d)
{
    FAIL_RETURN(ValidateIsFloat32())
    return Copy1DRangeFromUnchecked(off, count, d, Element::DataType::FLOAT_32, d->GetLength());
}

ECode Allocation::Copy1DRangeFrom(
    /* [in] */ Int32 off,
    /* [in] */ Int32 count,
    /* [in] */ IAllocation* data,
    /* [in] */ Int32 dataOff)
{
    // Trace.traceBegin(RenderScript.TRACE_TAG, "copy1DRangeFrom");
    Int64 id;
    GetIDSafe(&id);
    Int64 dataId;
    data->GetID(mRS, &dataId);
    AutoPtr<Allocation> dataObj = (Allocation*)data;
    mRS->NAllocationData2D(id, off, 0,
            mSelectedLOD, mSelectedFace,
            count, 1, dataId, dataOff, 0,
            dataObj->mSelectedLOD, dataObj->mSelectedFace);
    return NOERROR;
}

ECode Allocation::Validate2DRange(
    /* [in] */ Int32 xoff,
    /* [in] */ Int32 yoff,
    /* [in] */ Int32 w,
    /* [in] */ Int32 h)
{
    if (mAdaptedAllocation == NULL) {
        if (xoff < 0 || yoff < 0) {
            Slogger::E("Allocation", "Offset cannot be negative.");
            return E_RS_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        if (h < 0 || w < 0) {
            Slogger::E("Allocation", "Height or width cannot be negative.");
            return E_RS_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        if (((xoff + w) > mCurrentDimX) || ((yoff + h) > mCurrentDimY)) {
            Slogger::E("Allocation", "Updated region larger than allocation.");
            return E_RS_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }
    return NOERROR;
}

ECode Allocation::Copy2DRangeFromUnchecked(
    /* [in] */ Int32 xoff,
    /* [in] */ Int32 yoff,
    /* [in] */ Int32 w,
    /* [in] */ Int32 h, Object array,
    /* [in] */ Element::DataType* dt,
    /* [in] */ Int32 arrayLen)
{
    // Trace.traceBegin(RenderScript.TRACE_TAG, "copy2DRangeFromUnchecked");
    FAIL_RETURN(mRS->Validate())
    FAIL_RETURN(validate2DRange(xoff, yoff, w, h))
    Int64 id;
    GetIDSafe(&id);
    mRS->NAllocationData2D(id, xoff, yoff, mSelectedLOD, mSelectedFace, w, h,
            array, arrayLen * dt->mSize, dt);
    // Trace.traceEnd(RenderScript.TRACE_TAG);
    return NOERROR;
}

ECode Allocation::Copy2DRangeFrom(
    /* [in] */ Int32 xoff,
    /* [in] */ Int32 yoff,
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ IInterface* array)
{
    // Trace.traceBegin(RenderScript.TRACE_TAG, "copy2DRangeFrom");
    AutoPtr<Element::DataType> type;
    FAIL_RETURN(ValidateObjectIsPrimitiveArray(array, TRUE), (Element::DataType**)&type);
    Int32 len;
    IArrayOf::Probe(array)->GetLength(&len);
    return Copy2DRangeFromUnchecked(xoff, yoff, w, h, array, type, len);
    // Trace.traceEnd(RenderScript.TRACE_TAG);
}

ECode Allocation::Copy2DRangeFrom(
    /* [in] */ Int32 xoff,
    /* [in] */ Int32 yoff,
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ ArrayOf<Byte>* data)
{
    FAIL_RETURN(validateIsInt8())
    return Copy2DRangeFromUnchecked(xoff, yoff, w, h, data,
            Element::DataType::SIGNED_8, data->GetLength());
}

ECode Allocation::Copy2DRangeFrom(
    /* [in] */ Int32 xoff,
    /* [in] */ Int32 yoff,
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ ArrayOf<Int16>* data)
{
    FAIL_RETURN(validateIsInt16())
    return Copy2DRangeFromUnchecked(xoff, yoff, w, h, data,
            Element::DataType::SIGNED_16, data->GetLength());
}

ECode Allocation::Copy2DRangeFrom(
    /* [in] */ Int32 xoff,
    /* [in] */ Int32 yoff,
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ ArrayOf<Int32>* data)
{
    FAIL_RETURN(validateIsInt32())
    return Copy2DRangeFromUnchecked(xoff, yoff, w, h, data,
            Element::DataType::SIGNED_32, data->GetLength());
}

ECode Allocation::Copy2DRangeFrom(
    /* [in] */ Int32 xoff,
    /* [in] */ Int32 yoff,
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ ArrayOf<Float>* data)
{
    FAIL_RETURN(ValidateIsFloat32())
    return Copy2DRangeFromUnchecked(xoff, yoff, w, h, data,
            Element::DataType::FLOAT_32, data->GetLength());
}

ECode Allocation::Copy2DRangeFrom(
    /* [in] */ Int32 xoff,
    /* [in] */ Int32 yoff,
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ IAllocation* data,
    /* [in] */ Int32 dataXoff,
    /* [in] */ Int32 dataYoff)
{
    // Trace.traceBegin(RenderScript.TRACE_TAG, "copy2DRangeFrom");
    FAIL_RETURN(mRS->Validate())
    FAIL_RETURN(Validate2DRange(xoff, yoff, w, h))
    Int64 id;
    GetIDSafe(&id);
    Int64 dataId;
    data->GetID(mRS, &dataId);
    AutoPtr<Allocation> dataObj = (Allocation*)data;
    mRS->NAllocationData2D(id, xoff, yoff,
            mSelectedLOD, mSelectedFace.mID,
            w, h, dataId, dataXoff, dataYoff,
            dataObj->mSelectedLOD, dataObj->mSelectedFace);
    // Trace.traceEnd(RenderScript.TRACE_TAG);
    return NOERROR;
}

ECode Allocation::Copy2DRangeFrom(
    /* [in] */ Int32 xoff,
    /* [in] */ Int32 yoff,
    /* [in] */ IBitmap* data)
{
    // Trace.traceBegin(RenderScript.TRACE_TAG, "copy2DRangeFrom");
    FAIL_RETURN(mRS->Validate())
    BitmapConfig config;
    if (data->GetConfig(&config), config == BitmapConfig_NONE) {
        Int32 width, height;
        data->GetWidth(&width);
        data->GetHeight(&height);
        AutoPtr<IBitmapHelper> helper;
        CBitmapHelper::AcquireSingleton((IBitmapHelper**)&helper);
        AutoPtr<IBitmap> newBitmap;
        helper->CreateBitmap(width, height, BitmapConfig_ARGB_8888);
        AutoPtr<ICanvas> c;
        CCanvas::New(newBitmap, (ICanvas**)&c);
        c->DrawBitmap(data, 0, 0, NULL);
        return Copy2DRangeFrom(xoff, yoff, newBitmap);
    }
    FAIL_RETURN(ValidateBitmapFormat(data))
    Int32 width, height;
    data->GetWidth(&width);
    data->GetHeight(&height);
    FAIL_RETURN(Validate2DRange(xoff, yoff, width, height))
    Int64 id;
    GetIDSafe(&id);
    mRS->NAllocationData2D(id, xoff, yoff, mSelectedLOD, mSelectedFace, data);
    // Trace.traceEnd(RenderScript.TRACE_TAG);
    return NOERROR;
}

ECode Allocation::Validate3DRange(
    /* [in] */ Int32 xoff,
    /* [in] */ Int32 yoff,
    /* [in] */ Int32 zoff,
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ Int32 d)
{
    if (mAdaptedAllocation == null) {
        if (xoff < 0 || yoff < 0 || zoff < 0) {
            Slogger::E("Allocation", "Offset cannot be negative.");
            return E_RS_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        if (h < 0 || w < 0 || d < 0) {
            Slogger::E("Allocation", "Height or width cannot be negative.");
            return E_RS_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        if (((xoff + w) > mCurrentDimX) || ((yoff + h) > mCurrentDimY) || ((zoff + d) > mCurrentDimZ)) {
            Slogger::E("Allocation", "Updated region larger than allocation.");
            return E_RS_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }
}

ECode Allocation::Copy3DRangeFromUnchecked(
    /* [in] */ Int32 xoff,
    /* [in] */ Int32 yoff,
    /* [in] */ Int32 zoff,
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ Int32 d,
    /* [in] */ IInterface* array,
    /* [in] */ Element::DataType* dt,
    /* [in] */ Int32 arrayLen)
{
    // Trace.traceBegin(RenderScript.TRACE_TAG, "copy3DRangeFromUnchecked");
    FAIL_RETURN(mRS->Validate())
    FAIL_RETURN(Validate3DRange(xoff, yoff, zoff, w, h, d))
    Int64 id;
    GetIDSafe(&id);
    mRS->NAllocationData3D(id, xoff, yoff, zoff, mSelectedLOD, w, h, d,
            array, arrayLen * dt->mSize, dt);
    // Trace.traceEnd(RenderScript.TRACE_TAG);
    return NOERROR;
}

ECode Allocation::Copy3DRangeFrom(
    /* [in] */ Int32 xoff,
    /* [in] */ Int32 yoff,
    /* [in] */ Int32 zoff,
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ Int32 d,
    /* [in] */ IInterface* array)
{
    // Trace.traceBegin(RenderScript.TRACE_TAG, "copy3DRangeFrom");
    AutoPtr<Element::DataType> type;
    ValidateObjectIsPrimitiveArray(array, TRUE, (Element::DataType**)&type);
    Int32 len;
    IArrayOf::Probe(array)->GetLength(&len);
    return Copy3DRangeFromUnchecked(xoff, yoff, zoff, w, h, d, array,
            type, len);
    // Trace.traceEnd(RenderScript.TRACE_TAG);
    return NOERROR;
}

ECode Allocation::Copy3DRangeFrom(
    /* [in] */ Int32 xoff,
    /* [in] */ Int32 yoff,
    /* [in] */ Int32 zoff,
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ Int32 d,
    /* [in] */ IAllocation* data,
    /* [in] */ Int32 dataXoff,
    /* [in] */ Int32 dataYoff,
    /* [in] */ Int32 dataZoff)
{
    FAIL_RETURN(mRS->Validate())
    FAIL_RETURN(Validate3DRange(xoff, yoff, zoff, w, h, d))
    Int64 id;
    GetIDSafe(&id);
    Int64 dataId;
    data->GetID(mRS, &dataId);
    AutoPtr<Allocation> dataObj = (Allocation*)data;
    mRS->NAllocationData3D(getIDSafe(), xoff, yoff, zoff, mSelectedLOD,
            w, h, d, dataId, dataXoff, dataYoff, dataZoff, data->mSelectedLOD);
    return NOERROR;
}

ECode Allocation::CopyTo(
    /* [in] */ IBitmap* b)
{
    // Trace.traceBegin(RenderScript.TRACE_TAG, "copyTo");
    FAIL_RETURN(mRS->Validate())
    FAIL_RETURN(ValidateBitmapFormat(b))
    FAIL_RETURN(ValidateBitmapSize(b))
    Int64 id;
    GetID(mRS, &id);
    mRS->NAllocationCopyToBitmap(id, b);
    // Trace.traceEnd(RenderScript.TRACE_TAG);
    return NOERROR;
}

ECode Allocation::CopyTo(
    /* [in] */ IInterface* array,
    /* [in] */ Element::DataType* dt,
    /* [in] */ Int32 arrayLen)
{
    // Trace.traceBegin(RenderScript.TRACE_TAG, "copyTo");
    FAIL_RETURN(mRS->Validate())
    Int64 id;
    GetID(mRS, &id);
    mRS->NAllocationRead(id, array, dt);
    // Trace.traceEnd(RenderScript.TRACE_TAG);
    return NOERROR;
}

ECode Allocation::CopyTo(
    /* [in] */ IInterface* array)
{
    AutoPtr<Element::DataType> type;
    ValidateObjectIsPrimitiveArray(array, TRUE, (Element::DataType**)&type);
    Int32 len;
    IArrayOf::Probe(array)->GetLength(&len);
    return CopyTo(array, type, len);
}

ECode Allocation::CopyTo(
    /* [in] */ ArrayOf<Byte>* d)
{
    FAIL_RETURN(ValidateIsInt8())
    return CopyTo(d, Element::DataType::SIGNED_8, d.->GetLength());
}

ECode Allocation::CopyTo(
    /* [in] */ ArrayOf<Int16>* d)
{
    FAIL_RETURN(ValidateIsInt16())
    return CopyTo(d, Element::DataType::SIGNED_16, d.->GetLength());
}

ECode Allocation::CopyTo(
    /* [in] */ ArrayOf<Int32>* d)
{
    FAIL_RETURN(ValidateIsInt32())
    return CopyTo(d, Element::DataType::SIGNED_32, d.->GetLength());
}

ECode Allocation::CopyTo(
    /* [in] */ ArrayOf<Float>* d)
{
    FAIL_RETURN(ValidateIsFloat32())
    return CopyTo(d, Element::DataType::FLOAT_32, d.->GetLength());
}

ECode Allocation::Resize(
    /* [in] */ Int32 dimX)
{
    AutoPtr<IContext> ctx;
    mRS->GetApplicationContext((IContext**)&ctx);
    AutoPtr<IApplicationInfo> info;
    ctx->GetApplicationInfo((IApplicationInfo**)&info);
    Int32 version;
    if (info->GetTargetSdkVersion(&version), version >= 21) {
        Slogger::E("Allocation", "Resize is not allowed in API 21+.");
        return E_RUNTIME_EXCEPTION;
    }
    Int32 y, z;
    Boolean hasFaces, hasMipmaps;
    if ((mType->GetY(&y), y > 0)|| (mType->GetZ(&z), z > 0) ||
            (mType->HasFaces(&hasFaces), hasFaces) || (mType->HasMipmaps(&hasMipmaps), hasMipmaps)) {
        Slogger::E("Allocation", "Resize only support for 1D allocations at this time.");
        return E_RS_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Int64 id;
    GetID(mRS, &id);
    mRS->NAllocationResize1D(id, dimX);
    mRS->Finish();  // Necessary because resize is fifoed and update is async.

    Int64 typeID = mRS->NAllocationGetType(id);
    mType = new Type(typeID, mRS);
    mType->UpdateFromNative();
    UpdateCacheInfo(mType);
    return NOERROR;
}

ECode Allocation::CreateTyped(
    /* [in] */ IRenderScript* rs,
    /* [in] */ IType* type,
    /* [in] */ AllocationMipmapControl mips,
    /* [in] */ Int32 usage,
    /* [out] */ IAllocation** allocation)
{
    VALIDATE_NOT_NULL(allocation)
    *allocation = NULL;
    // Trace.traceBegin(RenderScript.TRACE_TAG, "createTyped");
    FAIL_RETURN(rs->Validate())
    Int64 id;
    if (type->GetID(rs, &id), id == 0) {
        Slogger::E("Allocation", "Bad Type");
        retyrb E_RS_INVILID_STATE_EXCEPTION;
    }
    Int64 id = rs->NAllocationCreateTyped(id, mips, usage, 0);
    if (id == 0) {
        Slogger::E("Allocation", "Allocation creation failed.");
        retyrb E_RS_RUNTIME_EXCEPTION;
    }
    // Trace.traceEnd(RenderScript.TRACE_TAG);
    *allocation = (IAllocation*)new Allocation(id, rs, type, usage);
    REFCOUNT_ADD(*allocation)
    return NOERROR;
}

ECode Allocation::CreateTyped(
    /* [in] */ IRenderScript* rs,
    /* [in] */ IType* type,
    /* [in] */ Int32 usage,
    /* [out] */ IAllocation** allocation)
{
    return CreateTyped(rs, type, AllocationMipmapControl_MIPMAP_NONE, usage, allocation);
}

ECode Allocation::CreateTyped(
    /* [in] */ IRenderScript* rs,
    /* [in] */ IType* type,
    /* [out] */ IAllocation** allocation)
{
    return CreateTyped(rs, type, AllocationMipmapControl_MIPMAP_NONE, USAGE_SCRIPT, allocation);
}

ECode Allocation::CreateSized(
    /* [in] */ IRenderScript* rs,
    /* [in] */ IElement* e,
    /* [in] */ Int32 count,
    /* [in] */ Int32 usage,
    /* [out] */ IAllocation** allocation)
{
    VALIDATE_NOT_NULL(allocation)
    *allocation = NULL;
    // Trace.traceBegin(RenderScript.TRACE_TAG, "createSized");
    FAIL_RETURN(rs->Validate())
    AutoPtr<ITyeBuilder> b;
    CTypeBuilder::New(rs, e, (ITyeBuilder**)&b);
    b->SetX(count);
    AutoPtr<IType> t;
    b->Create((IType**)&t);

    Int64 tId;
    t->GetID(rs, &tId);
    Int64 id = rs->NAllocationCreateTyped(tId, AllocationMipmapControl_MIPMAP_NONE, usage, 0);
    if (id == 0) {
        Slogger::E("Allocation", "Allocation creation failed.");
    }
    // Trace.traceEnd(RenderScript.TRACE_TAG);
    *allocation = (IAllocation*)new Allocation(id, rs, t, usage);
    REFCOUNT_ADD(*allocation)
    return NOERROR;
}

ECode Allocation::CreateSized(
    /* [in] */ IRenderScript* rs,
    /* [in] */ IElement* e,
    /* [in] */ Int32 count,
    /* [out] */ IAllocation** allocation)
{
    return CreateSized(rs, e, count, USAGE_SCRIPT);
}

ECode Allocation::ElementFromBitmap(
    /* [in] */ IRenderScript* rs,
    /* [in] */ IBitmap* b,
    /* [out] */ IElement** e)
{
    VALIDATE_NOT_NULL(e)
    *e = NULL;
    BitmapConfig bc;
    b->GetConfig(&bc);
    if (bc == BitmapConfig_ALPHA_8) {
        return Element::A_8(rs, e);
    }
    if (bc == BitmapConfig_ARGB_4444) {
        return Element::RGBA_4444(rs, e);
    }
    if (bc == BitmapConfig_ARGB_8888) {
        return Element::RGBA_8888(rs, e);
    }
    if (bc == BitmapConfig_RGB_565) {
        return Element::RGB_565(rs, e);
    }
    Slogger::E("Allocation", "Bad bitmap type: %d", bc);
    return E_RS_INVILID_STATE_EXCEPTION;
}

ECode Allocation::TypeFromBitmap(
    /* [in] */ IRenderScript* rs,
    /* [in] */ IBitmap* b,
    /* [in] */ AllocationMipmapControl mip,
    /* [out] */ IType** t)
{
    AutoPtr<IElement> e;
    ElementFromBitmap(rs, b, (IElement**)&e);
    AutoPtr<ITypeBuilder> tb;
    CTypeBuilder::New(rs, e, (ITypeBuilder**)&tb);
    Int32 width, height;
    b->GetWidth(&width);
    b->GetHeight(&height);
    tb->SetX(width);
    tb->SetY(height);
    tb->SetMipmaps(mip == AllocationMipmapControl_MIPMAP_FULL);
    return tb->Create(t);
}

ECode Allocation::CreateFromBitmap(
    /* [in] */ IRenderScript* rs,
    /* [in] */ IBitmap* b,
    /* [in] */ AllocationMipmapControl mips,
    /* [in] */ Int32 usage,
    /* [out] */ IAllocation** allocation)
{
    VALIDATE_NOT_NULL(allocation)
    *allocation = NULL;
    // Trace.traceBegin(RenderScript.TRACE_TAG, "createFromBitmap");
    FAIL_RETURN(rs->Validate())

    // WAR undocumented color formats
    BitmapConfig config;
    if (b->GetConfig(&config), config == BitmapConfig_NONE) {
        if ((usage & USAGE_SHARED) != 0) {
            Slogger::E("Allocation", "USAGE_SHARED cannot be used with a Bitmap that has a null config.");
            return E_RS_INVILID_STATE_EXCEPTION;
        }
        Int32 width, height;
        b->GetWidth(&width);
        b->GetHeight(&height);
        AutoPtr<IBitmapHelper> helper;
        CBitmapHelper::AcquireSingleton((IBitmapHelper**)&helper);
        AutoPtr<IBitmap> newBitmap;
        helper->CreateBitmap(width, height, BitmapConfig_ARGB_8888, (IBitmap**)&newBitmap);
        AutoPtr<ICanvas> c;
        CCanvas::New(newBitmap, (ICanvas**)&c);
        c->DrawBitmap(b, 0, 0, NULL);
        return CreateFromBitmap(rs, newBitmap, mips, usage, allocation);
    }

    AutoPtr<IType> t;
    TypeFromBitmap(rs, b, mips, (IType**)&t);

    // enable optimized bitmap path only with no mipmap and script-only usage
    if (mips == AllocationMipmapControl_MIPMAP_NONE) {
        AutoPtr<IElement> e;
        t->GetElement((IElement**)&e);
        Boolean isCompatible;
        AutoPtr<IElement> 8888e;
        Element::RGBA_8888(rs, (IElement**)&8888e);
        if ((e->IsCompatible(8888e, &isCompatible), isCompatible) &&
                (usage == (USAGE_SHARED | USAGE_SCRIPT | USAGE_GRAPHICS_TEXTURE))) {
            Int64 tId;
            t->GetID(rs, &tId);
            Int64 id = rs->NAllocationCreateBitmapBackedAllocation(tId, mips, b, usage);
            if (id == 0) {
                Slogger::E("Allocation", "Load failed.");
                return E_RS_RUNTIME_EXCEPTION;
            }

            // keep a reference to the Bitmap around to prevent GC
            *allocation = (IAllocation*)new Allocation(id, rs, t, usage);
            (*allocation)->SetBitmap(b);
            REFCOUNT_ADD(*allocation)
            return NOERROR;
        }
    }

    Int64 tId;
    t->GetID(rs, &tId);
    Int64 id = rs->NAllocationCreateFromBitmap(tid, mips, b, usage);
    if (id == 0) {
        Slogger::E("Allocation", "Load failed.");
        return E_RS_RUNTIME_EXCEPTION;
    }
    // Trace.traceEnd(RenderScript.TRACE_TAG);
    *allocation = (IAllocation*)new Allocation(id, rs, t, usage);
    REFCOUNT_ADD(*allocation)
    return NOERROR;
}

ECode Allocation::GetSurface(
    /* [out] */ ISurface** surface)
{
    VALIDATE_NOT_NULL(surface)
    *surface = NULL;
    if ((mUsage & USAGE_IO_INPUT) == 0) {
        Slogger::E("Allocation", "Allocation is not a surface texture.");
        return E_RS_INVILID_STATE_EXCEPTION;
    }
    Int64 id;
    GetID(mRS, &id);
    AutoPtr<ISurface> s = mRS->NAllocationGetSurface(id);
    *surface = s;
    REFCOUNT_ADD(*surface)
    return NOERROR;
}

ECode Allocation::SetSurface(
    /* [in] */ ISurface* sur)
{
    FAIL_RETURN(mRS->Validate())
    if ((mUsage & USAGE_IO_OUTPUT) == 0) {\
        Slogger::E("Allocation", "Allocation is not USAGE_IO_OUTPUT.");
        return E_RS_INVILID_STATE_EXCEPTION;
    }

    Int64 id;
    GetID(mRS, &id);
    mRS->NAllocationSetSurface(id, sur);
    return NOERROR;
}

ECode Allocation::CreateFromBitmap(
    /* [in] */ IRenderScript* rs,
    /* [in] */ IBitmap* b
    /* [out] */ IAllocation** allocation)
{
    VALIDATE_NOT_NULL(allocation)
    AutoPtr<IContext> ctx;
    rs->GetApplicationContext((IContext**)&ctx);
    AutoPtr<IApplicationInfo> info;
    ctx->GetApplicationInfo((IApplicationInfo**)&info);
    Int32 version;
    if (info->GetTargetSdkVersion(&version), version >= 18) {
        return CreateFromBitmap(rs, b, AllocationMipmapControl_MIPMAP_NONE,
                USAGE_SHARED | USAGE_SCRIPT | USAGE_GRAPHICS_TEXTURE, allocation);
    }
    return CreateFromBitmap(rs, b, AllocationMipmapControl_MIPMAP_NONE,
            USAGE_GRAPHICS_TEXTURE, allocation);
}

ECode Allocation::CreateCubemapFromBitmap(
    /* [in] */ IRenderScript* rs,
    /* [in] */ IBitmap* b,
    /* [in] */ AllocationMipmapControl mips,
    /* [in] */ Int32 usage,
    /* [out] */ IAllocation** allocation)
{
    VALIDATE_NOT_NULL(allocation)
    *allocation = NULL;
    FAIL_RETURN(rs->Validate())

    Int32 height;
    b->GetHeight(&height);
    Int32 width;
    b->GetWidth(&width);

    if (width % 6 != 0) {
        Slogger::E("Allocation", "Cubemap height must be multiple of 6");
        return E_RS_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (width / 6 != height) {
        Slogger::E("Allocation", "Only square cube map faces supported");
        return E_RS_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Boolean isPow2 = (height & (height - 1)) == 0;
    if (!isPow2) {
        Slogger::E("Allocation", "Only power of 2 cube faces supported");
        return E_RS_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IElement> e;
    ElementFromBitmap(rs, b, (IElement**)&e);
    AutoPtr<ITypeBuilder> tb;
    CTypeBuilder::New(rs, e, (ITypeBuilder**)&tb);
    tb->SetX(height);
    tb->SetY(height);
    tb->SetFaces(TRUE);
    tb->SetMipmaps(mips == AllocationMipmapControl_MIPMAP_FULL);
    AutoPtr<IType> t;
    tb->Create((IType**)&t);

    Int64 tId;
    t->GetID(rs, &tId);
    Int64 id = rs->NAllocationCubeCreateFromBitmap(tid, mips, b, usage);
    if(id == 0) {
        Slogger::E("Allocation", "Load failed for bitmap %s element %s", TO_CSTR(b), TO_CSTR(e));
        return E_RS_RUNTIME_EXCEPTION;
    }
    *allocation = (IAllocation*)new Allocation(id, rs, t, usage);
    REFCOUNT_ADD(*allocation)
    return NOERROR;
}

ECode Allocation::CreateCubemapFromBitmap(
    /* [in] */ IRenderScript* rs,
    /* [in] */ IBitmap* b,
    /* [out] */ IAllocation** allocation)
{
    return CreateCubemapFromBitmap(rs, b, AllocationMipmapControl_MIPMAP_NONE,
            USAGE_GRAPHICS_TEXTURE, allocation);
}

ECode Allocation::CreateCubemapFromCubeFaces(
    /* [in] */ IRenderScript* rs,
    /* [in] */ IBitmap* xpos,
    /* [in] */ IBitmap* xneg,
    /* [in] */ IBitmap* ypos,
    /* [in] */ IBitmap* yneg,
    /* [in] */ IBitmap* zpos,
    /* [in] */ IBitmap* zneg,
    /* [in] */ AllocationMipmapControl mips,
    /* [in] */ Int32 usage,
    /* [out] */ IAllocation** allocation)
{
    VALIDATE_NOT_NULL(allocation)
    *allocation = NULL;
    Int32 height;
    xpos->GetHeight(&height);
    Int32 width1, width2, width3, width4, width5, width6, height1, height2, height3, height4, height5;
    if ((xpos->GetWidth(&width1), width1 != height) ||
        (xneg->GetWidth(&width2), width2 != height) || (xneg->GetHeight(&height1), height1 != height) ||
        (ypos->GetWidth(&width3), width3 != height) || (ypos->GetHeight(&height2), height2 != height) ||
        (yneg->GetWidth(&width4), width4 != height) || (yneg->GetHeight(&height3), height3 != height) ||
        (zpos->GetWidth(&width5), width5 != height) || (zpos->GetHeight(&height4), height4 != height) ||
        (zneg->GetWidth(&width6), width6 != height) || (zneg->GetHeight(&height5), height5 != height)) {
        Slogger::E("Allocation", "Only square cube map faces supported");
        return E_RS_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Boolean isPow2 = (height & (height - 1)) == 0;
    if (!isPow2) {
        Slogger::E("Allocation", "Only power of 2 cube faces supported");
        return E_RS_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IElement> e;
    ElementFromBitmap(rs, xpos, (IElement**)&e);
    AutoPtr<ITypeBuilder> tb;
    CTypeBuilder::New(rs, e, (ITypeBuilder**)&tb);
    tb->SetX(height);
    tb->SetY(height);
    tb->SetFaces(TRUE);
    tb->SetMipmaps(mips == AllocationMipmapControl_MIPMAP_FULL);
    AutoPtr<IType> t;
    tb->Create((IType**)&t);
    AutoPtr<IAllocation> cubemap;
    FAIL_RETURN(CreateTyped(rs, t, mips, usage, (IAllocation**)&cubemap))

    AutoPtr<IAllocationAdapter> adapter;
    FAIL_RETURN(AllocationAdapter::Create2D(rs, cubemap, (IAllocationAdapter**)&adapter))
    adapter->SetFace(TypeCubemapFace_POSITIVE_X);
    AutoPtr<IAllocation> a = IAllocation::Probe(adapter);
    a->CopyFrom(xpos);
    adapter->SetFace(TypeCubemapFace_NEGATIVE_X);
    a->CopyFrom(xneg);
    adapter->SetFace(TypeCubemapFace_POSITIVE_Y);
    a->CopyFrom(ypos);
    adapter->SetFace(TypeCubemapFace_NEGATIVE_Y);
    a->CopyFrom(yneg);
    adapter->SetFace(TypeCubemapFace_POSITIVE_Z);
    a->CopyFrom(zpos);
    adapter->SetFace(TypeCubemapFace_NEGATIVE_Z);
    a->CopyFrom(zneg);

    *allocation = cubemap;
    REFCOUNT_ADD(*allocation)
    return NOERROR;
}

ECode Allocation::CreateCubemapFromCubeFaces(
    /* [in] */ IRenderScript* rs,
    /* [in] */ IBitmap* xpos,
    /* [in] */ IBitmap* xneg,
    /* [in] */ IBitmap* ypos,
    /* [in] */ IBitmap* yneg,
    /* [in] */ IBitmap* zpos,
    /* [in] */ IBitmap* zneg,
    /* [out] */ IAllocation** allocation)
{
    return CreateCubemapFromCubeFaces(rs, xpos, xneg, ypos, yneg,
            zpos, zneg, AllocationMipmapControl_MIPMAP_NONE,
            USAGE_GRAPHICS_TEXTURE, allocation);
}

ECode Allocation::CreateFromBitmapResource(
    /* [in] */ IRenderScript* rs,
    /* [in] */ IResources* res,
    /* [in] */ Int32 id,
    /* [in] */ AllocationMipmapControl mips,
    /* [in] */ Int32 usage,
    /* [out] */ IAllocation** allocation)
{
    VALIDATE_NOT_NULL(allocation)
    *allocation = NULL;
    FAIL_RETURN(rs->Validate())
    if ((usage & (USAGE_SHARED | USAGE_IO_INPUT | USAGE_IO_OUTPUT)) != 0) {
        Slogger::E("Allocation", "Unsupported usage specified.");
        return E_RS_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IBitmapFactory> factory;
    CBitmapFactory::AcquireSingleton((IBitmapFactory**)&factory);
    AutoPtr<IBitmap> b;
    factory->DecodeResource(res, id, (IBitmap**)&b);
    AutoPtr<IAllocation> alloc;
    FAIL_RETURN(CreateFromBitmap(rs, b, mips, usage, (IAllocation**)&alloc))
    b->Recycle();
    *allocation = alloc;
    REFCOUNT_ADD(*allocation)
    return NOERROR;
}

ECode Allocation::CreateFromBitmapResource(
    /* [in] */ IRenderScript* rs,
    /* [in] */ IResources* res,
    /* [in] */ Int32 id,
    /* [out] */ IAllocation** allocation)
{
    VALIDATE_NOT_NULL(allocation)
    AutoPtr<IContext> ctx;
    rs->GetApplicationContext((IContext**)&ctx);
    AutoPtr<IApplicationInfo> info;
    ctx->GetApplicationInfo((IApplicationInfo**)&info);
    Int32 version;
    if (info->GetTargetSdkVersion(&version), version >= 18) {
        return CreateFromBitmapResource(rs, res, id,
                AllocationMipmapControl_MIPMAP_NONE,
                USAGE_SCRIPT | USAGE_GRAPHICS_TEXTURE, allocation);
    }
    return CreateFromBitmapResource(rs, res, id,
            AllocationMipmapControl_MIPMAP_NONE,
            USAGE_GRAPHICS_TEXTURE, allocation);
}

ECode Allocation::CreateFromString(
    /* [in] */ IRenderScript* rs,
    /* [in] */ String str,
    /* [in] */ Int32 usage,
    /* [out] */ IAllocation** allocation)
{
    VALIDATE_NOT_NULL(allocation)
    *allocation = NULL;
    FAIL_RETURN(rs->Validate())
    AutoPtr<ArrayOf<Byte> > allocArray;
    // try {
    allocArray = str.GetBytes();
    AutoPtr<IAllocation> alloc;
    AutoPtr<IElement> e;
    if (FAILED(Element::U8(rs, (IElement**)&e))) {
        Slogger::E("Allocation", "Could not convert string to utf-8.");
        return E_RS_RUNTIME_EXCEPTION;
    }
    AutoPtr<IAllocation> alloc;
    FAIL_RETURN(CreateSized(rs, e, allocArray.length, usage, (IAllocation**)&alloc))
    FAIL_RETURN(alloc->CopyFrom(allocArray))
    *allocation = alloc;
    REFCOUNT_ADD(*allocation)
    return NOERROR;
    // }
    // catch (Exception e) {
    //     throw new RSRuntimeException();
    // }
}

ECode Allocation::SetOnBufferAvailableListener(
    /* [in] */ IOnBufferAvailableListener* cb)
{
    synchronized(sAllocationMapLock) {
        Int64 id;
        GetID(mRS, &id);
        sAllocationMap[id] = this;
        mBufferNotifier = callback;
    }
    return NOERROR;
}

void Allocation::SendBufferNotification(
    /* [in] */ Int64 id)
{
    synchronized(sAllocationMapLock) {
        AutoPtr<Allocation> a;
        HashMap<Int64, AutoPtr<Allocation> >::Iterator it = sAllocationMap.Find(id);
        if (it != sAllocationMap.End()) {
            a = it->mSecond;
        }

        if ((a != NULL) && (a->mBufferNotifier != NULL)) {
            a->mBufferNotifier->OnBufferAvailable(a);
        }
    }
}

} // namespace RenderScript
} // namespace Droid
} // namespace Elastos
