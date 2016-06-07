
#include "elastos/droid/server/telecom/Log.h"
#include <Elastos.CoreLibrary.Security.h>
#include <Elastos.Droid.Telephony.h>
#include <Elastos.Droid.Telecomm.h>
#include <elastos/core/StringBuffer.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/droid/net/Uri.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Net::IUri;
using Elastos::Droid::Net::Uri;
using Elastos::Droid::Telecomm::Telecom::IPhoneAccount;
using Elastos::Droid::Telephony::CPhoneNumberUtils;
using Elastos::Droid::Telephony::IPhoneNumberUtils;
using Elastos::Core::CObject;
using Elastos::Core::StringBuffer;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Security::CMessageDigestHelper;
using Elastos::Security::IMessageDigest;
using Elastos::Security::IMessageDigestHelper;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

const String Log::TAG("Telecom");

const Boolean Log::FORCE_LOGGING = FALSE; /* STOP SHIP if true */
const Boolean Log::DEBUG = TRUE;
const Boolean Log::INFO = IsLoggable(Logger::INFO);
const Boolean Log::VERBOSE = IsLoggable(Logger::VERBOSE);
const Boolean Log::WARN = IsLoggable(Logger::WARN);
const Boolean Log::ERROR = IsLoggable(Logger::ERROR);

Log::Log()
{}

Boolean Log::IsLoggable(
    /* [in] */ Int32 level)
{
    return FORCE_LOGGING || Logger::IsLoggable(TAG, level);
}

#define MSG_BUF_SIZE    1024

ECode Log::D(
    /* [in] */ const char * prefix,
    /* [in] */ const char * fmt, ...)
{
    if (DEBUG) {
        char msgBuf[MSG_BUF_SIZE];
        va_list args;
        va_start(args, fmt);
        vsnprintf(msgBuf, MSG_BUF_SIZE, fmt, args);
        va_end(args);
        Slogger::D(TAG, BuildMessage(prefix, msgBuf));
    }
    return NOERROR;
}

ECode Log::D(
    /* [in] */ IInterface* objectPrefix,
    /* [in] */ const char * fmt, ...)
{
    if (DEBUG) {
        char msgBuf[MSG_BUF_SIZE];
        va_list args;
        va_start(args, fmt);
        vsnprintf(msgBuf, MSG_BUF_SIZE, fmt, args);
        va_end(args);
        Slogger::D(TAG, BuildMessage(GetPrefixFromObject(objectPrefix), msgBuf));
    }
    return NOERROR;
}

ECode Log::I(
    /* [in] */ const char * prefix,
    /* [in] */ const char * fmt, ...)
{
    if (INFO) {
        char msgBuf[MSG_BUF_SIZE];
        va_list args;
        va_start(args, fmt);
        vsnprintf(msgBuf, MSG_BUF_SIZE, fmt, args);
        va_end(args);
        Slogger::I(TAG, BuildMessage(prefix, msgBuf));
    }
    return NOERROR;
}

ECode Log::I(
    /* [in] */ IInterface* objectPrefix,
    /* [in] */ const char * fmt, ...)
{
    if (INFO) {
        char msgBuf[MSG_BUF_SIZE];
        va_list args;
        va_start(args, fmt);
        vsnprintf(msgBuf, MSG_BUF_SIZE, fmt, args);
        va_end(args);
        Slogger::I(TAG, BuildMessage(GetPrefixFromObject(objectPrefix), msgBuf));
    }
    return NOERROR;
}

ECode Log::V(
    /* [in] */ const char * prefix,
    /* [in] */ const char * fmt, ...)
{
    if (VERBOSE) {
        char msgBuf[MSG_BUF_SIZE];
        va_list args;
        va_start(args, fmt);
        vsnprintf(msgBuf, MSG_BUF_SIZE, fmt, args);
        va_end(args);
        Slogger::V(TAG, BuildMessage(prefix, msgBuf));
    }
    return NOERROR;
}

