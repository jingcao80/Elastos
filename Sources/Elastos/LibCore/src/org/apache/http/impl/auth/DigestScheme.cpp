
#include "org/apache/http/impl/auth/DigestScheme.h"
#include "org/apache/http/utility/EncodingUtils.h"
#include "org/apache/http/utility/CCharArrayBuffer.h"
#include "org/apache/http/message/CBufferedHeader.h"
#include "org/apache/http/message/CBasicNameValuePair.h"
#include "org/apache/http/message/BasicHeaderValueFormatter.h"
#include "org/apache/http/auth/params/AuthParams.h"
#include "elastos/core/CSystem.h"
#include "elastos/core/CString.h"
#include "elastos/core/StringBuilder.h"
#include "elastos/core/StringUtils.h"
#include "elastos/security/CMessageDigestHelper.h"
#include "elastos/utility/logging/Logger.h"
#include "elastos/utility/CStringTokenizer.h"
#include <elastos/utility/etl/List.h>

using Elastos::Core::StringUtils;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Core::StringBuilder;
using Elastos::Security::IMessageDigestHelper;
using Elastos::Security::CMessageDigestHelper;
using Elastos::Security::IPrincipal;
using Elastos::Utility::IStringTokenizer;
using Elastos::Utility::CStringTokenizer;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Etl::List;
using Org::Apache::Http::IRequestLine;
using Org::Apache::Http::Auth::IAUTH;
using Org::Apache::Http::Auth::Params::AuthParams;
using Org::Apache::Http::Message::CBufferedHeader;
using Org::Apache::Http::Message::IBasicNameValuePair;
using Org::Apache::Http::Message::CBasicNameValuePair;
using Org::Apache::Http::Message::BasicHeaderValueFormatter;
using Org::Apache::Http::Message::IHeaderValueFormatter;
using Org::Apache::Http::Auth::Params::AuthParams;
using Org::Apache::Http::Utility::ICharArrayBuffer;
using Org::Apache::Http::Utility::CCharArrayBuffer;
using Org::Apache::Http::Utility::EncodingUtils;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Auth {

static AutoPtr< ArrayOf<Char32> > InitHEXADECIMAL()
{
    AutoPtr< ArrayOf<Char32> > array = ArrayOf<Char32> ::Alloc(16);
    (*array)[0] = '0';
    (*array)[1] = '1';
    (*array)[2] = '2';
    (*array)[3] = '3';
    (*array)[4] = '4';
    (*array)[5] = '5';
    (*array)[6] = '6';
    (*array)[7] = '7';
    (*array)[8] = '8';
    (*array)[9] = '9';
    (*array)[10] = 'a';
    (*array)[11] = 'b';
    (*array)[12] = 'c';
    (*array)[13] = 'd';
    (*array)[14] = 'e';
    (*array)[15] = 'f';
    return array;
}
const AutoPtr< ArrayOf<Char32> > DigestScheme::HEXADECIMAL = InitHEXADECIMAL();
const String DigestScheme::NC("00000001"); //nonce-count is always 1
const Int32 DigestScheme::QOP_MISSING;
const Int32 DigestScheme::QOP_AUTH_INT;
const Int32 DigestScheme::QOP_AUTH;

DigestScheme::DigestScheme()
    : RFC2617Scheme()
    , mComplete(FALSE)
    , mQopVariant(QOP_MISSING)
{}

ECode DigestScheme::ProcessChallenge(
    /* [in] */ IHeader* header)
{
    String realm;
    if (GetParameter(String("realm"), &realm), realm.IsNull()) {
        Logger::E("DigestScheme", "missing realm in challange");
        return E_MALFORMED_CHALLENGE_EXCEPTION;
    }
    String nonce;
    if (GetParameter(String("nonce"), &nonce), nonce.IsNull()) {
        Logger::E("DigestScheme", "missing nonce in challange");
        return E_MALFORMED_CHALLENGE_EXCEPTION;
    }

    Boolean unsupportedQop = FALSE;
    // qop parsing
    String qop;
    if (GetParameter(String("qop"), &qop), !qop.IsNull()) {
        AutoPtr<IStringTokenizer> tok;
        CStringTokenizer::New(qop, String(","), (IStringTokenizer**)&tok);
        Boolean hasMore;
        while (tok->HasMoreTokens(&hasMore), hasMore) {
            String variant;
            tok->GetNextToken(&variant);
            variant.Trim();
            if (variant.Equals(String("auth"))) {
                mQopVariant = QOP_AUTH;
                break; //that's our favourite, because auth-int is unsupported
            }
            else if (variant.Equals(String("auth-int"))) {
                mQopVariant = QOP_AUTH_INT;
            }
            else {
                unsupportedQop = TRUE;
            }
        }
    }

    if (unsupportedQop && (mQopVariant == QOP_MISSING)) {
        Logger::E("DigestScheme", "None of the qop methods is supported");
        return E_MALFORMED_CHALLENGE_EXCEPTION;
    }
    // Reset cnonce
    mCnonce = NULL;
    mComplete = TRUE;
    return NOERROR;
}

ECode DigestScheme::IsComplete(
    /* [out] */ Boolean* isComplete)
{
    VALIDATE_NOT_NULL(isComplete)
    String s;
    GetParameter(String("stale"), &s);
    if (String("true").EqualsIgnoreCase(s)) {
        *isComplete = FALSE;
    }
    else {
        *isComplete = mComplete;
    }
    return NOERROR;
}

ECode DigestScheme::GetSchemeName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)
    *name = String("digest");
    return NOERROR;
}

