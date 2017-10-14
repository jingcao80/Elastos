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

#include "Elastos.CoreLibrary.Utility.Concurrent.h"
#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/uicc/SIMRecords.h"
#include "elastos/droid/internal/telephony/uicc/UiccCardApplication.h"
#include "elastos/droid/content/res/CResourcesHelper.h"
#include "elastos/droid/internal/telephony/uicc/CIccUtils.h"
#include "elastos/droid/internal/telephony/uicc/CAdnRecordLoader.h"
#include "elastos/droid/internal/telephony/uicc/CAdnRecordCache.h"
#include "elastos/droid/internal/telephony/uicc/VoiceMailConstants.h"
#include "elastos/droid/internal/telephony/uicc/CSpnOverride.h"
#include "elastos/droid/internal/telephony/uicc/CAdnRecord.h"
#include "elastos/droid/internal/telephony/uicc/IccVmFixedException.h"
#include "elastos/droid/internal/telephony/uicc/IccVmNotSupportedException.h"
#include "elastos/droid/internal/telephony/uicc/CUsimServiceTable.h"
#include "elastos/droid/internal/telephony/gsm/CSimTlv.h"
#include "elastos/droid/internal/telephony/MccTable.h"
#include "elastos/droid/internal/telephony/CSubscriptionControllerHelper.h"
#include "elastos/droid/telephony/CPhoneNumberUtils.h"
#include "elastos/droid/telephony/CSmsMessageHelper.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/os/CSystemProperties.h"
#include "elastos/droid/R.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/Arrays.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::IResourcesHelper;
using Elastos::Droid::Content::Res::CResourcesHelper;
using Elastos::Droid::Content::Res::IAssetManager;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Internal::Telephony::MccTable;
using Elastos::Droid::Internal::Telephony::ISubscriptionControllerHelper;
using Elastos::Droid::Internal::Telephony::CSubscriptionControllerHelper;
using Elastos::Droid::Internal::Telephony::Gsm::ISimTlv;
using Elastos::Droid::Internal::Telephony::Gsm::CSimTlv;
using Elastos::Droid::Telephony::CPhoneNumberUtils;
using Elastos::Droid::Telephony::IPhoneNumberUtils;
using Elastos::Droid::Telephony::ISmsMessageHelper;
using Elastos::Droid::Telephony::CSmsMessageHelper;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::R;
using Elastos::Core::CoreUtils;
using Elastos::Core::IArrayOf;
using Elastos::Core::IByte;
using Elastos::Core::IInteger32;
using Elastos::Core::StringUtils;
using Elastos::Core::IThrowable;
using Elastos::IO::IFlushable;
using Elastos::Utility::Arrays;
using Elastos::Utility::IIterator;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

//=====================================================================
//                        SIMRecords::EfPlLoaded
//=====================================================================
CAR_INTERFACE_IMPL(SIMRecords::EfPlLoaded, Object, IIccRecordLoaded);

SIMRecords::EfPlLoaded::EfPlLoaded(
    /* [in] */ SIMRecords* host)
    : mHost(host)
{}

ECode SIMRecords::EfPlLoaded::GetEfName(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = String("EF_PL");
    return NOERROR;
}

ECode SIMRecords::EfPlLoaded::OnRecordLoaded(
    /* [in] */ IAsyncResult* ar)
{
    AutoPtr<IArrayOf> array = IArrayOf::Probe(((AsyncResult*)ar)->mResult);
    assert(array != NULL);
    Int32 len = 0;
    array->GetLength(&len);
    mHost->mEfPl = ArrayOf<Byte>::Alloc(len);
    for (Int32 i = 0; i < len; i++) {
        AutoPtr<IInterface> obj;
        array->Get(i, (IInterface**)&obj);
        IByte::Probe(obj)->GetValue(&(*mHost->mEfPl)[i]);
    }

    if (DBG) {
        AutoPtr<IIccUtils> iccu;
        CIccUtils::AcquireSingleton((IIccUtils**)&iccu);
        String str;
        iccu->BytesToHexString(mHost->mEfPl, &str);
        mHost->Log(String("EF_PL=") + str);
    }
    return NOERROR;
}

//=====================================================================
//                      SIMRecords::EfUsimLiLoaded
//=====================================================================
CAR_INTERFACE_IMPL(SIMRecords::EfUsimLiLoaded, Object, IIccRecordLoaded);

SIMRecords::EfUsimLiLoaded::EfUsimLiLoaded(
    /* [in] */ SIMRecords* host)
    : mHost(host)
{}

ECode SIMRecords::EfUsimLiLoaded::GetEfName(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = String("EF_LI");
    return NOERROR;
}

ECode SIMRecords::EfUsimLiLoaded::OnRecordLoaded(
    /* [in] */ IAsyncResult* ar)
{
    AutoPtr<IArrayOf> array = IArrayOf::Probe(((AsyncResult*)ar)->mResult);
    assert(array != NULL);
    Int32 len = 0;
    array->GetLength(&len);
    mHost->mEfLi = ArrayOf<Byte>::Alloc(len);
    for (Int32 i = 0; i < len; i++) {
        AutoPtr<IInterface> obj;
        array->Get(i, (IInterface**)&obj);
        IByte::Probe(obj)->GetValue(&(*mHost->mEfLi)[i]);
    }
    if (DBG) {
        AutoPtr<IIccUtils> iccu;
        CIccUtils::AcquireSingleton((IIccUtils**)&iccu);
        String str;
        iccu->BytesToHexString(mHost->mEfLi, &str);
        mHost->Log(String("EF_LI=") + str);
    }
    return NOERROR;
}

//=====================================================================
//                              SIMRecords
//=====================================================================
CAR_INTERFACE_IMPL(SIMRecords, IccRecords, ISIMRecords);

const Int32 SIMRecords::TAG_SPDI;
const Int32 SIMRecords::TAG_SPDI_PLMN_LIST;
const Int32 SIMRecords::TAG_FULL_NETWORK_NAME;
const Int32 SIMRecords::TAG_SHORT_NETWORK_NAME;
const Int32 SIMRecords::CFF_UNCONDITIONAL_ACTIVE;
const Int32 SIMRecords::CFF_UNCONDITIONAL_DEACTIVE;
const Int32 SIMRecords::CFF_LINE1_MASK;
const Int32 SIMRecords::CFF_LINE1_RESET;
const String SIMRecords::LOGTAG("SIMRecords");
const Int32 SIMRecords::EVENT_GET_AD_DONE;
const Int32 SIMRecords::EVENT_GET_MSISDN_DONE;
const Int32 SIMRecords::EVENT_GET_SST_DONE;
const Boolean SIMRecords::CRASH_RIL = FALSE;
const Int32 SIMRecords::CPHS_SST_MBN_MASK;
const Int32 SIMRecords::CPHS_SST_MBN_ENABLED;
const Int32 SIMRecords::CFIS_BCD_NUMBER_LENGTH_OFFSET;
const Int32 SIMRecords::CFIS_TON_NPI_OFFSET;
const Int32 SIMRecords::CFIS_ADN_CAPABILITY_ID_OFFSET;
const Int32 SIMRecords::CFIS_ADN_EXTENSION_ID_OFFSET;
const Int32 SIMRecords::EVENT_GET_IMSI_DONE;
const Int32 SIMRecords::EVENT_GET_ICCID_DONE;
const Int32 SIMRecords::EVENT_GET_MBI_DONE;
const Int32 SIMRecords::EVENT_GET_MBDN_DONE;
const Int32 SIMRecords::EVENT_GET_MWIS_DONE;
const Int32 SIMRecords::EVENT_GET_VOICE_MAIL_INDICATOR_CPHS_DONE;
const Int32 SIMRecords::EVENT_GET_CPHS_MAILBOX_DONE;
const Int32 SIMRecords::EVENT_GET_SPN_DONE;
const Int32 SIMRecords::EVENT_GET_SPDI_DONE;
const Int32 SIMRecords::EVENT_UPDATE_DONE;
const Int32 SIMRecords::EVENT_GET_PNN_DONE;
const Int32 SIMRecords::EVENT_GET_ALL_SMS_DONE;
const Int32 SIMRecords::EVENT_MARK_SMS_READ_DONE;
const Int32 SIMRecords::EVENT_SET_MBDN_DONE;
const Int32 SIMRecords::EVENT_GET_SMS_DONE;
const Int32 SIMRecords::EVENT_GET_CFF_DONE;
const Int32 SIMRecords::EVENT_SET_CPHS_MAILBOX_DONE;
const Int32 SIMRecords::EVENT_GET_INFO_CPHS_DONE;
const Int32 SIMRecords::EVENT_GET_CFIS_DONE;
const Int32 SIMRecords::EVENT_GET_CSP_CPHS_DONE;
const Int32 SIMRecords::EVENT_GET_GID1_DONE;
const Int32 SIMRecords::EVENT_APP_LOCKED;
AutoPtr<ArrayOf<String> > SIMRecords::MCCMNC_CODES_HAVING_3DIGITS_MNC = SIMRecords::MiddleInitMccmncCodesHaving3digitsMnc();
SIMRecords::SIMRecords()
{
}

ECode SIMRecords::constructor(
    /* [in] */ IUiccCardApplication* app,
    /* [in] */ IContext* c,
    /* [in] */ ICommandsInterface* ci)
{
    IccRecords::constructor(app, c, ci);

    CAdnRecordCache::New(mFh, (IAdnRecordCache**)&mAdnCache);

    mVmConfig = new VoiceMailConstants();
    CSpnOverride::New((ISpnOverride**)&mSpnOverride);

    mRecordsRequested = FALSE;  // No load request is made till SIM ready

    // recordsToLoad is set to 0 because no requests are made yet
    mRecordsToLoad = 0;

    // Start off by setting empty state
    ResetRecords();
    mParentApp->RegisterForReady(this, EVENT_APP_READY, NULL);
    mParentApp->RegisterForLocked(this, EVENT_APP_LOCKED, NULL);
    if (DBG) {
        Log(String("SIMRecords X ctor this=") + TO_CSTR(this));
    }
    return NOERROR;
}

ECode SIMRecords::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    String str, strSuper;
    IccRecords::ToString(&strSuper);
    GetOperatorNumeric(&str);
    *result = String("SimRecords: ") + strSuper
            + String(" mVmConfig") + TO_CSTR(mVmConfig)
            + String(" mSpnOverride=") + String("mSpnOverride")
            + String(" callForwardingEnabled=") + StringUtils::ToString(mCallForwardingEnabled)
            + String(" spnState=") + StringUtils::ToString(mSpnState)
            + String(" mCphsInfo=")// + Arrays::ToString(mCphsInfo)
            + String(" mCspPlmnEnabled=") + StringUtils::ToString(mCspPlmnEnabled)
            + String(" efMWIS=")// + Arrays::ToString(mEfMWIS)
            + String(" efCPHS_MWI=")// + Arrays::ToString(mEfCPHS_MWI)
            + String(" mEfCff=")// + Arrays::ToString(mEfCff)
            + String(" mEfCfis=")// + Arrays::ToString(mEfCfis)
            + String(" getOperatorNumeric=") + str;
    return NOERROR;
}

ECode SIMRecords::Dispose()
{
    if (DBG) {
        Log(String("Disposing SIMRecords this=") + TO_CSTR(this));
    }
    //Unregister for all events
    mParentApp->UnregisterForReady(this);
    mParentApp->UnregisterForLocked(this);
    ResetRecords();
    IccRecords::Dispose();
    return NOERROR;
}

ECode SIMRecords::GetIMSI(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mImsi;
    return NOERROR;
}

ECode SIMRecords::GetMsisdnNumber(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mMsisdn;
    return NOERROR;
}

ECode SIMRecords::GetGid1(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mGid1;
    return NOERROR;
}

