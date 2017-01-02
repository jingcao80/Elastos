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

#include "Elastos.Droid.Internal.h"
#include "Elastos.CoreLibrary.Utility.Concurrent.h"
#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.Droid.Content.h"
#include "elastos/droid/internal/telephony/uicc/RuimRecords.h"
#include "elastos/droid/internal/telephony/uicc/CIccUtils.h"
#include "elastos/droid/internal/telephony/uicc/CAdnRecordCache.h"
#include "elastos/droid/internal/telephony/uicc/IccException.h"
#include "elastos/droid/internal/telephony/CGsmAlphabet.h"
#include "elastos/droid/internal/telephony/MccTable.h"
#include "elastos/droid/os/AsyncResult.h"
#include "elastos/droid/os/CSystemProperties.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/R.h"

#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/Arrays.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::IAssetManager;
using Elastos::Droid::Internal::Telephony::Cdma::Sms::IUserData;
using Elastos::Droid::Internal::Telephony::CGsmAlphabet;
using Elastos::Droid::Internal::Telephony::MccTable;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::AsyncResult;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::R;

using Elastos::Core::CoreUtils;
using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::IO::IFlushable;
using Elastos::Utility::Arrays;
using Elastos::Utility::IIterator;
using Elastos::Utility::ILocaleHelper;
using Elastos::Utility::CLocaleHelper;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

//=====================================================================
//                       RuimRecords::EfPlLoaded
//=====================================================================
CAR_INTERFACE_IMPL(RuimRecords::EfPlLoaded, Object, IIccRecordLoaded);

RuimRecords::EfPlLoaded::EfPlLoaded(
    /* [in] */ RuimRecords* host)
    : mHost(host)
{}

ECode RuimRecords::EfPlLoaded::GetEfName(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = String("EF_PL");
    return NOERROR;
}

ECode RuimRecords::EfPlLoaded::OnRecordLoaded(
    /* [in] */ IAsyncResult* ar)
{
    AutoPtr<IArrayList> pArr = IArrayList::Probe(((AsyncResult*)ar)->mResult);
    Int32 size = 0;
    pArr->GetSize(&size);
    mHost->mEFpl = ArrayOf<Byte>::Alloc(size);
    for (Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> v;
        pArr->Get(i, (IInterface**)&v);
        IByte::Probe(v)->GetValue(&((*(mHost->mEFpl))[i]));
    }
    if (DBG) {
        AutoPtr<IIccUtils> iccu;
        CIccUtils::AcquireSingleton((IIccUtils**)&iccu);
        String str;
        iccu->BytesToHexString(mHost->mEFpl, &str);
        mHost->Log(String("EF_PL=") + str);
    }
    return NOERROR;
}

//=====================================================================
//                     RuimRecords::EfCsimLiLoaded
//=====================================================================
CAR_INTERFACE_IMPL(RuimRecords::EfCsimLiLoaded, Object, IIccRecordLoaded);

RuimRecords::EfCsimLiLoaded::EfCsimLiLoaded(
    /* [in] */ RuimRecords* host)
    : mHost(host)
{}

ECode RuimRecords::EfCsimLiLoaded::GetEfName(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = String("EF_CSIM_LI");
    return NOERROR;
}

ECode RuimRecords::EfCsimLiLoaded::OnRecordLoaded(
    /* [in] */ IAsyncResult* ar)
{
    AutoPtr<IArrayList> pArr = IArrayList::Probe(((AsyncResult*)ar)->mResult);
    Int32 size = 0;
    pArr->GetSize(&size);
    mHost->mEFpl = ArrayOf<Byte>::Alloc(size);
    for (Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> v;
        pArr->Get(i, (IInterface**)&v);
        IByte::Probe(v)->GetValue(&((*(mHost->mEFli))[i]));
    }
    // convert csim efli data to iso 639 format
    for (Int32 i = 0; i < mHost->mEFli->GetLength(); i+=2) {
        switch((*(mHost->mEFli))[i+1]) {
        case 0x01: (*(mHost->mEFli))[i] = 'e'; (*(mHost->mEFli))[i+1] = 'n';break;
        case 0x02: (*(mHost->mEFli))[i] = 'f'; (*(mHost->mEFli))[i+1] = 'r';break;
        case 0x03: (*(mHost->mEFli))[i] = 'e'; (*(mHost->mEFli))[i+1] = 's';break;
        case 0x04: (*(mHost->mEFli))[i] = 'j'; (*(mHost->mEFli))[i+1] = 'a';break;
        case 0x05: (*(mHost->mEFli))[i] = 'k'; (*(mHost->mEFli))[i+1] = 'o';break;
        case 0x06: (*(mHost->mEFli))[i] = 'z'; (*(mHost->mEFli))[i+1] = 'h';break;
        case 0x07: (*(mHost->mEFli))[i] = 'h'; (*(mHost->mEFli))[i+1] = 'e';break;
        default: (*(mHost->mEFli))[i] = ' '; (*(mHost->mEFli))[i+1] = ' ';
        }
    }

    if (DBG) {
        AutoPtr<IIccUtils> iccu;
        CIccUtils::AcquireSingleton((IIccUtils**)&iccu);
        String str;
        iccu->BytesToHexString(mHost->mEFli, &str);
        mHost->Log(String("EF_LI=") + str);
    }
    return NOERROR;
}

//=====================================================================
//                     RuimRecords::EfCsimSpnLoaded
//=====================================================================
CAR_INTERFACE_IMPL(RuimRecords::EfCsimSpnLoaded, Object, IIccRecordLoaded);

RuimRecords::EfCsimSpnLoaded::EfCsimSpnLoaded(
    /* [in] */ RuimRecords* host)
    : mHost(host)
{}

ECode RuimRecords::EfCsimSpnLoaded::GetEfName(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = String("EF_CSIM_SPN");
    return NOERROR;
}

ECode RuimRecords::EfCsimSpnLoaded::OnRecordLoaded(
    /* [in] */ IAsyncResult* ar)
{
    AutoPtr<IArrayList> pArr = IArrayList::Probe(((AsyncResult*)ar)->mResult);
    Int32 size = 0;
    pArr->GetSize(&size);
    AutoPtr<ArrayOf<Byte> > data = ArrayOf<Byte>::Alloc(size);
    for (Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> v;
        pArr->Get(i, (IInterface**)&v);
        IByte::Probe(v)->GetValue(&((*data)[i]));
    }
    if (DBG) {
        AutoPtr<IIccUtils> iccu;
        CIccUtils::AcquireSingleton((IIccUtils**)&iccu);
        String str;
        iccu->BytesToHexString(data, &str);
        mHost->Log(String("CSIM_SPN=") + str);
    }

    // C.S0065 for EF_SPN decoding
    mHost->mCsimSpnDisplayCondition = ((0x01 & (*data)[0]) != 0);

    Int32 encoding = (*data)[1];
    Int32 language = (*data)[2];
    AutoPtr<ArrayOf<Byte> > spnData = ArrayOf<Byte>::Alloc(32);
    Int32 len = ((data->GetLength() - 3) < 32) ? (data->GetLength() - 3) : 32;
    // System::Arraycopy(data, 3, spnData, 0, len);
    spnData->Copy(0, data, 3, len);

    Int32 numBytes;
    for (numBytes = 0; numBytes < spnData->GetLength(); numBytes++) {
        if (((*spnData)[numBytes] & 0xFF) == 0xFF) break;
    }

    if (numBytes == 0) {
        mHost->SetServiceProviderName(String(""));
        return NOERROR;
    }
    // try {
    AutoPtr<IGsmAlphabet> gsm;
    CGsmAlphabet::AcquireSingleton((IGsmAlphabet**)&gsm);
    switch (encoding) {
        case IUserData::ENCODING_OCTET:
        case IUserData::ENCODING_LATIN: {
            // mHost->SetServiceProviderName(String(spnData, 0, numBytes, "ISO-8859-1"));
            mHost->SetServiceProviderName(String(*spnData, 0, numBytes));
            break;
        }
        case IUserData::ENCODING_IA5:
        case IUserData::ENCODING_GSM_7BIT_ALPHABET: {
            String name;
            gsm->Gsm7BitPackedToString(spnData, 0, (numBytes * 8) / 7, &name);
            mHost->SetServiceProviderName(name);
            break;
        }
        case IUserData::ENCODING_7BIT_ASCII: {
            // String spn(spnData, 0, numBytes, "US-ASCII");
            String spn(*spnData, 0, numBytes);
            // To address issues with incorrect encoding scheme
            // programmed in some commercial CSIM cards, the decoded
            // SPN is checked to have characters in printable ASCII
            // range. If not, they are decoded with
            // ENCODING_GSM_7BIT_ALPHABET scheme.
            if (TextUtils::IsPrintableAsciiOnly(CoreUtils::Convert(spn))) {
                mHost->SetServiceProviderName(spn);
            }
            else {
                if (DBG) mHost->Log(String("Some corruption in SPN decoding = ") + spn);
                if (DBG) mHost->Log(String("Using ENCODING_GSM_7BIT_ALPHABET scheme..."));
                String name;
                gsm->Gsm7BitPackedToString(spnData, 0, (numBytes * 8) / 7, &name);
                mHost->SetServiceProviderName(name);
            }
        break;
        }
        case IUserData::ENCODING_UNICODE_16: {
            // mHost->SetServiceProviderName(String(spnData, 0, numBytes, "utf-16"));
            mHost->SetServiceProviderName(String(*spnData, 0, numBytes));
            break;
        }
        default:
            mHost->Log(String("SPN encoding not supported"));
        }
    // } catch(Exception e) {
    //     log("spn decode error: " + e);
    // }
    String name;
    mHost->GetServiceProviderName(&name);
    if (DBG) {
        mHost->Log(String("spn=") + name);
    }
    if (DBG) {
        mHost->Log(String("spnCondition=") + StringUtils::ToString(mHost->mCsimSpnDisplayCondition));
    }
    AutoPtr<ISystemProperties> sp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
    sp->Set(ITelephonyProperties::PROPERTY_ICC_OPERATOR_ALPHA, name);
    return NOERROR;
}