ECode DigestScheme::IsConnectionBased(
    /* [out] */ Boolean* connectionBased)
{
    VALIDATE_NOT_NULL(connectionBased)
    *connectionBased = FALSE;
    return NOERROR;
}

void DigestScheme::OverrideParamter(
    /* [in] */ const String& name,
    /* [in] */ const String& value)
{
    AutoPtr<ICharSequence> cs1, cs2;
    CString::New(name, (ICharSequence**)&cs1);
    CString::New(value, (ICharSequence**)&cs2);
    GetParameters()->Put(cs1, cs2);
}

String DigestScheme::GetCnonce()
{
    if (mCnonce.IsNull()) {
        mCnonce = CreateCnonce();
    }
    return mCnonce;
}

ECode DigestScheme::Authenticate(
    /* [in] */ ICredentials* credentials,
    /* [in] */ IHttpRequest* request,
    /* [out] */ IHeader** header)
{
    VALIDATE_NOT_NULL(header)
    *header = NULL;

    if (credentials == NULL) {
        Logger::E("DigestScheme", "Credentials may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (request == NULL) {
        Logger::E("DigestScheme", "HTTP request may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    // Add method name and request-URI to the parameter map
    AutoPtr<ICharSequence> cs1;
    CString::New(String("methodname"), (ICharSequence**)&cs1);
    AutoPtr<IRequestLine> line;
    request->GetRequestLine((IRequestLine**)&line);
    String method;
    line->GetMethod(&method);
    AutoPtr<ICharSequence> cs2;
    CString::New(method, (ICharSequence**)&cs2);
    GetParameters()->Put(cs1, cs2);
    String uri;
    line->GetUri(&uri);
    AutoPtr<ICharSequence> cs3, cs4;
    CString::New(String("uri"), (ICharSequence**)&cs3);
    CString::New(uri, (ICharSequence**)&cs4);
    GetParameters()->Put(cs3, cs4);
    String charset;
    GetParameter(String("charset"), &charset);
    if (charset.IsNull()) {
        AutoPtr<IHttpParams> params;
        AuthParams::GetCredentialCharset(params, &charset);
        AutoPtr<ICharSequence> cs5, cs6;
        CString::New(String("charset"), (ICharSequence**)&cs5);
        CString::New(charset, (ICharSequence**)&cs6);
        GetParameters()->Put(cs5, cs6);
    }
    String digest;
    CreateDigest(credentials, &digest);
    return CreateDigestHeader(credentials, digest, header);
}

ECode DigestScheme::CreateMessageDigest(
    /* [in] */ const String& digAlg,
    /* [out] */ IMessageDigest** digest)
{
    VALIDATE_NOT_NULL(digest)
    // try {
    AutoPtr<IMessageDigestHelper> helper;
    CMessageDigestHelper::AcquireSingleton((IMessageDigestHelper**)&helper);
    ECode ec = helper->GetInstance(digAlg, digest);
    if (FAILED(ec)) {
        *digest = NULL;
        Logger::E("DigestScheme", "Unsupported algorithm in HTTP Digest authentication: %s", digAlg.string());
        return E_UNSUPPORTED_DIGEST_ALGORITHM_EXCEPTION;
    }
    return NOERROR;
    // } catch (Exception e) {
    //     throw new UnsupportedDigestAlgorithmException(
    //       "Unsupported algorithm in HTTP Digest authentication: "
    //        + digAlg);
    // }
}

ECode DigestScheme::CreateDigest(
    /* [in] */ ICredentials* credentials,
    /* [out] */ String* digest)
{
    VALIDATE_NOT_NULL(digest)
    *digest = String(NULL);

    // Collecting required tokens
    String uri, realm, nonce, method, algorithm;
    GetParameter(String("uri"), &uri);
    GetParameter(String("realm"), &realm);
    GetParameter(String("nonce"), &nonce);
    GetParameter(String("methodname"), &method);
    GetParameter(String("algorithm"), &algorithm);
    if (uri.IsNull()) {
        Logger::E("DigestScheme", "URI may not be null");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    if (realm.IsNull()) {
        Logger::E("DigestScheme", "Realm may not be null");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    if (nonce.IsNull()) {
        Logger::E("DigestScheme", "Nonce may not be null");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    // If an algorithm is not specified, default to MD5.
    if (algorithm.IsNull()) {
        algorithm = String("MD5");
    }
    // If an charset is not specified, default to ISO-8859-1.
    String charset;
    GetParameter(String("charset"), &charset);
    if (charset.IsNull()) {
        charset = String("ISO-8859-1");
    }

    if (mQopVariant == QOP_AUTH_INT) {
        Logger::E("DigestScheme", "Unsupported qop in HTTP Digest authentication");
        return E_AUTHENTICATION_EXCEPTION;
    }

    AutoPtr<IMessageDigest> md5Helper;
    CreateMessageDigest(String("MD5"), (IMessageDigest**)&md5Helper);

    AutoPtr<IPrincipal> principal;
    credentials->GetUserPrincipal((IPrincipal**)&principal);
    String uname;
    principal->GetName(&uname);
    String pwd;
    credentials->GetPassword(&pwd);

    // 3.2.2.2: Calculating digest
    StringBuilder tmp(uname.GetLength() + realm.GetLength() + pwd.GetLength() + 2);
    tmp.Append(uname);
    tmp.AppendChar(':');
    tmp.Append(realm);
    tmp.AppendChar(':');
    tmp.Append(pwd);
    // unq(username-value) ":" unq(realm-value) ":" passwd
    String a1 = tmp.ToString();

    //a1 is suitable for MD5 algorithm
    if(algorithm.EqualsIgnoreCase(String("MD5-sess"))) { // android-changed: ignore case
        // H( unq(username-value) ":" unq(realm-value) ":" passwd )
        //      ":" unq(nonce-value)
        //      ":" unq(cnonce-value)

        String cnonce = GetCnonce();

        AutoPtr< ArrayOf<Byte> > bytes;
        EncodingUtils::GetBytes(a1, charset, (ArrayOf<Byte>**)&bytes);
        AutoPtr< ArrayOf<Byte> > binaryData;
        md5Helper->Digest(bytes, (ArrayOf<Byte>**)&binaryData);
        String tmp2 = Encode(binaryData);
        StringBuilder tmp3(tmp2.GetLength() + nonce.GetLength() + cnonce.GetLength() + 2);
        tmp3.Append(tmp2);
        tmp3.AppendChar(':');
        tmp3.Append(nonce);
        tmp3.AppendChar(':');
        tmp3.Append(cnonce);
        a1 = tmp3.ToString();
    }
    else if (!algorithm.EqualsIgnoreCase(String("MD5"))) { // android-changed: ignore case
        Logger::E("DigestScheme", "Unhandled algorithm %s requested", algorithm.string());
        return E_AUTHENTICATION_EXCEPTION;
    }
    AutoPtr< ArrayOf<Byte> > bytes;
    EncodingUtils::GetBytes(a1, charset, (ArrayOf<Byte>**)&bytes);
    AutoPtr< ArrayOf<Byte> > binaryData;
    md5Helper->Digest(bytes, (ArrayOf<Byte>**)&binaryData);
    String md5a1 = Encode(binaryData);

    String a2 = String(NULL);
    if (mQopVariant == QOP_AUTH_INT) {
        // Unhandled qop auth-int
        //we do not have access to the entity-body or its hash
        //TODO: add Method ":" digest-uri-value ":" H(entity-body)
    }
    else {
        a2 = method + String(":") + uri;
    }
    bytes = NULL;
    EncodingUtils::GetAsciiBytes(a2, (ArrayOf<Byte>**)&bytes);
    binaryData = NULL;
    md5Helper->Digest(bytes, (ArrayOf<Byte>**)&binaryData);
    String md5a2 = Encode(binaryData);

    // 3.2.2.1
    String serverDigestValue;
    if (mQopVariant == QOP_MISSING) {
        StringBuilder tmp2(md5a1.GetLength() + nonce.GetLength() + md5a2.GetLength());
        tmp2.Append(md5a1);
        tmp2.AppendChar(':');
        tmp2.Append(nonce);
        tmp2.AppendChar(':');
        tmp2.Append(md5a2);
        serverDigestValue = tmp2.ToString();
    }
    else {
        String qopOption = GetQopVariantString();
        String cnonce = GetCnonce();

        StringBuilder tmp2(md5a1.GetLength() + nonce.GetLength()
                + NC.GetLength() + cnonce.GetLength() + qopOption.GetLength() + md5a2.GetLength() + 5);
        tmp2.Append(md5a1);
        tmp2.AppendChar(':');
        tmp2.Append(nonce);
        tmp2.AppendChar(':');
        tmp2.Append(NC);
        tmp2.AppendChar(':');
        tmp2.Append(cnonce);
        tmp2.AppendChar(':');
        tmp2.Append(qopOption);
        tmp2.AppendChar(':');
        tmp2.Append(md5a2);
        serverDigestValue = tmp2.ToString();
    }

    bytes = NULL;
    EncodingUtils::GetAsciiBytes(serverDigestValue, (ArrayOf<Byte>**)&bytes);
    binaryData = NULL;
    md5Helper->Digest(bytes, (ArrayOf<Byte>**)&binaryData);
    String serverDigest = Encode(binaryData);

    *digest = serverDigest;
    return NOERROR;
}

ECode DigestScheme::CreateDigestHeader(
    /* [in] */ ICredentials* credentials,
    /* [in] */ const String& digest,
    /* [out] */ IHeader** header)
{
    VALIDATE_NOT_NULL(header)
    AutoPtr<ICharArrayBuffer> buffer;
    CCharArrayBuffer::New(128, (ICharArrayBuffer**)&buffer);
    if (IsProxy()) {
        buffer->Append(IAUTH::PROXY_AUTH_RESP);
    }
    else {
        buffer->Append(IAUTH::WWW_AUTH_RESP);
    }
    buffer->Append(String(": Digest "));

    String uri, realm, nonce, opaque, algorithm;
    GetParameter(String("uri"), &uri);
    GetParameter(String("realm"), &realm);
    GetParameter(String("nonce"), &nonce);
    GetParameter(String("opaque"), &opaque);
    String response = digest;
    GetParameter(String("algorithm"), &algorithm);

    AutoPtr<IPrincipal> principal;
    credentials->GetUserPrincipal((IPrincipal**)&principal);
    String uname;
    principal->GetName(&uname);

    AutoPtr< List<AutoPtr<IBasicNameValuePair> > > params = new List<AutoPtr<IBasicNameValuePair> >();
    AutoPtr<IBasicNameValuePair> unameP;
    CBasicNameValuePair::New(String("username"), uname, (IBasicNameValuePair**)&unameP);
    params->PushBack(unameP);
    AutoPtr<IBasicNameValuePair> realmP;
    CBasicNameValuePair::New(String("realm"), realm, (IBasicNameValuePair**)&realmP);
    params->PushBack(realmP);
    AutoPtr<IBasicNameValuePair> nonceP;
    CBasicNameValuePair::New(String("nonce"), nonce, (IBasicNameValuePair**)&nonceP);
    params->PushBack(nonceP);
    AutoPtr<IBasicNameValuePair> uriP;
    CBasicNameValuePair::New(String("uri"), uri, (IBasicNameValuePair**)&uriP);
    params->PushBack(uriP);
    AutoPtr<IBasicNameValuePair> responseP;
    CBasicNameValuePair::New(String("response"), response, (IBasicNameValuePair**)&responseP);
    params->PushBack(responseP);

    if (mQopVariant != QOP_MISSING) {
        AutoPtr<IBasicNameValuePair> qopP;
        CBasicNameValuePair::New(String("qop"), GetQopVariantString(), (IBasicNameValuePair**)&qopP);
        params->PushBack(qopP);
        AutoPtr<IBasicNameValuePair> ncP;
        CBasicNameValuePair::New(String("nc"), NC, (IBasicNameValuePair**)&ncP);
        params->PushBack(ncP);
        AutoPtr<IBasicNameValuePair> cnonceP;
        CBasicNameValuePair::New(String("cnonce"), GetCnonce(), (IBasicNameValuePair**)&cnonceP);
        params->PushBack(cnonceP);
    }
    if (!algorithm.IsNull()) {
        AutoPtr<IBasicNameValuePair> algorithmP;
        CBasicNameValuePair::New(String("algorithm"), algorithm, (IBasicNameValuePair**)&algorithmP);
        params->PushBack(algorithmP);
    }
    if (!opaque.IsNull()) {
        AutoPtr<IBasicNameValuePair> opaqueP;
        CBasicNameValuePair::New(String("opaque"), opaque, (IBasicNameValuePair**)&opaqueP);
        params->PushBack(opaqueP);
    }

    List<AutoPtr<IBasicNameValuePair> >::Iterator it = params->Begin();
    for (; it != params->End(); ++it) {
        AutoPtr<IBasicNameValuePair> param = *it;
        if (it != params->Begin()) {
            buffer->Append(String(", "));
        }
        String name;
        INameValuePair::Probe(param)->GetName(&name);
        Boolean noQuotes = name.Equals("nc") || name.Equals("qop");
        AutoPtr<ICharArrayBuffer> b;
        IHeaderValueFormatter::Probe(BasicHeaderValueFormatter::DEFAULT)->FormatNameValuePair(
                buffer, INameValuePair::Probe(param), !noQuotes, (ICharArrayBuffer**)&b);
    }
    return CBufferedHeader::New(buffer, header);
}

String DigestScheme::GetQopVariantString()
{
    String qopOption;
    if (mQopVariant == QOP_AUTH_INT) {
        qopOption = String("auth-int");
    }
    else {
        qopOption = String("auth");
    }
    return qopOption;
}

String DigestScheme::Encode(
    /* [in] */ ArrayOf<Byte>* binaryData)
{
    if (binaryData->GetLength() != 16) {
        return String(NULL);
    }

    AutoPtr< ArrayOf<Char32> > buffer = ArrayOf<Char32>::Alloc(32);
    for (Int32 i = 0; i < 16; i++) {
        Int32 low = ((*binaryData)[i] & 0x0f);
        Int32 high = (((*binaryData)[i] & 0xf0) >> 4);
        (*buffer)[i * 2] = (*HEXADECIMAL)[high];
        (*buffer)[(i * 2) + 1] = (*HEXADECIMAL)[low];
    }

    return String(*buffer);
}

String DigestScheme::CreateCnonce()
{
    String cnonce;

    AutoPtr<IMessageDigest> md5Helper;
    CreateMessageDigest(String("MD5"), (IMessageDigest**)&md5Helper);

    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    Int64 millis;
    system->GetCurrentTimeMillis(&millis);
    cnonce = StringUtils::ToString(millis);
    AutoPtr< ArrayOf<Byte> > bytes;
    EncodingUtils::GetAsciiBytes(cnonce, (ArrayOf<Byte>**)&bytes);
    AutoPtr< ArrayOf<Byte> > binaryData;
    md5Helper->Digest(bytes, (ArrayOf<Byte>**)&binaryData);
    cnonce = Encode(binaryData);

    return cnonce;
}

} // namespace Auth
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org