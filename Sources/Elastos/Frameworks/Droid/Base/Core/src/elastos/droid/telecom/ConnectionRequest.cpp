//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/telecom/ConnectionRequest.h"
#include "elastos/droid/telecom/Connection.h"
#include "elastos/droid/telecom/CPhoneAccountHandle.h"
#include "elastos/droid/net/CUriHelper.h"
#include "elastos/droid/os/CBundle.h"
#include <elastos/core/StringUtils.h>

using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Os::CBundle;

using Elastos::Droid::Telecom::Connection;
using Elastos::Core::CString;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
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
    if (mAccountHandle != NULL) {
        destination->WriteInt32(1);
        //IParcelable::Probe(mAccountHandle)->WriteToParcel(destination);
        destination->WriteInterfacePtr(mAccountHandle);
    }
    else {
        destination->WriteInt32(0);
    }

    if (mAddress != NULL) {
        destination->WriteInt32(1);
        //IParcelable::Probe(mAddress)->WriteToParcel(destination);
        destination->WriteInterfacePtr(mAddress);
    }
    else {
        destination->WriteInt32(0);
    }
    if (mExtras != NULL) {
        destination->WriteInt32(1);
        //IParcelable::Probe(mExtras)->WriteToParcel(destination);
        destination->WriteInterfacePtr(mExtras);
    }
    else {
        destination->WriteInt32(0);
    }
    destination->WriteInt32(mVideoState);
    return NOERROR;
}

ECode ConnectionRequest::ReadFromParcel(
    /* [in] */ IParcel* in)
{
    Int32 value = 0;
    in->ReadInt32(&value);
    if (value != 0) {
        //AutoPtr<IPhoneAccountHandle> pa;
        //CPhoneAccountHandle::New((IPhoneAccountHandle**)&pa);
        //IParcelable* parcel = IParcelable::Probe(pa);
        //parcel->ReadFromParcel(in);
        //mAccountHandle = pa;
        AutoPtr<IInterface> accountHandle;
        in->ReadInterfacePtr((HANDLE*)&accountHandle);
        mAccountHandle = IPhoneAccountHandle::Probe(accountHandle);
    }
    else {
        mAccountHandle = NULL;
    }

    in->ReadInt32(&value);
    if (value != 0) {
        //AutoPtr<IUri> address;
        //AutoPtr<IUriHelper> helper;
        //CUriHelper::AcquireSingleton((IUriHelper**)&helper);
        //helper->GetEMPTY((IUri**)&address);
        //IParcelable* parcel = IParcelable::Probe(address);
        //parcel->ReadFromParcel(in);
        //mAddress= address;
        AutoPtr<IInterface> address;
        in->ReadInterfacePtr((HANDLE*)&address);
        mAddress = IUri::Probe(address);
    }
    else {
        mAddress = NULL;
    }

    in->ReadInt32(&value);
    if (value != 0) {
        //AutoPtr<IBundle> bundle;
        //CBundle::New((IBundle**)&bundle);
        //IParcelable* parcel = IParcelable::Probe(bundle);
        //parcel->ReadFromParcel(in);
        //mExtras = bundle;
        AutoPtr<IInterface> extras;
        in->ReadInterfacePtr((HANDLE*)&extras);
        mExtras = IBundle::Probe(extras);
    }
    else {
        mExtras = NULL;
    }


    in->ReadInt32(&mVideoState);
    return NOERROR;
}

} // namespace Telecom
} // namespace Droid
} // namespace Elastos