//=====================================================================
//                     RuimRecords::EfCsimMdnLoaded
//=====================================================================
CAR_INTERFACE_IMPL(RuimRecords::EfCsimMdnLoaded, Object, IIccRecordLoaded);

RuimRecords::EfCsimMdnLoaded::EfCsimMdnLoaded(
    /* [in] */ RuimRecords* host)
    : mHost(host)
{}

ECode RuimRecords::EfCsimMdnLoaded::GetEfName(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = String("EF_CSIM_MDN");
    return NOERROR;
}

ECode RuimRecords::EfCsimMdnLoaded::OnRecordLoaded(
    /* [in] */ IAsyncResult* ar)
{
    AutoPtr<IArrayList> pArr = IArrayList::Probe(((AsyncResult*)ar)->mResult);
    Int32 size = 0;
    pArr->GetSize(&size);
    AutoPtr<ArrayOf<Byte> > data = ArrayOf<Byte>::Alloc(size);
    for (Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> v;
        pArr->Get(i, (IInterface**)&v);
        IByte::Probe(v)->GetValue(&((*data)[i]));
    }
    AutoPtr<IIccUtils> iccu;
    CIccUtils::AcquireSingleton((IIccUtils**)&iccu);
    if (DBG) {
        String str;
        iccu->BytesToHexString(data, &str);
        mHost->Log(String("CSIM_MDN=") + str);
    }
    // Refer to C.S0065 5.2.35
    Int32 mdnDigitsNum = 0x0F & (*data)[0];
    iccu->CdmaBcdToString(data, 1, mdnDigitsNum, &(mHost->mMdn));
    if (DBG) mHost->Log(String("CSIM MDN=") + mHost->mMdn);
    return NOERROR;
}

//=====================================================================
//                    RuimRecords::EfCsimImsimLoaded
//=====================================================================
CAR_INTERFACE_IMPL(RuimRecords::EfCsimImsimLoaded, Object, IIccRecordLoaded);

RuimRecords::EfCsimImsimLoaded::EfCsimImsimLoaded(
    /* [in] */ RuimRecords* host)
    : mHost(host)
{}

ECode RuimRecords::EfCsimImsimLoaded::GetEfName(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = String("EF_CSIM_IMSIM");
    return NOERROR;
}

ECode RuimRecords::EfCsimImsimLoaded::OnRecordLoaded(
    /* [in] */ IAsyncResult* ar)
{
    AutoPtr<IArrayList> pArr = IArrayList::Probe(((AsyncResult*)ar)->mResult);
    Int32 size = 0;
    pArr->GetSize(&size);
    AutoPtr<ArrayOf<Byte> > data = ArrayOf<Byte>::Alloc(size);
    for (Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> v;
        pArr->Get(i, (IInterface**)&v);
        IByte::Probe(v)->GetValue(&((*data)[i]));
    }

    AutoPtr<IIccUtils> iccu;
    CIccUtils::AcquireSingleton((IIccUtils**)&iccu);
    if (data == NULL || data->GetLength() < 10) {
        String str;
        iccu->BytesToHexString(data, &str);
        mHost->Log(String("Invalid IMSI from EF_CSIM_IMSIM ") + str);
        mHost->mImsi = NULL;
        mHost->mMin = NULL;
        return NOERROR;
    }
    if (DBG) {
        String str;
        iccu->BytesToHexString(data, &str);
        mHost->Log(String("CSIM_IMSIM=") + str);
    }

    // C.S0065 section 5.2.2 for IMSI_M encoding
    // C.S0005 section 2.3.1 for MIN encoding in IMSI_M.
    Boolean provisioned = (((*data)[7] & 0x80) == 0x80);

    if (provisioned) {
        mHost->mImsi = mHost->DecodeImsi(data);
        if (NULL != mHost->mImsi) {
            mHost->mMin = mHost->mImsi.Substring(5, 15);
        }
        mHost->Log(String("IMSI: ") + mHost->mImsi.Substring(0, 5) + String("xxxxxxxxx"));

    }
    else {
        if (DBG) mHost->Log(String("IMSI not provisioned in card"));
    }

    //Update MccTable with the retrieved IMSI
    String operatorNumeric;
    mHost->GetOperatorNumeric(&operatorNumeric);
    if (!operatorNumeric.IsNull()) {
        if(operatorNumeric.GetLength() <= 6){
            MccTable::UpdateMccMncConfiguration(mHost->mContext, operatorNumeric, FALSE);
        }
    }

    mHost->mImsiReadyRegistrants->NotifyRegistrants();
    return NOERROR;
}

//=====================================================================
//                  RuimRecords::EfCsimCdmaHomeLoaded
//=====================================================================
CAR_INTERFACE_IMPL(RuimRecords::EfCsimCdmaHomeLoaded, Object, IIccRecordLoaded);

RuimRecords::EfCsimCdmaHomeLoaded::EfCsimCdmaHomeLoaded(
    /* [in] */ RuimRecords* host)
    : mHost(host)
{}

ECode RuimRecords::EfCsimCdmaHomeLoaded::GetEfName(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = String("EF_CSIM_CDMAHOME");
    return NOERROR;
}

