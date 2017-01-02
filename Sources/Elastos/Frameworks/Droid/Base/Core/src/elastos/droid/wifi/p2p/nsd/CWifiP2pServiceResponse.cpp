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

#include "elastos/droid/wifi/p2p/nsd/CWifiP2pServiceResponse.h"
#include "elastos/droid/wifi/p2p/nsd/CWifiP2pDnsSdServiceResponse.h"
#include "elastos/droid/wifi/p2p/nsd/CWifiP2pUpnpServiceResponse.h"
#include "elastos/droid/wifi/p2p/CWifiP2pDevice.h"
#include <elastos/core/StringUtils.h>
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/utility/etl/List.h>

using Elastos::Utility::Etl::List;
using Elastos::Core::StringUtils;
using Elastos::IO::IByteArrayInputStream;
using Elastos::IO::CByteArrayInputStream;
using Elastos::IO::IDataInputStream;
using Elastos::IO::CDataInputStream;
using Elastos::IO::IDataInput;

namespace Elastos {
namespace Droid {
namespace Wifi {
namespace P2p {
namespace Nsd {

CAR_OBJECT_IMPL(CWifiP2pServiceResponse)

ECode CWifiP2pServiceResponse::GetServiceType(
    /* [out] */ Int32* serviceType)
{
    return WifiP2pServiceResponse::GetServiceType(serviceType);
}

ECode CWifiP2pServiceResponse::GetStatus(
    /* [out] */ Int32* status)
{
    return WifiP2pServiceResponse::GetStatus(status);
}

ECode CWifiP2pServiceResponse::GetTransactionId(
    /* [out] */ Int32* transactionId)
{
    return WifiP2pServiceResponse::GetTransactionId(transactionId);
}

ECode CWifiP2pServiceResponse::GetRawData(
    /* [out, callee] */ ArrayOf<Byte>** rawData)
{
    return WifiP2pServiceResponse::GetRawData(rawData);
}

ECode CWifiP2pServiceResponse::GetSrcDevice(
    /* [out] */ IWifiP2pDevice** srcDevice)
{
    return WifiP2pServiceResponse::GetSrcDevice(srcDevice);
}

ECode CWifiP2pServiceResponse::SetSrcDevice(
    /* [in] */ IWifiP2pDevice* dev)
{
    return WifiP2pServiceResponse::SetSrcDevice(dev);
}

ECode CWifiP2pServiceResponse::ToString(
    /* [out] */ String* string)
{
    return WifiP2pServiceResponse::ToString(string);
}

ECode CWifiP2pServiceResponse::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* isEqual)
{
    return WifiP2pServiceResponse::Equals(obj, isEqual);
}

ECode CWifiP2pServiceResponse::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    return WifiP2pServiceResponse::GetHashCode(hashCode);
}

ECode CWifiP2pServiceResponse::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    return WifiP2pServiceResponse::ReadFromParcel(source);
}

ECode CWifiP2pServiceResponse::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    return WifiP2pServiceResponse::WriteToParcel(dest);
}

ECode CWifiP2pServiceResponse::NewInstance(
    /* [in] */ const String& supplicantEvent,
    /* [out, callee] */ ArrayOf<IWifiP2pServiceResponse*>** list)
{
    VALIDATE_NOT_NULL(list);
    *list = NULL;

    List<AutoPtr<IWifiP2pServiceResponse> > respList;

    AutoPtr<ArrayOf<String> > args;
    StringUtils::Split(supplicantEvent, String(" "), (ArrayOf<String>**)&args);
    if ((args == NULL) || (args->GetLength() != 4)) {
        return NOERROR;
    }

    AutoPtr<IWifiP2pDevice> dev;
    FAIL_RETURN(CWifiP2pDevice::New((IWifiP2pDevice**)&dev));

    String srcAddr;
    srcAddr = (*args)[1];
    FAIL_RETURN(dev->SetDeviceAddress(srcAddr));
    //String updateIndicator = args[2];//not used.
    AutoPtr<ArrayOf<Byte> > bin;
    FAIL_RETURN(WifiP2pServiceResponse::HexStr2Bin((*args)[3], (ArrayOf<Byte>**)&bin));
    if (bin == NULL) {
        return NOERROR;
    }

    AutoPtr<IByteArrayInputStream> ins;
    FAIL_RETURN(CByteArrayInputStream::New(bin, (IByteArrayInputStream**)&ins));
    AutoPtr<IDataInputStream> dis;
    assert(0);
    // TODO
    // FAIL_RETURN(CDataInputStream::New(ins, (IDataInputStream**)&dis));
    AutoPtr<IDataInput> idi = IDataInput::Probe(dis);
    if (idi == NULL) return E_NO_INTERFACE;

    Int32 i, type, status;
    Int64 temp, length;
    List<AutoPtr<IWifiP2pServiceResponse> >::Iterator it;
    AutoPtr<ArrayOf<IWifiP2pServiceResponse*> > ret;

    Int32 temp1, temp2, transId;
    // try {
    Int32 number;
    while (TRUE) {
        assert(0);
        // TODO
        // FAIL_GOTO(dis->Available(&number), L_ERR_EXIT);
        if (number <= 0) break;

        /*
         * Service discovery header is as follows.
         * ______________________________________________________________
         * |           Length(2byte)     | Type(1byte) | TransId(1byte)}|
         * ______________________________________________________________
         * | status(1byte)  |            vendor specific(variable)      |
         */
        // The length equals to 3 plus the number of octets in the vendor
        // specific content field. And this is little endian.

        FAIL_GOTO(idi->ReadUnsignedByte(&temp1), L_ERR_EXIT);
        FAIL_GOTO(idi->ReadUnsignedByte(&temp2), L_ERR_EXIT);

        length = (temp1 + (temp2 << 8)) - 3;
        if (length < 0) {
            return NOERROR;
        }

        FAIL_GOTO(idi->ReadUnsignedByte(&type), L_ERR_EXIT);
        FAIL_GOTO(idi->ReadUnsignedByte(&transId), L_ERR_EXIT);
        FAIL_GOTO(idi->ReadUnsignedByte(&status), L_ERR_EXIT);

        if (length == 0) {
            if (status == IWifiP2pServiceResponseStatus::SUCCESS) {
                AutoPtr<IWifiP2pServiceResponse> resp;
                FAIL_GOTO(CWifiP2pServiceResponse::New(
                    type, status, (Int32)transId, dev, NULL, (IWifiP2pServiceResponse**)&resp), L_ERR_EXIT);
                respList.PushBack(resp);
            }
            continue;
        }
        if (length > MAX_BUF_SIZE) {
            assert(0);
            // TODO
            // FAIL_GOTO(dis->Skip(length, &temp), L_ERR_EXIT);
            continue;
        }

        AutoPtr<ArrayOf<Byte> > data = ArrayOf<Byte>::Alloc(length);
        if (data == NULL) goto L_ERR_EXIT;
        FAIL_GOTO(idi->ReadFully((ArrayOf<Byte>*)data), L_ERR_EXIT);

        AutoPtr<IWifiP2pServiceResponse> resp;
        if (type == IWifiP2pServiceInfo::SERVICE_TYPE_BONJOUR) {
            FAIL_GOTO(CWifiP2pDnsSdServiceResponse::New(
                status, (Int32)transId, dev, data, (IWifiP2pServiceResponse**)&resp), L_ERR_EXIT);
        }
        else if (type == IWifiP2pServiceInfo::SERVICE_TYPE_UPNP) {
            FAIL_GOTO(CWifiP2pUpnpServiceResponse::New(
                status, (Int32)transId, dev, data, (IWifiP2pServiceResponse**)&resp), L_ERR_EXIT);
        }
        else {
            AutoPtr<CWifiP2pServiceResponse> rsp;
            FAIL_GOTO(CWifiP2pServiceResponse::New(
                type, status, (Int32)transId, dev, data, (IWifiP2pServiceResponse**)&resp), L_ERR_EXIT);
        }

        if (resp != NULL) {
            FAIL_GOTO(resp->GetStatus(&status), L_ERR_EXIT);
            if (status == IWifiP2pServiceResponseStatus::SUCCESS) {
                respList.PushBack(resp);
            }
        }
    }

L_ERR_EXIT:

    ret = ArrayOf<IWifiP2pServiceResponse*>::Alloc(respList.GetSize());
    if (ret == NULL) goto L_ERR_EXIT;

    i = 0;
    for (it = respList.Begin(); it != respList.End(); ++it) {
        AutoPtr<IWifiP2pServiceResponse> resp = *it;
        ret->Set(i++, resp);
    }

    *list = ret;
    REFCOUNT_ADD(*list);
    return NOERROR;
}

} // namespace Nsd
} // namespace P2p
} // namespace Wifi
} // namespace Droid
} // namespace Elastos