ECode SIMRecords::GetUsimServiceTable(
    /* [out] */ IUsimServiceTable** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mUsimServiceTable;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode SIMRecords::SetMsisdnNumber(
    /* [in] */ const String& alphaTag,
    /* [in] */ const String& number,
    /* [in] */ IMessage* onComplete)
{
    mMsisdn = number;
    mMsisdnTag = alphaTag;

    if(DBG) {
        Log(String("Set MSISDN: ") + mMsisdnTag
            + String(" ") + /*mMsisdn*/ String("xxxxxxx"));
    }

    AutoPtr<IAdnRecord> adn;
    CAdnRecord::New(mMsisdnTag, mMsisdn, (IAdnRecord**)&adn);

    AutoPtr<IAdnRecordLoader> loader;
    CAdnRecordLoader::New(mFh, (IAdnRecordLoader**)&loader);
    AutoPtr<IMessage> msg;
    ObtainMessage(EVENT_SET_MSISDN_DONE, onComplete, (IMessage**)&msg);
    loader->UpdateEF(adn, EF_MSISDN, GetExtFromEf(EF_MSISDN), 1, String(NULL),
            msg);
    return NOERROR;
}

ECode SIMRecords::GetMsisdnAlphaTag(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mMsisdnTag;
    return NOERROR;
}

ECode SIMRecords::GetVoiceMailNumber(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mVoiceMailNum;
    return NOERROR;
}

ECode SIMRecords::SetVoiceMailNumber(
    /* [in] */ const String& alphaTag,
    /* [in] */ const String& voiceNumber,
    /* [in] */ IMessage* onComplete)
{
    if (mIsVoiceMailFixed) {
        AutoPtr<IccVmFixedException> p =
            new IccVmFixedException(
                String("Voicemail number is fixed by operator"));
        AsyncResult::ForMessage((onComplete))->mException =
            IThrowable::Probe(p);
        onComplete->SendToTarget();
        return NOERROR;
    }

    mNewVoiceMailNum = voiceNumber;
    mNewVoiceMailTag = alphaTag;

    AutoPtr<IAdnRecord> adn;
    CAdnRecord::New(mNewVoiceMailTag, mNewVoiceMailNum, (IAdnRecord**)&adn);

    if (mMailboxIndex != 0 && mMailboxIndex != 0xff) {
        AutoPtr<IAdnRecordLoader> loader;
        CAdnRecordLoader::New(mFh, (IAdnRecordLoader**)&loader);
        AutoPtr<IMessage> msg;
        ObtainMessage(EVENT_SET_MBDN_DONE, onComplete, (IMessage**)&msg);
        loader->UpdateEF(adn, EF_MBDN, EF_EXT6,
                mMailboxIndex, String(NULL),
                msg);

    }
    else if (IsCphsMailboxEnabled()) {
        AutoPtr<IAdnRecordLoader> loader;
        CAdnRecordLoader::New(mFh, (IAdnRecordLoader**)&loader);
        AutoPtr<IMessage> msg;
        ObtainMessage(EVENT_SET_CPHS_MAILBOX_DONE, onComplete, (IMessage**)&msg);
        loader->UpdateEF(adn, EF_MAILBOX_CPHS,
                EF_EXT1, 1, String(NULL),
                msg);

    }
    else {
        AutoPtr<IccVmNotSupportedException> p =
            new IccVmNotSupportedException(
                String("Update SIM voice mailbox error"));
        AsyncResult::ForMessage((onComplete))->mException =
            IThrowable::Probe(p);

        onComplete->SendToTarget();
    }
    return NOERROR;
}

ECode SIMRecords::GetVoiceMailAlphaTag(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mVoiceMailTag;
    return NOERROR;
}

ECode SIMRecords::SetVoiceMessageWaiting(
    /* [in] */ Int32 line,
    /* [in] */ Int32 countWaiting)
{
    if (line != 1) {
        // only profile 1 is supported
        return NOERROR;
    }

    // try {
        if (mEfMWIS != NULL) {
            // TS 51.011 10.3.45

            // lsb of byte 0 is 'voicemail' status
            (*mEfMWIS)[0] = (Byte)(((*mEfMWIS)[0] & 0xfe)
                                | (countWaiting == 0 ? 0 : 1));

            // byte 1 is the number of voice messages waiting
            if (countWaiting < 0) {
                // The spec does not define what this should be
                // if we don't know the count
                (*mEfMWIS)[1] = 0;
            }
            else {
                (*mEfMWIS)[1] = (Byte) countWaiting;
            }

            AutoPtr<IMessage> msg;
            ObtainMessage(EVENT_UPDATE_DONE, EF_MWIS, 0, (IMessage**)&msg);
            mFh->UpdateEFLinearFixed(
                EF_MWIS, 1, mEfMWIS, String(NULL),
                msg);
        }

        if (mEfCPHS_MWI != NULL) {
                // Refer CPHS4_2.WW6 B4.2.3
            (*mEfCPHS_MWI)[0] = (Byte)(((*mEfCPHS_MWI)[0] & 0xf0)
                        | (countWaiting == 0 ? 0x5 : 0xa));
            AutoPtr<IMessage> msg;
            ObtainMessage(EVENT_UPDATE_DONE, CoreUtils::Convert(EF_VOICE_MAIL_INDICATOR_CPHS), (IMessage**)&msg);
            mFh->UpdateEFTransparent(
                EF_VOICE_MAIL_INDICATOR_CPHS, mEfCPHS_MWI,
                msg);
        }
    // } catch (ArrayIndexOutOfBoundsException ex) {
    //     logw("Error saving voice mail state to SIM. Probably malformed SIM record", ex);
    // }
    return NOERROR;
}

ECode SIMRecords::GetVoiceMessageCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean voiceMailWaiting = FALSE;
    Int32 countVoiceMessages = -1;
    if (mEfMWIS != NULL) {
        // Use this data if the EF[MWIS] exists and
        // has been loaded
        // Refer TS 51.011 Section 10.3.45 for the content description
        voiceMailWaiting = (((*mEfMWIS)[0] & 0x01) != 0);
        countVoiceMessages = (*mEfMWIS)[1] & 0xff;

        if (voiceMailWaiting && countVoiceMessages == 0) {
            // Unknown count = -1
            countVoiceMessages = -1;
        }
        if (DBG) Log(String(" VoiceMessageCount from SIM MWIS = ") + StringUtils::ToString(countVoiceMessages));
    }
    else if (mEfCPHS_MWI != NULL) {
        // use voice mail count from CPHS
        Int32 indicator = (Int32) ((*mEfCPHS_MWI)[0] & 0xf);

        // Refer CPHS4_2.WW6 B4.2.3
        if (indicator == 0xA) {
            // Unknown count = -1
            countVoiceMessages = -1;
        }
        else if (indicator == 0x5) {
            countVoiceMessages = 0;
        }
        if (DBG) Log(String(" VoiceMessageCount from SIM CPHS = ") + StringUtils::ToString(countVoiceMessages));
    }
    *result = countVoiceMessages;
    return NOERROR;
}

ECode SIMRecords::IsCallForwardStatusStored(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = (mEfCfis != NULL) || (mEfCff != NULL);
    return NOERROR;
}

ECode SIMRecords::GetVoiceCallForwardingFlag(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mCallForwardingEnabled;
    return NOERROR;
}

ECode SIMRecords::SetVoiceCallForwardingFlag(
    /* [in] */ Int32 line,
    /* [in] */ Boolean enable,
    /* [in] */ const String& dialNumber)
{
    if (line != 1) return NOERROR; // only line 1 is supported

    mCallForwardingEnabled = enable;

    mRecordsEventsRegistrants->NotifyResult(CoreUtils::Convert(EVENT_CFI));

    AutoPtr<IIccUtils> iccu;
    CIccUtils::AcquireSingleton((IIccUtils**)&iccu);
    // try {
        if (ValidEfCfis(mEfCfis)) {
            // lsb is of byte 1 is voice status
            if (enable) {
                (*mEfCfis)[1] |= 1;
            }
            else {
                (*mEfCfis)[1] &= 0xfe;
            }

            String str;
            iccu->BytesToHexString(mEfCfis, &str);
            Log(String("setVoiceCallForwardingFlag: enable=") + StringUtils::ToString(enable)
                    + String(" mEfCfis=") + str);

            // Update dialNumber if not empty and CFU is enabled.
            // Spec reference for EF_CFIS contents, TS 51.011 section 10.3.46.
            if (enable && !TextUtils::IsEmpty(dialNumber)) {
                Log(String("EF_CFIS: updating cf number, ") + dialNumber);
                AutoPtr<IPhoneNumberUtils> pn;
                CPhoneNumberUtils::AcquireSingleton((IPhoneNumberUtils**)&pn);
                AutoPtr<ArrayOf<Byte> > bcdNumber;
                pn->NumberToCalledPartyBCD(dialNumber, (ArrayOf<Byte>**)&bcdNumber);

                // System::Arraycopy(bcdNumber, 0, mEfCfis, CFIS_TON_NPI_OFFSET, bcdNumber->GetLength());
                mEfCfis->Copy(CFIS_TON_NPI_OFFSET, bcdNumber, 0, bcdNumber->GetLength());

                (*mEfCfis)[CFIS_BCD_NUMBER_LENGTH_OFFSET] = (Byte) (bcdNumber->GetLength());
                (*mEfCfis)[CFIS_ADN_CAPABILITY_ID_OFFSET] = (Byte) 0xFF;
                (*mEfCfis)[CFIS_ADN_EXTENSION_ID_OFFSET] = (Byte) 0xFF;
            }

            AutoPtr<IMessage> msg;
            ObtainMessage(EVENT_UPDATE_DONE, CoreUtils::Convert(EF_CFIS), (IMessage**)&msg);
            mFh->UpdateEFLinearFixed(
                    EF_CFIS, 1, mEfCfis, String(NULL),
                    msg);
        }
        else {
            String str;
            iccu->BytesToHexString(mEfCfis, &str);
            Log(String("setVoiceCallForwardingFlag: ignoring enable=") + StringUtils::ToString(enable)
                    + String(" invalid mEfCfis=") + str);
        }

        if (mEfCff != NULL) {
            if (enable) {
                (*mEfCff)[0] = (Byte) (((*mEfCff)[0] & CFF_LINE1_RESET)
                        | CFF_UNCONDITIONAL_ACTIVE);
            }
            else {
                (*mEfCff)[0] = (Byte) (((*mEfCff)[0] & CFF_LINE1_RESET)
                        | CFF_UNCONDITIONAL_DEACTIVE);
            }

            AutoPtr<IMessage> msg;
            ObtainMessage(EVENT_UPDATE_DONE, CoreUtils::Convert(EF_CFF_CPHS), (IMessage**)&msg);
            mFh->UpdateEFTransparent(
                    EF_CFF_CPHS, mEfCff,
                    msg);
        }
    // } catch (ArrayIndexOutOfBoundsException ex) {
    //     logw("Error saving call forwarding flag to SIM. "
    //                     + "Probably malformed SIM record", ex);

    // }
    return NOERROR;
}

ECode SIMRecords::OnRefresh(
    /* [in] */ Boolean fileChanged,
    /* [in] */ ArrayOf<Int32>* fileList)
{
    if (fileChanged) {
        // A future optimization would be to inspect fileList and
        // only reload those files that we care about.  For now,
        // just re-fetch all SIM records that we cache.
        FetchSimRecords();
    }
    return NOERROR;
}

ECode SIMRecords::GetOperatorNumeric(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    if (mImsi == NULL) {
        Log(String("getOperatorNumeric: IMSI == NULL"));
        *result = String(NULL);
        return NOERROR;
    }
    if (mMncLength == UNINITIALIZED || mMncLength == UNKNOWN) {
        Log(String("getSIMOperatorNumeric: bad mncLength"));
        *result = String(NULL);
        return NOERROR;
    }

    // Length = length of MCC + length of MNC
    // length of mcc = 3 (TS 23.003 Section 2.2)
    *result = mImsi.Substring(0, 3 + mMncLength);
    return NOERROR;
}

