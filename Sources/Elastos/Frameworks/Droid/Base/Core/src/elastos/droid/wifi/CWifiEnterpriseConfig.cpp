
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/wifi/CWifiEnterpriseConfig.h"
#include "elastos/droid/wifi/CWifiEnterpriseConfigEap.h"
#include "elastos/droid/wifi/CWifiEnterpriseConfigPhase2.h"
#include <elastos/core/StringBuffer.h>

using Elastos::Droid::Text::TextUtils;
using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringBuffer;
using Elastos::Security::IKey;
using Elastos::Utility::CHashMap;
using Elastos::Utility::IMap;
using Elastos::Utility::IIterator;
using Elastos::Utility::IMapEntry;

namespace Elastos {
namespace Droid {
namespace Wifi {

CAR_INTERFACE_IMPL_2(CWifiEnterpriseConfig, Object, IParcelable, IWifiEnterpriseConfig)

CAR_OBJECT_IMPL(CWifiEnterpriseConfig)

CWifiEnterpriseConfig::CWifiEnterpriseConfig()
{
    CHashMap::New((IHashMap**)&mFields);
}

ECode CWifiEnterpriseConfig::constructor()
{
    // Do not set defaults so that the enterprise fields that are not changed
    // by API are not changed underneath
    // This is essential because an app may not have all fields like password
    // available. It allows modification of subset of fields.
    return NOERROR;
}

ECode CWifiEnterpriseConfig::constructor(
    /* [in] */ IWifiEnterpriseConfig* source)
{
    AutoPtr<IHashMap> fields;
    source->GetFields((IHashMap**)&fields);
    AutoPtr<ISet> keySet;
    fields->GetKeySet((ISet**)&keySet);
    AutoPtr<IIterator> iter;
    keySet->GetIterator((IIterator**)&iter);
    Boolean bNext;
    iter->HasNext(&bNext);
    for (; bNext; iter->HasNext(&bNext)) {
        AutoPtr<IInterface> iKey;
        iter->GetNext((IInterface**)&iKey);
        AutoPtr<IInterface> value;
        fields->Get(iKey, (IInterface**)&value);
        mFields->Put(iKey, value);
    }

    return NOERROR;
}

/** Internal use only
 * @hide
 */
ECode CWifiEnterpriseConfig::GetFields(
    /* [out] */ IHashMap** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mFields;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

/**
 * Set the EAP authentication method.
 * @param  eapMethod is one {@link Eap#PEAP}, {@link Eap#TLS}, {@link Eap#TTLS} or
 *                   {@link Eap#PWD}
 * @throws IllegalArgumentException on an invalid eap method
 */
ECode CWifiEnterpriseConfig::SetEapMethod(
    /* [in] */ Int32 eapMethod)
{
    switch (eapMethod) {
        /** Valid methods */
        case IWifiEnterpriseConfigEap::TLS:
            SetPhase2Method(IWifiEnterpriseConfigPhase2::NONE);
            /* fall through */
        case IWifiEnterpriseConfigEap::PEAP:
        case IWifiEnterpriseConfigEap::PWD:
        case IWifiEnterpriseConfigEap::TTLS:
        case IWifiEnterpriseConfigEap::SIM:
        case IWifiEnterpriseConfigEap::AKA: {
                AutoPtr< ArrayOf<String> > arrays;
                AutoPtr<IWifiEnterpriseConfigEap> iEap;
                CWifiEnterpriseConfigEap::AcquireSingleton((IWifiEnterpriseConfigEap**)&iEap);
                iEap->GetStrings((ArrayOf<String>**)&arrays);
                AutoPtr<ICharSequence> iEAP_KEY;
                CString::New(EAP_KEY, (ICharSequence**)&iEAP_KEY);
                AutoPtr<ICharSequence> im;
                CString::New((*arrays)[eapMethod], (ICharSequence**)&im);
                mFields->Put(iEAP_KEY, im);
                AutoPtr<ICharSequence> iValue;
                CString::New(String("1"), (ICharSequence**)&iValue);
                AutoPtr<ICharSequence> iOPP_KEY_CACHING;
                CString::New(OPP_KEY_CACHING, (ICharSequence**)&iOPP_KEY_CACHING);
                mFields->Put(iOPP_KEY_CACHING, iValue);
            }
            break;
        default: {
                //throw new IllegalArgumentException("Unknown EAP method");
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
    }

    return NOERROR;
}

/**
 * Get the eap method.
 * @return eap method configured
 */
ECode CWifiEnterpriseConfig::GetEapMethod(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ICharSequence> iKey;
    CString::New(EAP_KEY, (ICharSequence**)&iKey);
    AutoPtr<IInterface> iEapMethod;
    mFields->Get(iKey, (IInterface**)&iEapMethod);
    String eapMethod(NULL);
    ICharSequence* iEapMethodcs = ICharSequence::Probe(iEapMethod);
    if (iEapMethodcs != NULL) {
        iEapMethodcs->ToString(&eapMethod);
    }
    AutoPtr< ArrayOf<String> > strings;
    AutoPtr<IWifiEnterpriseConfigEap> iEap;
    CWifiEnterpriseConfigEap::AcquireSingleton((IWifiEnterpriseConfigEap**)&iEap);
    iEap->GetStrings((ArrayOf<String>**)&strings);
    *result = GetStringIndex(strings, eapMethod, IWifiEnterpriseConfigEap::NONE);
    return NOERROR;
}

/**
 * Set Phase 2 authentication method. Sets the inner authentication method to be used in
 * phase 2 after setting up a secure channel
 * @param phase2Method is the inner authentication method and can be one of {@link Phase2#NONE},
 *                     {@link Phase2#PAP}, {@link Phase2#MSCHAP}, {@link Phase2#MSCHAPV2},
 *                     {@link Phase2#GTC}
 * @throws IllegalArgumentException on an invalid phase2 method
 *
 */
ECode CWifiEnterpriseConfig::SetPhase2Method(
    /* [in] */ Int32 phase2Method)
{
    switch (phase2Method) {
        case IWifiEnterpriseConfigPhase2::NONE: {
                AutoPtr<ICharSequence> key, value;
                CString::New(PHASE2_KEY, (ICharSequence**)&key);
                CString::New(EMPTY_VALUE, (ICharSequence**)&value);
                mFields->Put(key, value);
            }
            break;
        /** Valid methods */
        case IWifiEnterpriseConfigPhase2::PAP:
        case IWifiEnterpriseConfigPhase2::MSCHAP:
        case IWifiEnterpriseConfigPhase2::MSCHAPV2:
        case IWifiEnterpriseConfigPhase2::GTC: {
                AutoPtr<ICharSequence> key;
                CString::New(PHASE2_KEY, (ICharSequence**)&key);
                String str(CWifiEnterpriseConfigPhase2::PREFIX);
                AutoPtr<IWifiEnterpriseConfigPhase2> phase2;
                CWifiEnterpriseConfigPhase2::AcquireSingleton((IWifiEnterpriseConfigPhase2**)&phase2);
                AutoPtr< ArrayOf<String> > strings;
                phase2->GetStrings((ArrayOf<String>**)&strings);
                str += (*strings)[phase2Method];
                String value  = ConvertToQuotedString(str);
                AutoPtr<ICharSequence> iValue;
                CString::New(value, (ICharSequence**)&iValue);
                mFields->Put(key, iValue);
            }
            break;
        default: {
            //throw new IllegalArgumentException("Unknown Phase 2 method");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }

    return NOERROR;
}

/**
 * Get the phase 2 authentication method.
 * @return a phase 2 method defined at {@link Phase2}
 * */
ECode CWifiEnterpriseConfig::GetPhase2Method(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<ICharSequence> iKey;
    CString::New(PHASE2_KEY, (ICharSequence**)&iKey);
    AutoPtr<IInterface> iValue;
    mFields->Get(iKey, (IInterface**)&iValue);
    String value;
    ICharSequence::Probe(iValue)->ToString(&value);
    String phase2Method = RemoveDoubleQuotes(value);
    // Remove auth= prefix
    if (phase2Method.StartWith(CWifiEnterpriseConfigPhase2::PREFIX)) {
        phase2Method = phase2Method.Substring(CWifiEnterpriseConfigPhase2::PREFIX.GetLength());
    }

    AutoPtr<IWifiEnterpriseConfigPhase2> phase2;
    CWifiEnterpriseConfigPhase2::AcquireSingleton((IWifiEnterpriseConfigPhase2**)&phase2);
    AutoPtr< ArrayOf<String> > strings;
    phase2->GetStrings((ArrayOf<String>**)&strings);

    *result = GetStringIndex(strings, phase2Method, IWifiEnterpriseConfigPhase2::NONE);

    return NOERROR;
}

/**
 * Set the identity
 * @param identity
 */
ECode CWifiEnterpriseConfig::SetIdentity(
    /* [in] */ const String& identity)
{
    return SetFieldValue(IDENTITY_KEY, identity, String(""));
}

/**
 * Get the identity
 * @return the identity
 */
ECode CWifiEnterpriseConfig::GetIdentity(
    /* [out] */ String* result)
{
    return GetFieldValue(IDENTITY_KEY, String(""), result);
}

/**
 * Set anonymous identity. This is used as the unencrypted identity with
 * certain EAP types
 * @param anonymousIdentity the anonymous identity
 */
ECode CWifiEnterpriseConfig::SetAnonymousIdentity(
    /* [in] */ const String& anonymousIdentity)
{
    return SetFieldValue(ANON_IDENTITY_KEY, anonymousIdentity, String(""));
}

/** Get the anonymous identity
 * @return anonymous identity
 */
ECode CWifiEnterpriseConfig::GetAnonymousIdentity(
    /* [out] */ String* result)
{
    return GetFieldValue(ANON_IDENTITY_KEY, String(""), result);
}

/**
 * Set the password.
 * @param password the password
 */
ECode CWifiEnterpriseConfig::SetPassword(
    /* [in] */ const String& password)
{
    return SetFieldValue(PASSWORD_KEY, password, String(""));
}

/**
 * Get the password.
 *
 * Returns locally set password value. For networks fetched from
 * framework, returns "*".
 */
ECode CWifiEnterpriseConfig::GetPassword(
    /* [out] */ String* result)
{
    return GetFieldValue(PASSWORD_KEY, String(""), result);
}

/**
 * Set CA certificate alias.
 *
 * <p> See the {@link android.security.KeyChain} for details on installing or choosing
 * a certificate
 * </p>
 * @param alias identifies the certificate
 * @hide
 */
ECode CWifiEnterpriseConfig::SetCaCertificateAlias(
    /* [in] */ const String& alias)
{
    return SetFieldValue(CA_CERT_KEY, alias, CA_CERT_PREFIX);
}

/**
 * Get CA certificate alias
 * @return alias to the CA certificate
 * @hide
 */
ECode CWifiEnterpriseConfig::GetCaCertificateAlias(
    /* [out] */ String* result)
{
    return GetFieldValue(CA_CERT_KEY, CA_CERT_PREFIX, result);
}

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
ECode CWifiEnterpriseConfig::SetCaCertificate(
    /* [in] */ IX509Certificate* cert)
{
    if (cert != NULL) {
        Int32 constraints;
        cert->GetBasicConstraints(&constraints);
        if (constraints >= 0) {
            mCaCert = cert;
        }
        else {
            //throw new IllegalArgumentException("Not a CA certificate");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }
    else {
        mCaCert = NULL;
    }

    return NOERROR;
}

/**
 * Get CA certificate
 * @return X.509 CA certificate
 */
ECode CWifiEnterpriseConfig::GetCaCertificate(
    /* [out] */ IX509Certificate** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mCaCert;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

/**
 * @hide
 */
ECode CWifiEnterpriseConfig::ResetCaCertificate()
{
    mCaCert = NULL;
    return NOERROR;
}

/** Set Client certificate alias.
 *
 * <p> See the {@link android.security.KeyChain} for details on installing or choosing
 * a certificate
 * </p>
 * @param alias identifies the certificate
 * @hide
 */
ECode CWifiEnterpriseConfig::SetClientCertificateAlias(
    /* [in] */ const String& alias)
{
    SetFieldValue(CLIENT_CERT_KEY, alias, CLIENT_CERT_PREFIX);
    assert(0);
    // TODO
    // SetFieldValue(PRIVATE_KEY_ID_KEY, alias, ICredentials::USER_PRIVATE_KEY);

    // Also, set engine parameters
    if (TextUtils::IsEmpty(alias)) {
        AutoPtr<ICharSequence> key1, value1;
        CString::New(ENGINE_KEY, (ICharSequence**)&key1);
        CString::New(ENGINE_DISABLE, (ICharSequence**)&value1);
        mFields->Put(key1, value1);
        AutoPtr<ICharSequence> key2, value2;
        CString::New(ENGINE_ID_KEY, (ICharSequence**)&key2);
        CString::New(EMPTY_VALUE, (ICharSequence**)&value2);
        mFields->Put(key2, value2);
    }
    else {
        AutoPtr<ICharSequence> key1, value1;
        CString::New(ENGINE_KEY, (ICharSequence**)&key1);
        CString::New(ENGINE_ENABLE, (ICharSequence**)&value1);
        mFields->Put(key1, value1);
        AutoPtr<ICharSequence> key2, value2;
        CString::New(ENGINE_ID_KEY, (ICharSequence**)&key2);
        CString::New(ConvertToQuotedString(ENGINE_ID_KEYSTORE), (ICharSequence**)&value2);
        mFields->Put(key2, value2);
    }

    return NOERROR;
}

/**
 * Get client certificate alias
 * @return alias to the client certificate
 * @hide
 */
ECode CWifiEnterpriseConfig::GetClientCertificateAlias(
    /* [out] */ String* result)
{
    return GetFieldValue(CLIENT_CERT_KEY, CLIENT_CERT_PREFIX, result);
}

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
ECode CWifiEnterpriseConfig::SetClientKeyEntry(
    /* [in] */ IPrivateKey* privateKey,
    /* [in] */ IX509Certificate* clientCertificate)
{
    if (clientCertificate != NULL) {
        Int32 constraints;
        clientCertificate->GetBasicConstraints(&constraints);
        if (constraints != -1) {
            //throw new IllegalArgumentException("Cannot be a CA certificate");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }

        if (privateKey == NULL) {
            //throw new IllegalArgumentException("Client cert without a private key");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }

        AutoPtr< ArrayOf<Byte> > encoded;
        IKey::Probe(privateKey)->GetEncoded((ArrayOf<Byte>**)&encoded);
        if (encoded == NULL) {
            //throw new IllegalArgumentException("Private key cannot be encoded");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }

    mClientPrivateKey = privateKey;
    mClientCertificate = clientCertificate;

    return NOERROR;
}

/**
 * Get client certificate
 *
 * @return X.509 client certificate
 */
ECode CWifiEnterpriseConfig::GetClientCertificate(
    /* [out] */ IX509Certificate** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mClientCertificate;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

/**
 * @hide
 */
ECode CWifiEnterpriseConfig::ResetClientKeyEntry()
{
    mClientPrivateKey = NULL;
    mClientCertificate = NULL;
    return NOERROR;
}

/**
 * @hide
 */
ECode CWifiEnterpriseConfig::GetClientPrivateKey(
    /* [out] */ IPrivateKey** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mClientPrivateKey;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

/**
 * Set subject match. This is the substring to be matched against the subject of the
 * authentication server certificate.
 * @param subjectMatch substring to be matched
 */
ECode CWifiEnterpriseConfig::SetSubjectMatch(
    /* [in] */ const String& subjectMatch)
{
    return SetFieldValue(SUBJECT_MATCH_KEY, subjectMatch, String(""));
}

/**
 * Get subject match
 * @return the subject match string
 */
ECode CWifiEnterpriseConfig::GetSubjectMatch(
    /* [out] */ String* result)
{
    return GetFieldValue(SUBJECT_MATCH_KEY, String(""), result);
}

/** Returns the field value for the key.
 * @param key into the hash
 * @param prefix is the prefix that the value may have
 * @return value
 * @hide
 */
ECode CWifiEnterpriseConfig::GetFieldValue(
    /* [in] */ const String& key,
    /* [in] */ const String& prefix,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<ICharSequence> iKey;
    CString::New(key, (ICharSequence**)&iKey);
    AutoPtr<IInterface> iValue;
    mFields->Get(iKey, (IInterface**)&iValue);
    String value;
    ICharSequence::Probe(iValue)->ToString(&value);
    // Uninitialized or known to be empty after reading from supplicant
    if (TextUtils::IsEmpty(value) || String(EMPTY_VALUE).Equals(value)) {
        *result = "";
        return NOERROR;
    }

    value = RemoveDoubleQuotes(value);
    if (value.StartWith(prefix)) {
        *result = value.Substring(prefix.GetLength());
        return NOERROR;
    }
    else {
        *result = value;
        return NOERROR;
    }

    return NOERROR;
}

/** Set a value with an optional prefix at key
 * @param key into the hash
 * @param value to be set
 * @param prefix an optional value to be prefixed to actual value
 * @hide
 */
ECode CWifiEnterpriseConfig::SetFieldValue(
    /* [in] */ const String& key,
    /* [in] */ const String& value,
    /* [in] */ const String& prefix)
{
    if (TextUtils::IsEmpty(value)) {
        AutoPtr<ICharSequence> iKey, iValue;
        CString::New(key, (ICharSequence**)&iKey);
        CString::New(EMPTY_VALUE, (ICharSequence**)&iValue);
        mFields->Put(iKey, iValue);
    }
    else {
        AutoPtr<ICharSequence> iKey, iValue;
        CString::New(key, (ICharSequence**)&iKey);
        String str(prefix);
        str += value;
        CString::New(str, (ICharSequence**)&iValue);
        mFields->Put(iKey, iValue);
    }

    return NOERROR;
}

/** Set a value with an optional prefix at key
 * @param key into the hash
 * @param value to be set
 * @param prefix an optional value to be prefixed to actual value
 * @hide
 */
ECode CWifiEnterpriseConfig::SetFieldValue(
    /* [in] */ const String& key,
    /* [in] */ const String& value)
{
    if (TextUtils::IsEmpty(value)) {
        AutoPtr<ICharSequence> iKey, iValue;
        CString::New(key, (ICharSequence**)&iKey);
        CString::New(EMPTY_VALUE, (ICharSequence**)&iValue);
        mFields->Put(iKey, iValue);
    }
    else {
        AutoPtr<ICharSequence> iKey, iValue;
        CString::New(key, (ICharSequence**)&iKey);
        CString::New(ConvertToQuotedString(value), (ICharSequence**)&iValue);
        IMap::Probe(mFields)->Put(iKey, iValue);
    }

    return NOERROR;
}

//@Override
ECode CWifiEnterpriseConfig::DescribeContents(
    /* [out] */ Int32* contents)
{
    VALIDATE_NOT_NULL(contents);
    *contents = 0;
    return NOERROR;
}

//@Override
ECode CWifiEnterpriseConfig::WriteToParcel(
    /* [in] */ IParcel* dest,
    /* [in] */ Int32 flags)
{
    Int32 size;
    mFields->GetSize(&size);
    dest->WriteInt32(size);
    AutoPtr<ISet> entries;
    mFields->GetEntrySet((ISet**)&entries);
    AutoPtr<IIterator> iter;
    entries->GetIterator((IIterator**)&iter);
    Boolean bNext;
    iter->HasNext(&bNext);
    for (; bNext; iter->HasNext(&bNext)) {
        AutoPtr<IInterface> obj;
        iter->GetNext((IInterface**)&obj);
        IMapEntry* entry = IMapEntry::Probe(obj);
        AutoPtr<IInterface> iKey, iValue;
        entry->GetKey((IInterface**)&iKey);
        entry->GetValue((IInterface**)&iValue);
        String key, value;
        ICharSequence::Probe(iKey)->ToString(&key);
        ICharSequence::Probe(iValue)->ToString(&value);
        dest->WriteString(key);
        dest->WriteString(value);
    }

    WriteCertificate(dest, mCaCert);

    if (mClientPrivateKey != NULL) {
        String algorithm;
        assert(0);
        // TODO
        // mClientPrivateKey->GetAlgorithm(&algorithm);
        AutoPtr< ArrayOf<Byte> > userKeyBytes;
        assert(0);
        // TODO
        // mClientPrivateKey->GetEncoded((ArrayOf<Byte>**)&userKeyBytes);
        dest->WriteInt32(userKeyBytes->GetLength());
        dest->WriteArrayOf((Handle32)userKeyBytes.Get());
        dest->WriteString(algorithm);
    }
    else {
        dest->WriteInt32(0);
    }

    WriteCertificate(dest, mClientCertificate);

    return NOERROR;
}

/** See {@link WifiConfiguration#getKeyIdForCredentials} @hide */
ECode CWifiEnterpriseConfig::GetKeyId(
    /* [in] */ IWifiEnterpriseConfig* current,
    /* [out] */ String* keyId)
{
    VALIDATE_NOT_NULL(keyId);

    AutoPtr<ICharSequence> key1, key2;
    CString::New(EAP_KEY, (ICharSequence**)&key1);
    CString::New(PHASE2_KEY, (ICharSequence**)&key2);
    AutoPtr<IInterface> iEap, iPhase2;
    mFields->Get(key1, (IInterface**)&iEap);
    String eap;
    ICharSequence::Probe(iEap)->ToString(&eap);
    mFields->Get(key2, (IInterface**)&iPhase2);
    String phase2;
    ICharSequence::Probe(iPhase2)->ToString(&phase2);

    // If either eap or phase2 are not initialized, use current config details
    if (TextUtils::IsEmpty((eap))) {
        AutoPtr<IHashMap> fields;
        current->GetFields((IHashMap**)&fields);
        AutoPtr<ICharSequence> key;
        CString::New(EAP_KEY, (ICharSequence**)&key);
        AutoPtr<IInterface> value;
        fields->Get(key, (IInterface**)&value);
        ICharSequence::Probe(value)->ToString(&eap);
    }

    if (TextUtils::IsEmpty(phase2)) {
        AutoPtr<IHashMap> fields;
        current->GetFields((IHashMap**)&fields);
        AutoPtr<ICharSequence> key;
        CString::New(PHASE2_KEY, (ICharSequence**)&key);
        AutoPtr<IInterface> value;
        fields->Get(key, (IInterface**)&value);
        ICharSequence::Probe(value)->ToString(&eap);
    }

    *keyId = eap;
    *keyId += "_";
    *keyId += phase2;

    return NOERROR;
}

//@Override
ECode CWifiEnterpriseConfig::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    StringBuffer sb;
    AutoPtr<ISet> keySet;
    mFields->GetKeySet((ISet**)&keySet);
    AutoPtr<IIterator> iter;
    keySet->GetIterator((IIterator**)&iter);
    Boolean bNext;
    iter->HasNext(&bNext);
    for (; bNext; iter->HasNext(&bNext)) {
        AutoPtr<IInterface> iKeyObj;
        iter->GetNext((IInterface**)&iKeyObj);
        ICharSequence* iKey = ICharSequence::Probe(iKeyObj);
        String key;
        iKey->ToString(&key);
        sb.Append(key);
        sb.Append(" ");
        AutoPtr<IInterface> iValue;
        mFields->Get(iKey, (IInterface**)&iValue);
        String value;
        ICharSequence::Probe(iValue)->ToString(&value);
        sb.Append(value);
        sb.Append("\n");
    }

    return sb.ToString(str);
}

void CWifiEnterpriseConfig::WriteCertificate(
    /* [in] */ IParcel* dest,
    /* [in] */ IX509Certificate* cert)
{
    if (cert != NULL) {
        // try {
            AutoPtr< ArrayOf<Byte> > certBytes;
            assert(0);
            // TODO
            // cert->GetEncoded((ArrayOf<Byte>**)&certBytes);
            dest->WriteInt32(certBytes->GetLength());
            dest->WriteArrayOf((Handle32)certBytes.Get());
        // } catch (CertificateEncodingException e) {
        //     dest.writeInt(0);
        // }
    }
    else {
        dest->WriteInt32(0);
    }
}

String CWifiEnterpriseConfig::RemoveDoubleQuotes(
    /* [in] */ const String& string)
{
    if (TextUtils::IsEmpty(string)) {
        return String("");
    }

    Int32 length = string.GetLength();
    if ((length > 1) && (string.GetChar(0) == '"')
            && (string.GetChar(length - 1) == '"')) {
        return string.Substring(1, length - 1);
    }

    return string;
}

String CWifiEnterpriseConfig::ConvertToQuotedString(
    /* [in] */ const String& string)
{
    String str("\"");
    str += string;
    str += "\"";
    return str;
}

/** Returns the index at which the toBeFound string is found in the array.
 * @param arr array of strings
 * @param toBeFound string to be found
 * @param defaultIndex default index to be returned when string is not found
 * @return the index into array
 */
Int32 CWifiEnterpriseConfig::GetStringIndex(
    /* [in] */ ArrayOf<String>* arr,
    /* [in] */ const String& toBeFound,
    /* [in] */ Int32 defaultIndex)
{
    if (TextUtils::IsEmpty(toBeFound)) {
        return defaultIndex;
    }

    for (Int32 i = 0; i < arr->GetLength(); i++) {
        if (toBeFound.Equals((*arr)[i])) return i;
    }

    return defaultIndex;
}

ECode CWifiEnterpriseConfig::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CWifiEnterpriseConfig::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

} // namespace Wifi
} // namespace Droid
} // namespace Elastos
