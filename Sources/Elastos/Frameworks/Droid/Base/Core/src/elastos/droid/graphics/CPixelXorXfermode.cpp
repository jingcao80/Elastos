
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/graphics/CPixelXorXfermode.h"
#include <skia/effects/SkPixelXorXfermode.h>

namespace Elastos {
namespace Droid {
namespace Graphics {

CAR_OBJECT_IMPL(CPixelXorXfermode);
ECode CPixelXorXfermode::constructor(
    /* [in] */ Int32 opColor)
{
    mNativeInstance = NativeCreate(opColor);
    return NOERROR;
}

PInterface CPixelXorXfermode::Probe(
    /* [in]  */ REIID riid)
{
    if (riid == EIID_Xfermode) {
        return reinterpret_cast<PInterface>((Xfermode*)this);
    }
    else if (riid == EIID_IPixelXorXfermode) {
        return (IPixelXorXfermode*)this;
    }
    return Xfermode::Probe(riid);
}

UInt32 CPixelXorXfermode::AddRef()
{
    return Xfermode::AddRef();
}

UInt32 CPixelXorXfermode::Release()
{
    return Xfermode::Release();
}

ECode CPixelXorXfermode::GetInterfaceID(
    /* [in] */ IInterface* object,
    /* [out] */ InterfaceID* iid)
{
    return Xfermode::GetInterfaceID(object, iid);
}

Int64 CPixelXorXfermode::NativeCreate(
    /* [in] */ Int32 opColor)
{
    return reinterpret_cast<Int64>(SkPixelXorXfermode::Create(opColor));
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