ECode SIMRecords::HandleMessage(
    /* [in] */ IMessage* msg)
{
    AutoPtr<AsyncResult> ar;
    AutoPtr<AdnRecord> adn;

    AutoPtr<ArrayOf<Byte> > data;

    Boolean isRecordLoadResponse = FALSE;

    Int32 what = 0;
    msg->GetWhat(&what);
    if (mDestroyed.Get()) {
        Loge(String("Received message ") + TO_CSTR(msg) +
            String("[") + StringUtils::ToString(what) + String("] ") +
            String(" while being destroyed. Ignoring."));
        return NOERROR;
    }

    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    Int32 arg1 = 0;
    msg->GetArg1(&arg1);
    AutoPtr<IIccUtils> iccu;
    CIccUtils::AcquireSingleton((IIccUtils**)&iccu);
    // try {
    switch (what) {
        case EVENT_APP_READY: {
            OnReady();
            break;
        }
        case EVENT_APP_LOCKED: {
            OnLocked();
            break;
        }
        /* IO events */
        case EVENT_GET_IMSI_DONE: {
            isRecordLoadResponse = TRUE;

            ar = (AsyncResult*)IAsyncResult::Probe(obj);

            if (ar->mException != NULL) {
                Loge(String("Exception querying IMSI, Exception:") + TO_CSTR(ar->mException));
                break;
            }

            ICharSequence::Probe(ar->mResult)->ToString(&mImsi);

            // IMSI (MCC+MNC+MSIN) is at least 6 digits, but not more
            // than 15 (and usually 15).
            if (!mImsi.IsNull() && (mImsi.GetLength() < 6 || mImsi.GetLength() > 15)) {
                Loge(String("invalid IMSI ") + mImsi);
                mImsi = String(NULL);
            }

            Log(String("IMSI: mMncLength=") + StringUtils::ToString(mMncLength));
            Log(String("IMSI: ") + mImsi.Substring(0, 6) + String("xxxxxxx"));

            if (((mMncLength == UNKNOWN) || (mMncLength == 2)) &&
                    ((!mImsi.IsNull()) && (mImsi.GetLength() >= 6))) {
                String mccmncCode = mImsi.Substring(0, 6);
                for (Int32 i = 0; i < MCCMNC_CODES_HAVING_3DIGITS_MNC->GetLength(); i++) {
                    String mccmnc = (*MCCMNC_CODES_HAVING_3DIGITS_MNC)[i];
                    if (mccmnc.Equals(mccmncCode)) {
                        mMncLength = 3;
                        Log(String("IMSI: setting1 mMncLength=") + StringUtils::ToString(mMncLength));
                        break;
                    }
                }
            }

            if (mMncLength == UNKNOWN) {
                // the SIM has told us all it knows, but it didn't know the mnc length.
                // guess using the mcc
                // try {
                    Int32 mcc = StringUtils::ParseInt32(mImsi.Substring(0,3));
                    mMncLength = MccTable::SmallestDigitsMccForMnc(mcc);
                    Log(String("setting2 mMncLength=") + StringUtils::ToString(mMncLength));
                // } catch (NumberFormatException e) {
                //     mMncLength = UNKNOWN;
                //     loge("Corrupt IMSI! setting3 mMncLength=" + mMncLength);
                // }
            }

            AutoPtr<IUICCConfig> uicc;
            mParentApp->GetUICCConfig((IUICCConfig**)&uicc);
            uicc->SetImsi(mImsi);
            if (mMncLength == UNKNOWN || mMncLength == UNINITIALIZED) {
                // We need to default to something that seems common
                uicc->SetMncLength(3);
            }
            else {
                uicc->SetMncLength(mMncLength);
            }

            if (mMncLength != UNKNOWN && mMncLength != UNINITIALIZED) {
                Log(String("update mccmnc=") + mImsi.Substring(0, 3 + mMncLength));
                // finally have both the imsi and the mncLength and can parse the imsi properly
                MccTable::UpdateMccMncConfiguration(mContext,
                        mImsi.Substring(0, 3 + mMncLength), FALSE);
            }
            mImsiReadyRegistrants->NotifyRegistrants();
        break;
        }
        case EVENT_GET_MBI_DONE: {
            Boolean isValidMbdn;
            isRecordLoadResponse = TRUE;

            ar = (AsyncResult*)IAsyncResult::Probe(obj);
            AutoPtr<IArrayOf> array = IArrayOf::Probe(ar->mResult);
            assert(array != NULL);
            Int32 len = 0;
            array->GetLength(&len);
            data = ArrayOf<Byte>::Alloc(len);
            for (Int32 i = 0; i < len; i++) {
                AutoPtr<IInterface> obj;
                array->Get(i, (IInterface**)&obj);
                IByte::Probe(obj)->GetValue(&(*data)[i]);
            }

            isValidMbdn = FALSE;
            if (ar->mException == NULL) {
                // Refer TS 51.011 Section 10.3.44 for content details
                String str;
                iccu->BytesToHexString(data, &str);
                Log(String("EF_MBI: ") + str);

                // Voice mail record number stored first
                mMailboxIndex = (*data)[0] & 0xff;

                // check if dailing numbe id valid
                if (mMailboxIndex != 0 && mMailboxIndex != 0xff) {
                    Log(String("Got valid mailbox number for MBDN"));
                    isValidMbdn = TRUE;
                }
            }

            // one more record to load
            mRecordsToLoad += 1;

            if (isValidMbdn) {
                // Note: MBDN was not included in NUM_OF_SIM_RECORDS_LOADED
                AutoPtr<IAdnRecordLoader> loader;
                CAdnRecordLoader::New(mFh, (IAdnRecordLoader**)&loader);
                AutoPtr<IMessage> msg;
                ObtainMessage(EVENT_GET_MBDN_DONE, (IMessage**)&msg);
                loader->LoadFromEF(EF_MBDN, EF_EXT6,
                        mMailboxIndex, msg);
            }
            else {
                // If this EF not present, try mailbox as in CPHS standard
                // CPHS (CPHS4_2.WW6) is a european standard.
                AutoPtr<IAdnRecordLoader> loader;
                CAdnRecordLoader::New(mFh, (IAdnRecordLoader**)&loader);
                AutoPtr<IMessage> msg;
                ObtainMessage(EVENT_GET_CPHS_MAILBOX_DONE, (IMessage**)&msg);
                loader->LoadFromEF(EF_MAILBOX_CPHS,
                        EF_EXT1, 1,
                        msg);
            }

            break;
        }
        case EVENT_GET_CPHS_MAILBOX_DONE:
        case EVENT_GET_MBDN_DONE: {
            //Resetting the voice mail number and voice mail tag to NULL
            //as these should be updated from the data read from EF_MBDN.
            //If they are not reset, incase of invalid data/exception these
            //variables are retaining their previous values and are
            //causing invalid voice mailbox info display to user.
            mVoiceMailNum = NULL;
            mVoiceMailTag = NULL;
            isRecordLoadResponse = TRUE;

            ar = (AsyncResult*)IAsyncResult::Probe(obj);

            if (ar->mException != NULL) {

                Log(String("Invalid or missing EF")
                    + ((what == EVENT_GET_CPHS_MAILBOX_DONE) ? String("[MAILBOX]") : String("[MBDN]")));

                // Bug #645770 fall back to CPHS
                // FIXME should use SST to decide

                if (what == EVENT_GET_MBDN_DONE) {
                    //load CPHS on fail...
                    // FIXME right now, only load line1's CPHS voice mail entry

                    mRecordsToLoad += 1;
                    AutoPtr<IAdnRecordLoader> loader;
                    CAdnRecordLoader::New(mFh, (IAdnRecordLoader**)&loader);
                    AutoPtr<IMessage> msg;
                    ObtainMessage(EVENT_GET_CPHS_MAILBOX_DONE, (IMessage**)&msg);
                    loader->LoadFromEF(
                            EF_MAILBOX_CPHS, EF_EXT1, 1,
                            msg);
                }
                break;
            }

            adn = (AdnRecord*)IAdnRecord::Probe(ar->mResult);

            Log(String("VM: ") + TO_CSTR(adn) +
                    ((what == EVENT_GET_CPHS_MAILBOX_DONE) ? String(" EF[MAILBOX]") : String(" EF[MBDN]")));

            Boolean bEmp = FALSE;
            adn->IsEmpty(&bEmp);
            if (bEmp && what == EVENT_GET_MBDN_DONE) {
                // Bug #645770 fall back to CPHS
                // FIXME should use SST to decide
                // FIXME right now, only load line1's CPHS voice mail entry
                mRecordsToLoad += 1;
                AutoPtr<IAdnRecordLoader> loader;
                CAdnRecordLoader::New(mFh, (IAdnRecordLoader**)&loader);
                AutoPtr<IMessage> msg;
                ObtainMessage(EVENT_GET_CPHS_MAILBOX_DONE, (IMessage**)&msg);
                loader->LoadFromEF(
                        EF_MAILBOX_CPHS, EF_EXT1, 1,
                        msg);

                break;
            }

            adn->GetNumber(&mVoiceMailNum);
            adn->GetAlphaTag(&mVoiceMailTag);
        break;
        }
        case EVENT_GET_MSISDN_DONE: {
            isRecordLoadResponse = TRUE;

            ar = (AsyncResult*)IAsyncResult::Probe(obj);

            if (ar->mException != NULL) {
                Log(String("Invalid or missing EF[MSISDN]"));
                break;
            }

            adn = (AdnRecord*)IAdnRecord::Probe(ar->mResult);

            adn->GetNumber(&mMsisdn);
            adn->GetAlphaTag(&mMsisdnTag);

            Log(String("MSISDN: ") + /*mMsisdn*/ String("xxxxxxx"));
        break;
        }
        case EVENT_SET_MSISDN_DONE: {
            isRecordLoadResponse = FALSE;
            ar = (AsyncResult*)IAsyncResult::Probe(obj);

            if (ar->mUserObj != NULL) {
                AsyncResult::ForMessage(IMessage::Probe(ar->mUserObj))->mException
                        = ar->mException;
                IMessage::Probe(ar->mUserObj)->SendToTarget();
            }
            break;
        }
        case EVENT_GET_MWIS_DONE: {
            isRecordLoadResponse = TRUE;

            ar = (AsyncResult*)IAsyncResult::Probe(obj);
            AutoPtr<IArrayOf> array = IArrayOf::Probe(ar->mResult);
            assert(array != NULL);
            Int32 len = 0;
            array->GetLength(&len);
            data = ArrayOf<Byte>::Alloc(len);
            for (Int32 i = 0; i < len; i++) {
                AutoPtr<IInterface> obj;
                array->Get(i, (IInterface**)&obj);
                IByte::Probe(obj)->GetValue(&(*data)[i]);
            }

            if (DBG) {
                String str;
                iccu->BytesToHexString(data, &str);
                Log(String("EF_MWIS : ") + str);
            }

            if (ar->mException != NULL) {
                if (DBG) {
                    Log(String("EVENT_GET_MWIS_DONE exception = ") + TO_CSTR(ar->mException));
                }
                break;
            }

            if (((*data)[0] & 0xff) == 0xff) {
                if (DBG) Log(String("SIMRecords: Uninitialized record MWIS"));
                break;
            }

            mEfMWIS = data;
            break;
        }
        case EVENT_GET_VOICE_MAIL_INDICATOR_CPHS_DONE: {
            isRecordLoadResponse = TRUE;

            ar = (AsyncResult*)IAsyncResult::Probe(obj);
            AutoPtr<IArrayOf> array = IArrayOf::Probe(ar->mResult);
            assert(array != NULL);
            Int32 len = 0;
            array->GetLength(&len);
            data = ArrayOf<Byte>::Alloc(len);
            for (Int32 i = 0; i < len; i++) {
                AutoPtr<IInterface> obj;
                array->Get(i, (IInterface**)&obj);
                IByte::Probe(obj)->GetValue(&(*data)[i]);
            }

            if (DBG) {
                String str;
                iccu->BytesToHexString(data, &str);
                Log(String("EF_CPHS_MWI: ") + str);
            }

            if (ar->mException != NULL) {
                if (DBG) {
                    Log(String("EVENT_GET_VOICE_MAIL_INDICATOR_CPHS_DONE exception = ") + TO_CSTR(ar->mException));
                }
                break;
            }

            mEfCPHS_MWI = data;
            break;
        }
        case EVENT_GET_ICCID_DONE: {
            isRecordLoadResponse = TRUE;

            ar = (AsyncResult*)IAsyncResult::Probe(obj);
            AutoPtr<IArrayOf> array = IArrayOf::Probe(ar->mResult);
            assert(array != NULL);
            Int32 len = 0;
            array->GetLength(&len);
            data = ArrayOf<Byte>::Alloc(len);
            for (Int32 i = 0; i < len; i++) {
                AutoPtr<IInterface> obj;
                array->Get(i, (IInterface**)&obj);
                IByte::Probe(obj)->GetValue(&(*data)[i]);
            }

            if (ar->mException != NULL) {
                break;
            }

            iccu->BcdToString(data, 0, data->GetLength(), &mIccId);

            Log(String("iccid: ") + mIccId);

        break;
        }
        case EVENT_GET_AD_DONE: {
            // try {
            isRecordLoadResponse = TRUE;

            ar = (AsyncResult*)IAsyncResult::Probe(obj);
            AutoPtr<IArrayOf> array = IArrayOf::Probe(ar->mResult);
            assert(array != NULL);
            Int32 len = 0;
            array->GetLength(&len);
            data = ArrayOf<Byte>::Alloc(len);
            for (Int32 i = 0; i < len; i++) {
                AutoPtr<IInterface> obj;
                array->Get(i, (IInterface**)&obj);
                IByte::Probe(obj)->GetValue(&(*data)[i]);
            }

            if (ar->mException != NULL) {
                break;
            }

            String str;
            iccu->BytesToHexString(data, &str);
            Log(String("EF_AD: ") + str);

            if (data->GetLength() < 3) {
                Log(String("Corrupt AD data on SIM"));
                break;
            }

            if (data->GetLength() == 3) {
                Log(String("MNC length not present in EF_AD"));
                break;
            }

            mMncLength = (*data)[3] & 0xf;
            Log(String("setting4 mMncLength=") + StringUtils::ToString(mMncLength));

            if (mMncLength == 0xf) {
                mMncLength = UNKNOWN;
                Log(String("setting5 mMncLength=") + StringUtils::ToString(mMncLength));
            }
            else {
                AutoPtr<IUICCConfig> uicc;
                mParentApp->GetUICCConfig((IUICCConfig**)&uicc);
                uicc->SetMncLength(mMncLength);
            }

        // } finally {
            if (((mMncLength == UNINITIALIZED) || (mMncLength == UNKNOWN) ||
                    (mMncLength == 2)) && ((mImsi != NULL) && (mImsi.GetLength() >= 6))) {
                String mccmncCode = mImsi.Substring(0, 6);
                Log(String("mccmncCode=") + mccmncCode);
                for (Int32 i = 0; i < MCCMNC_CODES_HAVING_3DIGITS_MNC->GetLength(); i++) {
                    String mccmnc = (*MCCMNC_CODES_HAVING_3DIGITS_MNC)[i];
                    if (mccmnc.Equals(mccmncCode)) {
                        mMncLength = 3;
                        Log(String("setting6 mMncLength=") + StringUtils::ToString(mMncLength));
                        break;
                    }
                }
            }

            if (mMncLength == UNKNOWN || mMncLength == UNINITIALIZED) {
                if (!mImsi.IsNull()) {
                    // try {
                        Int32 mcc = StringUtils::ParseInt32(mImsi.Substring(0,3));

                        mMncLength = MccTable::SmallestDigitsMccForMnc(mcc);
                        Log(String("setting7 mMncLength=") + StringUtils::ToString(mMncLength));
                    // } catch (NumberFormatException e) {
                    //     mMncLength = UNKNOWN;
                    //     loge("Corrupt IMSI! setting8 mMncLength=" + mMncLength);
                    // }
                }
                else {
                    // Indicate we got this info, but it didn't contain the length.
                    mMncLength = UNKNOWN;
                    Log(String("MNC length not present in EF_AD setting9 mMncLength=") + StringUtils::ToString(mMncLength));
                }
            }
            if (!mImsi.IsNull() && mMncLength != UNKNOWN) {
                // finally have both imsi and the length of the mnc and can parse
                // the imsi properly
                Log(String("update mccmnc=") + mImsi.Substring(0, 3 + mMncLength));
                MccTable::UpdateMccMncConfiguration(mContext,
                        mImsi.Substring(0, 3 + mMncLength), FALSE);
            }
            // }
        break;
        }
        case EVENT_GET_SPN_DONE: {
            isRecordLoadResponse = TRUE;
            ar = (AsyncResult*)IAsyncResult::Probe(obj);
            GetSpnFsm(FALSE, ar);
        break;
        }
        case EVENT_GET_CFF_DONE: {
            isRecordLoadResponse = TRUE;

            ar = (AsyncResult*)IAsyncResult::Probe(obj);
            AutoPtr<IArrayOf> array = IArrayOf::Probe(ar->mResult);
            assert(array != NULL);
            Int32 len = 0;
            array->GetLength(&len);
            data = ArrayOf<Byte>::Alloc(len);
            for (Int32 i = 0; i < len; i++) {
                AutoPtr<IInterface> obj;
                array->Get(i, (IInterface**)&obj);
                IByte::Probe(obj)->GetValue(&(*data)[i]);
            }

            if (ar->mException != NULL) {
                break;
            }

            String str;
            iccu->BytesToHexString(data, &str);
            Log(String("EF_CFF_CPHS: ") + str);
            mEfCff = data;

            // if EF_CFIS is valid, prefer it to EF_CFF_CPHS
            if (!ValidEfCfis(mEfCfis)) {
                mCallForwardingEnabled =
                    (((*data)[0] & CFF_LINE1_MASK) == CFF_UNCONDITIONAL_ACTIVE);

                mRecordsEventsRegistrants->NotifyResult(CoreUtils::Convert(EVENT_CFI));
            }
            else {
                Log(String("EVENT_GET_CFF_DONE: EF_CFIS is valid, ignoring EF_CFF_CPHS"));
            }
            break;
        }
        case EVENT_GET_SPDI_DONE: {
            isRecordLoadResponse = TRUE;

            ar = (AsyncResult*)IAsyncResult::Probe(obj);
            AutoPtr<IArrayOf> array = IArrayOf::Probe(ar->mResult);
            assert(array != NULL);
            Int32 len = 0;
            array->GetLength(&len);
            data = ArrayOf<Byte>::Alloc(len);
            for (Int32 i = 0; i < len; i++) {
                AutoPtr<IInterface> obj;
                array->Get(i, (IInterface**)&obj);
                IByte::Probe(obj)->GetValue(&(*data)[i]);
            }

            if (ar->mException != NULL) {
                break;
            }

            ParseEfSpdi(data);
        break;
        }
        case EVENT_UPDATE_DONE: {
            ar = (AsyncResult*)IAsyncResult::Probe(obj);
            if (ar->mException != NULL) {
                Logw(String("update failed. ") + TO_CSTR(ar->mException));
            }
        break;
        }
        case EVENT_GET_PNN_DONE: {
            isRecordLoadResponse = TRUE;

            ar = (AsyncResult*)IAsyncResult::Probe(obj);
            AutoPtr<IArrayOf> array = IArrayOf::Probe(ar->mResult);
            assert(array != NULL);
            Int32 len = 0;
            array->GetLength(&len);
            data = ArrayOf<Byte>::Alloc(len);
            for (Int32 i = 0; i < len; i++) {
                AutoPtr<IInterface> obj;
                array->Get(i, (IInterface**)&obj);
                IByte::Probe(obj)->GetValue(&(*data)[i]);
            }

            if (ar->mException != NULL) {
                break;
            }

            AutoPtr<ISimTlv> tlv;
            CSimTlv::New(data, 0, data->GetLength(), (ISimTlv**)&tlv);

            Boolean bValid = FALSE, nextObj = FALSE;
            for ( ; (tlv->IsValidObject(&bValid), bValid); tlv->NextObject(&nextObj)) {
                Int32 tag = 0;
                tlv->GetTag(&tag);
                if (tag == TAG_FULL_NETWORK_NAME) {
                    AutoPtr<ArrayOf<Byte> > data;
                    tlv->GetData((ArrayOf<Byte>**)&data);
                    iccu->NetworkNameToString(
                        data, 0, data->GetLength(),
                        &mPnnHomeName);
                    break;
                }
            }
        break;
        }
        case EVENT_GET_ALL_SMS_DONE: {
            isRecordLoadResponse = TRUE;

            ar = (AsyncResult*)IAsyncResult::Probe(obj);
            if (ar->mException != NULL)
                break;

            HandleSmses(IArrayList::Probe(ar->mResult));
            break;
        }

        case EVENT_MARK_SMS_READ_DONE: {
            Logger::I(String("ENF"), String("marked read: sms ") + StringUtils::ToString(arg1));
            break;
        }

        case EVENT_GET_SMS_DONE: {
            isRecordLoadResponse = FALSE;
            ar = (AsyncResult*)IAsyncResult::Probe(obj);
            if (ar->mException == NULL) {
                AutoPtr<IArrayOf> array = IArrayOf::Probe(ar->mResult);
                assert(array != NULL);
                Int32 len = 0;
                array->GetLength(&len);
                AutoPtr<ArrayOf<Byte> > d = ArrayOf<Byte>::Alloc(len);
                for (Int32 i = 0; i < len; i++) {
                    AutoPtr<IInterface> obj;
                    array->Get(i, (IInterface**)&obj);
                    IByte::Probe(obj)->GetValue(&(*d)[i]);
                }
                HandleSms(d);
            }
            else {
                Loge(String("Error on GET_SMS with exp ") + TO_CSTR(ar->mException));
            }
            break;
        }
        case EVENT_GET_SST_DONE: {
            isRecordLoadResponse = TRUE;

            ar = (AsyncResult*)IAsyncResult::Probe(obj);
            AutoPtr<IArrayOf> array = IArrayOf::Probe(ar->mResult);
            assert(array != NULL);
            Int32 len = 0;
            array->GetLength(&len);
            data = ArrayOf<Byte>::Alloc(len);
            for (Int32 i = 0; i < len; i++) {
                AutoPtr<IInterface> obj;
                array->Get(i, (IInterface**)&obj);
                IByte::Probe(obj)->GetValue(&(*data)[i]);
            }

            if (ar->mException != NULL) {
                break;
            }

            CUsimServiceTable::New(data, (IUsimServiceTable**)&mUsimServiceTable);
            if (DBG) {
                Log(String("SST: ") + TO_CSTR(mUsimServiceTable));
            }
            break;
        }

        case EVENT_GET_INFO_CPHS_DONE: {
            isRecordLoadResponse = TRUE;

            ar = (AsyncResult*)IAsyncResult::Probe(obj);

            if (ar->mException != NULL) {
                break;
            }

            AutoPtr<IArrayOf> array = IArrayOf::Probe(ar->mResult);
            assert(array != NULL);
            Int32 len = 0;
            array->GetLength(&len);
            mCphsInfo = ArrayOf<Byte>::Alloc(len);
            for (Int32 i = 0; i < len; i++) {
                AutoPtr<IInterface> obj;
                array->Get(i, (IInterface**)&obj);
                IByte::Probe(obj)->GetValue(&(*mCphsInfo)[i]);
            }

            if (DBG) {
                String str;
                iccu->BytesToHexString(mCphsInfo, &str);
                Log(String("iCPHS: ") + str);
            }
        break;
        }

        case EVENT_SET_MBDN_DONE: {
            isRecordLoadResponse = FALSE;
            ar = (AsyncResult*)IAsyncResult::Probe(obj);

            if (DBG) {
                Log(String("EVENT_SET_MBDN_DONE ex:") + TO_CSTR(ar->mException));
            }
            if (ar->mException == NULL) {
                mVoiceMailNum = mNewVoiceMailNum;
                mVoiceMailTag = mNewVoiceMailTag;
            }

            if (IsCphsMailboxEnabled()) {
                AutoPtr<IAdnRecord> tmp;
                CAdnRecord::New(mVoiceMailTag, mVoiceMailNum, (IAdnRecord**)&tmp);
                adn = (AdnRecord*)tmp.Get();
                AutoPtr<IMessage> onCphsCompleted = IMessage::Probe(ar->mUserObj);

                /* write to cphs mailbox whenever it is available but
                * we only need notify caller once if both updating are
                * successful.
                *
                * so if set_mbdn successful, notify caller here and set
                * onCphsCompleted to NULL
                */
                if (ar->mException == NULL && ar->mUserObj != NULL) {
                    AsyncResult::ForMessage(IMessage::Probe(ar->mUserObj))->mException
                            = NULL;
                    IMessage::Probe(ar->mUserObj)->SendToTarget();

                    if (DBG) Log(String("Callback with MBDN successful."));

                    onCphsCompleted = NULL;
                }

                AutoPtr<IAdnRecordLoader> loader;
                CAdnRecordLoader::New(mFh, (IAdnRecordLoader**)&loader);
                AutoPtr<IMessage> msg;
                ObtainMessage(EVENT_SET_CPHS_MAILBOX_DONE,
                                onCphsCompleted, (IMessage**)&msg);
                loader->UpdateEF(adn, EF_MAILBOX_CPHS, EF_EXT1, 1, String(NULL),
                        msg);
            }
            else {
                if (ar->mUserObj != NULL) {
                    AutoPtr<IResourcesHelper> hlp;
                    CResourcesHelper::AcquireSingleton((IResourcesHelper**)&hlp);
                    AutoPtr<IResources> resource;
                    hlp->GetSystem((IResources**)&resource);
                    Boolean b = FALSE;
                    resource->GetBoolean(
                                R::bool_::editable_voicemailnumber, &b);
                    if (ar->mException != NULL && b) {
                        // GSMPhone will store vm number on device
                        // when IccVmNotSupportedException occurred
                        AutoPtr<IccVmNotSupportedException> p =
                            new IccVmNotSupportedException(
                                    String("Update SIM voice mailbox error"));
                        AsyncResult::ForMessage(IMessage::Probe(ar->mUserObj))->mException
                            = IThrowable::Probe(p);
                    }
                    else {
                        AsyncResult::ForMessage(IMessage::Probe(ar->mUserObj))->mException
                            = ar->mException;
                    }
                    IMessage::Probe(ar->mUserObj)->SendToTarget();
                }
            }
            break;
        }
        case EVENT_SET_CPHS_MAILBOX_DONE: {
            isRecordLoadResponse = FALSE;
            ar = (AsyncResult*)IAsyncResult::Probe(obj);
            if (ar->mException == NULL) {
                mVoiceMailNum = mNewVoiceMailNum;
                mVoiceMailTag = mNewVoiceMailTag;
            }
            else {
                if (DBG) {
                    Log(String("Set CPHS MailBox with exception: ") + TO_CSTR(ar->mException));
                }
            }
            if (ar->mUserObj != NULL) {
                if (DBG) Log(String("Callback with CPHS MB successful."));
                AsyncResult::ForMessage(IMessage::Probe(ar->mUserObj))->mException
                        = ar->mException;
                IMessage::Probe(ar->mUserObj)->SendToTarget();
            }
            break;
        }
        case EVENT_GET_CFIS_DONE: {
            isRecordLoadResponse = TRUE;

            ar = (AsyncResult*)IAsyncResult::Probe(obj);
            AutoPtr<IArrayOf> array = IArrayOf::Probe(ar->mResult);
            assert(array != NULL);
            Int32 len = 0;
            array->GetLength(&len);
            data = ArrayOf<Byte>::Alloc(len);
            for (Int32 i = 0; i < len; i++) {
                AutoPtr<IInterface> obj;
                array->Get(i, (IInterface**)&obj);
                IByte::Probe(obj)->GetValue(&(*data)[i]);
            }

            if (ar->mException != NULL) {
                break;
            }

            String str;
            iccu->BytesToHexString(data, &str);
            Log(String("EF_CFIS: ") + str);

            if (ValidEfCfis(data)) {
                mEfCfis = data;

                // Refer TS 51.011 Section 10.3.46 for the content description
                mCallForwardingEnabled = (((*data)[1] & 0x01) != 0);
                Log(String("EF_CFIS: callForwardingEnabled=") + StringUtils::ToString(mCallForwardingEnabled));

                mRecordsEventsRegistrants->NotifyResult(CoreUtils::Convert(EVENT_CFI));
            }
            else {
                String str;
                iccu->BytesToHexString(data, &str);
                Log(String("EF_CFIS: invalid data=") + str);
            }
            break;
        }

        case EVENT_GET_CSP_CPHS_DONE: {
            isRecordLoadResponse = TRUE;

            ar = (AsyncResult*)IAsyncResult::Probe(obj);

            if (ar->mException != NULL) {
                Loge(String("Exception in fetching EF_CSP data ") + TO_CSTR(ar->mException));
                break;
            }

            AutoPtr<IArrayOf> array = IArrayOf::Probe(ar->mResult);
            assert(array != NULL);
            Int32 len = 0;
            array->GetLength(&len);
            data = ArrayOf<Byte>::Alloc(len);
            for (Int32 i = 0; i < len; i++) {
                AutoPtr<IInterface> obj;
                array->Get(i, (IInterface**)&obj);
                IByte::Probe(obj)->GetValue(&(*data)[i]);
            }
            String str;
            iccu->BytesToHexString(data, &str);
            Log(String("EF_CSP: ") + str);
            HandleEfCspData(data);
            break;
        }

        case EVENT_GET_GID1_DONE: {
            isRecordLoadResponse = TRUE;

            ar = (AsyncResult*)IAsyncResult::Probe(obj);
            AutoPtr<IArrayOf> array = IArrayOf::Probe(ar->mResult);
            assert(array != NULL);
            Int32 len = 0;
            array->GetLength(&len);
            data = ArrayOf<Byte>::Alloc(len);
            for (Int32 i = 0; i < len; i++) {
                AutoPtr<IInterface> obj;
                array->Get(i, (IInterface**)&obj);
                IByte::Probe(obj)->GetValue(&(*data)[i]);
            }

            if (ar->mException != NULL) {
                Loge(String("Exception in get GID1 ") + TO_CSTR(ar->mException));
                mGid1 = NULL;
                break;
            }
            iccu->BytesToHexString(data, &mGid1);
            Log(String("GID1: ") + mGid1);

            break;
        }
        default: {
            IccRecords::HandleMessage(msg);   // IccRecords handles generic record load responses
        }
    }

    // }}catch (RuntimeException exc) {
    //     // I don't want these exceptions to be fatal
    //     logw("Exception parsing SIM record", exc);
    // } finally {
        // Count up record load responses even if they are fails
        if (isRecordLoadResponse) {
            OnRecordLoaded();
        }
    // }
    return NOERROR;
}

