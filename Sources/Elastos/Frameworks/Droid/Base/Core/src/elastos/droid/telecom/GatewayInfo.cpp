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
#include "elastos/droid/telecom/GatewayInfo.h"
#include "elastos/droid/text/TextUtils.h"
//#include "elastos/droid/net/CStringUri.h"
//#include "elastos/droid/net/COpaqueUri.h"
//#include "elastos/droid/net/CHierarchicalUri.h"
//#include <elastos/utility/logging/Logger.h>

//using Elastos::Droid::Net::CStringUri;
//using Elastos::Droid::Net::COpaqueUri;
//using Elastos::Droid::Net::CHierarchicalUri;
using Elastos::Droid::Text::TextUtils;
//using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Telecom {

//===============================================================
// GatewayInfo::
//===============================================================
CAR_INTERFACE_IMPL_2(GatewayInfo, Object, IGatewayInfo, IParcelable)

ECode GatewayInfo::constructor()
{
    return NOERROR;
}

ECode GatewayInfo::constructor(
    /* [in] */ const String& packageName,
    /* [in] */ IUri* gatewayUri,
    /* [in] */ IUri* originalAddress)
{
    mGatewayProviderPackageName = packageName;
    mGatewayAddress = gatewayUri;
    mOriginalAddress = originalAddress;
    return NOERROR;
}

ECode GatewayInfo::GetGatewayProviderPackageName(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mGatewayProviderPackageName;
    return NOERROR;
}

ECode GatewayInfo::GetGatewayAddress(
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mGatewayAddress;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode GatewayInfo::GetOriginalAddress(
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mOriginalAddress;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode GatewayInfo::IsEmpty(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = TextUtils::IsEmpty(mGatewayProviderPackageName) || mGatewayAddress == NULL;
    return NOERROR;
}

ECode GatewayInfo::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadString(&mGatewayProviderPackageName);
    Int32 value = 0;
    source->ReadInt32(&value);

    if (value != 0) {
        mGatewayAddress = NULL;
        //Int32 id = 0;
        //source->ReadInt32(&id);
        //if (id == 1) {
        //    CStringUri::New((IUri**)&mGatewayAddress);
        //    IParcelable::Probe(mGatewayAddress)->ReadFromParcel(source);
        //}
        //else if (id == 2) {
        //    COpaqueUri::New((IUri**)&mGatewayAddress);
        //    IParcelable::Probe(mGatewayAddress)->ReadFromParcel(source);
        //}
        //else if (id == 3) {
        //    CHierarchicalUri::New((IUri**)&mGatewayAddress);
        //    IParcelable::Probe(mGatewayAddress)->ReadFromParcel(source);
        //}
        //else {
        //    Logger::E("GatewayInfo", "Wrong URI id:%d, only 1, 2, 3 accepted", id);
        //}
        AutoPtr<IInterface> obj;
        source->ReadInterfacePtr((Handle32*)&obj);
        mGatewayAddress = IUri::Probe(obj);
    }
    else {
        mGatewayAddress = NULL;
    }

    source->ReadInt32(&value);
    if (value != 0) {
        mOriginalAddress = NULL;
        //Int32 id = 0;
        //source->ReadInt32(&id);
        //if (id == 1) {
        //    CStringUri::New((IUri**)&mOriginalAddress);
        //    IParcelable::Probe(mOriginalAddress)->ReadFromParcel(source);
        //}
        //else if (id == 2) {
        //    COpaqueUri::New((IUri**)&mOriginalAddress);
        //    IParcelable::Probe(mOriginalAddress)->ReadFromParcel(source);
        //}
        //else if (id == 3) {
        //    CHierarchicalUri::New((IUri**)&mOriginalAddress);
        //    IParcelable::Probe(mOriginalAddress)->ReadFromParcel(source);
        //}
        //else {
        //    Logger::E("GatewayInfo", "Wrong URI id:%d, only 1, 2, 3 accepted", id);
        //}
        AutoPtr<IInterface> obj;
        source->ReadInterfacePtr((Handle32*)&obj);
        mOriginalAddress = IUri::Probe(obj);
    }
    else {
        mOriginalAddress = NULL;
    }
    return NOERROR;
}

ECode GatewayInfo::WriteToParcel(
    /* [in] */ IParcel* destination)
{
    destination->WriteString(mGatewayProviderPackageName);
    if (mGatewayAddress != NULL) {
        destination->WriteInt32(1);
        //IParcelable::Probe(mGatewayAddress)->WriteToParcel(destination);
        destination->WriteInterfacePtr(mGatewayAddress);//TODO should update
    }
    else {
        destination->WriteInt32(0);
    }

    if (mOriginalAddress != NULL) {
        destination->WriteInt32(1);
        //IParcelable::Probe(mOriginalAddress)->WriteToParcel(destination);
        destination->WriteInterfacePtr(mOriginalAddress);//TODO should update
    }
    else {
        destination->WriteInt32(0);
    }
    return NOERROR;
}

} // namespace Telecom
} // namespace Droid
} // namespace Elastos