ECode Log::V(
    /* [in] */ IInterface* objectPrefix,
    /* [in] */ const char * fmt, ...)
{
    if (VERBOSE) {
        char msgBuf[MSG_BUF_SIZE];
        va_list args;
        va_start(args, fmt);
        vsnprintf(msgBuf, MSG_BUF_SIZE, fmt, args);
        va_end(args);
        Slogger::V(TAG, BuildMessage(GetPrefixFromObject(objectPrefix), msgBuf));
    }
    return NOERROR;
}

ECode Log::W(
    /* [in] */ const char * prefix,
    /* [in] */ const char * fmt, ...)
{
    if (WARN) {
        char msgBuf[MSG_BUF_SIZE];
        va_list args;
        va_start(args, fmt);
        vsnprintf(msgBuf, MSG_BUF_SIZE, fmt, args);
        va_end(args);
        Slogger::W(TAG, BuildMessage(prefix, msgBuf));
    }
    return NOERROR;
}

ECode Log::W(
    /* [in] */ IInterface* objectPrefix,
    /* [in] */ const char * fmt, ...)
{
    if (WARN) {
        char msgBuf[MSG_BUF_SIZE];
        va_list args;
        va_start(args, fmt);
        vsnprintf(msgBuf, MSG_BUF_SIZE, fmt, args);
        va_end(args);
        Slogger::W(TAG, BuildMessage(GetPrefixFromObject(objectPrefix), msgBuf));
    }
    return NOERROR;
}

ECode Log::E(
    /* [in] */ const char * prefix,
    /* [in] */ const char * fmt, ...)
{
    if (ERROR) {
        char msgBuf[MSG_BUF_SIZE];
        va_list args;
        va_start(args, fmt);
        vsnprintf(msgBuf, MSG_BUF_SIZE, fmt, args);
        va_end(args);
        Slogger::E(TAG, BuildMessage(prefix, msgBuf));
    }
    return NOERROR;
}

ECode Log::E(
    /* [in] */ IInterface* objectPrefix,
    /* [in] */ const char * fmt, ...)
{
    if (ERROR) {
        char msgBuf[MSG_BUF_SIZE];
        va_list args;
        va_start(args, fmt);
        vsnprintf(msgBuf, MSG_BUF_SIZE, fmt, args);
        va_end(args);
        Slogger::E(TAG, BuildMessage(GetPrefixFromObject(objectPrefix), msgBuf));
    }
    return NOERROR;
}

ECode Log::Wtf(
    /* [in] */ const char * prefix,
    /* [in] */ const char * fmt, ...)
{
    char msgBuf[MSG_BUF_SIZE];
    va_list args;
    va_start(args, fmt);
    vsnprintf(msgBuf, MSG_BUF_SIZE, fmt, args);
    va_end(args);
    String msg = BuildMessage(prefix, msgBuf);
    Slogger::E(TAG, msg);
    return E_ILLEGAL_STATE_EXCEPTION;
}

ECode Log::Wtf(
    /* [in] */ IInterface* objectPrefix,
    /* [in] */ const char * fmt, ...)
{
    char msgBuf[MSG_BUF_SIZE];
    va_list args;
    va_start(args, fmt);
    vsnprintf(msgBuf, MSG_BUF_SIZE, fmt, args);
    va_end(args);
    String msg = BuildMessage(GetPrefixFromObject(objectPrefix), msgBuf);
    Slogger::E(TAG, msg);
    return E_ILLEGAL_STATE_EXCEPTION;
}

