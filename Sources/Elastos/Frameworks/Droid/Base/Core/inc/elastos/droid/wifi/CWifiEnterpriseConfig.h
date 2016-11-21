
#ifndef __ELASTOS_DROID_NET_WIFI_CWIFIENTERPRISECONFIG_H__
#define __ELASTOS_DROID_NET_WIFI_CWIFIENTERPRISECONFIG_H__

#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.CoreLibrary.Security.h>
#include "_Elastos_Droid_Wifi_CWifiEnterpriseConfig.h"
#include <elastos/core/Object.h>

using Elastos::Security::Cert::IX509Certificate;
using Elastos::Security::IPrivateKey;
using Elastos::Utility::IHashMap;
using Elastos::Utility::ISet;

namespace Elastos {
namespace Droid {
namespace Wifi {

CarClass(CWifiEnterpriseConfig)
    , public Object
    , public IParcelable
    , public IWifiEnterpriseConfig
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CWifiEnterpriseConfig();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IWifiEnterpriseConfig* source);

    /** Internal use only
     * @hide
     */
    CARAPI GetFields(
        /* [out] */ IHashMap** result);

    /**
     * Set the EAP authentication method.
     * @param  eapMethod is one {@link Eap#PEAP}, {@link Eap#TLS}, {@link Eap#TTLS} or
     *                   {@link Eap#PWD}
     * @throws IllegalArgumentException on an invalid eap method
     */
    CARAPI SetEapMethod(
        /* [in] */ Int32 eapMethod);

    /**
     * Get the eap method.
     * @return eap method configured
     */
    CARAPI GetEapMethod(
        /* [out] */ Int32* result);

    /**
     * Set Phase 2 authentication method. Sets the inner authentication method to be used in
     * phase 2 after setting up a secure channel
     * @param phase2Method is the inner authentication method and can be one of {@link Phase2#NONE},
     *                     {@link Phase2#PAP}, {@link Phase2#MSCHAP}, {@link Phase2#MSCHAPV2},
     *                     {@link Phase2#GTC}
     * @throws IllegalArgumentException on an invalid phase2 method
     *
     */
    CARAPI SetPhase2Method(
        /* [in] */ Int32 phase2Method);

    /**
     * Get the phase 2 authentication method.
     * @return a phase 2 method defined at {@link Phase2}
     * */
    CARAPI GetPhase2Method(
        /* [out] */ Int32* result);

    /**
     * Set the identity
     * @param identity
     */
    CARAPI SetIdentity(
        /* [in] */ const String& identity);

    /**
     * Get the identity
     * @return the identity
     */
    CARAPI GetIdentity(
        /* [out] */ String* result);

    /**
     * Set anonymous identity. This is used as the unencrypted identity with
     * certain EAP types
     * @param anonymousIdentity the anonymous identity
     */
    CARAPI SetAnonymousIdentity(
        /* [in] */ const String& anonymousIdentity);

    /** Get the anonymous identity
     * @return anonymous identity
     */
    CARAPI GetAnonymousIdentity(
        /* [out] */ String* result);

    /**
     * Set the password.
     * @param password the password
     */
    CARAPI SetPassword(
        /* [in] */ const String& password);

    /**
     * Get the password.
     *
     * Returns locally set password value. For networks fetched from
     * framework, returns "*".
     */
    CARAPI GetPassword(
        /* [out] */ String* result);

    /**
     * Set CA certificate alias.
     *
     * <p> See the {@link android.security.KeyChain} for details on installing or choosing
     * a certificate
     * </p>
     * @param alias identifies the certificate
     * @hide
     */
    CARAPI SetCaCertificateAlias(
        /* [in] */ const String& alias);

    /**
     * Get CA certificate alias
     * @return alias to the CA certificate
     * @hide
     */
    CARAPI GetCaCertificateAlias(
        /* [out] */ String* result);

    /**
     * Specify a X.509 certificate that identifies the server.
     *
     * <p>A default name is automatically assigned to the certificate and used
     * with this configuration. The framework takes care of installing the
     * certificate when the config is saved and removing the certificate when
     * the config is removed.
     *
     * @param cert X.509 CA certificate
     * @throws IllegalArgumentException if not a CA certificate
     */
    CARAPI SetCaCertificate(
        /* [in] */ IX509Certificate* cert);

    /**
     * Get CA certificate
     * @return X.509 CA certificate
     */
    CARAPI GetCaCertificate(
        /* [out] */ IX509Certificate** result);

