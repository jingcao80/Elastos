
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/telecomm/telecom/ConnectionRequest.h"
#include "elastos/droid/telecomm/telecom/Connection.h"
#include "elastos/droid/net/CUriHelper.h"
#include <elastos/core/StringUtils.h>

using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Telecomm::Telecom::Connection;
using Elastos::Core::CString;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Telecomm {
namespace Telecom {

//===============================================================
// ConnectionRequest::
//===============================================================
CAR_INTERFACE_IMPL_2(ConnectionRequest, Object, IConnectionRequest, IParcelable)

ECode ConnectionRequest::constructor()
{
    return NOERROR;
}

ECode ConnectionRequest::constructor(
    /* [in] */ IPhoneAccountHandle* accountHandle,
    /* [in] */ IUri* handle,
    /* [in] */ IBundle* extras)
{
    return constructor(accountHandle, handle, extras, IVideoProfileVideoState::AUDIO_ONLY);
}

ECode ConnectionRequest::constructor(
    /* [in] */ IPhoneAccountHandle* accountHandle,
    /* [in] */ IUri* handle,
    /* [in] */ IBundle* extras,
    /* [in] */ Int32 videoState)
{
    mAccountHandle = accountHandle;
    mAddress = handle;
    mExtras = extras;
    mVideoState = videoState;
    return NOERROR;
}

ECode ConnectionRequest::GetAccountHandle(
    /* [out] */ IPhoneAccountHandle** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mAccountHandle;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode ConnectionRequest::GetAddress(
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mAddress;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode ConnectionRequest::GetExtras(
    /* [out] */ IBundle** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mExtras;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode ConnectionRequest::GetVideoState(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mVideoState;
    return NOERROR;
}

ECode ConnectionRequest::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    String content1;
    if (mAddress == NULL) {
        AutoPtr<IUriHelper> hlp;
        CUriHelper::AcquireSingleton((IUriHelper**)&hlp);
        AutoPtr<IUri> emp;
        hlp->GetEMPTY((IUri**)&emp);
        IObject::Probe(emp)->ToString(&content1);
    }
    else {
        String str;
        IObject::Probe(mAddress)->ToString(&str);
        String phoneNumber;
        Connection::ToLogSafePhoneNumber(str, &phoneNumber);
        content1 = phoneNumber;
    }

    String content2;
    if (mExtras == NULL) {
        content2 = String("");
    }
    else {
        IObject::Probe(mExtras)->ToString(&content2);
    }

    String res;
    res.AppendFormat("ConnectionRequest %p %p", content1.string(), content2.string());
    *result = res;
    return NOERROR;
}

ECode ConnectionRequest::WriteToParcel(
    /* [in] */ IParcel* destination)
{
    destination->WriteInterfacePtr(mAccountHandle);
    destination->WriteInterfacePtr(mAddress);
    destination->WriteInterfacePtr(mExtras);
    destination->WriteInt32(mVideoState);
    return NOERROR;
}

ECode ConnectionRequest::ReadFromParcel(
    /* [in] */ IParcel* in)
{
    AutoPtr<IInterface> accountHandle;
    in->ReadInterfacePtr((Handle32*)&accountHandle);
    mAccountHandle = IPhoneAccountHandle::Probe(accountHandle);

    AutoPtr<IInterface> address;
    in->ReadInterfacePtr((Handle32*)&address);
    mAddress = IUri::Probe(address);

    AutoPtr<IInterface> extras;
    in->ReadInterfacePtr((Handle32*)&extras);
    mExtras = IBundle::Probe(extras);

    in->ReadInt32(&mVideoState);
    return NOERROR;
}

} // namespace Telecom
} // namespace Telecomm
} // namespace Droid
} // namespace Elastos