ECode SIMRecords::HandleSmsOnIcc(
    /* [in] */ AsyncResult* ar)
{

    AutoPtr<ArrayOf<Int32> > index;// = (ArrayOf<Int32>*)ar->mResult;
    AutoPtr<IArrayOf> array = IArrayOf::Probe(ar->mResult);
    assert(array != NULL);
    Int32 len = 0;
    array->GetLength(&len);
    index = ArrayOf<Int32>::Alloc(len);
    for (Int32 i = 0; i < len; i++) {
        AutoPtr<IInterface> obj;
        array->Get(i, (IInterface**)&obj);
        IInteger32::Probe(obj)->GetValue(&(*index)[i]);
    }

    if (ar->mException != NULL || index->GetLength() != 1) {
        Loge(String(" Error on SMS_ON_SIM with exp ") + TO_CSTR(ar->mException)
            + String(" length ") + StringUtils::ToString(index->GetLength()));
    }
    else {
        Log(String("READ EF_SMS RECORD index= ") + StringUtils::ToString((*index)[0]));
        AutoPtr<IMessage> msg;
        ObtainMessage(EVENT_GET_SMS_DONE, (IMessage**)&msg);
        mFh->LoadEFLinearFixed(EF_SMS, (*index)[0],
                        msg);
    }
    return NOERROR;
}

