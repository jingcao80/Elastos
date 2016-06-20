#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.CoreLibrary.Utility.Concurrent.h"
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/uicc/UiccCarrierPrivilegeRules.h"

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
    // ==================before translated======================
    // this.certificateHash = certificateHash;
    // this.packageName = packageName;
    // this.accessType = accessType;
}

ECode UiccCarrierPrivilegeRules::AccessRule::Matches(
    /* [in] */ ArrayOf<Byte>* certHash,
    /* [in] */ const String& packageName,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return certHash != null && Arrays.equals(this.certificateHash, certHash) &&
    //       (this.packageName == null || this.packageName.equals(packageName));
    assert(0);
    return NOERROR;
}

ECode UiccCarrierPrivilegeRules::AccessRule::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return "cert: " + certificateHash + " pkg: " + packageName +
    //     " access: " + accessType;
    assert(0);
    return NOERROR;
}

//=====================================================================
//                    UiccCarrierPrivilegeRules::TLV
//=====================================================================
UiccCarrierPrivilegeRules::TLV::TLV(
    /* [in] */ const String& tag)
{
    // ==================before translated======================
    // this.tag = tag;
}

ECode UiccCarrierPrivilegeRules::TLV::Parse(
    /* [in] */ const String& data,
    /* [in] */ Boolean shouldConsumeAll,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // Rlog.d(LOGTAG, "Parse TLV: " + tag);
    // if (!data.startsWith(tag)) {
    //     throw new IllegalArgumentException("Tags don't match.");
    // }
    // int index = tag.length();
    // if (index + 2 > data.length()) {
    //     throw new IllegalArgumentException("No length.");
    // }
    // length = new Integer(2 * Integer.parseInt(
    //         data.substring(index, index + 2), 16));
    // index += 2;
    //
    // int remainingLength = data.length() - (index + length);
    // if (remainingLength < 0) {
    //     throw new IllegalArgumentException("Not enough data.");
    // }
    // if (shouldConsumeAll && (remainingLength != 0)) {
    //     throw new IllegalArgumentException("Did not consume all.");
    // }
    // value = data.substring(index, index + length);
    //
    // Rlog.d(LOGTAG, "Got TLV: " + tag + "," + length + "," + value);
    //
    // return data.substring(index + length);
    assert(0);
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
}

ECode UiccCarrierPrivilegeRules::constructor(
    /* [in] */ IUiccCard* uiccCard,
    /* [in] */ IMessage* loadedCallback)
{
    // ==================before translated======================
    // Rlog.d(LOGTAG, "Creating UiccCarrierPrivilegeRules");
    // mUiccCard = uiccCard;
    // mState = new AtomicInteger(STATE_LOADING);
    // mLoadedCallback = loadedCallback;
    //
    // // Start loading the rules.
    // mUiccCard.iccOpenLogicalChannel(AID,
    //     obtainMessage(EVENT_OPEN_LOGICAL_CHANNEL_DONE, null));
    return NOERROR;
}

ECode UiccCarrierPrivilegeRules::AreCarrierPriviligeRulesLoaded(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mState.get() != STATE_LOADING;
    assert(0);
    return NOERROR;
}

ECode UiccCarrierPrivilegeRules::GetCarrierPrivilegeStatus(
    /* [in] */ ISignature* signature,
    /* [in] */ const String& packageName,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // Rlog.d(LOGTAG, "hasCarrierPrivileges: " + signature + " : " + packageName);
    // int state = mState.get();
    // if (state == STATE_LOADING) {
    //     Rlog.d(LOGTAG, "Rules not loaded.");
    //     return TelephonyManager.CARRIER_PRIVILEGE_STATUS_RULES_NOT_LOADED;
    // } else if (state == STATE_ERROR) {
    //     Rlog.d(LOGTAG, "Error loading rules.");
    //     return TelephonyManager.CARRIER_PRIVILEGE_STATUS_ERROR_LOADING_RULES;
    // }
    //
    // byte[] certHash = getCertHash(signature);
    // if (certHash == null) {
    //   return TelephonyManager.CARRIER_PRIVILEGE_STATUS_NO_ACCESS;
    // }
    // Rlog.e(LOGTAG, "Checking: " + IccUtils.bytesToHexString(certHash) + " : " + packageName);
    //
    // for (AccessRule ar : mAccessRules) {
    //     if (ar.matches(certHash, packageName)) {
    //         Rlog.d(LOGTAG, "Match found!");
    //         return TelephonyManager.CARRIER_PRIVILEGE_STATUS_HAS_ACCESS;
    //     }
    // }
    //
    // Rlog.d(LOGTAG, "No matching rule found. Returning false.");
    // return TelephonyManager.CARRIER_PRIVILEGE_STATUS_NO_ACCESS;
    assert(0);
    return NOERROR;
}

