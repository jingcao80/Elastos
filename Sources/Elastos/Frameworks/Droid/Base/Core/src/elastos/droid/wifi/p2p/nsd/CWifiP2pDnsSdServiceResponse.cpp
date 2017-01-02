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

#include "elastos/droid/wifi/p2p/nsd/CWifiP2pDnsSdServiceResponse.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::IO::IInputStream;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Wifi {
namespace P2p {
namespace Nsd {

static CWifiP2pDnsSdServiceResponse::CStatic sStatic;

CAR_OBJECT_IMPL(CWifiP2pDnsSdServiceResponse)

ECode CWifiP2pDnsSdServiceResponse::constructor()
{
    return NOERROR;
}

ECode CWifiP2pDnsSdServiceResponse::constructor(
    /* [in] */ Int32 status,
    /* [in] */ Int32 transId,
    /* [in] */ IWifiP2pDevice* device,
    /* [in] */ ArrayOf<Byte>* data)
{
    FAIL_RETURN(WifiP2pServiceResponse::constructor(
        IWifiP2pServiceInfo::SERVICE_TYPE_BONJOUR, status, transId, device, data));

    Boolean ret;
    FAIL_RETURN(Parse(&ret));
    if (!ret) {
        Slogger::E("CWifiP2pDnsSdServiceResponse", "Malformed upnp service response.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    return NOERROR;
}

ECode CWifiP2pDnsSdServiceResponse::GetServiceType(
    /* [out] */ Int32* serviceType)
{
    return WifiP2pServiceResponse::GetServiceType(serviceType);
}

ECode CWifiP2pDnsSdServiceResponse::GetStatus(
    /* [out] */ Int32* status)
{
    return WifiP2pServiceResponse::GetStatus(status);
}

ECode CWifiP2pDnsSdServiceResponse::GetTransactionId(
    /* [out] */ Int32* transactionId)
{
    return WifiP2pServiceResponse::GetTransactionId(transactionId);
}

ECode CWifiP2pDnsSdServiceResponse::GetRawData(
    /* [out, callee] */ ArrayOf<Byte>** rawData)
{
    return WifiP2pServiceResponse::GetRawData(rawData);
}

ECode CWifiP2pDnsSdServiceResponse::GetSrcDevice(
    /* [out] */ IWifiP2pDevice** srcDevice)
{
    return WifiP2pServiceResponse::GetSrcDevice(srcDevice);
}

ECode CWifiP2pDnsSdServiceResponse::SetSrcDevice(
    /* [in] */ IWifiP2pDevice* dev)
{
    return WifiP2pServiceResponse::SetSrcDevice(dev);
}

ECode CWifiP2pDnsSdServiceResponse::ToString(
    /* [out] */ String* string)
{
    VALIDATE_NOT_NULL(string);

    StringBuilder sb;
    sb += "serviceType:DnsSd(";
    sb += mServiceType;
    sb += ")";

    sb += " status:";
    sb += StatusToString(mStatus);
    sb += " srcAddr:";
    String temp;
    mDevice->GetDeviceAddress(&temp);
    sb += temp;
    sb += " version:";
    String version;
    version.AppendFormat("%02x", mVersion);
    sb += version;
    sb += " dnsName:";
    sb += mDnsQueryName;
    sb += " TxtRecord:";

    AutoPtr<ArrayOf<String> > keys;
    assert(0);
    // TODO
    // mTxtRecord->GetKeys((ArrayOf<String>**)&keys);
    if (keys != NULL) {
        String key, value;
        AutoPtr<ICharSequence> obj;
        for (Int32 i = 0; i < keys->GetLength(); ++i) {
            key = (*keys)[i];
            obj = NULL;
            assert(0);
            // TODO
            // mTxtRecord->Get(key, (IInterface**)&obj);
            obj->ToString(&value);
            sb += " key:";
            sb += key;
            sb += " value:";
            sb += value;
        }
    }

    if (!mInstanceName.IsNull()) {
        sb += " InsName:";
        sb += mInstanceName;
    }

    *string = sb.ToString();
    return NOERROR;
}

ECode CWifiP2pDnsSdServiceResponse::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* isEqual)
{
    return WifiP2pServiceResponse::Equals(obj, isEqual);
}

ECode CWifiP2pDnsSdServiceResponse::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    return WifiP2pServiceResponse::GetHashCode(hashCode);
}

ECode CWifiP2pDnsSdServiceResponse::GetDnsQueryName(
    /* [out] */ String* dnsQueryName)
{
    VALIDATE_NOT_NULL(dnsQueryName);

    *dnsQueryName = mDnsQueryName;

    return NOERROR;
}

ECode CWifiP2pDnsSdServiceResponse::GetDnsType(
    /* [out] */ Int32* dnsType)
{
    VALIDATE_NOT_NULL(dnsType);

    *dnsType = mDnsType;

    return NOERROR;
}

ECode CWifiP2pDnsSdServiceResponse::GetVersion(
    /* [out] */ Int32* versionNumber)
{
    VALIDATE_NOT_NULL(versionNumber);

    *versionNumber = mVersion;

    return NOERROR;
}

ECode CWifiP2pDnsSdServiceResponse::GetInstanceName(
    /* [out] */ String* instanceName)
{
    VALIDATE_NOT_NULL(instanceName);

    *instanceName = mInstanceName;

    return NOERROR;
}

ECode CWifiP2pDnsSdServiceResponse::GetTxtRecord(
    /* [out] */ IMap** txtRecord)
{
    VALIDATE_NOT_NULL(txtRecord);

    *txtRecord = mTxtRecord;
    REFCOUNT_ADD(*txtRecord);

    return NOERROR;
}

ECode CWifiP2pDnsSdServiceResponse::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    return WifiP2pServiceResponse::ReadFromParcel(source);
}

ECode CWifiP2pDnsSdServiceResponse::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    return WifiP2pServiceResponse::WriteToParcel(dest);
}

