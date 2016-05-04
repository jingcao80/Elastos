/*
 * Copyright (C) 2014 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.android.internal.telephony.uicc;

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Pm::ISignature;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Telephony::IRlog;
using Elastos::Droid::Telephony::ITelephonyManager;

using Elastos::Droid::Internal::Telephony::ICommandsInterface;
using Elastos::Droid::Internal::Telephony::Uicc::IIccUtils;

using Elastos::IO::IByteArrayInputStream;
using Elastos::Security::IMessageDigest;
using Elastos::Security::Cert::ICertificate;
using Elastos::Security::Cert::ICertificateFactory;
using Elastos::Security::Cert::IX509Certificate;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IArrays;
using Elastos::Utility::IList;
using Elastos::Utility::ILocale;
using Elastos::Utility::Concurrent::Atomic::IAtomicInteger;

/**
 * Class that reads and stores the carrier privileged rules from the UICC.
 *
 * The rules are read when the class is created, hence it should only be created
 * after the UICC can be read. And it should be deleted when a UICC is changed.
 *
 * The spec for the rules:
 *     GP Secure Element Access Control:
 *     http://www.globalplatform.org/specifications/review/GPD_SE_Access_Control_v1.0.20.pdf
 *     Extension spec:
 *     https://code.google.com/p/seek-for-android/
 *
 *
 * TODO: Notifications.
 *
 * {@hide}
 */
public class UiccCarrierPrivilegeRules extends Handler {
    private static const String LOG_TAG = "UiccCarrierPrivilegeRules";

    private static const String AID = "A00000015141434C00";
    private static const Int32 CLA = 0x80;
    private static const Int32 COMMAND = 0xCA;
    private static const Int32 P1 = 0xFF;
    private static const Int32 P2 = 0x40;
    private static const Int32 P3 = 0x00;
    private static const String DATA = "";

    /*
     * Rules format:
     *   ALL_REF_AR_DO = TAG_ALL_REF_AR_DO + len + [REF_AR_DO]*n
     *   REF_AR_DO = TAG_REF_AR_DO + len + REF-DO + AR-DO
     *
     *   REF_DO = TAG_REF_DO + len + DEVICE_APP_ID_REF_DO + (optional) PKG_REF_DO
     *   AR_DO = TAG_AR_DO + len + PERM_AR_DO
     *
     *   DEVICE_APP_ID_REF_DO = TAG_DEVICE_APP_ID_REF_DO + len + sha1 hexstring of Cert (20 bytes)
     *   PKG_REF_DO = TAG_PKG_REF_DO + len + package name
     *   PERM_AR_DO = TAG_PERM_AR_DO + len + detailed Permission (8 bytes)
     *
     * Data objects hierarchy by TAG:
     * FF40
     *   E2
     *     E1
     *       C1
     *       CA
     *     E3
     *       DB
     */
    // Values from the data standard.
    private static const String TAG_ALL_REF_AR_DO = "FF40";
    private static const String TAG_REF_AR_DO = "E2";
    private static const String TAG_REF_DO = "E1";
    private static const String TAG_DEVICE_APP_ID_REF_DO = "C1";
    private static const String TAG_PKG_REF_DO = "CA";
    private static const String TAG_AR_DO = "E3";
    private static const String TAG_PERM_AR_DO = "DB";

    private static const Int32 EVENT_OPEN_LOGICAL_CHANNEL_DONE = 1;
    private static const Int32 EVENT_TRANSMIT_LOGICAL_CHANNEL_DONE = 2;
    private static const Int32 EVENT_CLOSE_LOGICAL_CHANNEL_DONE = 3;

    // State of the object.
    private static const Int32 STATE_LOADING  = 0;
    private static const Int32 STATE_LOADED   = 1;
    private static const Int32 STATE_ERROR    = 2;

    // Describes a single rule.
    private static class AccessRule {
        public Byte[] certificateHash;
        public String packageName;
        public Int64 accessType;   // This bit is not currently used, but reserved for future use.

        AccessRule(Byte[] certificateHash, String packageName, Int64 accessType) {
            this.certificateHash = certificateHash;
            this.packageName = packageName;
            this.accessType = accessType;
        }

        Boolean Matches(Byte[] certHash, String packageName) {
          return certHash != NULL && Arrays->Equals(this.certificateHash, certHash) &&
                (this.packageName == NULL || this.packageName->Equals(packageName));
        }

        //@Override
        CARAPI ToString(
        /* [out] */ String* str)
    {
            return "cert: " + certificateHash + " pkg: " + packageName +
                " access: " + accessType;
        }
    }

    // Used for parsing the data from the UICC.
    private static class TLV {
        private String tag;
        private Integer length;
        private String value;