ECode UiccCarrierPrivilegeRules::GetCarrierPrivilegeStatus(
    /* [in] */ IPackageManager* packageManager,
    /* [in] */ const String& packageName,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // try {
    //     PackageInfo pInfo = packageManager.getPackageInfo(packageName,
    //         PackageManager.GET_SIGNATURES);
    //     Signature[] signatures = pInfo.signatures;
    //     for (Signature sig : signatures) {
    //         int accessStatus = getCarrierPrivilegeStatus(sig, pInfo.packageName);
    //         if (accessStatus != TelephonyManager.CARRIER_PRIVILEGE_STATUS_NO_ACCESS) {
    //             return accessStatus;
    //         }
    //     }
    // } catch (PackageManager.NameNotFoundException ex) {
    //     Rlog.e(LOGTAG, "NameNotFoundException", ex);
    // }
    // return TelephonyManager.CARRIER_PRIVILEGE_STATUS_NO_ACCESS;
    assert(0);
    return NOERROR;
}

ECode UiccCarrierPrivilegeRules::GetCarrierPrivilegeStatusForCurrentTransaction(
    /* [in] */ IPackageManager* packageManager,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // String[] packages = packageManager.getPackagesForUid(Binder.getCallingUid());
    //
    // for (String pkg : packages) {
    //     int accessStatus = getCarrierPrivilegeStatus(packageManager, pkg);
    //     if (accessStatus != TelephonyManager.CARRIER_PRIVILEGE_STATUS_NO_ACCESS) {
    //         return accessStatus;
    //     }
    // }
    // return TelephonyManager.CARRIER_PRIVILEGE_STATUS_NO_ACCESS;
    assert(0);
    return NOERROR;
}

ECode UiccCarrierPrivilegeRules::GetCarrierPackageNamesForIntent(
    /* [in] */ IPackageManager* packageManager,
    /* [in] */ IIntent* intent,
    /* [out] */ IList/*<String>*/** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // List<String> packages = new ArrayList<String>();
    // List<ResolveInfo> receivers = new ArrayList<ResolveInfo>();
    // receivers.addAll(packageManager.queryBroadcastReceivers(intent, 0));
    // receivers.addAll(packageManager.queryIntentContentProviders(intent, 0));
    // receivers.addAll(packageManager.queryIntentActivities(intent, 0));
    // receivers.addAll(packageManager.queryIntentServices(intent, 0));
    //
    // for (ResolveInfo resolveInfo : receivers) {
    //     if (resolveInfo.activityInfo == null) {
    //         continue;
    //     }
    //     String packageName = resolveInfo.activityInfo.packageName;
    //     int status = getCarrierPrivilegeStatus(packageManager, packageName);
    //     if (status == TelephonyManager.CARRIER_PRIVILEGE_STATUS_HAS_ACCESS) {
    //         packages.add(packageName);
    //     } else if (status != TelephonyManager.CARRIER_PRIVILEGE_STATUS_NO_ACCESS) {
    //         // Any status apart from HAS_ACCESS and NO_ACCESS is considered an error.
    //         return null;
    //     }
    // }
    //
    // return packages;
    assert(0);
    return NOERROR;
}

