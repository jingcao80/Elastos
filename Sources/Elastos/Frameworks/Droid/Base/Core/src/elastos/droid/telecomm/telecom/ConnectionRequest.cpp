
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Os.h"
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
    assert(0 && "TODO");
    // return constructor(accountHandle, handle, extras, VideoProfile.VideoState.AUDIO_ONLY);
    return NOERROR;
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

    AutoPtr<IInterface> content1;
    if (mAddress == NULL) {
        AutoPtr<IUriHelper> hlp;
        CUriHelper::AcquireSingleton((IUriHelper**)&hlp);
        AutoPtr<IUri> emp;
        hlp->GetEMPTY((IUri**)&emp);
        content1 = IInterface::Probe(emp);
    }
    else {
        String str;
        // mAddress->ToString(&str);
        String phoneNumber;
        Connection::ToLogSafePhoneNumber(str, &phoneNumber);
        AutoPtr<ICharSequence> pStr;
        CString::New(phoneNumber, (ICharSequence**)&pStr);
        content1 = pStr;
    }

    AutoPtr<IInterface> content2;
    if (mExtras == NULL) {
        AutoPtr<ICharSequence> pStr;
        CString::New(String(""), (ICharSequence**)&pStr);
        content2 = pStr;
    }

    AutoPtr<ArrayOf<IInterface*> > arr = ArrayOf<IInterface*>::Alloc(2);
    (*arr)[0] = content1;
    (*arr)[1] = content2;
    String res = StringUtils::Format(String("ConnectionRequest %p %p"), arr);
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