ECode RuimRecords::EfCsimCdmaHomeLoaded::OnRecordLoaded(
    /* [in] */ IAsyncResult* ar)
{
    // Per C.S0065 section 5.2.8
    AutoPtr<IArrayList> dataList = IArrayList::Probe(((AsyncResult*)ar)->mResult);
    Int32 size = 0;
    dataList->GetSize(&size);
    if (DBG) mHost->Log(String("CSIM_CDMAHOME data size=") + StringUtils::ToString(size));
    Boolean bEmp = FALSE;
    dataList->IsEmpty(&bEmp);
    if (bEmp) {
        return NOERROR;
    }
    StringBuilder sidBuf;
    StringBuilder nidBuf;

    AutoPtr<IIterator> it;
    dataList->GetIterator((IIterator**)&it);
    Boolean bHasNext = FALSE;
    while ((it->HasNext(&bHasNext), bHasNext)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<IArrayList> pArr = IArrayList::Probe(p);
        Int32 size = 0;
        pArr->GetSize(&size);
        AutoPtr<ArrayOf<Byte> > data = ArrayOf<Byte>::Alloc(size);
        for (Int32 i = 0; i < size; ++i) {
            AutoPtr<IInterface> v;
            pArr->Get(i, (IInterface**)&v);
            IByte::Probe(v)->GetValue(&((*data)[i]));
        }
        if (data->GetLength() == 5) {
            Int32 sid = (((*data)[1] & 0xFF) << 8) | ((*data)[0] & 0xFF);
            Int32 nid = (((*data)[3] & 0xFF) << 8) | ((*data)[2] & 0xFF);
            sidBuf.Append(sid);
            sidBuf.Append(',');
            nidBuf.Append(nid);
            nidBuf.Append(',');
        }
    }
    // remove trailing ","
    sidBuf.SetLength(sidBuf.GetLength()-1);
    nidBuf.SetLength(nidBuf.GetLength()-1);

    mHost->mHomeSystemId = sidBuf.ToString();
    mHost->mHomeNetworkId = nidBuf.ToString();
    return NOERROR;
}

//=====================================================================
//                    RuimRecords::EfCsimEprlLoaded
//=====================================================================
CAR_INTERFACE_IMPL(RuimRecords::EfCsimEprlLoaded, Object, IIccRecordLoaded);

RuimRecords::EfCsimEprlLoaded::EfCsimEprlLoaded(
    /* [in] */ RuimRecords* host)
    : mHost(host)
{}

ECode RuimRecords::EfCsimEprlLoaded::GetEfName(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = String("EF_CSIM_EPRL");
    return NOERROR;
}

ECode RuimRecords::EfCsimEprlLoaded::OnRecordLoaded(
    /* [in] */ IAsyncResult* ar)
{
    mHost->OnGetCSimEprlDone(((AsyncResult*)ar));
    return NOERROR;
}

//=====================================================================
//                    RuimRecords::EfCsimModelLoaded
//=====================================================================
CAR_INTERFACE_IMPL(RuimRecords::EfCsimModelLoaded, Object, IIccRecordLoaded);

RuimRecords::EfCsimModelLoaded::EfCsimModelLoaded(
    /* [in] */ RuimRecords* host)
    : mHost(host)
{}

ECode RuimRecords::EfCsimModelLoaded::GetEfName(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = String("EF_CSIM_MODEL");
    return NOERROR;
}

ECode RuimRecords::EfCsimModelLoaded::OnRecordLoaded(
    /* [in] */ IAsyncResult* ar)
{
    AutoPtr<IArrayList> pArr = IArrayList::Probe(((AsyncResult*)ar)->mResult);
    Int32 size = 0;
    pArr->GetSize(&size);
    AutoPtr<ArrayOf<Byte> > data = ArrayOf<Byte>::Alloc(size);
    for (Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> v;
        pArr->Get(i, (IInterface**)&v);
        IByte::Probe(v)->GetValue(&((*data)[i]));
    }
    if (DBG) {
        AutoPtr<IIccUtils> iccu;
        CIccUtils::AcquireSingleton((IIccUtils**)&iccu);
        String str;
        iccu->BytesToHexString(data, &str);
        mHost->Log(String("EF_CSIM_MODEL=") + str);
    }
    return NOERROR;
}

//=====================================================================
//                    RuimRecords::EfRuimModelLoaded
//=====================================================================
CAR_INTERFACE_IMPL(RuimRecords::EfRuimModelLoaded, Object, IIccRecordLoaded);

RuimRecords::EfRuimModelLoaded::EfRuimModelLoaded(
    /* [in] */ RuimRecords* host)
    : mHost(host)
{}

ECode RuimRecords::EfRuimModelLoaded::GetEfName(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = String("EF_RUIM_MODEL");
    return NOERROR;
}

ECode RuimRecords::EfRuimModelLoaded::OnRecordLoaded(
    /* [in] */ IAsyncResult* ar)
{
    AutoPtr<IArrayList> pArr = IArrayList::Probe(((AsyncResult*)ar)->mResult);
    Int32 size = 0;
    pArr->GetSize(&size);
    AutoPtr<ArrayOf<Byte> > data = ArrayOf<Byte>::Alloc(size);
    for (Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> v;
        pArr->Get(i, (IInterface**)&v);
        IByte::Probe(v)->GetValue(&((*data)[i]));
    }
    if (DBG) {
        AutoPtr<IIccUtils> iccu;
        CIccUtils::AcquireSingleton((IIccUtils**)&iccu);
        String str;
        iccu->BytesToHexString(data, &str);
        mHost->Log(String("EF_RUIM_MODEL=") + str);
    }
    return NOERROR;
}

//=====================================================================
//                     RuimRecords::EfRuimIdLoaded
//=====================================================================
CAR_INTERFACE_IMPL(RuimRecords::EfRuimIdLoaded, Object, IIccRecordLoaded);

RuimRecords::EfRuimIdLoaded::EfRuimIdLoaded(
    /* [in] */ RuimRecords* host)
    : mHost(host)
{}

ECode RuimRecords::EfRuimIdLoaded::GetEfName(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = String("EF_RUIM_ID");
    return NOERROR;
}

ECode RuimRecords::EfRuimIdLoaded::OnRecordLoaded(
    /* [in] */ IAsyncResult* ar)
{
    // The first byte represent the num bytes of valid data for RUIM ID data.
    // It is valid RUIM ID data From the second byte to num+1 byte.
    // And the second byte is the lowest-order byte, the num+1 byte is highest-order
    AutoPtr<IArrayList> pArr = IArrayList::Probe(((AsyncResult*)ar)->mResult);
    Int32 size = 0;
    pArr->GetSize(&size);
    AutoPtr<ArrayOf<Byte> > data = ArrayOf<Byte>::Alloc(size);
    for (Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> v;
        pArr->Get(i, (IInterface**)&v);
        IByte::Probe(v)->GetValue(&((*data)[i]));
    }
    AutoPtr<IIccUtils> iccu;
    CIccUtils::AcquireSingleton((IIccUtils**)&iccu);
    if (DBG) {
        String str;
        iccu->BytesToHexString(data, &str);
        mHost->Log(String("RuimId Data=") + str);
    }
    if (data == NULL) {
        return NOERROR;
    }
    Int32 numOfBytes = (*data)[0];
    if (numOfBytes < NUM_BYTES_RUIM_ID) {
        AutoPtr<ArrayOf<Byte> > decodeData = ArrayOf<Byte>::Alloc(numOfBytes);
        for (Int32 i = 0; i < numOfBytes; i++) {
            (*decodeData)[i] = (*data)[numOfBytes - i];
        }
        String ruimId;
        iccu->BytesToHexString(decodeData, &ruimId);
        if (DBG) {
            mHost->Log(String("RUIM_ID=") + ruimId);
        }
    }
    return NOERROR;
}

//=====================================================================
//                             RuimRecords
//=====================================================================
CAR_INTERFACE_IMPL(RuimRecords, IccRecords, IRuimRecords);

const String RuimRecords::LOGTAG("RuimRecords");
const Int32 RuimRecords::EF_MODEL_FILE_SIZE;
const Int32 RuimRecords::MODEL_INFORMATION_SIZE;
const Int32 RuimRecords::MANUFACTURER_NAME_SIZE;
const Int32 RuimRecords::SOFTWARE_VERSION_INFORMATION_SIZE;
const Int32 RuimRecords::LANGUAGE_INDICATOR_ENGLISH;
const Int32 RuimRecords::CSIM_IMSI_MNC_LENGTH;
const Int32 RuimRecords::EVENT_GET_DEVICE_IDENTITY_DONE;
const Int32 RuimRecords::EVENT_GET_ICCID_DONE;
const Int32 RuimRecords::EVENT_GET_RUIM_CST_DONE;
const Int32 RuimRecords::EVENT_GET_CDMA_SUBSCRIPTION_DONE;
const Int32 RuimRecords::EVENT_UPDATE_DONE;
const Int32 RuimRecords::EVENT_SET_MODEL_DONE;
const Int32 RuimRecords::EVENT_GET_SST_DONE;
const Int32 RuimRecords::EVENT_GET_ALL_SMS_DONE;
const Int32 RuimRecords::EVENT_MARK_SMS_READ_DONE;
const Int32 RuimRecords::EVENT_SMS_ON_RUIM;
const Int32 RuimRecords::EVENT_GET_SMS_DONE;
const Int32 RuimRecords::NUM_BYTES_RUIM_ID;