ECode SIMRecords::OnReady()
{
    FetchSimRecords();
    return NOERROR;
}

ECode SIMRecords::GetDisplayRule(
    /* [in] */ const String& plmn,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 rule = 0;
    if (mContext != NULL) {
        AutoPtr<IResources> res;
        mContext->GetResources((IResources**)&res);
        Boolean b = FALSE;
        res->GetBoolean(
            R::bool_::def_telephony_spn_spec_enabled, &b);
        if (b) {
            rule = SPN_RULE_SHOW_SPN;
            *result = rule;
            return NOERROR;
        }
    }

    String name;
    GetServiceProviderName(&name);
    if (mParentApp != NULL) {
        AutoPtr<IUiccCard> card;
        mParentApp->GetUiccCard((IUiccCard**)&card);
        if (card != NULL) {
            String str;
            card->GetOperatorBrandOverride(&str);
            if (!str.IsNull()) {
                // If the operator has been overridden, treat it as the SPN file on the SIM did not exist.
                rule = SPN_RULE_SHOW_PLMN;
            }
        }
    }
    else if (TextUtils::IsEmpty(name) || mSpnDisplayCondition == -1) {
        // No EF_SPN content was found on the SIM, or not yet loaded.  Just show ONS.
        rule = SPN_RULE_SHOW_PLMN;
    }
    else if (IsOnMatchingPlmn(plmn)) {
        rule = SPN_RULE_SHOW_SPN;
        if ((mSpnDisplayCondition & 0x01) == 0x01) {
            // ONS required when registered to HPLMN or PLMN in EF_SPDI
            rule |= SPN_RULE_SHOW_PLMN;
        }
    }
    else {
        rule = SPN_RULE_SHOW_PLMN;
        if ((mSpnDisplayCondition & 0x02) == 0x00) {
            // SPN required if not registered to HPLMN or PLMN in EF_SPDI
            rule |= SPN_RULE_SHOW_SPN;
        }
    }
    *result = rule;
    return NOERROR;
}

ECode SIMRecords::IsCspPlmnEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mCspPlmnEnabled;
    return NOERROR;
}

ECode SIMRecords::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    pw->Println(String("SIMRecords: ") + TO_CSTR(this));
    pw->Println(String(" extends:"));
    IccRecords::Dump(fd, pw, args);
    pw->Println(String(" mVmConfig=") + TO_CSTR(mVmConfig));
    pw->Println(String(" mSpnOverride=") + TO_CSTR(mSpnOverride));
    pw->Println(String(" mCallForwardingEnabled=") + StringUtils::ToString(mCallForwardingEnabled));
    pw->Println(String(" mSpnState=") + StringUtils::ToString(mSpnState));
    pw->Println(String(" mCphsInfo=") + Arrays::ToString(mCphsInfo));
    pw->Println(String(" mCspPlmnEnabled=") + StringUtils::ToString(mCspPlmnEnabled));
    pw->Println(String(" mEfMWIS[]=") + Arrays::ToString(mEfMWIS));
    pw->Println(String(" mEfCPHS_MWI[]=") + Arrays::ToString(mEfCPHS_MWI));
    pw->Println(String(" mEfCff[]=") + Arrays::ToString(mEfCff));
    pw->Println(String(" mEfCfis[]=") + Arrays::ToString(mEfCfis));
    pw->Println(String(" mSpnDisplayCondition=") + StringUtils::ToString(mSpnDisplayCondition));
    pw->Println(String(" mSpdiNetworks[]=") + TO_CSTR(mSpdiNetworks));
    pw->Println(String(" mPnnHomeName=") + mPnnHomeName);
    pw->Println(String(" mUsimServiceTable=") + TO_CSTR(mUsimServiceTable));
    pw->Println(String(" mGid1=") + mGid1);
    IFlushable::Probe(pw)->Flush();
    return NOERROR;
}

ECode SIMRecords::Finalize()
{
    if (DBG) Log(String("finalized"));
    return NOERROR;
}

void SIMRecords::ResetRecords()
{
    mImsi = NULL;
    mMsisdn = NULL;
    mVoiceMailNum = NULL;
    mMncLength = UNINITIALIZED;
    Log(String("setting0 mMncLength") + StringUtils::ToString(mMncLength));
    mIccId = NULL;
    // -1 means no EF_SPN found; treat accordingly.
    mSpnDisplayCondition = -1;
    mEfMWIS = NULL;
    mEfCPHS_MWI = NULL;
    mSpdiNetworks = NULL;
    mPnnHomeName = NULL;
    mGid1 = NULL;

    mAdnCache->Reset();

    Log(String("SIMRecords: onRadioOffOrNotAvailable set 'gsm.sim.operator.numeric' to operator=NULL"));
    Log(String("update icc_operator_numeric=") + String(NULL));
    SetSystemProperty(ITelephonyProperties::PROPERTY_ICC_OPERATOR_NUMERIC, String(NULL));
    SetSystemProperty(ITelephonyProperties::PROPERTY_ICC_OPERATOR_ALPHA, String(NULL));
    SetSystemProperty(ITelephonyProperties::PROPERTY_ICC_OPERATOR_ISO_COUNTRY, String(NULL));
    AutoPtr<IUICCConfig> uicc;
    mParentApp->GetUICCConfig((IUICCConfig**)&uicc);
    uicc->SetImsi(mImsi);
    uicc->SetMncLength(mMncLength);

    // recordsRequested is set to FALSE indicating that the SIM
    // read requests made so far are not valid. This is set to
    // true only when fresh set of read requests are made.
    mRecordsRequested = FALSE;
}