ECode UiccCarrierPrivilegeRules::HandleMessage(
    /* [in] */ IMessage* msg)
{
    // ==================before translated======================
    // AsyncResult ar;
    //
    // switch (msg.what) {
    //
    //   case EVENT_OPEN_LOGICAL_CHANNEL_DONE:
    //       Rlog.d(LOGTAG, "EVENT_OPEN_LOGICAL_CHANNEL_DONE");
    //       ar = (AsyncResult) msg.obj;
    //       if (ar.exception == null && ar.result != null) {
    //           int channelId = ((int[]) ar.result)[0];
    //           mUiccCard.iccTransmitApduLogicalChannel(channelId, CLA, COMMAND, P1, P2, P3, DATA,
    //               obtainMessage(EVENT_TRANSMIT_LOGICAL_CHANNEL_DONE, new Integer(channelId)));
    //       } else {
    //           Rlog.e(LOGTAG, "Error opening channel");
    //           updateState(STATE_ERROR);
    //       }
    //       break;
    //
    //   case EVENT_TRANSMIT_LOGICAL_CHANNEL_DONE:
    //       Rlog.d(LOGTAG, "EVENT_TRANSMIT_LOGICAL_CHANNEL_DONE");
    //       ar = (AsyncResult) msg.obj;
    //       if (ar.exception == null && ar.result != null) {
    //           IccIoResult response = (IccIoResult) ar.result;
    //           if (response.payload != null && response.sw1 == 0x90 && response.sw2 == 0x00) {
    //               try {
    //                   mAccessRules = parseRules(IccUtils.bytesToHexString(response.payload));
    //                   updateState(STATE_LOADED);
    //               } catch (IllegalArgumentException ex) {
    //                   Rlog.e(LOGTAG, "Error parsing rules: " + ex);
    //                   updateState(STATE_ERROR);
    //               }
    //            } else {
    //               Rlog.e(LOGTAG, "Invalid response: payload=" + response.payload +
    //                       " sw1=" + response.sw1 + " sw2=" + response.sw2);
    //               updateState(STATE_ERROR);
    //            }
    //       } else {
    //           Rlog.e(LOGTAG, "Error reading value from SIM.");
    //           updateState(STATE_ERROR);
    //       }
    //
    //       int channelId = (Integer) ar.userObj;
    //       mUiccCard.iccCloseLogicalChannel(channelId, obtainMessage(
    //               EVENT_CLOSE_LOGICAL_CHANNEL_DONE));
    //       break;
    //
    //   case EVENT_CLOSE_LOGICAL_CHANNEL_DONE:
    //       Rlog.d(LOGTAG, "EVENT_CLOSE_LOGICAL_CHANNEL_DONE");
    //       break;
    //
    //   default:
    //       Rlog.e(LOGTAG, "Unknown event " + msg.what);
    // }
    assert(0);
    return NOERROR;
}

AutoPtr<IList/*< AutoPtr<AccessRule> >*/> UiccCarrierPrivilegeRules::ParseRules(
    /* [in] */ const String& rules)
{
    // ==================before translated======================
    // rules = rules.toUpperCase(Locale.US);
    // Rlog.d(LOGTAG, "Got rules: " + rules);
    //
    // TLV allRefArDo = new TLV(TAG_ALL_REF_AR_DO); //FF40
    // allRefArDo.parse(rules, true);
    //
    // String arDos = allRefArDo.value;
    // List<AccessRule> accessRules = new ArrayList<AccessRule>();
    // while (!arDos.isEmpty()) {
    //     TLV refArDo = new TLV(TAG_REF_AR_DO); //E2
    //     arDos = refArDo.parse(arDos, false);
    //     AccessRule accessRule = parseRefArdo(refArDo.value);
    //     if (accessRule != null) {
    //         accessRules.add(accessRule);
    //     } else {
    //       Rlog.e(LOGTAG, "Skip unrecognized rule." + refArDo.value);
    //     }
    // }
    // return accessRules;
    assert(0);
    AutoPtr<IList/*< AutoPtr<AccessRule>>*/> empty;
    return empty;
}

