
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/graphics/CAvoidXfermode.h"
#include <SkAvoidXfermode.h>
#include <SkPixelXorXfermode.h>

namespace Elastos {
namespace Droid {
namespace Graphics {

CAR_OBJECT_IMPL(CAvoidXfermode);
ECode CAvoidXfermode::constructor(
    /* [in] */ Int32 opColor,
    /* [in] */ Int32 tolerance,
    /* [in] */ Int32 mode)
{
    if (tolerance < 0 || tolerance > 255) {
        // throw new IllegalArgumentException("tolerance must be 0..255");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mNativeInstance = NativeCreate(opColor, tolerance, mode);
    return NOERROR;
}

UInt32 CAvoidXfermode::AddRef()
{
    return Xfermode::AddRef();
}

UInt32 CAvoidXfermode::Release()
{
    return Xfermode::Release();
}

ECode CAvoidXfermode::GetInterfaceID(
    /* [in] */ IInterface* object,
    /* [out] */ InterfaceID* iid)
{
    return Xfermode::GetInterfaceID(object, iid);
}

PInterface CAvoidXfermode::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_Xfermode) {
        return reinterpret_cast<PInterface>((Xfermode*)this);
    }
    else if (riid == EIID_IAvoidXfermode) {
        return (IAvoidXfermode*)this;
    }
    return Xfermode::Probe(riid);
}

Int64 CAvoidXfermode::NativeCreate(
    /* [in] */ Int32 opColor,
    /* [in] */ Int32 tolerance,
    /* [in] */ Int32 nativeMode)
{
    SkAvoidXfermode::Mode mode = static_cast<SkAvoidXfermode::Mode>(nativeMode);
    return reinterpret_cast<Int64>(SkAvoidXfermode::Create(opColor, tolerance, mode));
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
