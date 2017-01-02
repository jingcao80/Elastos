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

#include "elastos/droid/codeaurora/ims/csvt/CallForwardInfoP.h"

namespace Elastos {
namespace Droid {
namespace CodeAurora {
namespace Ims {
namespace Csvt {

CAR_INTERFACE_IMPL_2(CallForwardInfoP, Object, ICallForwardInfoP, IParcelable);

CallForwardInfoP::CallForwardInfoP()
    : status(0)
    , reason(0)
    , serviceClass(0)
    , toa(0)
    , timeSeconds(0)
{
}

ECode CallForwardInfoP::constructor()
{
    return NOERROR;
}

ECode CallForwardInfoP::GetStatus(
    /* [out] */ Int32* _status)
{
    VALIDATE_NOT_NULL(_status);
    *_status = status;
    return NOERROR;
}

ECode CallForwardInfoP::SetStatus(
    /* [in] */ Int32 _status)
{
    status = _status;
    return NOERROR;
}

ECode CallForwardInfoP::GetReason(
    /* [out] */ Int32* _reason)
{
    VALIDATE_NOT_NULL(_reason);
    *_reason = reason;
    return NOERROR;
}

ECode CallForwardInfoP::SetReason(
    /* [in] */ Int32 _reason)
{
    reason = _reason;
    return NOERROR;
}

ECode CallForwardInfoP::GetServiceClass(
    /* [out] */ Int32* _serviceClass)
{
    VALIDATE_NOT_NULL(_serviceClass);
    *_serviceClass = serviceClass;
    return NOERROR;
}

ECode CallForwardInfoP::SetServiceClass(
    /* [in] */ Int32 _serviceClass)
{
    serviceClass = _serviceClass;
    return NOERROR;
}

ECode CallForwardInfoP::GetToa(
    /* [out] */ Int32* _toa)
{
   VALIDATE_NOT_NULL(_toa);
   *_toa = toa;
   return NOERROR;
}

ECode CallForwardInfoP::SetToa(
    /* [in] */ Int32 _toa)
{
    toa = _toa;
    return NOERROR;
}

ECode CallForwardInfoP::GetNumber(
    /* [out] */ String* _number)
{
    VALIDATE_NOT_NULL(_number);
    *_number = number;
    return NOERROR;
}

ECode CallForwardInfoP::SetNumber(
    /* [in] */ const String& _number)
{
    number = _number;
    return NOERROR;
}

ECode CallForwardInfoP::GetTimeSeconds(
    /* [out] */ Int32* _timeSeconds)
{
    VALIDATE_NOT_NULL(_timeSeconds);
    *_timeSeconds = timeSeconds;
    return NOERROR;
}

ECode CallForwardInfoP::SetTimeSeconds(
    /* [in] */ Int32 _timeSeconds)
{
    timeSeconds = _timeSeconds;
    return NOERROR;
}

ECode CallForwardInfoP::ReadFromParcel(
  /* [in] */ IParcel* parcel)
{
    parcel->ReadInt32(&status);
    parcel->ReadInt32(&reason);
    parcel->ReadInt32(&toa);
    parcel->ReadString(&number);
    parcel->ReadInt32(&timeSeconds);
    parcel->ReadInt32(&serviceClass);
    return NOERROR;
}

ECode CallForwardInfoP::WriteToParcel(
  /* [in] */ IParcel* dest)
{
    dest->WriteInt32(status);
    dest->WriteInt32(reason);
    dest->WriteInt32(toa);
    dest->WriteString(number);
    dest->WriteInt32(timeSeconds);
    dest->WriteInt32(serviceClass);
    return NOERROR;
}

} // namespace Csvt
} // namespace Ims
} // namespace Codeaurora
} // namespace Droid
} // namespace Elastos