ECode CWifiP2pDnsSdServiceResponse::Parse(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    /*
     * The data format from Wi-Fi Direct spec is as follows.
     * ________________________________________________
     * |  encoded and compressed dns name (variable)  |
     * ________________________________________________
     * |       dnstype(2byte)      |  version(1byte)  |
     * ________________________________________________
     * |              RDATA (variable)                |
     */
    if (mData == NULL) {
        // the empty is OK.
        *result = TRUE;
        return NOERROR;
    }

    AutoPtr<IByteArrayInputStream> ins;
    FAIL_RETURN(CByteArrayInputStream::New(mData, (IByteArrayInputStream**)&ins));
    AutoPtr<IDataInputStream> dis;
    assert(0);
    // TODO
    // FAIL_RETURN(CDataInputStream::New(ins, (IDataInputStream**)&dis));
    AutoPtr<IDataInput> idi = IDataInput::Probe(ins);
    if (idi == NULL) return E_NO_INTERFACE;

    FAIL_RETURN(ReadDnsName(idi, &mDnsQueryName));
    if (mDnsQueryName.IsNull()) {
        return NOERROR;
    }

    // try {
    FAIL_RETURN(idi->ReadUnsignedInt16(&mDnsType));
    FAIL_RETURN(idi->ReadUnsignedByte(&mVersion));
    // } catch (IOException e) {
    //     e.printStackTrace();
    //     return false;
    // }

    if (mDnsType == IWifiP2pDnsSdServiceInfo::DNS_TYPE_PTR) {
        String rData;
        FAIL_RETURN(ReadDnsName(idi, &rData));
        if (rData.IsNull()) {
            return NOERROR;
        }
        if (rData.GetLength() <= mDnsQueryName.GetLength()) {
            return NOERROR;
        }

        mInstanceName = rData.Substring(0,
            rData.GetLength() - mDnsQueryName.GetLength() - 1);
    }
    else if (mDnsType == IWifiP2pDnsSdServiceInfo::DNS_TYPE_TXT) {
        return ReadTxtData(idi, result);
    }
    else {
        *result = FALSE;
        return NOERROR;
    }

    return NOERROR;
}