AutoPtr<UiccCarrierPrivilegeRules::AccessRule> UiccCarrierPrivilegeRules::ParseRefArdo(
    /* [in] */ const String& rule)
{
    // ==================before translated======================
    // Rlog.d(LOGTAG, "Got rule: " + rule);
    //
    // String certificateHash = null;
    // String packageName = null;
    // String tmp = null;
    // long accessType = 0;
    //
    // while (!rule.isEmpty()) {
    //     if (rule.startsWith(TAG_REF_DO)) {
    //         TLV refDo = new TLV(TAG_REF_DO); //E1
    //         rule = refDo.parse(rule, false);
    //
    //         // Skip unrelated rules.
    //         if (!refDo.value.startsWith(TAG_DEVICE_APP_ID_REF_DO)) {
    //             return null;
    //         }
    //
    //         TLV deviceDo = new TLV(TAG_DEVICE_APP_ID_REF_DO); //C1
    //         tmp = deviceDo.parse(refDo.value, false);
    //         certificateHash = deviceDo.value;
    //
    //         if (!tmp.isEmpty()) {
    //           if (!tmp.startsWith(TAG_PKG_REF_DO)) {
    //               return null;
    //           }
    //           TLV pkgDo = new TLV(TAG_PKG_REF_DO); //CA
    //           pkgDo.parse(tmp, true);
    //           packageName = new String(IccUtils.hexStringToBytes(pkgDo.value));
    //         } else {
    //           packageName = null;
    //         }
    //     } else if (rule.startsWith(TAG_AR_DO)) {
    //         TLV arDo = new TLV(TAG_AR_DO); //E3
    //         rule = arDo.parse(rule, false);
    //
    //         // Skip unrelated rules.
    //         if (!arDo.value.startsWith(TAG_PERM_AR_DO)) {
    //             return null;
    //         }
    //
    //         TLV permDo = new TLV(TAG_PERM_AR_DO); //DB
    //         permDo.parse(arDo.value, true);
    //         Rlog.e(LOGTAG, permDo.value);
    //     } else  {
    //         // Spec requires it must be either TAG_REF_DO or TAG_AR_DO.
    //         throw new RuntimeException("Invalid Rule type");
    //     }
    // }
    //
    // Rlog.e(LOGTAG, "Adding: " + certificateHash + " : " + packageName + " : " + accessType);
    //
    // AccessRule accessRule = new AccessRule(IccUtils.hexStringToBytes(certificateHash),
    //     packageName, accessType);
    // Rlog.e(LOGTAG, "Parsed rule: " + accessRule);
    // return accessRule;
    assert(0);
    AutoPtr<AccessRule> empty;
    return empty;
}

AutoPtr<ArrayOf<Byte> > UiccCarrierPrivilegeRules::GetCertHash(
    /* [in] */ ISignature* signature)
{
    // ==================before translated======================
    // // TODO: Is the following sufficient.
    // try {
    //     CertificateFactory certFactory = CertificateFactory.getInstance("X.509");
    //     X509Certificate cert = (X509Certificate) certFactory.generateCertificate(
    //             new ByteArrayInputStream(signature.toByteArray()));
    //
    //     MessageDigest md = MessageDigest.getInstance("SHA");
    //     return md.digest(cert.getEncoded());
    // } catch (CertificateException ex) {
    //     Rlog.e(LOGTAG, "CertificateException: " + ex);
    // } catch (NoSuchAlgorithmException ex) {
    //     Rlog.e(LOGTAG, "NoSuchAlgorithmException: " + ex);
    // }
    //
    // Rlog.e(LOGTAG, "Cannot compute cert hash");
    // return null;
    assert(0);
    AutoPtr<ArrayOf<Byte> > empty;
    return empty;
}

void UiccCarrierPrivilegeRules::UpdateState(
    /* [in] */ Int32 newState)
{
    // ==================before translated======================
    // mState.set(newState);
    // if (mLoadedCallback != null) {
    //     mLoadedCallback.sendToTarget();
    // }
    assert(0);
}

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