RuimRecords::RuimRecords()
{
}

ECode RuimRecords::constructor(
    /* [in] */ IUiccCardApplication* app,
    /* [in] */ IContext* c,
    /* [in] */ ICommandsInterface* ci)
{
    IccRecords::constructor(app, c, ci);

    CAdnRecordCache::New(mFh, (IAdnRecordCache**)&mAdnCache);

    mRecordsRequested = FALSE;  // No load request is made till SIM ready

    // recordsToLoad is set to 0 because no requests are made yet
    mRecordsToLoad = 0;

    // NOTE the EVENT_SMS_ON_RUIM is not registered

    // Start off by setting empty state
    ResetRecords();

    mParentApp->RegisterForReady(this, EVENT_APP_READY, NULL);
    if (DBG) {
        Log(String("RuimRecords X ctor this=") + TO_CSTR(this));
    }
    return NOERROR;
}

ECode RuimRecords::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    String str;
    IccRecords::ToString(&str);
    *result = String("RuimRecords: ") + str
            + String(" m_ota_commited") + StringUtils::ToString(mOtaCommited)
            + String(" mMyMobileNumber=") + String("xxxx")
            + String(" mMin2Min1=") + mMin2Min1
            + String(" mPrlVersion=") + mPrlVersion
            + String(" mEFpl=") + Arrays::ToString(mEFpl)
            + String(" mEFli=") + Arrays::ToString(mEFli)
            + String(" mCsimSpnDisplayCondition=") + StringUtils::ToString(mCsimSpnDisplayCondition)
            + String(" mMdn=") + mMdn
            + String(" mMin=") + mMin
            + String(" mHomeSystemId=") + mHomeSystemId
            + String(" mHomeNetworkId=") + mHomeNetworkId;
    return NOERROR;
}

ECode RuimRecords::Dispose()
{
    if (DBG) {
        Log(String("Disposing RuimRecords ") + TO_CSTR(this));
    }
    //Unregister for all events
    mParentApp->UnregisterForReady(this);
    ResetRecords();
    IccRecords::Dispose();
    return NOERROR;
}

ECode RuimRecords::GetIMSI(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mImsi;
    return NOERROR;
}

ECode RuimRecords::GetMdnNumber(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mMyMobileNumber;
    return NOERROR;
}

ECode RuimRecords::GetCdmaMin(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mMin2Min1;
    return NOERROR;
}

ECode RuimRecords::GetPrlVersion(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mPrlVersion;
    return NOERROR;
}

ECode RuimRecords::SetVoiceMailNumber(
    /* [in] */ const String& alphaTag,
    /* [in] */ const String& voiceNumber,
    /* [in] */ IMessage* onComplete)
{
    // In CDMA this is Operator/OEM dependent
    AutoPtr<IccException> p = new IccException(String("setVoiceMailNumber not implemented"));
    AsyncResult::ForMessage((onComplete))->mException =
            IThrowable::Probe(p);
    onComplete->SendToTarget();
    Loge(String("method setVoiceMailNumber is not implemented"));
    return NOERROR;
}

ECode RuimRecords::OnRefresh(
    /* [in] */ Boolean fileChanged,
    /* [in] */ ArrayOf<Int32>* fileList)
{
    if (fileChanged) {
        // A future optimization would be to inspect fileList and
        // only reload those files that we care about.  For now,
        // just re-fetch all RUIM records that we cache.
        FetchRuimRecords();
    }
    return NOERROR;
}

ECode RuimRecords::GetOperatorNumeric(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    if (mImsi == NULL) {
        *result = NULL;
        return NOERROR;
    }

    AutoPtr<ISystemProperties> sp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
    Boolean b = FALSE;
    sp->GetBoolean(String("ro.telephony.get_imsi_from_sim"), FALSE, &b);
    if (b) {
        AutoPtr<IUICCConfig> uicc;
        mParentApp->GetUICCConfig((IUICCConfig**)&uicc);
        String imsi;
        uicc->GetImsi(&imsi);
        Int32 mnclength = 0;
        uicc->GetMncLength(&mnclength);

        // If we are LTE over CDMA (Verizon), then pull the correct info from SIMRecords
        if (imsi != NULL) {
            Log(String("Overriding with Operator Numeric: ") + imsi.Substring(0, 3 + mnclength));
            *result = imsi.Substring(0, 3 + mnclength);
            return NOERROR;
        }
    }


    if (mMncLength != UNINITIALIZED && mMncLength != UNKNOWN) {
        // Length = length of MCC + length of MNC
        // length of mcc = 3 (3GPP2 C.S0005 - Section 2.3)
        *result = mImsi.Substring(0, 3 + mMncLength);
        return NOERROR;
    }

    // Guess the MNC length based on the MCC if we don't
    // have a valid value in ef[ad]

    Int32 mcc = StringUtils::ParseInt32(mImsi.Substring(0, 3));
    *result = mImsi.Substring(0, 3 + CSIM_IMSI_MNC_LENGTH);
    return NOERROR;
}

