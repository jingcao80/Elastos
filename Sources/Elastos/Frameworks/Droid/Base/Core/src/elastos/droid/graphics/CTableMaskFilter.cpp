
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/graphics/CTableMaskFilter.h"
#include <skia/effects/SkTableMaskFilter.h>

namespace Elastos {
namespace Droid {
namespace Graphics {

CAR_OBJECT_IMPL(CTableMaskFilter);
ECode CTableMaskFilter::constructor(
    /* [in] */ const ArrayOf<Byte>& table)
{
    if (table.GetLength() < 256) {
        // throw new RuntimeException("table.length must be >= 256");
        return E_RUNTIME_EXCEPTION;
    }
    mNativeInstance = NativeNewTable(table);
    return NOERROR;
}

ECode CTableMaskFilter::constructor(
    /* [in] */ Int64 ni)
{
    mNativeInstance = ni;
    return NOERROR;
}

PInterface CTableMaskFilter::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_MaskFilter) {
        return reinterpret_cast<PInterface>((MaskFilter*)this);
    }
    else if (riid == EIID_ITableMaskFilter) {
        return (ITableMaskFilter*)this;
    }
    return MaskFilter::Probe(riid);
}

UInt32 CTableMaskFilter::AddRef()
{
    return MaskFilter::AddRef();
}

UInt32 CTableMaskFilter::Release()
{
    return MaskFilter::Release();
}

ECode CTableMaskFilter::GetInterfaceID(
    /* [in] */ IInterface* object,
    /* [out] */ InterfaceID* iid)
{
    return MaskFilter::GetInterfaceID(object, iid);
}

ECode CTableMaskFilter::CreateClipTable(
    /* [in] */ Int32 min,
    /* [in] */ Int32 max,
    /* [out] */ ITableMaskFilter** tf)
{
    VALIDATE_NOT_NULL(tf);

    return CTableMaskFilter::New(NativeNewClip(min, max), tf);
}

ECode CTableMaskFilter::CreateGammaTable(
    /* [in] */ Float gamma,
    /* [out] */ ITableMaskFilter** tf)
{
    VALIDATE_NOT_NULL(tf);

    return CTableMaskFilter::New(NativeNewGamma(gamma), tf);
}

Int64 CTableMaskFilter::NativeNewTable(
    /* [in] */ const ArrayOf<Byte>& table)
{
    // AutoJavaByteArray autoTable(env, jtable, 256);
    assert(table.GetLength() >= 256);
    SkMaskFilter* filter = SkTableMaskFilter::Create((const uint8_t*)table.GetPayload());
    return reinterpret_cast<Int64>(filter);
}

Int64 CTableMaskFilter::NativeNewClip(
    /* [in] */ Int32 min,
    /* [in] */ Int32 max)
{
    SkMaskFilter* filter = SkTableMaskFilter::CreateClip(min, max);
    return reinterpret_cast<Int64>(filter);
}

Int64 CTableMaskFilter::NativeNewGamma(
    /* [in] */ Float gamma)
{
    SkMaskFilter* filter = SkTableMaskFilter::CreateGamma(gamma);
    return reinterpret_cast<Int64>(filter);
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
