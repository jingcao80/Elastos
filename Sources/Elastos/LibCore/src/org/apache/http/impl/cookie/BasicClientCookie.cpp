
#include "org/apache/http/impl/cookie/BasicClientCookie.h"
#include "elastos/core/StringBuilder.h"
#include "elastos/core/StringUtils.h"
#include "elastos/core/CString.h"
#include "elastos/utility/CHashMap.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Utility::IHashMap;
using Elastos::Utility::CHashMap;
using Elastos::Utility::ILocale;
using Elastos::Utility::Logging::Logger;
using Elastos::Core::EIID_ICloneable;
using Org::Apache::Http::Cookie::EIID_ISetCookie;
using Org::Apache::Http::Cookie::EIID_IClientCookie;
using Org::Apache::Http::Cookie::EIID_ICookie;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Cookie {

BasicClientCookie::BasicClientCookie()
    : mIsSecure(FALSE)
    , mCookieVersion(0)
{}

BasicClientCookie::BasicClientCookie(
    /* [in] */ const String& name,
    /* [in] */ const String& value)
    : mIsSecure(FALSE)
    , mCookieVersion(0)
{
    if (name.IsNull()) {
        Logger::E("BasicClientCookie", "Name may not be null");
        assert(0);
        // throw new IllegalArgumentException("Name may not be null");
    }
    mName = name;
    AutoPtr<IHashMap> hm;
    CHashMap::New((IHashMap**)&hm);
    mAttribs = IMap::Probe(hm);
    mValue = value;
}

CAR_INTERFACE_IMPL_4(BasicClientCookie, Object, ISetCookie, IClientCookie, ICookie, ICloneable)

ECode BasicClientCookie::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)
    *name = mName;
    return NOERROR;
}

ECode BasicClientCookie::GetValue(
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value)
    *value = mValue;
    return NOERROR;
}

ECode BasicClientCookie::SetValue(
    /* [in] */ const String& value)
{
    mValue = value;
    return NOERROR;
}

ECode BasicClientCookie::GetComment(
    /* [out] */ String* comment)
{
    VALIDATE_NOT_NULL(comment)
    *comment = mCookieComment;
    return NOERROR;
}

ECode BasicClientCookie::SetComment(
    /* [in] */ const String& comment)
{
    mCookieComment = comment;
    return NOERROR;
}

ECode BasicClientCookie::GetCommentURL(
    /* [out] */ String* commentURL)
{
    VALIDATE_NOT_NULL(commentURL)
    *commentURL = String(NULL);
    return NOERROR;
}

ECode BasicClientCookie::GetExpiryDate(
    /* [out] */ IDate** date)
{
    VALIDATE_NOT_NULL(date)
    *date = mCookieExpiryDate;
    REFCOUNT_ADD(*date)
    return NOERROR;
}

ECode BasicClientCookie::SetExpiryDate(
    /* [in] */ IDate* expiryDate)
{
    mCookieExpiryDate = expiryDate;
    return NOERROR;
}

ECode BasicClientCookie::IsPersistent(
    /* [out] */ Boolean* isPersistent)
{
    VALIDATE_NOT_NULL(isPersistent)
    *isPersistent = (NULL != mCookieExpiryDate);
    return NOERROR;
}

ECode BasicClientCookie::GetDomain(
    /* [out] */ String* domain)
{
    VALIDATE_NOT_NULL(domain)
    *domain = mCookieDomain;
    return NOERROR;
}

ECode BasicClientCookie::SetDomain(
    /* [in] */ const String& domain)
{
    if (!domain.IsNull()) {
        mCookieDomain = domain.ToLowerCase(/*ILocale::ENGLISH*/);
    } else {
        mCookieDomain = String(NULL);
    }
    return NOERROR;
}

ECode BasicClientCookie::GetPath(
    /* [out] */ String* path)
{
    VALIDATE_NOT_NULL(path)
    *path = mCookiePath;
    return NOERROR;
}

ECode BasicClientCookie::SetPath(
    /* [in] */ const String& path)
{
    mCookiePath = path;
    return NOERROR;
}

ECode BasicClientCookie::IsSecure(
    /* [out] */ Boolean* isSecure)
{
    VALIDATE_NOT_NULL(isSecure)
    *isSecure = mIsSecure;
    return NOERROR;
}