ECode RuimRecords::HandleMessage(
    /* [in] */ IMessage* msg)
{
    AutoPtr<AsyncResult> ar;

    AutoPtr<ArrayOf<Byte> > data;

    Boolean isRecordLoadResponse = FALSE;

    Int32 what = 0;
    msg->GetWhat(&what);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    if (mDestroyed.Get()) {
        Loge(String("Received message ") + TO_CSTR(msg) +
                String("[") + StringUtils::ToString(what) +
                String("] while being destroyed. Ignoring."));
        return NOERROR;
    }

    // try {
    switch (what) {
        case EVENT_APP_READY:
            OnReady();
            break;

        case EVENT_GET_DEVICE_IDENTITY_DONE:
            Log(String("Event EVENT_GET_DEVICE_IDENTITY_DONE Received"));
        break;

        case EVENT_GET_CDMA_SUBSCRIPTION_DONE: {
            ar = (AsyncResult*)IAsyncResult::Probe(obj);
            AutoPtr<IArrayList> pArr = IArrayList::Probe(((AsyncResult*)ar)->mResult);
            Int32 size = 0;
            pArr->GetSize(&size);
            AutoPtr<ArrayOf<String> > localTemp = ArrayOf<String>::Alloc(size);
            for (Int32 i = 0; i < size; ++i) {
                AutoPtr<IInterface> v;
                pArr->Get(i, (IInterface**)&v);
                ICharSequence::Probe(v)->ToString(&((*localTemp)[i]));
            }
            if (ar->mException != NULL) {
                break;
            }

            mMyMobileNumber = (*localTemp)[0];
            mMin2Min1 = (*localTemp)[3];
            mPrlVersion = (*localTemp)[4];

            Log(String("MDN: ") + mMyMobileNumber + String(" MIN: ") + mMin2Min1);

        break;
        }
        case EVENT_GET_ICCID_DONE: {
            isRecordLoadResponse = TRUE;

            ar = (AsyncResult*)IAsyncResult::Probe(obj);
            AutoPtr<IArrayList> pArr = IArrayList::Probe(((AsyncResult*)ar)->mResult);
            Int32 size = 0;
            pArr->GetSize(&size);
            data = ArrayOf<Byte>::Alloc(size);
            for (Int32 i = 0; i < size; ++i) {
                AutoPtr<IInterface> v;
                pArr->Get(i, (IInterface**)&v);
                IByte::Probe(v)->GetValue(&((*data)[i]));
            }
            if (ar->mException != NULL) {
                break;
            }

            AutoPtr<IIccUtils> iccu;
            CIccUtils::AcquireSingleton((IIccUtils**)&iccu);
            iccu->BcdToString(data, 0, data->GetLength(), &mIccId);

            Log(String("iccid: ") + mIccId);

        break;
        }
        case EVENT_UPDATE_DONE: {
            ar = (AsyncResult*)IAsyncResult::Probe(obj);
            if (ar->mException != NULL) {
                Logger::I(LOGTAG, "RuimRecords update failed %p", ar->mException.Get());
            }
        break;
        }
        case EVENT_GET_ALL_SMS_DONE:
        case EVENT_MARK_SMS_READ_DONE:
        case EVENT_SMS_ON_RUIM:
        case EVENT_GET_SMS_DONE:
            Logger::W(LOGTAG, String("Event not supported: ") + StringUtils::ToString(what));
            break;

        // TODO: probably EF_CST should be read instead
        case EVENT_GET_SST_DONE:
            Log(String("Event EVENT_GET_SST_DONE Received"));
        break;
        case EVENT_SET_MODEL_DONE: {
            ar = (AsyncResult*)IAsyncResult::Probe(obj);
            if (ar->mException != NULL) {
                Logger::I(LOGTAG, "Set EF Model failed %p", ar->mException.Get());
            }
            Log(String("EVENT_SET_MODEL_DONE"));
            break;
        }
        case EVENT_GET_RUIM_CST_DONE: {
            // CSIM flags
            Boolean omhEnabled = FALSE;
            Boolean mmsicpEnabled = FALSE;
            isRecordLoadResponse = TRUE;
            ar = (AsyncResult*)IAsyncResult::Probe(obj);
            if (ar != NULL && ar->mException == NULL) {
                AutoPtr<IArrayList> pArr = IArrayList::Probe(((AsyncResult*)ar)->mResult);
                Int32 size = 0;
                pArr->GetSize(&size);
                data = ArrayOf<Byte>::Alloc(size);
                for (Int32 i = 0; i < size; ++i) {
                    AutoPtr<IInterface> v;
                    pArr->Get(i, (IInterface**)&v);
                    IByte::Probe(v)->GetValue(&((*data)[i]));
                }
                AutoPtr<IIccUtils> iccu;
                CIccUtils::AcquireSingleton((IIccUtils**)&iccu);
                String str;
                iccu->BytesToHexString(data, &str);
                Log(String("EF CST data: ") + str);
                if (data == NULL) {
                    break;
                }

                // For CSIM card
                if (mParentApp != NULL) {
                    AppType type;
                    mParentApp->GetType(&type);
                    if (type == APPTYPE_CSIM) {
                        // Service n35, Messaging and 3GPD Extensions
                        if (data->GetLength() > 4) {
                            omhEnabled = (0x04 == (0x04 & (*data)[4])) ? TRUE : FALSE;
                            if (omhEnabled) {
                                // Service n19, Multimedia Messaging Service (MMS)
                                mmsicpEnabled = (0x04 == (0x04 & (*data)[2])) ? TRUE : FALSE;
                            }
                        }
                        else {
                            Loge(String("CSIM EF CST data length = ") + StringUtils::ToString(data->GetLength()));
                        }
                    }
                }
                else {
                    if (data->GetLength() > 3) {
                        omhEnabled = (0x30 == (0x30 & (*data)[3])) ? TRUE : FALSE;
                        // Judge the read MMSICP flag is enabled on CST or not
                        if (omhEnabled && data->GetLength() > 10) {
                            mmsicpEnabled = (0x0C == (0x0C & (*data)[10])) ? TRUE : FALSE;
                        }
                        else {
                            Loge(String("OMH EF CST data length = ") + StringUtils::ToString(data->GetLength()));
                        }
                    }
                    else {
                        Loge(String("OMH EF CST data length = ") + StringUtils::ToString(data->GetLength()));
                    }
                }
                Log(String("mms icp enabled =") + StringUtils::ToString(mmsicpEnabled) +
                    String(" omhEnabled ") + StringUtils::ToString(omhEnabled));
                AutoPtr<ISystemProperties> sp;
                CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
                sp->Set(ITelephonyProperties::PROPERTY_RUIM_OMH_CARD,
                        omhEnabled ? String("true") : String("false"));
            }
            // We already know if it is an OMH card in this point
            FetchOMHCardRecords(omhEnabled);
            break;
        }
        default:
            IccRecords::HandleMessage(msg);   // IccRecords handles generic record load responses
    }
    // }}catch (RuntimeException exc) {
    //     // I don't want these exceptions to be fatal
    //     Rlog.w(LOGTAG, "Exception parsing RUIM record", exc);
    // } finally {
        // Count up record load responses even if they are fails
        if (isRecordLoadResponse) {
            OnRecordLoaded();
        }
    // }
    return NOERROR;
}

ECode RuimRecords::OnReady()
{
    FetchRuimRecords();

    AutoPtr<IMessage> msg;
    ObtainMessage(EVENT_GET_CDMA_SUBSCRIPTION_DONE, (IMessage**)&msg);
    mCi->GetCDMASubscription(msg);
    return NOERROR;
}

ECode RuimRecords::RecordsRequired()
{
    if (DBG) Log(String("recordsRequired"));
    mRecordsRequired = TRUE;

    // trigger to retrieve all records
    FetchRuimRecords();
    return NOERROR;
}

ECode RuimRecords::GetDisplayRule(
    /* [in] */ const String& plmnNumeric,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 rule = 0;

    String name;
    GetServiceProviderName(&name);
    if (mContext != NULL) {
        AutoPtr<IResources> res;
        mContext->GetResources((IResources**)&res);
        Boolean b = FALSE;
        res->GetBoolean(
            R::bool_::def_telephony_spn_spec_enabled, &b);
        if (b) {
            // Always display the SPN only from RUIM
            rule = SPN_RULE_SHOW_SPN;
        }
    }
    else if (mParentApp != NULL) {
        AutoPtr<IUiccCard> card;
        mParentApp->GetUiccCard((IUiccCard**)&card);
        if (card != NULL) {
            String override;
            card->GetOperatorBrandOverride(&override);
            if (!override.IsNull()) {
                // use operator brand override
                rule = SPN_RULE_SHOW_PLMN;
            }
        }
    }
    else if (TextUtils::IsEmpty(name)) {
        // EF_SPN content not found on this RUIM, or not yet loaded
        rule = SPN_RULE_SHOW_PLMN;
    }
    else if (IsOnMatchingPlmn(plmnNumeric)) {
        // on home network
        AutoPtr<ISystemProperties> sp;
        CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
        Boolean b = FALSE;
        sp->GetBoolean(String("ro.cdma.force_plmn_lookup"), FALSE, &b);
        if (mCsimSpnDisplayCondition && !TextUtils::IsEmpty(name)
                && (!b)) {
            // check CSIM SPN Display Condition (applicable on home network),
            // but only if SPN was found on this RUIM
            // if force plmn lookup then plmn should be used
            rule = SPN_RULE_SHOW_SPN;
        }
        else {
            // CSIM SPN Display does not require a SPN display, or SPN not found on RUIM,
            // then revert to currently registered network
            rule = SPN_RULE_SHOW_PLMN;
        }
    }
    else {
        // roaming, use the currently registered network
        rule = SPN_RULE_SHOW_PLMN;
    }

    *result = rule;
    return NOERROR;
}

ECode RuimRecords::IsProvisioned(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // If UICC card has CSIM app, look for MDN and MIN field
    // to determine if the SIM is provisioned.  Otherwise,
    // consider the SIM is provisioned. (for case of ordinal
    // USIM only UICC.)
    // If PROPERTY_TEST_CSIM is defined, bypess provision check
    // and consider the SIM is provisioned.
    AutoPtr<ISystemProperties> sp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
    Boolean b = FALSE;
    sp->GetBoolean(ITelephonyProperties::PROPERTY_TEST_CSIM, FALSE, &b);
    if (b) {
        *result = TRUE;
        return NOERROR;
    }

    if (mParentApp == NULL) {
        *result = FALSE;
        return NOERROR;
    }

    AppType type;
    mParentApp->GetType(&type);
    if (type == APPTYPE_CSIM &&
        ((mMdn == NULL) || (mMin == NULL))) {
        *result = FALSE;
        return NOERROR;
    }
    *result = TRUE;
    return NOERROR;
}