        public TLV(String tag) {
            this.tag = tag;
        }

        public String Parse(String data, Boolean shouldConsumeAll) {
            Rlog->D(LOG_TAG, "Parse TLV: " + tag);
            If (!data->StartsWith(tag)) {
                throw new IllegalArgumentException("Tags don't match.");
            }
            Int32 index = tag->Length();
            If (index + 2 > data->Length()) {
                throw new IllegalArgumentException("No length.");
            }
            length = new Integer(2 * Integer->ParseInt(
                    data->Substring(index, index + 2), 16));
            index += 2;

            Int32 remainingLength = data->Length() - (index + length);
            If (remainingLength < 0) {
                throw new IllegalArgumentException("Not enough data.");
            }
            If (shouldConsumeAll && (remainingLength != 0)) {
                throw new IllegalArgumentException("Did not consume all.");
            }
            value = data->Substring(index, index + length);

            Rlog->D(LOG_TAG, "Got TLV: " + tag + "," + length + "," + value);

            return data->Substring(index + length);
        }
    }

    private UiccCard mUiccCard;  // Parent
    private AtomicInteger mState;
    private List<AccessRule> mAccessRules;
    private Message mLoadedCallback;

    public UiccCarrierPrivilegeRules(UiccCard uiccCard, Message loadedCallback) {
        Rlog->D(LOG_TAG, "Creating UiccCarrierPrivilegeRules");
        mUiccCard = uiccCard;
        mState = new AtomicInteger(STATE_LOADING);
        mLoadedCallback = loadedCallback;

        // Start loading the rules.
        mUiccCard->IccOpenLogicalChannel(AID,
            ObtainMessage(EVENT_OPEN_LOGICAL_CHANNEL_DONE, NULL));
    }

    /**
     * Returns TRUE if the carrier privilege rules have finished loading.
     */
    public Boolean AreCarrierPriviligeRulesLoaded() {
        return mState->Get() != STATE_LOADING;
    }

    /**
     * Returns the status of the carrier privileges for the input certificate and package name.
     *
     * @param signature The signature of the certificate.
     * @param packageName name of the package.
     * @return Access status.
     */
    public Int32 GetCarrierPrivilegeStatus(Signature signature, String packageName) {
        Rlog->D(LOG_TAG, "hasCarrierPrivileges: " + signature + " : " + packageName);
        Int32 state = mState->Get();
        If (state == STATE_LOADING) {
            Rlog->D(LOG_TAG, "Rules not loaded.");
            return TelephonyManager.CARRIER_PRIVILEGE_STATUS_RULES_NOT_LOADED;
        } else If (state == STATE_ERROR) {
            Rlog->D(LOG_TAG, "Error loading rules.");
            return TelephonyManager.CARRIER_PRIVILEGE_STATUS_ERROR_LOADING_RULES;
        }

        Byte[] certHash = GetCertHash(signature);
        If (certHash == NULL) {
          return TelephonyManager.CARRIER_PRIVILEGE_STATUS_NO_ACCESS;
        }
        Rlog->E(LOG_TAG, "Checking: " + IccUtils->BytesToHexString(certHash) + " : " + packageName);

        For (AccessRule ar : mAccessRules) {
            If (ar->Matches(certHash, packageName)) {
                Rlog->D(LOG_TAG, "Match found!");
                return TelephonyManager.CARRIER_PRIVILEGE_STATUS_HAS_ACCESS;
            }
        }

        Rlog->D(LOG_TAG, "No matching rule found. Returning FALSE.");
        return TelephonyManager.CARRIER_PRIVILEGE_STATUS_NO_ACCESS;
    }

    /**
     * Returns the status of the carrier privileges for the input package name.
     *
     * @param packageManager PackageManager for getting signatures.
     * @param packageName name of the package.
     * @return Access status.
     */
    public Int32 GetCarrierPrivilegeStatus(PackageManager packageManager, String packageName) {
        try {
            PackageInfo pInfo = packageManager->GetPackageInfo(packageName,
                PackageManager.GET_SIGNATURES);
            Signature[] signatures = pInfo.signatures;
            For (Signature sig : signatures) {
                Int32 accessStatus = GetCarrierPrivilegeStatus(sig, pInfo.packageName);
                If (accessStatus != TelephonyManager.CARRIER_PRIVILEGE_STATUS_NO_ACCESS) {
                    return accessStatus;
                }
            }
        } Catch (PackageManager.NameNotFoundException ex) {
            Rlog->E(LOG_TAG, "NameNotFoundException", ex);
        }
        return TelephonyManager.CARRIER_PRIVILEGE_STATUS_NO_ACCESS;
    }

