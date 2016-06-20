#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_UICCCARRIERPRIVILEGERULES_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_UICCCARRIERPRIVILEGERULES_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/Handler.h"

// import android.content.Intent;
// import android.content.pm.PackageInfo;
// import android.content.pm.PackageManager;
// import android.content.pm.ResolveInfo;
// import android.content.pm.Signature;
// import android.os.AsyncResult;
// import android.os.Binder;
// import android.telephony.Rlog;
// import android.telephony.TelephonyManager;
// import com.android.internal.telephony.CommandsInterface;
// import com.android.internal.telephony.uicc.IccUtils;
// import java.io.ByteArrayInputStream;
// import java.lang.IllegalArgumentException;
// import java.security.MessageDigest;
// import java.security.NoSuchAlgorithmException;
// import java.security.cert.Certificate;
// import java.security.cert.CertificateException;
// import java.security.cert.CertificateFactory;
// import java.security.cert.X509Certificate;
// import java.util.ArrayList;
// import java.util.Arrays;
// import java.util.List;
// import java.util.Locale;
// import java.util.concurrent.atomic.AtomicInteger;

using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Content::Pm::ISignature;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::IIntent;
using Elastos::Utility::Concurrent::Atomic::IAtomicInteger32;
using Elastos::Core::IInteger32;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

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
class UiccCarrierPrivilegeRules
    : public Handler
    , public IUiccCarrierPrivilegeRules
{
private:
    // Describes a single rule.
    class AccessRule
        : public Object
    {
    public:
        // This bit is not currently used, but reserved for future use.
        AccessRule(
            /* [in] */ ArrayOf<Byte>* certificateHash,
            /* [in] */ const String& packageName,
            /* [in] */ Int64 accessType);

        virtual CARAPI Matches(
            /* [in] */ ArrayOf<Byte>* certHash,
            /* [in] */ const String& packageName,
            /* [out] */ Boolean* result);

        // @Override
        CARAPI ToString(
            /* [out] */ String* result);

    public:
        AutoPtr<ArrayOf<Byte> > certificateHash;
        String packageName;
        Int64 accessType;
    };

    // Used for parsing the data from the UICC.
    class TLV
        : public Object
    {
    public:
        TLV(
            /* [in] */ const String& tag);

        virtual CARAPI Parse(
            /* [in] */ const String& data,
            /* [in] */ Boolean shouldConsumeAll,
            /* [out] */ String* result);

    private:
        String tag;
        AutoPtr<IInteger32> length;
        String value;
    };

public:
    CAR_INTERFACE_DECL();

    UiccCarrierPrivilegeRules();

    CARAPI constructor(
        /* [in] */ IUiccCard* uiccCard,
        /* [in] */ IMessage* loadedCallback);

    /**
      * Returns true if the carrier privilege rules have finished loading.
      */
    virtual CARAPI AreCarrierPriviligeRulesLoaded(
        /* [out] */ Boolean* result);

    /**
      * Returns the status of the carrier privileges for the input certificate and package name.
      *
      * @param signature The signature of the certificate.
      * @param packageName name of the package.
      * @return Access status.
      */
    virtual CARAPI GetCarrierPrivilegeStatus(
        /* [in] */ ISignature* signature,
        /* [in] */ const String& packageName,
        /* [out] */ Int32* result);

    /**
      * Returns the status of the carrier privileges for the input package name.
      *
      * @param packageManager PackageManager for getting signatures.
      * @param packageName name of the package.
      * @return Access status.
      */
    virtual CARAPI GetCarrierPrivilegeStatus(
        /* [in] */ IPackageManager* packageManager,
        /* [in] */ const String& packageName,
        /* [out] */ Int32* result);

    /**
      * Returns the status of the carrier privileges for the caller of the current transaction.
      *
      * @param packageManager PackageManager for getting signatures and package names.
      * @return Access status.
      */
    virtual CARAPI GetCarrierPrivilegeStatusForCurrentTransaction(
        /* [in] */ IPackageManager* packageManager,
        /* [out] */ Int32* result);

    /**
      * Returns the package name of the carrier app that should handle the input intent.
      *
      * @param packageManager PackageManager for getting receivers.
      * @param intent Intent that will be sent.
      * @return list of carrier app package names that can handle the intent.
      *         Returns null if there is an error and an empty list if there
      *         are no matching packages.
      */
    virtual CARAPI GetCarrierPackageNamesForIntent(
        /* [in] */ IPackageManager* packageManager,
        /* [in] */ IIntent* intent,
        /* [out] */ IList/*<String>*/** result);

    // @Override
    CARAPI HandleMessage(
        /* [in] */ IMessage* msg);

private:
    /*
      * Parses the rules from the input string.
      */
    static CARAPI_(AutoPtr<IList/*< AutoPtr<AccessRule> >*/>) ParseRules(
        /* [in] */ const String& rules);

    /*
      * Parses a single rule.
      */
    static CARAPI_(AutoPtr<AccessRule>) ParseRefArdo(
        /* [in] */ const String& rule);

    /*
      * Converts a Signature into a Certificate hash usable for comparison.
      */
    static CARAPI_(AutoPtr<ArrayOf<Byte> >) GetCertHash(
        /* [in] */ ISignature* signature);

    /*
      * Updates the state and notifies the UiccCard that the rules have finished loading.
      */
    CARAPI_(void) UpdateState(
        /* [in] */ Int32 newState);

private:
    static const String LOGTAG;
    static const String AID;
    static const Int32 CLA = 0x80;
    static const Int32 COMMAND = 0xCA;
    static const Int32 P1 = 0xFF;
    static const Int32 P2 = 0x40;
    static const Int32 P3 = 0x00;
    static const String DATA;
    /*
      * Rules format:
      *   ALL_REF_AR_DO = TAG_ALL_REF_AR_DO + len + [REF_AR_DO]*n
      *   REF_AR_DO = TAG_REF_AR_DO + len + REF-DO + AR-DO
      *
      *   REF_DO = TAG_REF_DO + len + DEVICE_APP_ID_REF_DO + (optional) PKG_REF_DO
      *   AR_DO = TAG_AR_DO + len + PERM_AR_DO
      *
      *   DEVICE_APP_ID_REF_DO = TAG_DEVICE_APP_ID_REF_DO + len + sha1 hexstring of cert (20 bytes)
      *   PKG_REF_DO = TAG_PKG_REF_DO + len + package name
      *   PERM_AR_DO = TAG_PERM_AR_DO + len + detailed permission (8 bytes)
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
    static const String TAG_ALL_REF_AR_DO;
    static const String TAG_REF_AR_DO;
    static const String TAG_REF_DO;
    static const String TAG_DEVICE_APP_ID_REF_DO;
    static const String TAG_PKG_REF_DO;
    static const String TAG_AR_DO;
    static const String TAG_PERM_AR_DO;
    static const Int32 EVENT_OPEN_LOGICAL_CHANNEL_DONE = 1;
    static const Int32 EVENT_TRANSMIT_LOGICAL_CHANNEL_DONE = 2;
    static const Int32 EVENT_CLOSE_LOGICAL_CHANNEL_DONE = 3;
    // State of the object.
    static const Int32 STATE_LOADING = 0;
    static const Int32 STATE_LOADED = 1;
    static const Int32 STATE_ERROR = 2;
    AutoPtr<IUiccCard> mUiccCard;
    // Parent
    AutoPtr<IAtomicInteger32> mState;
    AutoPtr<IList/*<AccessRule>*/> mAccessRules;
    AutoPtr<IMessage> mLoadedCallback;
};

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_UICCCARRIERPRIVILEGERULES_H__