ECode RuimRecords::SetVoiceMessageWaiting(
    /* [in] */ Int32 line,
    /* [in] */ Int32 countWaiting)
{
    // Will be used in future to store voice mail count in UIM
    // C.S0023-D_v1.0 does not have a file id in UIM for MWI
    Log(String("RuimRecords:setVoiceMessageWaiting - NOP for CDMA"));
    return NOERROR;
}

ECode RuimRecords::GetVoiceMessageCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // Will be used in future to retrieve voice mail count for UIM
    // C.S0023-D_v1.0 does not have a file id in UIM for MWI
    Log(String("RuimRecords:getVoiceMessageCount - NOP for CDMA"));
    *result = 0;
    return NOERROR;
}

ECode RuimRecords::GetMdn(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mMdn;
    return NOERROR;
}

ECode RuimRecords::GetMin(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mMin;
    return NOERROR;
}

ECode RuimRecords::GetSid(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mHomeSystemId;
    return NOERROR;
}

ECode RuimRecords::GetNid(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mHomeNetworkId;
    return NOERROR;
}

ECode RuimRecords::GetCsimSpnDisplayCondition(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mCsimSpnDisplayCondition;
    return NOERROR;
}

ECode RuimRecords::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    pw->Println(String("RuimRecords: ") + TO_CSTR(this));
    pw->Println(String(" extends:"));
    IccRecords::Dump(fd, pw, args);
    pw->Println(String(" mOtaCommited=") + StringUtils::ToString(mOtaCommited));
    pw->Println(String(" mMyMobileNumber=") + mMyMobileNumber);
    pw->Println(String(" mMin2Min1=") + mMin2Min1);
    pw->Println(String(" mPrlVersion=") + mPrlVersion);
    pw->Println(String(" mEFpl[]=") + Arrays::ToString(mEFpl));
    pw->Println(String(" mEFli[]=") + Arrays::ToString(mEFli));
    pw->Println(String(" mCsimSpnDisplayCondition=") + StringUtils::ToString(mCsimSpnDisplayCondition));
    pw->Println(String(" mMdn=") + mMdn);
    pw->Println(String(" mMin=") + mMin);
    pw->Println(String(" mHomeSystemId=") + mHomeSystemId);
    pw->Println(String(" mHomeNetworkId=") + mHomeNetworkId);
    IFlushable::Probe(pw)->Flush();
    return NOERROR;
}

ECode RuimRecords::Finalize()
{
    if (DBG) Log(String("RuimRecords finalized"));
    return NOERROR;
}

void RuimRecords::ResetRecords()
{
    mMncLength = UNINITIALIZED;
    Log(String("setting0 mMncLength") + StringUtils::ToString(mMncLength));
    mIccId = NULL;

    mAdnCache->Reset();

    SetSystemProperty(ITelephonyProperties::PROPERTY_APN_RUIM_OPERATOR_NUMERIC, String(""));

    // Don't clean up PROPERTY_ICC_OPERATOR_ISO_COUNTRY and
    // PROPERTY_ICC_OPERATOR_NUMERIC here. Since not all CDMA
    // devices have RUIM, these properties should keep the original
    // values, e.g. build time settings, when there is no RUIM but
    // set new values when RUIM is available and loaded.

    // recordsRequested is set to false indicating that the SIM
    // read requests made so far are not valid. This is set to
    // true only when fresh set of read requests are made.
    mRecordsRequested = FALSE;
}

ECode RuimRecords::OnRecordLoaded()
{
    // One record loaded successfully or failed, In either case
    // we need to update the recordsToLoad count
    mRecordsToLoad -= 1;
    if (DBG) {
        Log(String("onRecordLoaded ") + StringUtils::ToString(mRecordsToLoad)
            + String(" requested: ") + StringUtils::ToString(mRecordsRequested));
    }

    if (mRecordsToLoad == 0 && mRecordsRequested == TRUE) {
        OnAllRecordsLoaded();
    }
    else if (mRecordsToLoad < 0) {
        Loge(String("recordsToLoad <0, programmer error suspected"));
        mRecordsToLoad = 0;
    }
    return NOERROR;
}

ECode RuimRecords::OnAllRecordsLoaded()
{
    if (DBG) {
        Log(String("record load complete"));
    }

    // Further records that can be inserted are Operator/OEM dependent

    String opt;
    GetOperatorNumeric(&opt);
    if (!TextUtils::IsEmpty(opt)) {
        Log(String("onAllRecordsLoaded set 'gsm.sim.operator.numeric' to operator='") +
                opt + String("'"));
        SetSystemProperty(ITelephonyProperties::PROPERTY_ICC_OPERATOR_NUMERIC, opt);
        SetSystemProperty(ITelephonyProperties::PROPERTY_APN_RUIM_OPERATOR_NUMERIC, opt);
    }
    else {
        Log(String("onAllRecordsLoaded empty 'gsm.sim.operator.numeric' skipping"));
    }

    if (!TextUtils::IsEmpty(mImsi)) {
        Log(String("onAllRecordsLoaded set mcc imsi=") + mImsi);
        String str = mImsi.Substring(0,3);
        SetSystemProperty(ITelephonyProperties::PROPERTY_ICC_OPERATOR_ISO_COUNTRY,
                MccTable::CountryCodeForMcc(StringUtils::ParseInt32(str)));
    }
    else {
        Log(String("onAllRecordsLoaded empty imsi skipping setting mcc"));
    }

    SetLocaleFromCsim();
    AutoPtr<AsyncResult> ar = new AsyncResult(NULL, NULL, NULL);
    mRecordsLoadedRegistrants->NotifyRegistrants(ar);
    return NOERROR;
}

ECode RuimRecords::HandleFileUpdate(
    /* [in] */ Int32 efid)
{
    switch (efid) {
        case EF_ADN: {
            Log(String("SIM Refresh for EF_ADN"));
            mAdnCache->Reset();
            break;
        }
        default: {
            mAdnCache->Reset();
            FetchRuimRecords();
            break;
        }
    }
    return NOERROR;
}

ECode RuimRecords::Log(
    /* [in] */ const String& s)
{
    Logger::D(LOGTAG, String("[RuimRecords] ") + s);
    return NOERROR;
}

ECode RuimRecords::Loge(
    /* [in] */ const String& s)
{
    Logger::E(LOGTAG, String("[RuimRecords] ") + s);
    return NOERROR;
}

Int32 RuimRecords::DecodeImsiDigits(
    /* [in] */ Int32 digits,
    /* [in] */ Int32 length)
{
    // Per C.S0005 section 2.3.1.
    Int32 constant = 0;
    for (Int32 i = 0; i < length; i++ ) {
        constant = (constant * 10) + 1;
    }

    digits += constant;

    for (Int32 i = 0, denominator = 1; i < length; i++) {
        digits = ((digits / denominator) % 10 == 0) ? (digits - (10 * denominator)) : digits;
        denominator *= 10;
    }
    return digits;
}