ECode CWifiP2pDnsSdServiceResponse::ReadDnsName(
    /* [in] */ IDataInput* idi,
    /* [out] */ String* dnsName)
{
    VALIDATE_NOT_NULL(dnsName);
    *dnsName = String(NULL);

    StringBuilder sb;

    // copy virtual memory packet.
    HashMap<Int32, String> vmpack;
    HashMap<Int32, String>::Iterator it;

    if (!mDnsQueryName.IsNull()) {
        vmpack[0x27] = mDnsQueryName;
    }

    // try {
    Int32 i, temp;
    while (TRUE) {
        FAIL_RETURN(idi->ReadUnsignedByte(&i));
        if (i == 0x00) {
            *dnsName = sb.ToString();
            return NOERROR;
        }
        else if (i == 0xc0) {
            // refer to pointer.
            FAIL_RETURN(idi->ReadUnsignedByte(&temp));
            it = vmpack.Find(temp);
            if (it == vmpack.End()) {
                //invalid.
                return NOERROR;
            }

            sb += it->mSecond;
            *dnsName = sb.ToString();
            return NOERROR;
        }
        else {
            AutoPtr<ArrayOf<Byte> > data = ArrayOf<Byte>::Alloc(i);
            if (data == NULL) return E_OUT_OF_MEMORY;

            FAIL_RETURN(idi->ReadFully((ArrayOf<Byte>*)data));
            sb += String((const char *)data->GetPayload());
            sb += ".";
        }
    }
    // catch (IOException e) {
    //     e.printStackTrace();
    // }

    return NOERROR;
}

ECode CWifiP2pDnsSdServiceResponse::ReadTxtData(
    /* [in] */ IDataInput* idi,
    /* [out] */ Boolean* isValid)
{
    VALIDATE_NOT_NULL(isValid);
    *isValid = FALSE;

    AutoPtr<IInputStream> iis = IInputStream::Probe(idi);
    // try {
    Int32 available, len;
    while (TRUE) {
        FAIL_RETURN(iis->Available(&available));
        if (available <= 0) break;

        FAIL_RETURN(idi->ReadUnsignedByte(&len));
        if (len == 0) break;

        AutoPtr<ArrayOf<Byte> > data = ArrayOf<Byte>::Alloc(len);
        if (data == NULL) return E_OUT_OF_MEMORY;

        FAIL_RETURN(idi->ReadFully((ArrayOf<Byte>*)data));

        String temp((const char *)data->GetPayload());
        AutoPtr<ArrayOf<String> > keyVal;
        StringUtils::Split(temp, String("="), (ArrayOf<String>**)&keyVal);
        if (keyVal == NULL || keyVal->GetLength() != 2) {
            return NOERROR;
        }
        AutoPtr<ICharSequence> seq;
        CString::New((*keyVal)[1], (ICharSequence**)&seq);
        assert(0);
        // TODO
        // mTxtRecord->Put((*keyVal)[0], seq->Probe(EIID_IInterface));
    }

    *isValid = TRUE;
    // } catch (IOException e) {
    //     e.printStackTrace();
    // }

    return NOERROR;
}

ECode CWifiP2pDnsSdServiceResponse::NewInstance(
    /* [in] */ Int32 status,
    /* [in] */ Int32 transId,
    /* [in] */ IWifiP2pDevice* dev,
    /* [in] */ ArrayOf<Byte>* data,
    /* [out] */ IWifiP2pDnsSdServiceResponse** instance)
{
    VALIDATE_NOT_NULL(instance);
    *instance = NULL;

    if (status != IWifiP2pServiceResponseStatus::SUCCESS) {
        AutoPtr<CWifiP2pDnsSdServiceResponse> resp;
        CWifiP2pDnsSdServiceResponse::NewByFriend(status,
            transId, dev, NULL, (CWifiP2pDnsSdServiceResponse**)&resp);

        assert(0);
        // TODO
        // *instance = resp;
        REFCOUNT_ADD(*instance);
        return NOERROR;
    }

    AutoPtr<CWifiP2pDnsSdServiceResponse> resp;
    FAIL_RETURN(CWifiP2pDnsSdServiceResponse::NewByFriend(status,
        transId, dev, data, (CWifiP2pDnsSdServiceResponse**)&resp));
    assert(0);
    // TODO
    // *instance = resp;
    REFCOUNT_ADD(*instance);
    return NOERROR;
}

} // namespace Nsd
} // namespace P2p
} // namespace Wifi
} // namespace Droid
} // namespace Elastos