ECode SIMRecords::HandleFileUpdate(
    /* [in] */ Int32 efid)
{
    switch(efid) {
        case EF_MBDN: {
            mRecordsToLoad++;
            AutoPtr<IAdnRecordLoader> loader;
            CAdnRecordLoader::New(mFh, (IAdnRecordLoader**)&loader);
            AutoPtr<IMessage> msg;
            ObtainMessage(EVENT_GET_MBDN_DONE, (IMessage**)&msg);
            loader->LoadFromEF(EF_MBDN, EF_EXT6,
                    mMailboxIndex, msg);
            break;
        }
        case EF_MAILBOX_CPHS: {
            mRecordsToLoad++;
            AutoPtr<IAdnRecordLoader> loader;
            CAdnRecordLoader::New(mFh, (IAdnRecordLoader**)&loader);
            AutoPtr<IMessage> msg;
            ObtainMessage(EVENT_GET_CPHS_MAILBOX_DONE, (IMessage**)&msg);
            loader->LoadFromEF(EF_MAILBOX_CPHS, EF_EXT1,
                    1, msg);
            break;
        }
        case EF_CSP_CPHS: {
            mRecordsToLoad++;
            Log(String("[CSP] SIM Refresh for EF_CSP_CPHS"));
            AutoPtr<IMessage> msg;
            ObtainMessage(EVENT_GET_CSP_CPHS_DONE, (IMessage**)&msg);
            mFh->LoadEFTransparent(EF_CSP_CPHS,
                    msg);
            break;
        }
        case EF_FDN: {
            if (DBG) Log(String("SIM Refresh called for EF_FDN"));
            ((UiccCardApplication*)mParentApp.Get())->QueryFdn();
        }
        case EF_MSISDN: {
            mRecordsToLoad++;
            Log(String("SIM Refresh called for EF_MSISDN"));
            AutoPtr<IAdnRecordLoader> loader;
            CAdnRecordLoader::New(mFh, (IAdnRecordLoader**)&loader);
            AutoPtr<IMessage> msg;
            ObtainMessage(EVENT_GET_MSISDN_DONE, (IMessage**)&msg);
            loader->LoadFromEF(EF_MSISDN, GetExtFromEf(EF_MSISDN), 1,
                    msg);
            break;
        }
        case EF_CFIS: {
            mRecordsToLoad++;
            Log(String("SIM Refresh called for EF_CFIS"));
            AutoPtr<IMessage> msg;
            ObtainMessage(EVENT_GET_CFIS_DONE, (IMessage**)&msg);
            mFh->LoadEFLinearFixed(EF_CFIS,
                    1, msg);
            break;
        }
        case EF_CFF_CPHS: {
            mRecordsToLoad++;
            Log(String("SIM Refresh called for EF_CFF_CPHS"));
            AutoPtr<IMessage> msg;
            ObtainMessage(EVENT_GET_CFF_DONE, (IMessage**)&msg);
            mFh->LoadEFTransparent(EF_CFF_CPHS,
                    msg);
            break;
        }
        case EF_ADN: {
            Log(String("SIM Refresh for EF_ADN"));
            mAdnCache->Reset();
            break;
        }
        default:
            // For now, fetch all records if this is not a
            // voicemail number.
            // TODO: Handle other cases, instead of fetching all.
            mAdnCache->Reset();
            FetchSimRecords();
            break;
    }
    return NOERROR;
}

