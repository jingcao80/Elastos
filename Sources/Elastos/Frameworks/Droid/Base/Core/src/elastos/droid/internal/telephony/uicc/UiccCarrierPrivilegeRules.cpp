#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.CoreLibrary.Utility.Concurrent.h"
#include "Elastos.CoreLibrary.Security.h"
#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Telephony.h"
#include "elastos/droid/internal/telephony/uicc/UiccCarrierPrivilegeRules.h"
#include "elastos/droid/content/pm/CPackageInfo.h"
#include "elastos/droid/content/pm/CActivityInfo.h"
#include "elastos/droid/content/pm/CResolveInfo.h"
#include "elastos/droid/internal/telephony/uicc/CIccUtils.h"
#include "elastos/droid/internal/telephony/uicc/IccIoResult.h"
#include "elastos/droid/os/Binder.h"
#include "elastos/droid/os/AsyncResult.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/Arrays.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::Content::Pm::CPackageInfo;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Pm::CResolveInfo;
using Elastos::Droid::Content::Pm::CActivityInfo;
using Elastos::Droid::Os::AsyncResult;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringUtils;
using Elastos::IO::IInputStream;
using Elastos::IO::IByteArrayInputStream;
using Elastos::IO::CByteArrayInputStream;
using Elastos::Utility::Arrays;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IIterator;
using Elastos::Utility::ICollection;
using Elastos::Utility::Concurrent::Atomic::CAtomicInteger32;
using Elastos::Utility::Logging::Logger;
using Elastos::Security::IMessageDigest;
using Elastos::Security::IMessageDigestHelper;
using Elastos::Security::CMessageDigestHelper;
using Elastos::Security::Cert::CCertificateFactoryHelper;
using Elastos::Security::Cert::IX509Certificate;
using Elastos::Security::Cert::ICertificate;
using Elastos::Security::Cert::ICertificateFactory;
using Elastos::Security::Cert::ICertificateFactoryHelper;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

//=====================================================================
//                UiccCarrierPrivilegeRules::AccessRule
//=====================================================================
UiccCarrierPrivilegeRules::AccessRule::AccessRule(
    /* [in] */ ArrayOf<Byte>* certificateHash,
    /* [in] */ const String& packageName,
    /* [in] */ Int64 accessType)
{
    mCertificateHash = certificateHash;
    mPackageName = packageName;
    mAccessType = accessType;
}

ECode UiccCarrierPrivilegeRules::AccessRule::Matches(
    /* [in] */ ArrayOf<Byte>* certHash,
    /* [in] */ const String& packageName,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = certHash != NULL && Arrays::Equals(mCertificateHash.Get(), certHash) &&
          (mPackageName.IsNull() || mPackageName.Equals(packageName));
    return NOERROR;
}

ECode UiccCarrierPrivilegeRules::AccessRule::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = String("cert: ") + Arrays::ToString(mCertificateHash) +
            String(" pkg: ") + mPackageName +
            String(" access: ") + StringUtils::ToString(mAccessType);
    return NOERROR;
}

//=====================================================================
//                    UiccCarrierPrivilegeRules::TLV
//=====================================================================
UiccCarrierPrivilegeRules::TLV::TLV(
    /* [in] */ const String& tag)
{
    mTag = tag;
}