String RuimRecords::DecodeImsi(
    /* [in] */ ArrayOf<Byte>* data)
{
    // Retrieve the MCC and digits 11 and 12
    Int32 mcc_data = ((0x03 & (*data)[9]) << 8) | (0xFF & (*data)[8]);
    Int32 mcc = DecodeImsiDigits(mcc_data, 3);
    Int32 digits_11_12_data = (*data)[6] & 0x7f;
    Int32 digits_11_12 = DecodeImsiDigits(digits_11_12_data, 2);

    // Retrieve 10 MIN digits
    Int32 first3digits = ((0x03 & (*data)[2]) << 8) + (0xFF & (*data)[1]);
    Int32 second3digits = (((0xFF & (*data)[5]) << 8) | (0xFF & (*data)[4])) >> 6;
    Int32 digit7 = 0x0F & ((*data)[4] >> 2);
    if (digit7 > 0x09) digit7 = 0;
    Int32 last3digits = ((0x03 & (*data)[4]) << 8) | (0xFF & (*data)[3]);

    first3digits = DecodeImsiDigits(first3digits, 3);
    second3digits = DecodeImsiDigits(second3digits, 3);
    last3digits = DecodeImsiDigits(last3digits, 3);

    AutoPtr<ILocaleHelper> lochlp;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&lochlp);
    AutoPtr<ILocale> us;
    lochlp->GetUS((ILocale**)&us);
    StringBuilder builder;
    assert(0 && "TODO");
    // builder.Append(StringUtils::Format(us, String("%03d"), mcc));
    // builder.Append(StringUtils::Format(us, String("%02d"), digits_11_12));
    // builder.Append(StringUtils::Format(us, String("%03d"), first3digits));
    // builder.Append(StringUtils::Format(us, String("%03d"), second3digits));
    // builder.Append(StringUtils::Format(us, String("%d"), digit7));
    // builder.Append(StringUtils::Format(us, String("%03d"), last3digits));
    return  builder.ToString();
}

void RuimRecords::OnGetCSimEprlDone(
    /* [in] */ AsyncResult* ar)
{
    // C.S0065 section 5.2.57 for EFeprl encoding
    // C.S0016 section 3.5.5 for PRL format.
    AutoPtr<IArrayList> pArr = IArrayList::Probe(((AsyncResult*)ar)->mResult);
    Int32 size = 0;
    pArr->GetSize(&size);
    AutoPtr<ArrayOf<Byte> > data = ArrayOf<Byte>::Alloc(size);
    for (Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> v;
        pArr->Get(i, (IInterface**)&v);
        IByte::Probe(v)->GetValue(&((*data)[i]));
    }
    if (DBG) {
        AutoPtr<IIccUtils> iccu;
        CIccUtils::AcquireSingleton((IIccUtils**)&iccu);
        String str;
        iccu->BytesToHexString(data, &str);
        Log(String("CSIM_EPRL=") + str);
    }

    // Only need the first 4 bytes of record
    if (data->GetLength() > 3) {
        Int32 prlId = (((*data)[2] & 0xFF) << 8) | ((*data)[3] & 0xFF);
        mPrlVersion = StringUtils::ToString(prlId);
    }
    if (DBG) Log(String("CSIM PRL version=") + mPrlVersion);
}

void RuimRecords::FetchOMHCardRecords(
    /* [in] */ Boolean isOMHCard)
{
    // OMH related events
    if (isOMHCard) {
        // Write device's software version on EF6F90 on startup.
        SetModel();
    }
}

AutoPtr<ArrayOf<String> > RuimRecords::GetAssetLanguages(
    /* [in] */ IContext* ctx)
{
    AutoPtr<IAssetManager> am;
    ctx->GetAssets((IAssetManager**)&am);
    AutoPtr<ArrayOf<String> > locales;
    am->GetLocales((ArrayOf<String>**)&locales);
    AutoPtr<ArrayOf<String> > localeLangs = ArrayOf<String>::Alloc(locales->GetLength());
    for (Int32 i = 0; i < locales->GetLength(); ++i) {
        String localeStr = (*locales)[i];
        Int32 separator = localeStr.ByteIndexOf('-');
        if (separator < 0) {
            (*localeLangs)[i] = localeStr;
        }
        else {
            (*localeLangs)[i] = localeStr.Substring(0, separator);
        }
    }

    return localeLangs;
}

String RuimRecords::FindBestLanguage(
    /* [in] */ ArrayOf<Byte>* languages)
{
    AutoPtr<ArrayOf<String> > assetLanguages = GetAssetLanguages(mContext);

    if ((languages == NULL) || (assetLanguages == NULL)) return String(NULL);

    // Each 2-bytes consists of one language
    for (Int32 i = 0; (i + 1) < languages->GetLength(); i += 2) {
        String lang;
        // try {
            // lang = String(languages, i, 2, "ISO-8859-1");
        lang = String(*languages, i, 2);
        // } catch(java.io.UnsupportedEncodingException e) {
        //     log("Failed to parse SIM language records");
        //     continue;
        // }

        for (Int32 j = 0; j < assetLanguages->GetLength(); j++) {
            if ((*assetLanguages)[j].Equals(lang)) {
                return lang;
            }
        }
    }

    // no match found. return NULL
    return String(NULL);
}

void RuimRecords::SetLocaleFromCsim()
{
    String prefLang(NULL);
    // check EFli then EFpl
    prefLang = FindBestLanguage(mEFli);

    if (prefLang == NULL) {
        prefLang = FindBestLanguage(mEFpl);
    }

    if (prefLang != NULL) {
        // check country code from SIM
        String imsi;
        GetIMSI(&imsi);
        String country(NULL);
        if (imsi != NULL) {
            country = MccTable::CountryCodeForMcc(
                                StringUtils::ParseInt32(imsi.Substring(0,3)));
        }
        Log(String("Setting locale to ") + prefLang + String("_") + country);
        MccTable::SetSystemLocale(mContext, prefLang, country);
    }
    else {
        Log(String("No suitable CSIM selected locale"));
    }
}