    /**
     * Returns the status of the carrier privileges for the caller of the current transaction.
     *
     * @param packageManager PackageManager for getting signatures and package names.
     * @return Access status.
     */
    public Int32 GetCarrierPrivilegeStatusForCurrentTransaction(PackageManager packageManager) {
        String[] packages = packageManager->GetPackagesForUid(Binder->GetCallingUid());

        For (String pkg : packages) {
            Int32 accessStatus = GetCarrierPrivilegeStatus(packageManager, pkg);
            If (accessStatus != TelephonyManager.CARRIER_PRIVILEGE_STATUS_NO_ACCESS) {
                return accessStatus;
            }
        }
        return TelephonyManager.CARRIER_PRIVILEGE_STATUS_NO_ACCESS;
    }

    /**
     * Returns the package name of the carrier app that should handle the input intent.
     *
     * @param packageManager PackageManager for getting receivers.
     * @param intent Intent that will be sent.
     * @return list of carrier app package names that can handle the intent.
     *         Returns NULL if there is an error and an empty list if there
     *         are no matching packages.
     */
    public List<String> GetCarrierPackageNamesForIntent(
            PackageManager packageManager, Intent intent) {
        List<String> packages = new ArrayList<String>();
        List<ResolveInfo> receivers = new ArrayList<ResolveInfo>();
        receivers->AddAll(packageManager->QueryBroadcastReceivers(intent, 0));
        receivers->AddAll(packageManager->QueryIntentContentProviders(intent, 0));
        receivers->AddAll(packageManager->QueryIntentActivities(intent, 0));
        receivers->AddAll(packageManager->QueryIntentServices(intent, 0));

        For (ResolveInfo resolveInfo : receivers) {
            If (resolveInfo.activityInfo == NULL) {
                continue;
            }
            String packageName = resolveInfo.activityInfo.packageName;
            Int32 status = GetCarrierPrivilegeStatus(packageManager, packageName);
            If (status == TelephonyManager.CARRIER_PRIVILEGE_STATUS_HAS_ACCESS) {
                packages->Add(packageName);
            } else If (status != TelephonyManager.CARRIER_PRIVILEGE_STATUS_NO_ACCESS) {
                // Any status apart from HAS_ACCESS and NO_ACCESS is considered an error.
                return NULL;
            }
        }

        return packages;
    }

    //@Override
    CARAPI HandleMessage(Message msg) {
        AsyncResult ar;

        Switch (msg.what) {

          case EVENT_OPEN_LOGICAL_CHANNEL_DONE:
              Rlog->D(LOG_TAG, "EVENT_OPEN_LOGICAL_CHANNEL_DONE");
              ar = (AsyncResult) msg.obj;
              If (ar.exception == NULL && ar.result != NULL) {
                  Int32 channelId = ((Int32[]) ar.result)[0];
                  mUiccCard->IccTransmitApduLogicalChannel(channelId, CLA, COMMAND, P1, P2, P3, DATA,
                      ObtainMessage(EVENT_TRANSMIT_LOGICAL_CHANNEL_DONE, new Integer(channelId)));
              } else {
                  Rlog->E(LOG_TAG, "Error opening channel");
                  UpdateState(STATE_ERROR);
              }
              break;

          case EVENT_TRANSMIT_LOGICAL_CHANNEL_DONE:
              Rlog->D(LOG_TAG, "EVENT_TRANSMIT_LOGICAL_CHANNEL_DONE");
              ar = (AsyncResult) msg.obj;
              If (ar.exception == NULL && ar.result != NULL) {
                  IccIoResult response = (IccIoResult) ar.result;
                  If (response.payload != NULL && response.sw1 == 0x90 && response.sw2 == 0x00) {
                      try {
                          mAccessRules = ParseRules(IccUtils->BytesToHexString(response.payload));
                          UpdateState(STATE_LOADED);
                      } Catch (IllegalArgumentException ex) {
                          Rlog->E(LOG_TAG, "Error parsing rules: " + ex);
                          UpdateState(STATE_ERROR);
                      }
                   } else {
                      Rlog->E(LOG_TAG, "Invalid response: payload=" + response.payload +
                              " sw1=" + response.sw1 + " sw2=" + response.sw2);
                      UpdateState(STATE_ERROR);
                   }
              } else {
                  Rlog->E(LOG_TAG, "Error reading value from SIM.");
                  UpdateState(STATE_ERROR);
              }

              Int32 channelId = (Integer) ar.userObj;
              mUiccCard->IccCloseLogicalChannel(channelId, ObtainMessage(
                      EVENT_CLOSE_LOGICAL_CHANNEL_DONE));
              break;

          case EVENT_CLOSE_LOGICAL_CHANNEL_DONE:
              Rlog->D(LOG_TAG, "EVENT_CLOSE_LOGICAL_CHANNEL_DONE");
              break;

          default:
              Rlog->E(LOG_TAG, "Unknown event " + msg.what);
        }
    }

