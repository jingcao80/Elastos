
#include "CKeyStorePasswordProtection.h"

namespace Elastos {
namespace Security {

CAR_OBJECT_IMPL(CKeyStorePasswordProtection)
CAR_INTERFACE_IMPL_3(CKeyStorePasswordProtection, Object, IKeyStorePasswordProtection, IKeyStoreProtectionParameter, IDestroyable)
CKeyStorePasswordProtection::CKeyStorePasswordProtection()
    : mIsDestroyed(FALSE)
{}

ECode CKeyStorePasswordProtection::GetPassword(
    /* [out, callee] */ ArrayOf<Char32> **pwd)
{
    AutoLock lock(mLock);
    VALIDATE_NOT_NULL(pwd)
    if (mIsDestroyed) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    *pwd = mPassword;
    REFCOUNT_ADD(*pwd)
    return NOERROR;
}

ECode CKeyStorePasswordProtection::Destroy()
{
    AutoLock lock(mLock);
    if (mPassword) {
        mPassword = NULL;
    }
    return NOERROR;
}

ECode CKeyStorePasswordProtection::IsDestroyed(
    /* [out] */ Boolean *isDestroyed)
{
    AutoLock lock(mLock);
    VALIDATE_NOT_NULL(isDestroyed)
    *isDestroyed = mIsDestroyed;
    return NOERROR;
}

ECode CKeyStorePasswordProtection::constructor(
    /* [in] */ ArrayOf<Char32> *password)
{
    if (password){
        mPassword = password->Clone();
    }
    return NOERROR;
}

}
}