void RuimRecords::FetchRuimRecords()
{
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    Boolean mESNTrackerEnabled = FALSE;
    res->GetBoolean(
            R::bool_::config_telephony_ESN_Tracker_enabled, &mESNTrackerEnabled);
    /* Don't read records if we don't expect
     * anyone to ask for them
     *
     * If we have already requested records OR
     * records are not required by anyone OR
     * the app is not ready
     * then bail
     */
    Int32 state = 0;
    mParentApp->GetState(&state);
    if (mRecordsRequested || (mESNTrackerEnabled ? FALSE : !mRecordsRequired)
        || APPSTATE_READY != state) {
        if (DBG) {
            Log(String("fetchRuimRecords: Abort fetching records rRecordsRequested = ")
                        + StringUtils::ToString(mRecordsRequested)
                        + String(" state = ") + StringUtils::ToString(state)
                        + String(" required = ") + StringUtils::ToString(mRecordsRequired));
        }
        return;
    }

    mRecordsRequested = TRUE;

    if (DBG) {
        Log(String("fetchRuimRecords ") + StringUtils::ToString(mRecordsToLoad));
    }

    AutoPtr<IMessage> msg1;
    ObtainMessage(EVENT_GET_ICCID_DONE, (IMessage**)&msg1);
    mFh->LoadEFTransparent(EF_ICCID, msg1);
    mRecordsToLoad++;

    AutoPtr<EfPlLoaded> p1 = new EfPlLoaded(this);
    AutoPtr<IMessage> msg2;
    ObtainMessage(EVENT_GET_ICC_RECORD_DONE, (IObject*)p1.Get(), (IMessage**)&msg2);
    mFh->LoadEFTransparent(EF_PL, msg2);
    mRecordsToLoad++;

    AutoPtr<EfCsimLiLoaded> p2 = new EfCsimLiLoaded(this);
    AutoPtr<IMessage> msg3;
    ObtainMessage(EVENT_GET_ICC_RECORD_DONE, (IObject*)p2.Get(), (IMessage**)&msg3);
    mFh->LoadEFTransparent(EF_CSIM_LI, msg3);
    mRecordsToLoad++;

    AutoPtr<EfCsimSpnLoaded> p3 = new EfCsimSpnLoaded(this);
    AutoPtr<IMessage> msg4;
    ObtainMessage(EVENT_GET_ICC_RECORD_DONE, (IObject*)p3.Get(), (IMessage**)&msg4);
    mFh->LoadEFTransparent(EF_CSIM_SPN, msg4);
    mRecordsToLoad++;

    AutoPtr<EfCsimMdnLoaded> p4 = new EfCsimMdnLoaded(this);
    AutoPtr<IMessage> msg5;
    ObtainMessage(EVENT_GET_ICC_RECORD_DONE, (IObject*)p4.Get(), (IMessage**)&msg5);
    mFh->LoadEFLinearFixed(EF_CSIM_MDN, 1, msg5);
    mRecordsToLoad++;

    AutoPtr<EfCsimImsimLoaded> p5 = new EfCsimImsimLoaded(this);
    AutoPtr<IMessage> msg6;
    ObtainMessage(EVENT_GET_ICC_RECORD_DONE, (IObject*)p5.Get(), (IMessage**)&msg6);
    mFh->LoadEFTransparent(EF_CSIM_IMSIM, msg6);
    mRecordsToLoad++;

    AutoPtr<EfCsimCdmaHomeLoaded> p6 = new EfCsimCdmaHomeLoaded(this);
    AutoPtr<IMessage> msg7;
    ObtainMessage(EVENT_GET_ICC_RECORD_DONE, (IObject*)p6.Get(), (IMessage**)&msg7);
    mFh->LoadEFLinearFixedAll(EF_CSIM_CDMAHOME, msg7);
    mRecordsToLoad++;

    if (mESNTrackerEnabled) {
        AutoPtr<EfCsimModelLoaded> p7 = new EfCsimModelLoaded(this);
        AutoPtr<IMessage> msg8;
        ObtainMessage(EVENT_GET_ICC_RECORD_DONE, (IObject*)p7.Get(), (IMessage**)&msg8);
        mFh->LoadEFTransparent(EF_CSIM_MODEL, msg8);
        mRecordsToLoad++;

        AutoPtr<EfRuimModelLoaded> p8 = new EfRuimModelLoaded(this);
        AutoPtr<IMessage> msg9;
        ObtainMessage(EVENT_GET_ICC_RECORD_DONE, (IObject*)p8.Get(), (IMessage**)&msg9);
        mFh->LoadEFTransparent(EF_MODEL, msg9);
        mRecordsToLoad++;

        // We need to read CST table first to see if it is a CT/OMH/Legacy card
        AutoPtr<IMessage> msg10;
        ObtainMessage(EVENT_GET_RUIM_CST_DONE, (IMessage**)&msg10);
        mFh->LoadEFTransparent(IIccConstants::EF_CST, msg10);
        mRecordsToLoad++;

        AutoPtr<EfRuimIdLoaded> p9 = new EfRuimIdLoaded(this);
        AutoPtr<IMessage> msg11;
        ObtainMessage(EVENT_GET_ICC_RECORD_DONE, (IObject*)p9.Get(), (IMessage**)&msg11);
        mFh->LoadEFTransparent(EF_RUIM_ID, msg11);
        mRecordsToLoad++;
    }

    // Entire PRL could be huge. We are only interested in
    // the first 4 bytes of the record.
    AutoPtr<EfCsimEprlLoaded> p10 = new EfCsimEprlLoaded(this);
    AutoPtr<IMessage> msg12;
    ObtainMessage(EVENT_GET_ICC_RECORD_DONE, (IObject*)p10.Get(), (IMessage**)&msg12);
    mFh->LoadEFTransparent(EF_CSIM_EPRL, 4, msg12);
    mRecordsToLoad++;

    AutoPtr<IMessage> msg13;
    ObtainMessage(EVENT_GET_SMS_RECORD_SIZE_DONE, (IMessage**)&msg13);
    mFh->GetEFLinearRecordSize(EF_SMS, msg13);

    if (DBG) {
        Log(String("fetchRuimRecords ") + StringUtils::ToString(mRecordsToLoad)
            + String(" requested: ") + StringUtils::ToString(mRecordsRequested));
    }
    // Further records that can be inserted are Operator/OEM dependent
}

void RuimRecords::SetModel()
{
    AutoPtr<ArrayOf<Byte> > data = ArrayOf<Byte>::Alloc(EF_MODEL_FILE_SIZE);
    AutoPtr<ArrayOf<Byte> > model = NULL;
    AutoPtr<ArrayOf<Byte> > manufacturer = NULL;
    AutoPtr<ArrayOf<Byte> > softwareVersion = NULL;

    for (Int32 i = 0; i < data->GetLength(); i++) {
        (*data)[i] = (Byte) 0xff;
    }
    // EFmodel will be written into UTF-8 characters
    // try {
        model = Build::MODEL.GetBytes(); // "UTF-8";
        manufacturer = Build::MANUFACTURER.GetBytes(); // "UTF-8";
        AutoPtr<ISystemProperties> sp;
        CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
        String str;
        sp->Get(String("persist.product.sw.version"), Build::DISPLAY, &str);
        softwareVersion = str.GetBytes(); // "UTF-8"
    // } catch (UnsupportedEncodingException e) {
    //     loge("BearerData encode failed: " + e);
    // }
    (*data)[0] = IUserData::ENCODING_OCTET;
    (*data)[1] = LANGUAGE_INDICATOR_ENGLISH;

    // Restrict model length to prevent ArrayIndexOutOfBoundsException
    Int32 offset = 2;
    Int32 modelLen = (model->GetLength() > MODEL_INFORMATION_SIZE)
            ? MODEL_INFORMATION_SIZE : model->GetLength();
    // System::Arraycopy(model, 0, data, offset, modelLen);
    data->Copy(offset, model, 0, modelLen);
    offset += MODEL_INFORMATION_SIZE;
    // Restrict manufacture length to prevent ArrayIndexOutOfBoundsException
    Int32 manufactureLen = (manufacturer->GetLength() > MANUFACTURER_NAME_SIZE)
            ? MANUFACTURER_NAME_SIZE : manufacturer->GetLength();
    // System::Arraycopy(manufacturer, 0, data, offset, manufactureLen);
    data->Copy(offset, manufacturer, 0, manufactureLen);
    // We need to restrict versionLength in case it is too long which maybe lead to
    // arrayoutofindex exception
    offset += MANUFACTURER_NAME_SIZE;
    Int32 versionLength = (softwareVersion->GetLength() > SOFTWARE_VERSION_INFORMATION_SIZE)
            ? SOFTWARE_VERSION_INFORMATION_SIZE : softwareVersion->GetLength();
    // System::Arraycopy(softwareVersion, 0, data, offset, versionLength);
    data->Copy(offset, softwareVersion, 0, versionLength);

    AutoPtr<IIccUtils> iccu;
    CIccUtils::AcquireSingleton((IIccUtils**)&iccu);
    String strModel;
    iccu->BytesToHexString(model, &strModel);
    String strManufacturer;
    iccu->BytesToHexString(manufacturer, &strManufacturer);
    String strSoftwareVersion;
    iccu->BytesToHexString(softwareVersion, &strSoftwareVersion);
    String strData;
    iccu->BytesToHexString(data, &strData);
    Log(String("model: ") + strModel + String("manufacturer: ")
            + strManufacturer + String("softwareVersion: ")
            + strSoftwareVersion);
    Log(String("EF model write data : ") + strData
        + String(" version length=") + StringUtils::ToString(versionLength));

    if (mParentApp != NULL) {
        Int32 type = 0;
        mParentApp->GetType(&type);
        if (type == APPTYPE_CSIM) {
            Log(String("CSIM card type, set csim model"));
            AutoPtr<IMessage> msg;
            ObtainMessage(EVENT_SET_MODEL_DONE, CoreUtils::Convert(IIccConstants::EF_CSIM_MODEL), (IMessage**)&msg);
            mFh->UpdateEFTransparent(IIccConstants::EF_CSIM_MODEL, data, msg);
        }
    }
    else {
        AutoPtr<IMessage> msg;
        ObtainMessage(EVENT_SET_MODEL_DONE, CoreUtils::Convert(IIccConstants::EF_MODEL), (IMessage**)&msg);
        mFh->UpdateEFTransparent(IIccConstants::EF_MODEL, data,
                msg);
    }
}

Boolean RuimRecords::IsOnMatchingPlmn(
    /* [in] */ const String& plmnNumeric)
{
    if (plmnNumeric == NULL) return FALSE;
    String str;
    GetOperatorNumeric(&str);
    if (plmnNumeric.Equals(str)) {
        return TRUE;
    }
    return FALSE;
}

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