ECode BasicClientCookie::SetSecure(
    /* [in] */ Boolean secure)
{
    mIsSecure = secure;
    return NOERROR;
}

ECode BasicClientCookie::GetPorts(
    /* [out, callee] */ ArrayOf<Int32>** ports)
{
    VALIDATE_NOT_NULL(ports)
    *ports = NULL;
    return NOERROR;
}

ECode BasicClientCookie::GetVersion(
    /* [out] */ Int32* version)
{
    VALIDATE_NOT_NULL(version)
    *version = mCookieVersion;
    return NOERROR;
}

ECode BasicClientCookie::SetVersion(
    /* [in] */ Int32 version)
{
    mCookieVersion = version;
    return NOERROR;
}

ECode BasicClientCookie::IsExpired(
    /* [in] */ IDate* date,
    /* [out] */ Boolean* isExpired)
{
    VALIDATE_NOT_NULL(isExpired)
    *isExpired = FALSE;
    if (date == NULL) {
        Logger::E("BasicClientCookie", "Date may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (mCookieExpiryDate != NULL) {
        Int64 expiryTime, time;
        mCookieExpiryDate->GetTime(&expiryTime);
        date->GetTime(&time);
        *isExpired = expiryTime <= time;
    }
    else {
        *isExpired = FALSE;
    }
    return NOERROR;
}

ECode BasicClientCookie::SetAttribute(
    /* [in] */ const String& name,
    /* [in] */ const String& v)
{
    AutoPtr<ICharSequence> key, value;
    CString::New(name, (ICharSequence**)&key);
    CString::New(v, (ICharSequence**)&value);
    return mAttribs->Put(key, value);
}

ECode BasicClientCookie::GetAttribute(
    /* [in] */ const String& name,
    /* [out] */ String* attr)
{
    VALIDATE_NOT_NULL(attr)
    *attr = String(NULL);
    AutoPtr<ICharSequence> cs;
    CString::New(name, (ICharSequence**)&cs);
    AutoPtr<IInterface> value;
    mAttribs->Get(cs, (IInterface**)&value);
    if (value) {
        ICharSequence::Probe(value)->ToString(attr);
    }
    return NOERROR;
}

ECode BasicClientCookie::ContainsAttribute(
    /* [in] */ const String& name,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ICharSequence> cs;
    CString::New(name, (ICharSequence**)&cs);
    AutoPtr<IInterface> value;
    mAttribs->Get(cs, (IInterface**)&value);
    *result = (value != NULL);
    return NOERROR;
}

ECode BasicClientCookie::Clone(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object)
    AutoPtr<BasicClientCookie> cookie = new BasicClientCookie();
    CloneImpl(cookie);
    *object = cookie->Probe(EIID_IInterface);
    REFCOUNT_ADD(*object)
    return NOERROR;
}

ECode BasicClientCookie::CloneImpl(
    /* [in] */ BasicClientCookie* cookie)
{
    cookie->mName = mName;
    cookie->mValue = mValue;
    cookie->mCookieComment = mCookieComment;
    cookie->mCookieDomain = mCookieDomain;
    cookie->mCookieExpiryDate = mCookieExpiryDate;
    cookie->mCookiePath = mCookiePath;
    cookie->mIsSecure = mIsSecure;
    cookie->mCookieVersion = mCookieVersion;
    return NOERROR;
}

ECode BasicClientCookie::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    StringBuilder buffer;
    buffer.Append("[version: ");
    buffer.Append(StringUtils::ToString(mCookieVersion));
    buffer.Append("]");
    buffer.Append("[name: ");
    buffer.Append(mName);
    buffer.Append("]");
    buffer.Append("[value: ");
    buffer.Append(mValue);
    buffer.Append("]");
    buffer.Append("[domain: ");
    buffer.Append(mCookieDomain);
    buffer.Append("]");
    buffer.Append("[path: ");
    buffer.Append(mCookiePath);
    buffer.Append("]");
    buffer.Append("[expiry: ");
    buffer.Append(mCookieExpiryDate);
    buffer.Append("]");
    *str = buffer.ToString();
    return NOERROR;
}

} // namespace Cookie
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org