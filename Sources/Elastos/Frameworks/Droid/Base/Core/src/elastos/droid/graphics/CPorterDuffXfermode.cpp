
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/graphics/CPorterDuffXfermode.h"
#include <skia/effects/SkPorterDuff.h>

namespace Elastos {
namespace Droid {
namespace Graphics {

CAR_OBJECT_IMPL(CPorterDuffXfermode);
ECode CPorterDuffXfermode::constructor(
    /* [in] */ PorterDuffMode mode)
{
    this->mMode = mode;
    mNativeInstance = NativeCreateXfermode(mode);
    return NOERROR;
}

PInterface CPorterDuffXfermode::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_Xfermode) {
        return reinterpret_cast<PInterface>((Xfermode*)this);
    }
    else if (riid == EIID_IPorterDuffXfermode) {
        return (IPorterDuffXfermode*)this;
    }
    return Xfermode::Probe(riid);
}

UInt32 CPorterDuffXfermode::AddRef()
{
    return Xfermode::AddRef();
}

UInt32 CPorterDuffXfermode::Release()
{
    return Xfermode::Release();
}

ECode CPorterDuffXfermode::GetInterfaceID(
    /* [in] */ IInterface* object,
    /* [out] */ InterfaceID* iid)
{
    return Xfermode::GetInterfaceID(object, iid);
}

ECode CPorterDuffXfermode::GetMode(
    /* [out] */ PorterDuffMode* mode)
{
    VALIDATE_NOT_NULL(mode);
    *mode = mMode;
    return NOERROR;
}

Int64 CPorterDuffXfermode::NativeCreateXfermode(
    /* [in] */ PorterDuffMode modeHandle)
{
    SkPorterDuff::Mode mode = static_cast<SkPorterDuff::Mode>(modeHandle);
    return reinterpret_cast<Int64>(SkPorterDuff::CreateXfermode(mode));
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