    /**
     * @hide
     */
    CARAPI ResetCaCertificate();

    /** Set Client certificate alias.
     *
     * <p> See the {@link android.security.KeyChain} for details on installing or choosing
     * a certificate
     * </p>
     * @param alias identifies the certificate
     * @hide
     */
    CARAPI SetClientCertificateAlias(
        /* [in] */ const String& alias);

    /**
     * Get client certificate alias
     * @return alias to the client certificate
     * @hide
     */
    CARAPI GetClientCertificateAlias(
        /* [out] */ String* result);

    /**
     * Specify a private key and client certificate for client authorization.
     *
     * <p>A default name is automatically assigned to the key entry and used
     * with this configuration.  The framework takes care of installing the
     * key entry when the config is saved and removing the key entry when
     * the config is removed.
     * @param privateKey
     * @param clientCertificate
     * @throws IllegalArgumentException for an invalid key or certificate.
     */
    CARAPI SetClientKeyEntry(
        /* [in] */ IPrivateKey* privateKey,
        /* [in] */ IX509Certificate* clientCertificate);

    /**
     * Get client certificate
     *
     * @return X.509 client certificate
     */
    CARAPI GetClientCertificate(
        /* [out] */ IX509Certificate** result);

    /**
     * @hide
     */
    CARAPI ResetClientKeyEntry();

    /**
     * @hide
     */
    CARAPI GetClientPrivateKey(
        /* [out] */ IPrivateKey** result);

    /**
     * Set subject match. This is the substring to be matched against the subject of the
     * authentication server certificate.
     * @param subjectMatch substring to be matched
     */
    CARAPI SetSubjectMatch(
        /* [in] */ const String& subjectMatch);

    /**
     * Get subject match
     * @return the subject match string
     */
    CARAPI GetSubjectMatch(
        /* [out] */ String* result);

    /** Returns the field value for the key.
     * @param key into the hash
     * @param prefix is the prefix that the value may have
     * @return value
     * @hide
     */
    CARAPI GetFieldValue(
        /* [in] */ const String& key,
        /* [in] */ const String& prefix,
        /* [out] */ String* result);

    /** Set a value with an optional prefix at key
     * @param key into the hash
     * @param value to be set
     * @param prefix an optional value to be prefixed to actual value
     * @hide
     */
    CARAPI SetFieldValue(
        /* [in] */ const String& key,
        /* [in] */ const String& value,
        /* [in] */ const String& prefix);

    /** Set a value with an optional prefix at key
     * @param key into the hash
     * @param value to be set
     * @param prefix an optional value to be prefixed to actual value
     * @hide
     */
    CARAPI SetFieldValue(
        /* [in] */ const String& key,
        /* [in] */ const String& value);

    //@Override
    CARAPI DescribeContents(
        /* [out] */ Int32* contents);

    //@Override
    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest,
        /* [in] */ Int32 flags);

    /** See {@link WifiConfiguration#getKeyIdForCredentials} @hide */
    CARAPI GetKeyId(
        /* [in] */ IWifiEnterpriseConfig* current,
        /* [out] */ String* keyId);

    //@Override
    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

private:
    CARAPI_(void) WriteCertificate(
        /* [in] */ IParcel* dest,
        /* [in] */ IX509Certificate* cert);

    CARAPI ReadCertificate(
        /* [in] */ IParcel* in,
        /* [out] */ IX509Certificate** cert);


    CARAPI_(String) RemoveDoubleQuotes(
        /* [in] */ const String& string);

    CARAPI_(String) ConvertToQuotedString(
        /* [in] */ const String& string);

    /** Returns the index at which the toBeFound string is found in the array.
     * @param arr array of strings
     * @param toBeFound string to be found
     * @param defaultIndex default index to be returned when string is not found
     * @return the index into array
     */
    CARAPI_(Int32) GetStringIndex(
        /* [in] */ ArrayOf<String>* arr,
        /* [in] */ const String& toBeFound,
        /* [in] */ Int32 defaultIndex);

private:
    AutoPtr<IHashMap> mFields;
    AutoPtr<IX509Certificate> mCaCert;
    AutoPtr<IPrivateKey> mClientPrivateKey;
    AutoPtr<IX509Certificate> mClientCertificate;
};

} // namespace Wifi
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_NET_WIFI_CWIFIENTERPRISECONFIG_H__