ECode UiccCarrierPrivilegeRules::TLV::Parse(
    /* [in] */ const String& data,
    /* [in] */ Boolean shouldConsumeAll,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    Logger::D(LOGTAG, String("Parse TLV: ") + mTag);
    if (!data.StartWith(mTag)) {
        // throw new IllegalArgumentException("Tags don't match.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Int32 index = mTag.GetLength();
    if (index + 2 > data.GetLength()) {
        // throw new IllegalArgumentException("No length.");v
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mLength = 2 * StringUtils::ParseInt32(
            data.Substring(index, index + 2), 16);
    index += 2;

    Int32 remainingLength = data.GetLength() - (index + mLength);
    if (remainingLength < 0) {
        // throw new IllegalArgumentException("Not enough data.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (shouldConsumeAll && (remainingLength != 0)) {
        // throw new IllegalArgumentException("Did not consume all.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mValue = data.Substring(index, index + mLength);

    Logger::D(LOGTAG, String("Got TLV: ") + mTag + String(",") +
            StringUtils::ToString(mLength) + String(",") + mValue);

    *result = data.Substring(index + mLength);
    return NOERROR;
}

//=====================================================================
//                      UiccCarrierPrivilegeRules
//=====================================================================
CAR_INTERFACE_IMPL(UiccCarrierPrivilegeRules, Handler, IUiccCarrierPrivilegeRules);

const String UiccCarrierPrivilegeRules::LOGTAG("UiccCarrierPrivilegeRules");
const String UiccCarrierPrivilegeRules::AID("A00000015141434C00");
const Int32 UiccCarrierPrivilegeRules::CLA;
const Int32 UiccCarrierPrivilegeRules::COMMAND;
const Int32 UiccCarrierPrivilegeRules::P1;
const Int32 UiccCarrierPrivilegeRules::P2;
const Int32 UiccCarrierPrivilegeRules::P3;
const String UiccCarrierPrivilegeRules::DATA("");
const String UiccCarrierPrivilegeRules::TAG_ALL_REF_AR_DO("FF40");
const String UiccCarrierPrivilegeRules::TAG_REF_AR_DO("E2");
const String UiccCarrierPrivilegeRules::TAG_REF_DO("E1");
const String UiccCarrierPrivilegeRules::TAG_DEVICE_APP_ID_REF_DO("C1");
const String UiccCarrierPrivilegeRules::TAG_PKG_REF_DO("CA");
const String UiccCarrierPrivilegeRules::TAG_AR_DO("E3");
const String UiccCarrierPrivilegeRules::TAG_PERM_AR_DO("DB");
const Int32 UiccCarrierPrivilegeRules::EVENT_OPEN_LOGICAL_CHANNEL_DONE;
const Int32 UiccCarrierPrivilegeRules::EVENT_TRANSMIT_LOGICAL_CHANNEL_DONE;
const Int32 UiccCarrierPrivilegeRules::EVENT_CLOSE_LOGICAL_CHANNEL_DONE;
const Int32 UiccCarrierPrivilegeRules::STATE_LOADING;
const Int32 UiccCarrierPrivilegeRules::STATE_LOADED;
const Int32 UiccCarrierPrivilegeRules::STATE_ERROR;

UiccCarrierPrivilegeRules::UiccCarrierPrivilegeRules()
{
    Handler::constructor();
}

ECode UiccCarrierPrivilegeRules::constructor(
    /* [in] */ IUiccCard* uiccCard,
    /* [in] */ IMessage* loadedCallback)
{
    Handler::constructor();
    Logger::D(LOGTAG, String("Creating UiccCarrierPrivilegeRules"));
    mUiccCard = uiccCard;
    CAtomicInteger32::New(STATE_LOADING, (IAtomicInteger32**)&mState);
    mLoadedCallback = loadedCallback;

    // Start loading the rules.
    AutoPtr<IMessage> msg;
    ObtainMessage(EVENT_OPEN_LOGICAL_CHANNEL_DONE, NULL, (IMessage**)&msg);
    mUiccCard->IccOpenLogicalChannel(AID, msg);
    return NOERROR;
}

ECode UiccCarrierPrivilegeRules::AreCarrierPriviligeRulesLoaded(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 state = 0;
    mState->Get(&state);
    *result = state != STATE_LOADING;
    return NOERROR;
}

ECode UiccCarrierPrivilegeRules::GetCarrierPrivilegeStatus(
    /* [in] */ ISignature* signature,
    /* [in] */ const String& packageName,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    Logger::D(LOGTAG, String("hasCarrierPrivileges: ")/* + signature*/ +
            String(" : ") + packageName);
    Int32 state = 0;
    mState->Get(&state);
    if (state == STATE_LOADING) {
        Logger::D(LOGTAG, String("Rules not loaded."));
        *result = ITelephonyManager::CARRIER_PRIVILEGE_STATUS_RULES_NOT_LOADED;
        return NOERROR;
    }
    else if (state == STATE_ERROR) {
        Logger::D(LOGTAG, String("Error loading rules."));
        *result = ITelephonyManager::CARRIER_PRIVILEGE_STATUS_ERROR_LOADING_RULES;
        return NOERROR;
    }

    AutoPtr<ArrayOf<Byte> > certHash = GetCertHash(signature);
    if (certHash == NULL) {
        *result = ITelephonyManager::CARRIER_PRIVILEGE_STATUS_NO_ACCESS;
        return NOERROR;
    }
    AutoPtr<IIccUtils> iccu;
    CIccUtils::AcquireSingleton((IIccUtils**)&iccu);
    String str;
    iccu->BytesToHexString(certHash, &str);
    Logger::E(LOGTAG, String("Checking: ") + str
            + String(" : ") + packageName);

    AutoPtr<IIterator> it;
    mAccessRules->GetIterator((IIterator**)&it);
    Boolean bHasNext = FALSE;
    while ((it->HasNext(&bHasNext), bHasNext)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<AccessRule> ar = (AccessRule*)(IObject::Probe(p));
        Boolean b = FALSE;
        ar->Matches(certHash, packageName, &b);
        if (b) {
            Logger::D(LOGTAG, String("Match found!"));
            *result = ITelephonyManager::CARRIER_PRIVILEGE_STATUS_HAS_ACCESS;
            return NOERROR;
        }
    }

    Logger::D(LOGTAG, String("No matching rule found. Returning false."));
    *result = ITelephonyManager::CARRIER_PRIVILEGE_STATUS_NO_ACCESS;
    return NOERROR;
}

ECode UiccCarrierPrivilegeRules::GetCarrierPrivilegeStatus(
    /* [in] */ IPackageManager* packageManager,
    /* [in] */ const String& packageName,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // try {
    AutoPtr<IPackageInfo> pInfo;
    packageManager->GetPackageInfo(packageName,
        IPackageManager::GET_SIGNATURES, (IPackageInfo**)&pInfo);
    AutoPtr<CPackageInfo> _pInfo = (CPackageInfo*)pInfo.Get();
    AutoPtr<ArrayOf<ISignature*> > signatures = _pInfo->mSignatures;
    for (Int32 i = 0; i < signatures->GetLength(); i++) {
        AutoPtr<ISignature> sig = (*signatures)[i];
        Int32 accessStatus = 0;
        GetCarrierPrivilegeStatus(sig, _pInfo->mPackageName, &accessStatus);
        if (accessStatus != ITelephonyManager::CARRIER_PRIVILEGE_STATUS_NO_ACCESS) {
            *result = accessStatus;
            return NOERROR;
        }
    }
    // } catch (PackageManager.NameNotFoundException ex) {
    //     Rlog.e(LOGTAG, "NameNotFoundException", ex);
    // }
    *result = ITelephonyManager::CARRIER_PRIVILEGE_STATUS_NO_ACCESS;
    return NOERROR;
}

ECode UiccCarrierPrivilegeRules::GetCarrierPrivilegeStatusForCurrentTransaction(
    /* [in] */ IPackageManager* packageManager,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ArrayOf<String> > packages;
    packageManager->GetPackagesForUid(Binder::GetCallingUid(), (ArrayOf<String>**)&packages);

    for (Int32 i = 0; i < packages->GetLength(); i++) {
        String pkg = (*packages)[i];
        Int32 accessStatus = 0;
        GetCarrierPrivilegeStatus(packageManager, pkg, &accessStatus);
        if (accessStatus != ITelephonyManager::CARRIER_PRIVILEGE_STATUS_NO_ACCESS) {
            *result = accessStatus;
            return NOERROR;
        }
    }
    *result = ITelephonyManager::CARRIER_PRIVILEGE_STATUS_NO_ACCESS;
    return NOERROR;
}

ECode UiccCarrierPrivilegeRules::GetCarrierPackageNamesForIntent(
    /* [in] */ IPackageManager* packageManager,
    /* [in] */ IIntent* intent,
    /* [out] */ IList/*<String>*/** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IList> packages;
    CArrayList::New((IList**)&packages);
    AutoPtr<IList> receivers;
    CArrayList::New((IList**)&receivers);
    AutoPtr<IList> broadcastReceivers;
    packageManager->QueryBroadcastReceivers(intent, 0, (IList**)&broadcastReceivers);
    receivers->AddAll(ICollection::Probe(broadcastReceivers));
    AutoPtr<IList> contentProviders;
    packageManager->QueryIntentContentProviders(intent, 0, (IList**)&contentProviders);
    receivers->AddAll(ICollection::Probe(contentProviders));
    AutoPtr<IList> activities;
    packageManager->QueryIntentActivities(intent, 0, (IList**)&activities);
    receivers->AddAll(ICollection::Probe(activities));
    AutoPtr<IList> services;
    packageManager->QueryIntentServices(intent, 0, (IList**)&services);
    receivers->AddAll(ICollection::Probe(services));

    AutoPtr<IIterator> it;
    receivers->GetIterator((IIterator**)&it);
    Boolean bHasNext = FALSE;
    while ((it->HasNext(&bHasNext), bHasNext)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<CResolveInfo> resolveInfo = (CResolveInfo*)IResolveInfo::Probe(p);
        if (resolveInfo->mActivityInfo == NULL) {
            continue;
        }
        AutoPtr<CActivityInfo> _activityInfo = (CActivityInfo*)(resolveInfo->mActivityInfo).Get();
        String packageName = _activityInfo->mPackageName;
        Int32 status = 0;
        GetCarrierPrivilegeStatus(packageManager, packageName, &status);
        if (status == ITelephonyManager::CARRIER_PRIVILEGE_STATUS_HAS_ACCESS) {
            packages->Add(CoreUtils::Convert(packageName));
        }
        else if (status != ITelephonyManager::CARRIER_PRIVILEGE_STATUS_NO_ACCESS) {
            // Any status apart from HAS_ACCESS and NO_ACCESS is considered an error.
            *result = NULL;
            return NOERROR;
        }
    }

    *result = packages;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode UiccCarrierPrivilegeRules::HandleMessage(
    /* [in] */ IMessage* msg)
{
    AutoPtr<AsyncResult> ar;

    Int32 what = 0;
    msg->GetWhat(&what);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    switch (what) {
        case EVENT_OPEN_LOGICAL_CHANNEL_DONE: {
            Logger::D(LOGTAG, String("EVENT_OPEN_LOGICAL_CHANNEL_DONE"));
            ar = (AsyncResult*)IAsyncResult::Probe(obj);
            if (ar->mException == NULL && ar->mResult != NULL) {
                AutoPtr<IArrayOf> array = IArrayOf::Probe(ar->mResult);
                AutoPtr<IInterface> o;
                array->Get(0, (IInterface**)&o);
                Int32 channelId = 0;
                IInteger32::Probe(o)->GetValue(&channelId);
                AutoPtr<IMessage> msg;
                ObtainMessage(EVENT_TRANSMIT_LOGICAL_CHANNEL_DONE, CoreUtils::Convert(channelId), (IMessage**)&msg);
                mUiccCard->IccTransmitApduLogicalChannel(channelId, CLA, COMMAND, P1, P2, P3, DATA,
                    msg);
            }
            else {
                Logger::E(LOGTAG, String("Error opening channel"));
                UpdateState(STATE_ERROR);
            }
            break;
        }
        case EVENT_TRANSMIT_LOGICAL_CHANNEL_DONE: {
            Logger::D(LOGTAG, String("EVENT_TRANSMIT_LOGICAL_CHANNEL_DONE"));
            ar = (AsyncResult*)IAsyncResult::Probe(obj);
            if (ar->mException == NULL && ar->mResult != NULL) {
                AutoPtr<IccIoResult> response = (IccIoResult*)IIccIoResult::Probe(ar->mResult);
                if (response->mPayload != NULL && response->mSw1 == 0x90 && response->mSw2 == 0x00) {
                    // try {
                    AutoPtr<IIccUtils> iccu;
                    CIccUtils::AcquireSingleton((IIccUtils**)&iccu);
                    String str;
                    iccu->BytesToHexString(response->mPayload, &str);
                    mAccessRules = ParseRules(str);
                    UpdateState(STATE_LOADED);
                    // } catch (IllegalArgumentException ex) {
                    //     Rlog.e(LOGTAG, "Error parsing rules: " + ex);
                    //     updateState(STATE_ERROR);
                    // }
                }
                else {
                    Logger::E(LOGTAG, String("Invalid response: payload=") + Arrays::ToString(response->mPayload) +
                            String(" sw1=") + StringUtils::ToString(response->mSw1) +
                            String(" sw2=") + StringUtils::ToString(response->mSw2));
                    UpdateState(STATE_ERROR);
                }
            }
            else {
                Logger::E(LOGTAG, String("Error reading value from SIM."));
                UpdateState(STATE_ERROR);
            }

            Int32 channelId = 0;
            IInteger32::Probe(ar->mUserObj)->GetValue(&channelId);
            AutoPtr<IMessage> msg;
            ObtainMessage(EVENT_CLOSE_LOGICAL_CHANNEL_DONE, (IMessage**)&msg);
            mUiccCard->IccCloseLogicalChannel(channelId, msg);
        break;
        }
        case EVENT_CLOSE_LOGICAL_CHANNEL_DONE:
            Logger::D(LOGTAG, String("EVENT_CLOSE_LOGICAL_CHANNEL_DONE"));
            break;

        default:
            Logger::E(LOGTAG, String("Unknown event ") + StringUtils::ToString(what));
    }
    return NOERROR;
}

AutoPtr<IList/*< AutoPtr<AccessRule> >*/> UiccCarrierPrivilegeRules::ParseRules(
    /* [in] */ const String& _rules)
{
    String rules = _rules.ToUpperCase(); // rules.ToUpperCase(Locale.US);
    Logger::D(LOGTAG, String("Got rules: ") + rules);

    AutoPtr<TLV> allRefArDo = new TLV(TAG_ALL_REF_AR_DO); //FF40
    String res;
    allRefArDo->Parse(rules, TRUE, &res);

    String arDos = allRefArDo->mValue;
    AutoPtr<IList> accessRules;
    CArrayList::New((IList**)&accessRules);
    while (!arDos.IsEmpty()) {
        AutoPtr<TLV> refArDo = new TLV(TAG_REF_AR_DO); //E2
        refArDo->Parse(arDos, FALSE, &arDos);
        AutoPtr<AccessRule> accessRule = ParseRefArdo(refArDo->mValue);
        if (accessRule != NULL) {
            accessRules->Add(TO_IINTERFACE(accessRule));
        }
        else {
            Logger::E(LOGTAG, String("Skip unrecognized rule.") + refArDo->mValue);
        }
    }
    return accessRules;
}

AutoPtr<UiccCarrierPrivilegeRules::AccessRule> UiccCarrierPrivilegeRules::ParseRefArdo(
    /* [in] */ const String& rule)
{
    String _rule = rule;
    Logger::D(LOGTAG, String("Got rule: ") + rule);

    String certificateHash(NULL);
    String packageName(NULL);
    String tmp(NULL);
    Int64 accessType = 0;

    while (!_rule.IsEmpty()) {
        if (_rule.StartWith(TAG_REF_DO)) {
            AutoPtr<TLV> refDo = new TLV(TAG_REF_DO); //E1
            refDo->Parse(_rule, FALSE, &_rule);

            // Skip unrelated rules.
            if (!refDo->mValue.StartWith(TAG_DEVICE_APP_ID_REF_DO)) {
                return NULL;
            }

            AutoPtr<TLV> deviceDo = new TLV(TAG_DEVICE_APP_ID_REF_DO); //C1
            deviceDo->Parse(refDo->mValue, FALSE, &tmp);
            certificateHash = deviceDo->mValue;

            if (!tmp.IsEmpty()) {
                if (!tmp.StartWith(TAG_PKG_REF_DO)) {
                    return NULL;
                }
                AutoPtr<TLV> pkgDo = new TLV(TAG_PKG_REF_DO); //CA
                String res;
                pkgDo->Parse(tmp, TRUE, &res);
                AutoPtr<IIccUtils> iccu;
                CIccUtils::AcquireSingleton((IIccUtils**)&iccu);
                AutoPtr<ArrayOf<Byte> > arr;
                iccu->HexStringToBytes(pkgDo->mValue, (ArrayOf<Byte>**)&arr);
                packageName = String(*arr);
            }
            else {
                packageName = String(NULL);
            }
        }
        else if (_rule.StartWith(TAG_AR_DO)) {
            AutoPtr<TLV> arDo = new TLV(TAG_AR_DO); //E3
            arDo->Parse(_rule, FALSE, &_rule);

            // Skip unrelated rules.
            if (!arDo->mValue.StartWith(TAG_PERM_AR_DO)) {
                return NULL;
            }

            AutoPtr<TLV> permDo = new TLV(TAG_PERM_AR_DO); //DB
            String res;
            permDo->Parse(arDo->mValue, TRUE, &res);
            Logger::E(LOGTAG, permDo->mValue);
        }
        else  {
            // Spec requires it must be either TAG_REF_DO or TAG_AR_DO.
            // throw new RuntimeException("Invalid Rule type");
            return NULL;
        }
    }

    Logger::E(LOGTAG, String("Adding: ") + certificateHash +
            String(" : ") + packageName +
            String(" : ") + StringUtils::ToString(accessType));

    AutoPtr<IIccUtils> iccu;
    CIccUtils::AcquireSingleton((IIccUtils**)&iccu);
    AutoPtr<ArrayOf<Byte> > arr;
    iccu->HexStringToBytes(certificateHash, (ArrayOf<Byte>**)&arr);
    AutoPtr<AccessRule> accessRule = new AccessRule(arr,
                                        packageName, accessType);
    Logger::E(LOGTAG, String("Parsed rule: ")/* + accessRule*/);
    return accessRule;
}

AutoPtr<ArrayOf<Byte> > UiccCarrierPrivilegeRules::GetCertHash(
    /* [in] */ ISignature* signature)
{
    // TODO: Is the following sufficient.
    // try {
    AutoPtr<ICertificateFactoryHelper> hlp;
    CCertificateFactoryHelper::AcquireSingleton((ICertificateFactoryHelper**)&hlp);
    AutoPtr<ICertificateFactory> certFactory;
    hlp->GetInstance(String("X.509"), (ICertificateFactory**)&certFactory);

    AutoPtr<ArrayOf<Byte> > arr; // = signature.toByteArray();
    AutoPtr<IByteArrayInputStream> inputStream;
    CByteArrayInputStream::New(arr, (IByteArrayInputStream**)&inputStream);
    AutoPtr<ICertificate> cf;
    certFactory->GenerateCertificate(IInputStream::Probe(inputStream), (ICertificate**)&cf);
    AutoPtr<IX509Certificate> cert = IX509Certificate::Probe(cf);

    AutoPtr<IMessageDigestHelper> mdhlp;
    CMessageDigestHelper::AcquireSingleton((IMessageDigestHelper**)&mdhlp);
    AutoPtr<IMessageDigest> md;
    mdhlp->GetInstance(String("SHA"), (IMessageDigest**)&md);
    AutoPtr<ArrayOf<Byte> > encode;
    ICertificate::Probe(cert)->GetEncoded((ArrayOf<Byte>**)&encode);
    AutoPtr<ArrayOf<Byte> > res;
    md->Digest(encode, (ArrayOf<Byte>**)&res);
    return res;
    // } catch (CertificateException ex) {
    //     Rlog.e(LOGTAG, "CertificateException: " + ex);
    // } catch (NoSuchAlgorithmException ex) {
    //     Rlog.e(LOGTAG, "NoSuchAlgorithmException: " + ex);
    // }

    // Rlog.e(LOGTAG, "Cannot compute cert hash");
    // return NULL;
}

void UiccCarrierPrivilegeRules::UpdateState(
    /* [in] */ Int32 newState)
{
    mState->Set(newState);
    if (mLoadedCallback != NULL) {
        mLoadedCallback->SendToTarget();
    }
}

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