ECode SIMRecords::OnRecordLoaded()
{
    // One record loaded successfully or failed, In either case
    // we need to update the recordsToLoad count
    mRecordsToLoad -= 1;
    if (DBG) {
        Log(String("onRecordLoaded ") + StringUtils::ToString(mRecordsToLoad) +
            String(" requested: ") + StringUtils::ToString(mRecordsRequested));
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

ECode SIMRecords::OnAllRecordsLoaded()
{
    if (DBG) {
        Log(String("record load complete"));
    }

    SetLocaleFromUsim();

    AppState state;
    mParentApp->GetState(&state);
    if (state == APPSTATE_PIN ||
           state == APPSTATE_PUK) {
        // reset recordsRequested, since sim is not loaded really
        mRecordsRequested = FALSE;
        // lock state, only update language
        return NOERROR;
    }

    // Some fields require more than one SIM record to set

    String opt;
    GetOperatorNumeric(&opt);
    if (!TextUtils::IsEmpty(opt)) {
        Log(String("onAllRecordsLoaded set 'gsm.sim.operator.numeric' to operator='") +
                opt + String("'"));
        Log(String("update icc_operator_numeric=") + opt);
        SetSystemProperty(ITelephonyProperties::PROPERTY_ICC_OPERATOR_NUMERIC, opt);
        AutoPtr<ISubscriptionControllerHelper> hlp;
        CSubscriptionControllerHelper::AcquireSingleton((ISubscriptionControllerHelper**)&hlp);
        AutoPtr<ISubscriptionController> subController;
        hlp->GetInstance((ISubscriptionController**)&subController);
        Int64 subid = 0;
        IISub::Probe(subController)->GetDefaultSmsSubId(&subid);
        Int32 res = 0;
        subController->SetMccMnc(opt, subid, &res);
    }
    else {
        Log(String("onAllRecordsLoaded empty 'gsm.sim.operator.numeric' skipping"));
    }

    if (!TextUtils::IsEmpty(mImsi)) {
        Log(String("onAllRecordsLoaded set mcc imsi=") + mImsi);
        SetSystemProperty(ITelephonyProperties::PROPERTY_ICC_OPERATOR_ISO_COUNTRY,
                MccTable::CountryCodeForMcc(StringUtils::ParseInt32(mImsi.Substring(0,3))));
    }
    else {
        Log(String("onAllRecordsLoaded empty imsi skipping setting mcc"));
    }

    SetVoiceMailByCountry(opt);
    SetSpnFromConfig(opt);

    AutoPtr<AsyncResult> p = new AsyncResult(NULL, NULL, NULL);
    mRecordsLoadedRegistrants->NotifyRegistrants(p);
    return NOERROR;
}

void SIMRecords::FetchSimRecords()
{
    mRecordsRequested = TRUE;

    if (DBG) Log(String("fetchSimRecords ") + StringUtils::ToString(mRecordsToLoad));

    AutoPtr<IMessage> msg_imsi;
    ObtainMessage(EVENT_GET_IMSI_DONE, (IMessage**)&msg_imsi);
    String aid;
    mParentApp->GetAid(&aid);
    mCi->GetIMSIForApp(aid, msg_imsi);
    mRecordsToLoad++;

    AutoPtr<IMessage> msg_iccid;
    ObtainMessage(EVENT_GET_ICCID_DONE, (IMessage**)&msg_iccid);
    mFh->LoadEFTransparent(EF_ICCID, msg_iccid);
    mRecordsToLoad++;

    // FIXME should examine EF[MSISDN]'s capability configuration
    // to determine which is the voice/data/fax line
    AutoPtr<IAdnRecordLoader> loader;
    CAdnRecordLoader::New(mFh, (IAdnRecordLoader**)&loader);
    AutoPtr<IMessage> msg_msisdn;
    ObtainMessage(EVENT_GET_MSISDN_DONE, (IMessage**)&msg_msisdn);
    loader->LoadFromEF(EF_MSISDN, GetExtFromEf(EF_MSISDN), 1,
                msg_msisdn);
    mRecordsToLoad++;

    // Record number is subscriber profile
    AutoPtr<IMessage> msg_mbi;
    ObtainMessage(EVENT_GET_MBI_DONE, (IMessage**)&msg_mbi);
    mFh->LoadEFLinearFixed(EF_MBI, 1, msg_mbi);
    mRecordsToLoad++;

    AutoPtr<IMessage> msg_ad;
    ObtainMessage(EVENT_GET_AD_DONE, (IMessage**)&msg_ad);
    mFh->LoadEFTransparent(EF_AD, msg_ad);
    mRecordsToLoad++;

    // Record number is subscriber profile
    AutoPtr<IMessage> msg_mwis;
    ObtainMessage(EVENT_GET_MWIS_DONE, (IMessage**)&msg_mwis);
    mFh->LoadEFLinearFixed(EF_MWIS, 1, msg_mwis);
    mRecordsToLoad++;


    // Also load CPHS-style voice mail indicator, which stores
    // the same info as EF[MWIS]. If both exist, both are updated
    // but the EF[MWIS] data is preferred
    // Please note this must be loaded after EF[MWIS]
    AutoPtr<IMessage> msg_cphs;
    ObtainMessage(EVENT_GET_VOICE_MAIL_INDICATOR_CPHS_DONE, (IMessage**)&msg_cphs);
    mFh->LoadEFTransparent(
            EF_VOICE_MAIL_INDICATOR_CPHS,
            msg_cphs);
    mRecordsToLoad++;

    // Same goes for Call Forward Status indicator: fetch both
    // EF[CFIS] and CPHS-EF, with EF[CFIS] preferred.
    AutoPtr<IMessage> msg_cfis;
    ObtainMessage(EVENT_GET_CFIS_DONE, (IMessage**)&msg_cfis);
    mFh->LoadEFLinearFixed(EF_CFIS, 1, msg_cfis);
    mRecordsToLoad++;
    AutoPtr<IMessage> msg_cff;
    ObtainMessage(EVENT_GET_CFF_DONE, (IMessage**)&msg_cff);
    mFh->LoadEFTransparent(EF_CFF_CPHS, msg_cff);
    mRecordsToLoad++;

    GetSpnFsm(TRUE, NULL);

    AutoPtr<IMessage> msg_spdi;
    ObtainMessage(EVENT_GET_SPDI_DONE, (IMessage**)&msg_spdi);
    mFh->LoadEFTransparent(EF_SPDI, msg_spdi);
    mRecordsToLoad++;

    AutoPtr<IMessage> msg_pnn;
    ObtainMessage(EVENT_GET_PNN_DONE, (IMessage**)&msg_pnn);
    mFh->LoadEFLinearFixed(EF_PNN, 1, msg_pnn);
    mRecordsToLoad++;

    AutoPtr<IMessage> msg_sst;
    ObtainMessage(EVENT_GET_SST_DONE, (IMessage**)&msg_sst);
    mFh->LoadEFTransparent(EF_SST, msg_sst);
    mRecordsToLoad++;

    AutoPtr<IMessage> msg_info_cphs;
    ObtainMessage(EVENT_GET_INFO_CPHS_DONE, (IMessage**)&msg_info_cphs);
    mFh->LoadEFTransparent(EF_INFO_CPHS, msg_info_cphs);
    mRecordsToLoad++;

    AutoPtr<IMessage> msg_csp_cphs;
    ObtainMessage(EVENT_GET_CSP_CPHS_DONE, (IMessage**)&msg_csp_cphs);
    mFh->LoadEFTransparent(EF_CSP_CPHS, msg_csp_cphs);
    mRecordsToLoad++;

    AutoPtr<IMessage> msg_gid1;
    ObtainMessage(EVENT_GET_GID1_DONE, (IMessage**)&msg_gid1);
    mFh->LoadEFTransparent(EF_GID1, msg_gid1);
    mRecordsToLoad++;

    LoadEfLiAndEfPl();
    AutoPtr<IMessage> msg_record;
    ObtainMessage(EVENT_GET_SMS_RECORD_SIZE_DONE, (IMessage**)&msg_record);
    mFh->GetEFLinearRecordSize(EF_SMS, msg_record);

    // XXX should seek instead of examining them all
    if (FALSE) { // XXX
        AutoPtr<IMessage> msg_all_sms;
        ObtainMessage(EVENT_GET_ALL_SMS_DONE, (IMessage**)&msg_all_sms);
        mFh->LoadEFLinearFixedAll(EF_SMS, msg_all_sms);
        mRecordsToLoad++;
    }

    if (CRASH_RIL) {
        String sms = String("0107912160130310f20404d0110041007030208054832b0120")
                     + String("fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff")
                     + String("fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff")
                     + String("fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff")
                     + String("fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff")
                     + String("ffffffffffffffffffffffffffffff");
        AutoPtr<IIccUtils> iccu;
        CIccUtils::AcquireSingleton((IIccUtils**)&iccu);
        AutoPtr<ArrayOf<Byte> > ba;
        iccu->HexStringToBytes(sms, (ArrayOf<Byte>**)&ba);

        AutoPtr<IMessage> msg_sms_read;
        ObtainMessage(EVENT_MARK_SMS_READ_DONE, CoreUtils::Convert(1), (IMessage**)&msg_sms_read);
        mFh->UpdateEFLinearFixed(EF_SMS, 1, ba, String(NULL),
                        msg_sms_read);
    }
    if (DBG) {
        Log(String("fetchSimRecords ") + StringUtils::ToString(mRecordsToLoad) +
            String(" requested: ") + StringUtils::ToString(mRecordsRequested));
    }
}

ECode SIMRecords::Log(
    /* [in] */ const String& s)
{
    Logger::D(LOGTAG, String("[SIMRecords] ") + s);
    return NOERROR;
}

ECode SIMRecords::Loge(
    /* [in] */ const String& s)
{
    Logger::E(LOGTAG, String("[SIMRecords] ") + s);
    return NOERROR;
}

void SIMRecords::Logw(
    /* [in] */ const String& s)
    ///* [in] */ Throwable* tr)
{
    Logger::W(LOGTAG, String("[SIMRecords] ") + s);
}

void SIMRecords::Logv(
    /* [in] */ const String& s)
{
    Logger::V(LOGTAG, String("[SIMRecords] ") + s);
}

AutoPtr<ArrayOf<String> > SIMRecords::MiddleInitMccmncCodesHaving3digitsMnc()
{
    AutoPtr<ArrayOf<String> > result = ArrayOf<String>::Alloc(131);
    (*result)[0] = String("302370"); (*result)[1] = String("302720"); (*result)[2] = String("310260");
    (*result)[3] = String("405025"); (*result)[4] = String("405026"); (*result)[5] = String("405027");
    (*result)[6] = String("405028"); (*result)[7] = String("405029"); (*result)[8] = String("405030");
    (*result)[9] = String("405031"); (*result)[10] = String("405032"); (*result)[11] = String("405033");
    (*result)[12] = String("405034"); (*result)[13] = String("405035"); (*result)[14] = String("405036");
    (*result)[15] = String("405037"); (*result)[16] = String("405038"); (*result)[17] = String("405039");
    (*result)[18] = String("405040"); (*result)[19] = String("405041"); (*result)[20] = String("405042");
    (*result)[21] = String("405043"); (*result)[22] = String("405044"); (*result)[23] = String("405045");
    (*result)[24] = String("405046"); (*result)[25] = String("405047"); (*result)[26] = String("405750");
    (*result)[27] = String("405751"); (*result)[28] = String("405752"); (*result)[29] = String("405753");
    (*result)[30] = String("405754"); (*result)[31] = String("405755"); (*result)[32] = String("405756");
    (*result)[33] = String("405799"); (*result)[34] = String("405800"); (*result)[35] = String("405801");
    (*result)[36] = String("405802"); (*result)[37] = String("405803"); (*result)[38] = String("405804");
    (*result)[39] = String("405805"); (*result)[40] = String("405806"); (*result)[41] = String("405807");
    (*result)[42] = String("405808"); (*result)[43] = String("405809"); (*result)[44] = String("405810");
    (*result)[45] = String("405811"); (*result)[46] = String("405812"); (*result)[47] = String("405813");
    (*result)[48] = String("405814"); (*result)[49] = String("405815"); (*result)[50] = String("405816");
    (*result)[51] = String("405817"); (*result)[52] = String("405818"); (*result)[53] = String("405819");
    (*result)[54] = String("405820"); (*result)[55] = String("405821"); (*result)[56] = String("405822");
    (*result)[57] = String("405823"); (*result)[58] = String("405824"); (*result)[59] = String("405825");
    (*result)[60] = String("405826"); (*result)[61] = String("405827"); (*result)[62] = String("405828");
    (*result)[63] = String("405829"); (*result)[64] = String("405830"); (*result)[65] = String("405831");
    (*result)[66] = String("405832"); (*result)[67] = String("405833"); (*result)[68] = String("405834");
    (*result)[69] = String("405835"); (*result)[70] = String("405836"); (*result)[71] = String("405837");
    (*result)[72] = String("405838"); (*result)[73] = String("405839"); (*result)[74] = String("405840");
    (*result)[75] = String("405841"); (*result)[76] = String("405842"); (*result)[77] = String("405843");
    (*result)[78] = String("405844"); (*result)[79] = String("405845"); (*result)[80] = String("405846");
    (*result)[81] = String("405847"); (*result)[82] = String("405848"); (*result)[83] = String("405849");
    (*result)[84] = String("405850"); (*result)[85] = String("405851"); (*result)[86] = String("405852");
    (*result)[87] = String("405853"); (*result)[88] = String("405875"); (*result)[89] = String("405876");
    (*result)[90] = String("405877"); (*result)[91] = String("405878"); (*result)[92] = String("405879");
    (*result)[93] = String("405880"); (*result)[94] = String("405881"); (*result)[95] = String("405882");
    (*result)[96] = String("405883"); (*result)[97] = String("405884"); (*result)[98] = String("405885");
    (*result)[99] = String("405886"); (*result)[100] = String("405908"); (*result)[101] = String("405909");
    (*result)[102] = String("405910"); (*result)[103] = String("405911"); (*result)[104] = String("405912");
    (*result)[105] = String("405913"); (*result)[106] = String("405914"); (*result)[107] = String("405915");
    (*result)[108] = String("405916"); (*result)[109] = String("405917"); (*result)[110] = String("405918");
    (*result)[111] = String("405919"); (*result)[112] = String("405920"); (*result)[113] = String("405921");
    (*result)[114] = String("405922"); (*result)[115] = String("405923"); (*result)[116] = String("405924");
    (*result)[117] = String("405925"); (*result)[118] = String("405926"); (*result)[119] = String("405927");
    (*result)[120] = String("405928"); (*result)[121] = String("405929"); (*result)[122] = String("405930");
    (*result)[123] = String("405931"); (*result)[124] = String("405932"); (*result)[125] = String("502142");
    (*result)[126] = String("502143"); (*result)[127] = String("502145"); (*result)[128] = String("502146");
    (*result)[129] = String("502147"); (*result)[130] = String("502148");
    return result;
}

Int32 SIMRecords::GetExtFromEf(
    /* [in] */ Int32 ef)
{
    Int32 ext = 0;
    switch (ef) {
        case EF_MSISDN: {
            /* For USIM apps use EXT5. (TS 31.102 Section 4.2.37) */
            AppType type;
            mParentApp->GetType(&type);
            if (type == APPTYPE_USIM) {
                ext = EF_EXT5;
            }
            else {
                ext = EF_EXT1;
            }
            break;
        }
        default:
            ext = EF_EXT1;
    }
    return ext;
}

Boolean SIMRecords::ValidEfCfis(
    /* [in] */ ArrayOf<Byte>* data)
{
    return ((data != NULL) && ((*data)[0] >= 1) && ((*data)[0] <= 4));
}

Int32 SIMRecords::DispatchGsmMessage(
    /* [in] */ ISmsMessage* message)
{
    mNewSmsRegistrants->NotifyResult(message);
    return 0;
}

void SIMRecords::HandleSms(
    /* [in] */ ArrayOf<Byte>* ba)
{
    if ((*ba)[0] != 0) {
        Logger::D(String("ENF"), String("status : ") + (*ba)[0]);
    }

    // 3GPP TS 51.011 v5.0.0 (20011-12)  10.5.3
    // 3 == "received by MS from network; message to be read"
    if ((*ba)[0] == 3) {
        Int32 n = ba->GetLength();

        // Note: Data may include trailing FF's.  That's OK; message
        // should still parse correctly.
        AutoPtr<ArrayOf<Byte> > pdu = ArrayOf<Byte>::Alloc(n - 1);
        // System::Arraycopy(ba, 1, pdu, 0, n - 1);
        pdu->Copy(0, ba, 1, n - 1);
        AutoPtr<ISmsMessageHelper> hlp;
        CSmsMessageHelper::AcquireSingleton((ISmsMessageHelper**)&hlp);
        AutoPtr<ISmsMessage> message;
        hlp->CreateFromPdu(pdu, ISmsConstants::FORMAT_3GPP, (ISmsMessage**)&message);

        DispatchGsmMessage(message);
    }
}

void SIMRecords::HandleSmses(
    /* [in] */ IArrayList/*<ArrayOf<Byte>>*/* messages)
{
    Int32 count = 0;
    messages->GetSize(&count);

    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IInterface> p;
        messages->Get(i, (IInterface**)&p);
        AutoPtr<IArrayOf> array = IArrayOf::Probe(p);
        assert(array != NULL);
        Int32 len = 0;
        array->GetLength(&len);
        AutoPtr<ArrayOf<Byte> > ba = ArrayOf<Byte>::Alloc(len);
        for (Int32 j = 0; j < len; j++) {
            AutoPtr<IInterface> obj;
            array->Get(j, (IInterface**)&obj);
            IByte::Probe(obj)->GetValue(&(*ba)[j]);
        }

        if ((*ba)[0] != 0) {
            Logger::I(String("ENF"), String("status ") + StringUtils::ToString(i) +
                String(": ") + StringUtils::ToString((*ba)[0]));
        }

        // 3GPP TS 51.011 v5.0.0 (20011-12)  10.5.3
        // 3 == "received by MS from network; message to be read"

        if ((*ba)[0] == 3) {
            Int32 n = ba->GetLength();

            // Note: Data may include trailing FF's.  That's OK; message
            // should still parse correctly.
            AutoPtr<ArrayOf<Byte> > pdu = ArrayOf<Byte>::Alloc(n - 1);
            // System::Arraycopy(ba, 1, pdu, 0, n - 1);
            pdu->Copy(0, ba, 1, n - 1);
            AutoPtr<ISmsMessageHelper> hlp;
            CSmsMessageHelper::AcquireSingleton((ISmsMessageHelper**)&hlp);
            AutoPtr<ISmsMessage> message;
            hlp->CreateFromPdu(pdu, ISmsConstants::FORMAT_3GPP, (ISmsMessage**)&message);

            DispatchGsmMessage(message);

            // 3GPP TS 51.011 v5.0.0 (20011-12)  10.5.3
            // 1 == "received by MS from network; message read"

            (*ba)[0] = 1;

            if (FALSE) { // FIXME: writing seems to crash RdoServD
                AutoPtr<IMessage> msg;
                ObtainMessage(EVENT_MARK_SMS_READ_DONE, CoreUtils::Convert(i), (IMessage**)&msg);
                mFh->UpdateEFLinearFixed(EF_SMS,
                        i, ba, String(NULL), msg);
            }
        }
    }
}

String SIMRecords::FindBestLanguage(
    /* [in] */ ArrayOf<Byte>* languages)
{
    String bestMatch(NULL);
    AutoPtr<IAssetManager> am;
    mContext->GetAssets((IAssetManager**)&am);
    AutoPtr<ArrayOf<String> > locales;
    am->GetLocales((ArrayOf<String>**)&locales);

    if ((languages == NULL) || (locales == NULL)) return String(NULL);

    // Each 2-bytes consists of one language
    for (Int32 i = 0; (i + 1) < languages->GetLength(); i += 2) {
        // try {
            String lang = String(*languages, i, 2); //, "ISO-8859-1");
            if (DBG) Log(String("languages from sim = ") + lang);
            for (Int32 j = 0; j < locales->GetLength(); j++) {
                if (!((*locales)[j]).IsNull() && (*locales)[j].GetLength() >= 2 &&
                        (*locales)[j].Substring(0, 2).EqualsIgnoreCase(lang)) {
                    return lang;
                }
            }
            if (!bestMatch.IsNull()) break;
        // } catch(java.io.UnsupportedEncodingException e) {
        //     log ("Failed to parse USIM language records" + e);
        // }
    }
    // no match found. return NULL
    return String(NULL);
}

void SIMRecords::SetLocaleFromUsim()
{
    String prefLang(NULL);
    // check EFli then EFpl
    prefLang = FindBestLanguage(mEfLi);

    if (prefLang.IsNull()) {
        prefLang = FindBestLanguage(mEfPl);
    }

    if (!prefLang.IsNull()) {
        // check country code from SIM
        String imsi;
        GetIMSI(&imsi);
        String country(NULL);
        if (!imsi.IsNull()) {
            country = MccTable::CountryCodeForMcc(
                                StringUtils::ParseInt32(imsi.Substring(0,3)));
        }
        if (DBG) {
            Log(String("Setting locale to ") + prefLang +
                String("_") + country);
        }
        MccTable::SetSystemLocale(mContext, prefLang, country);
    }
    else {
        if (DBG) Log(String("No suitable USIM selected locale"));
    }
}

void SIMRecords::SetSpnFromConfig(
    /* [in] */ const String& carrier)
{
    Boolean bContain = FALSE;
    mSpnOverride->ContainsCarrier(carrier, &bContain);
    if (bContain) {
        String str;
        mSpnOverride->GetSpn(carrier, &str);
        SetServiceProviderName(str);
        String name;
        GetServiceProviderName(&name);
        AutoPtr<ISystemProperties> sp;
        CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
        sp->Set(ITelephonyProperties::PROPERTY_ICC_OPERATOR_ALPHA, name);
    }
}

void SIMRecords::SetVoiceMailByCountry(
    /* [in] */ const String& spn)
{
    Boolean bContain = FALSE;
    mVmConfig->ContainsCarrier(spn, &bContain);
    if (bContain) {
        mIsVoiceMailFixed = TRUE;
        mVmConfig->GetVoiceMailNumber(spn, &mVoiceMailNum);
        mVmConfig->GetVoiceMailTag(spn, &mVoiceMailTag);
    }
}

void SIMRecords::OnLocked()
{
    if (DBG) Log(String("only fetch EF_LI and EF_PL in lock state"));
    LoadEfLiAndEfPl();
}

void SIMRecords::LoadEfLiAndEfPl()
{
    AppType type;
    mParentApp->GetType(&type);
    if (type == APPTYPE_USIM) {
        AutoPtr<EfUsimLiLoaded> pUsim = new EfUsimLiLoaded(this);
        AutoPtr<IMessage> msg1;
        ObtainMessage(EVENT_GET_ICC_RECORD_DONE, (IObject*)pUsim.Get(), (IMessage**)&msg1);
        mRecordsRequested = TRUE;
        mFh->LoadEFTransparent(EF_LI, msg1);
        mRecordsToLoad++;

        AutoPtr<EfPlLoaded> pPl = new EfPlLoaded(this);
        AutoPtr<IMessage> msg2;
        ObtainMessage(EVENT_GET_ICC_RECORD_DONE, (IObject*)pPl.Get(), (IMessage**)&msg2);
        mFh->LoadEFTransparent(EF_PL, msg2);
        mRecordsToLoad++;
    }
}

Boolean SIMRecords::IsOnMatchingPlmn(
    /* [in] */ const String& plmn)
{
    if (plmn.IsNull()) return FALSE;

    String strNumeric;
    GetOperatorNumeric(&strNumeric);
    if (plmn.Equals(strNumeric)) {
        return TRUE;
    }

    if (mSpdiNetworks != NULL) {
        AutoPtr<IIterator> it;
        mSpdiNetworks->GetIterator((IIterator**)&it);
        Boolean bHasNext = FALSE;
        while ((it->HasNext(&bHasNext), bHasNext)) {
            AutoPtr<IInterface> p;
            it->GetNext((IInterface**)&p);
            String spdiNet;
            ICharSequence::Probe(p)->ToString(&spdiNet);
            if (plmn.Equals(spdiNet)) {
                return TRUE;
            }
        }
    }
    return FALSE;
}

void SIMRecords::GetSpnFsm(
    /* [in] */ Boolean start,
    /* [in] */ AsyncResult* ar)
{
    AutoPtr<ArrayOf<Byte> > data;

    if (start) {
        // Check previous state to see if there is outstanding
        // SPN read
        if (mSpnState == READ_SPN_3GPP ||
           mSpnState == READ_SPN_CPHS ||
           mSpnState == READ_SPN_SHORT_CPHS ||
           mSpnState == INIT) {
            // Set INIT then return so the INIT code
            // will run when the outstanding read done.
            mSpnState = INIT;
            return;
        }
        else {
            mSpnState = INIT;
        }
    }

    AutoPtr<IIccUtils> iccu;
    CIccUtils::AcquireSingleton((IIccUtils**)&iccu);
    switch (mSpnState) {
        case INIT: {
            SetServiceProviderName(String(NULL));

            AutoPtr<IMessage> msg;
            ObtainMessage(EVENT_GET_SPN_DONE, (IMessage**)&msg);
            mFh->LoadEFTransparent(EF_SPN, msg);
            mRecordsToLoad++;

            mSpnState = READ_SPN_3GPP;
            break;
        }
        case READ_SPN_3GPP:
            if (ar != NULL && ar->mException == NULL) {
                AutoPtr<IArrayOf> array = IArrayOf::Probe(ar->mResult);
                assert(array != NULL);
                Int32 len = 0;
                array->GetLength(&len);
                data = ArrayOf<Byte>::Alloc(len);
                for (Int32 i = 0; i < len; i++) {
                    AutoPtr<IInterface> obj;
                    array->Get(i, (IInterface**)&obj);
                    IByte::Probe(obj)->GetValue(&(*data)[i]);
                }

                mSpnDisplayCondition = 0xff & (*data)[0];
                String str;
                iccu->AdnStringFieldToString(
                        data, 1, data->GetLength() - 1,
                        &str);
                SetServiceProviderName(str);

                String name;
                GetServiceProviderName(&name);
                if (DBG) {
                    Log(String("Load EF_SPN: ") + name
                        + String(" spnDisplayCondition: ") + StringUtils::ToString(mSpnDisplayCondition));
                }
                SetSystemProperty(ITelephonyProperties::PROPERTY_ICC_OPERATOR_ALPHA, name);

                mSpnState = IDLE;
            }
            else {
                AutoPtr<IMessage> msg;
                ObtainMessage(EVENT_GET_SPN_DONE, (IMessage**)&msg);
                mFh->LoadEFTransparent(EF_SPN_CPHS, msg);
                mRecordsToLoad++;

                mSpnState = READ_SPN_CPHS;

                // See TS 51.011 10.3.11.  Basically, default to
                // show PLMN always, and SPN also if roaming.
                mSpnDisplayCondition = -1;
            }
            break;
        case READ_SPN_CPHS:
            if (ar != NULL && ar->mException == NULL) {
                AutoPtr<IArrayOf> array = IArrayOf::Probe(ar->mResult);
                assert(array != NULL);
                Int32 len = 0;
                array->GetLength(&len);
                data = ArrayOf<Byte>::Alloc(len);
                for (Int32 i = 0; i < len; i++) {
                    AutoPtr<IInterface> obj;
                    array->Get(i, (IInterface**)&obj);
                    IByte::Probe(obj)->GetValue(&(*data)[i]);
                }
                String str;
                iccu->AdnStringFieldToString(data, 0, data->GetLength(), &str);
                SetServiceProviderName(str);

                String name;
                GetServiceProviderName(&name);
                if (DBG) {
                    Log(String("Load EF_SPN_CPHS: ") + name);
                }
                SetSystemProperty(ITelephonyProperties::PROPERTY_ICC_OPERATOR_ALPHA, name);

                mSpnState = IDLE;
            }
            else {
                AutoPtr<IMessage> msg;
                ObtainMessage(EVENT_GET_SPN_DONE, (IMessage**)&msg);
                mFh->LoadEFTransparent(
                        EF_SPN_SHORT_CPHS, msg);
                mRecordsToLoad++;

                mSpnState = READ_SPN_SHORT_CPHS;
            }
            break;
        case READ_SPN_SHORT_CPHS:
            if (ar != NULL && ar->mException == NULL) {
                AutoPtr<IArrayOf> array = IArrayOf::Probe(ar->mResult);
                assert(array != NULL);
                Int32 len = 0;
                array->GetLength(&len);
                data = ArrayOf<Byte>::Alloc(len);
                for (Int32 i = 0; i < len; i++) {
                    AutoPtr<IInterface> obj;
                    array->Get(i, (IInterface**)&obj);
                    IByte::Probe(obj)->GetValue(&(*data)[i]);
                }
                String str;
                iccu->AdnStringFieldToString(data, 0, data->GetLength(), &str);
                SetServiceProviderName(str);

                String name;
                GetServiceProviderName(&name);
                if (DBG) {
                    Log(String("Load EF_SPN_SHORT_CPHS: ") + name);
                }
                SetSystemProperty(ITelephonyProperties::PROPERTY_ICC_OPERATOR_ALPHA, name);
            }
            else {
                if (DBG) Log(String("No SPN loaded in either CHPS or 3GPP"));
                if (!mPnnHomeName.IsNull() && mSpn.IsNull()) {
                    if (DBG) Log(String("Falling back to home network name for SPN"));
                    mSpn = mPnnHomeName;
                    SetSystemProperty(ITelephonyProperties::PROPERTY_ICC_OPERATOR_ALPHA, mSpn);
                    mRecordsEventsRegistrants->NotifyResult(CoreUtils::Convert(EVENT_SPN));
                }
            }

            mSpnState = IDLE;
            break;
        default:
            mSpnState = IDLE;
    }
}

void SIMRecords::ParseEfSpdi(
    /* [in] */ ArrayOf<Byte>* data)
{
    AutoPtr<ISimTlv> tlv;
    CSimTlv::New(data, 0, data->GetLength(), (ISimTlv**)&tlv);

    AutoPtr<ArrayOf<Byte> > plmnEntries;

    Boolean bValid = FALSE, bNext = FALSE;
    for ( ; (tlv->IsValidObject(&bValid), bValid); tlv->NextObject(&bNext)) {
        // Skip SPDI tag, if existant
        Int32 tag = 0;
        tlv->GetTag(&tag);
        if (tag == TAG_SPDI) {
            AutoPtr<ArrayOf<Byte> > data;
            tlv->GetData((ArrayOf<Byte>**)&data);
            CSimTlv::New(data, 0, data->GetLength(), (ISimTlv**)&tlv);
        }
        // There should only be one TAG_SPDI_PLMN_LIST
        if (tag == TAG_SPDI_PLMN_LIST) {
            tlv->GetData((ArrayOf<Byte>**)&plmnEntries);
            break;
        }
    }

    if (plmnEntries == NULL) {
        return;
    }

    CArrayList::New(plmnEntries->GetLength() / 3, (IArrayList**)&mSpdiNetworks);

    for (Int32 i = 0; i + 2 < plmnEntries->GetLength(); i += 3) {
        String plmnCode;
        AutoPtr<IIccUtils> iccu;
        CIccUtils::AcquireSingleton((IIccUtils**)&iccu);
        iccu->BcdToString(plmnEntries, i, 3, &plmnCode);

        // Valid operator codes are 5 or 6 digits
        if (plmnCode.GetLength() >= 5) {
            Log(String("EF_SPDI network: ") + plmnCode);
            mSpdiNetworks->Add(CoreUtils::Convert(plmnCode));
        }
    }
}

Boolean SIMRecords::IsCphsMailboxEnabled()
{
    if (mCphsInfo == NULL)  return FALSE;
    return (((*mCphsInfo)[1] & CPHS_SST_MBN_MASK) == CPHS_SST_MBN_ENABLED );
}

void SIMRecords::HandleEfCspData(
    /* [in] */ ArrayOf<Byte>* data)
{
    // As per spec CPHS4_2.WW6, CPHS B.4.7.1, EF_CSP contains CPHS defined
    // 18 bytes (i.e 9 service groups info) and additional data specific to
    // operator. The valueAddedServicesGroup is not part of standard
    // services. This is operator specific and can be programmed any where.
    // Normally this is programmed as 10th service after the standard
    // services.
    Int32 usedCspGroups = data->GetLength() / 2;
    // This is the "Service Group Number" of "Value Added Services Group".
    Byte valueAddedServicesGroup = (Byte)0xC0;

    mCspPlmnEnabled = TRUE;
    for (Int32 i = 0; i < usedCspGroups; i++) {
         if ((*data)[2 * i] == valueAddedServicesGroup) {
            Log(String("[CSP] found ValueAddedServicesGroup, value ") + StringUtils::ToString((*data)[(2 * i) + 1]));
            if (((*data)[(2 * i) + 1] & 0x80) == 0x80) {
                // Bit 8 is for
                // "Restriction of menu options for manual PLMN selection".
                // Operator Selection menu should be enabled.
                mCspPlmnEnabled = TRUE;
            }
            else {
                mCspPlmnEnabled = FALSE;
                // Operator Selection menu should be disabled.
                // Operator Selection Mode should be set to Automatic.
                Log(String("[CSP] Set Automatic Network Selection"));
                mNetworkSelectionModeAutomaticRegistrants->NotifyRegistrants();
            }
            return;
        }
    }

    Log(String("[CSP] Value Added Service Group (0xC0), not found!"));
}

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