    /*
     * Parses the rules from the input string.
     */
    private static List<AccessRule> ParseRules(String rules) {
        rules = rules->ToUpperCase(Locale.US);
        Rlog->D(LOG_TAG, "Got rules: " + rules);

        TLV allRefArDo = new TLV(TAG_ALL_REF_AR_DO); //FF40
        allRefArDo->Parse(rules, TRUE);

        String arDos = allRefArDo.value;
        List<AccessRule> accessRules = new ArrayList<AccessRule>();
        While (!arDos->IsEmpty()) {
            TLV refArDo = new TLV(TAG_REF_AR_DO); //E2
            arDos = refArDo->Parse(arDos, FALSE);
            AccessRule accessRule = ParseRefArdo(refArDo.value);
            If (accessRule != NULL) {
                accessRules->Add(accessRule);
            } else {
              Rlog->E(LOG_TAG, "Skip unrecognized rule." + refArDo.value);
            }
        }
        return accessRules;
    }

    /*
     * Parses a single rule.
     */
    private static AccessRule ParseRefArdo(String rule) {
        Rlog->D(LOG_TAG, "Got rule: " + rule);

        String certificateHash = NULL;
        String packageName = NULL;
        String tmp = NULL;
        Int64 accessType = 0;

        While (!rule->IsEmpty()) {
            If (rule->StartsWith(TAG_REF_DO)) {
                TLV refDo = new TLV(TAG_REF_DO); //E1
                rule = refDo->Parse(rule, FALSE);

                // Skip unrelated rules.
                If (!refDo.value->StartsWith(TAG_DEVICE_APP_ID_REF_DO)) {
                    return NULL;
                }

                TLV deviceDo = new TLV(TAG_DEVICE_APP_ID_REF_DO); //C1
                tmp = deviceDo->Parse(refDo.value, FALSE);
                certificateHash = deviceDo.value;

                If (!tmp->IsEmpty()) {
                  If (!tmp->StartsWith(TAG_PKG_REF_DO)) {
                      return NULL;
                  }
                  TLV pkgDo = new TLV(TAG_PKG_REF_DO); //CA
                  pkgDo->Parse(tmp, TRUE);
                  packageName = new String(IccUtils->HexStringToBytes(pkgDo.value));
                } else {
                  packageName = NULL;
                }
            } else If (rule->StartsWith(TAG_AR_DO)) {
                TLV arDo = new TLV(TAG_AR_DO); //E3
                rule = arDo->Parse(rule, FALSE);

                // Skip unrelated rules.
                If (!arDo.value->StartsWith(TAG_PERM_AR_DO)) {
                    return NULL;
                }

                TLV permDo = new TLV(TAG_PERM_AR_DO); //DB
                permDo->Parse(arDo.value, TRUE);
                Rlog->E(LOG_TAG, permDo.value);
            } else  {
                // Spec requires it must be either TAG_REF_DO or TAG_AR_DO.
                throw new RuntimeException("Invalid Rule type");
            }
        }

        Rlog->E(LOG_TAG, "Adding: " + certificateHash + " : " + packageName + " : " + accessType);

        AccessRule accessRule = new AccessRule(IccUtils->HexStringToBytes(certificateHash),
            packageName, accessType);
        Rlog->E(LOG_TAG, "Parsed rule: " + accessRule);
        return accessRule;
    }

    /*
     * Converts a Signature into a Certificate hash usable for comparison.
     */
    private static Byte[] GetCertHash(Signature signature) {
        // TODO: Is the following sufficient.
        try {
            CertificateFactory certFactory = CertificateFactory->GetInstance("X.509");
            X509Certificate cert = (X509Certificate) certFactory->GenerateCertificate(
                    new ByteArrayInputStream(signature->ToByteArray()));

            MessageDigest md = MessageDigest->GetInstance("SHA");
            return md->Digest(cert->GetEncoded());
        } Catch (CertificateException ex) {
            Rlog->E(LOG_TAG, "CertificateException: " + ex);
        } Catch (NoSuchAlgorithmException ex) {
            Rlog->E(LOG_TAG, "NoSuchAlgorithmException: " + ex);
        }

        Rlog->E(LOG_TAG, "Cannot compute cert hash");
        return NULL;
    }

    /*
     * Updates the state and notifies the UiccCard that the rules have finished loading.
     */
    private void UpdateState(Int32 newState) {
        mState->Set(newState);
        If (mLoadedCallback != NULL) {
            mLoadedCallback->SendToTarget();
        }
    }
}