String Log::PiiHandle(
    /* [in] */ IInterface* pii)
{
    if (pii == NULL || VERBOSE) {
        return TO_STR(pii);
    }
    if (IUri::Probe(pii) != NULL) {
        AutoPtr<IUri> uri = IUri::Probe(pii);
        // All Uri's which are not "tel" go through normal pii() method.
        String scheme;
        uri->GetScheme(&scheme);
        if (!IPhoneAccount::SCHEME_TEL.Equals(scheme)) {
            return Pii(pii);
        } else {
            String schemeSpecificPart;
            uri->GetSchemeSpecificPart(&schemeSpecificPart);
            pii = StringUtils::ParseCharSequence(schemeSpecificPart);
        }
    }
    String originalString;
    IObject::Probe(pii)->ToString(&originalString);
    StringBuilder stringBuilder(originalString.GetLength());
    AutoPtr<ArrayOf<Char32> > charArray = originalString.GetChars();
    for (Int32 i = 0; i < charArray->GetLength(); ++i) {
        Char32 c = (*charArray)[i];
        AutoPtr<IPhoneNumberUtils> phoneNumberUtilsHelper;
        CPhoneNumberUtils::AcquireSingleton((IPhoneNumberUtils**)&phoneNumberUtilsHelper);
        Boolean isDialable;
        phoneNumberUtilsHelper->IsDialable(c, &isDialable);
        if (isDialable) {
            stringBuilder.Append('*');
        } else {
            stringBuilder.AppendChar(c);
        }
    }
    return stringBuilder.ToString();
}

String Log::Pii(
    /* [in] */ IInterface* pii)
{
    if (pii == NULL || VERBOSE) {
        return TO_STR(pii);
    }
    String hash;
    SecureHash(TO_STR(pii).GetBytes(), &hash);
    return String("[") + hash + "]";
}

ECode Log::SecureHash(
    /* [in] */ ArrayOf<Byte>* input,
    /* [out] */ String* _result)
{
    VALIDATE_NOT_NULL(_result)
    *_result = String(NULL);

    AutoPtr<IMessageDigest> messageDigest;
    // try {
    ECode ec;
    AutoPtr<IMessageDigestHelper> helper;
    CMessageDigestHelper::AcquireSingleton((IMessageDigestHelper**)&helper);
    ec = helper->GetInstance(String("SHA-1"), (IMessageDigest**)&messageDigest);
    // } catch (NoSuchAlgorithmException e) {
    if (FAILED(ec)) {
        if ((ECode)E_NO_SUCH_ALGORITHM_EXCEPTION == ec) {
            return NOERROR;
        }
        return ec;
    }
    // }
    messageDigest->Update(input);
    AutoPtr<ArrayOf<Byte> > result;
    messageDigest->Digest((ArrayOf<Byte>**)&result);
    *_result = EncodeHex(result);
    return NOERROR;
}

String Log::EncodeHex(
    /* [in] */ ArrayOf<Byte>* bytes)
{
    StringBuffer hex(bytes->GetLength() * 2);
    for (Int32 i = 0; i < bytes->GetLength(); i++) {
        Int32 byteIntValue = (*bytes)[i] & 0xff;
        if (byteIntValue < 0x10) {
            hex.Append("0");
        }
        hex.Append(StringUtils::ToString(byteIntValue, 16));
    }
    return hex.ToString();
}

String Log::GetPrefixFromObject(
    /* [in] */ IInterface* obj)
{
    AutoPtr<IClassInfo> classInfo;
    CObject::ReflectClassInfo(obj, (IClassInfo**)&classInfo);
    String name;
    classInfo->GetName(&name);
    return obj == NULL ? String("<null>") : name;
}

String Log::BuildMessage(
    /* [in] */ const char * prefix,
    /* [in] */ const char * format, ...)
{
    // try {
    char msgBuf[MSG_BUF_SIZE];
    va_list args;
    va_start(args, format);
    vsnprintf(msgBuf, MSG_BUF_SIZE, format, args);
    va_end(args);
    // } catch (IllegalFormatException ife) {
    //     e("Log", ife, "IllegalFormatException: formatString='%s' numArgs=%d", format,
    //             args.length);
    //     msg = format + " (An error occurred while formatting the message.)";
    // }
    String rev;
    rev.AppendFormat("%s: %s", prefix, msgBuf);
    return rev;
}

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos
