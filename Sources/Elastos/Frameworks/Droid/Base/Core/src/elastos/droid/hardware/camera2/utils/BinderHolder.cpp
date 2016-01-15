
#include "Elastos.Droid.Os.h"
#include "elastos/droid/hardware/camera2/utils/BinderHolder.h"
#include <elastos/core/StringBuffer.h>

using Elastos::Core::StringBuffer;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Utils {

CAR_INTERFACE_IMPL_2(BinderHolder, Object, IBinderHolder, IParcelable)

ECode BinderHolder::constructor()
{
    return NOERROR;
}

ECode BinderHolder::constructor(
    /* [in] */ IBinder* binder)
{
    mBinder = binder;
    return NOERROR;
}

ECode BinderHolder::constructor(
    /* [in] */ IParcel* _in)
{
    assert(0);
    //return _in->readStrongBinder((IBinder**)&mBinder);
    return NOERROR;
}

ECode BinderHolder::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    assert(0);
    //return dest->WriteStrongBinder(mBinder);
    return NOERROR;
}

ECode BinderHolder::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    assert(0);
    //return source->ReadStrongBinder((IBinder**)&mBinder);
    return NOERROR;
}

ECode BinderHolder::GetBinder(
    /* [out] */ IBinder** binder)
{
    VALIDATE_NOT_NULL(binder);

    *binder = mBinder;
    REFCOUNT_ADD(*binder);
    return NOERROR;
}

ECode BinderHolder::SetBinder(
    /* [in] */ IBinder* binder)
{
    mBinder = binder;
    return NOERROR;
}

} // namespace Utils
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
