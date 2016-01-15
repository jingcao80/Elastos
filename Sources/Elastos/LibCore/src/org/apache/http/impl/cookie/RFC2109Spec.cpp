
#include "org/apache/http/impl/cookie/RFC2109Spec.h"
#include "org/apache/http/impl/cookie/BasicPathHandler.h"
#include "org/apache/http/impl/cookie/RFC2109VersionHandler.h"
#include "org/apache/http/impl/cookie/RFC2109DomainHandler.h"
#include "org/apache/http/impl/cookie/BasicSecureHandler.h"
#include "org/apache/http/impl/cookie/BasicMaxAgeHandler.h"
#include "org/apache/http/impl/cookie/BasicCommentHandler.h"
#include "org/apache/http/impl/cookie/BasicExpiresHandler.h"
#include "org/apache/http/impl/cookie/DateUtils.h"
#include "org/apache/http/message/CParserCursor.h"
#include "org/apache/http/message/CBufferedHeader.h"
#include "org/apache/http/utility/CCharArrayBuffer.h"
#include "elastos/core/Math.h"
#include "elastos/core/StringUtils.h"
#include "elastos/utility/CCollections.h"
#include "elastos/utility/CArrayList.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Core::Math;
using Elastos::Core::StringUtils;
using Elastos::Utility::IArrayList;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CCollections;
using Elastos::Utility::ICollections;
using Elastos::Utility::ICollection;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::IHeaderElement;
using Org::Apache::Http::Cookie::IClientCookie;
using Org::Apache::Http::Cookie::ISM;
using Org::Apache::Http::Message::IParserCursor;
using Org::Apache::Http::Message::CParserCursor;
using Org::Apache::Http::Message::CBufferedHeader;
using Org::Apache::Http::Utility::CCharArrayBuffer;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Cookie {

static AutoPtr<CCookiePathComparator> InitComparator()
{
    AutoPtr<CCookiePathComparator> comparator;
    CCookiePathComparator::NewByFriend((CCookiePathComparator**)&comparator);
    return comparator;
}
const AutoPtr<CCookiePathComparator> RFC2109Spec::PATH_COMPARATOR = InitComparator();

static AutoPtr< ArrayOf<String> > InitPatterns()
{
    AutoPtr< ArrayOf<String> > patterns = ArrayOf<String>::Alloc(3);
    (*patterns)[0] = DateUtils::PATTERN_RFC1123;
    (*patterns)[1] = DateUtils::PATTERN_RFC1036;
    (*patterns)[2] = DateUtils::PATTERN_ASCTIME;
    return patterns;
}
const AutoPtr< ArrayOf<String> > RFC2109Spec::DATE_PATTERNS = InitPatterns();

RFC2109Spec::RFC2109Spec(
    /* [in] */ ArrayOf<String>* datepatterns,
    /* [in] */ Boolean oneHeader)
    : CookieSpecBase()
    , mOneHeader(FALSE)
{
    Init(datepatterns, oneHeader);
}

RFC2109Spec::RFC2109Spec()
    : CookieSpecBase()
    , mOneHeader(FALSE)
{
    Init(NULL, FALSE);
}

void RFC2109Spec::Init(
    /* [in] */ ArrayOf<String>* datepatterns,
    /* [in] */ Boolean oneHeader)
{
    if (datepatterns != NULL) {
        mDatepatterns = datepatterns->Clone();
    }
    else {
        mDatepatterns = DATE_PATTERNS;
    }
    mOneHeader = oneHeader;
    AutoPtr<ICookieAttributeHandler> versionHandler = (ICookieAttributeHandler*)new RFC2109VersionHandler();
    AutoPtr<ICookieAttributeHandler> pathHandler = (ICookieAttributeHandler*)new BasicPathHandler();
    AutoPtr<ICookieAttributeHandler> domainHandler = (ICookieAttributeHandler*)new RFC2109DomainHandler();
    AutoPtr<ICookieAttributeHandler> maxAgeHandler = (ICookieAttributeHandler*)new BasicMaxAgeHandler();
    AutoPtr<ICookieAttributeHandler> secureHandler = (ICookieAttributeHandler*)new BasicSecureHandler();
    AutoPtr<ICookieAttributeHandler> commentHandler = (ICookieAttributeHandler*)new BasicCommentHandler();
    AutoPtr<ICookieAttributeHandler> expiresHandler = (ICookieAttributeHandler*)new BasicExpiresHandler(mDatepatterns);
    RegisterAttribHandler(IClientCookie::VERSION_ATTR, versionHandler);
    RegisterAttribHandler(IClientCookie::PATH_ATTR, pathHandler);
    RegisterAttribHandler(IClientCookie::DOMAIN_ATTR, domainHandler);
    RegisterAttribHandler(IClientCookie::MAX_AGE_ATTR, maxAgeHandler);
    RegisterAttribHandler(IClientCookie::SECURE_ATTR, secureHandler);
    RegisterAttribHandler(IClientCookie::COMMENT_ATTR, commentHandler);
    RegisterAttribHandler(IClientCookie::EXPIRES_ATTR, expiresHandler);
}

ECode RFC2109Spec::Parse(
    /* [in] */ IHeader* header,
    /* [in] */ ICookieOrigin* origin,
    /* [out] */ IList** cookies)
{
    VALIDATE_NOT_NULL(cookies)
    *cookies = NULL;

    if (header == NULL) {
        Logger::E("RFC2109Spec", "Header may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (origin == NULL) {
        Logger::E("RFC2109Spec", "Cookie origin may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr< ArrayOf<IHeaderElement*> > elems;
    header->GetElements((ArrayOf<IHeaderElement*>**)&elems);
    return Parse(elems, origin, cookies);
}

ECode RFC2109Spec::Validate(
    /* [in] */ ICookie* cookie,
    /* [in] */ ICookieOrigin* origin)
{
    if (cookie == NULL) {
        Logger::E("RFC2109Spec", "Cookie may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    String name;
    cookie->GetName(&name);
    if (name.IndexOf(' ') != -1) {
        Logger::E("RFC2109Spec", "Cookie name may not contain blanks");
        return E_MALFORMED_COOKIE_EXCEPTION;
    }
    if (name.StartWith("$")) {
        Logger::E("RFC2109Spec", "Cookie name may not start with $");
        return E_MALFORMED_COOKIE_EXCEPTION;
    }
    return CookieSpecBase::Validate(cookie, origin);
}

ECode RFC2109Spec::FormatCookies(
    /* [in] */ IList* cookies,
    /* [out] */ IList** headers)
{
    VALIDATE_NOT_NULL(headers)
    *headers = NULL;

    if (cookies == NULL) {
        Logger::E("RFC2109Spec", "List of cookie may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Boolean isEmpty;
    if (cookies->IsEmpty(&isEmpty), isEmpty) {
        Logger::E("RFC2109Spec", "List of cookie may not be empty");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IArrayList> newCookies;
    Int32 size;
    if (cookies->GetSize(&size), size > 1) {
        // Create a mutable copy and sort the copy.
        CArrayList::New(ICollection::Probe(cookies), (IArrayList**)&newCookies);
        AutoPtr<ICollections> collections;
        CCollections::AcquireSingleton((ICollections**)&collections);
        collections->Sort(IList::Probe(newCookies), IComparator::Probe(PATH_COMPARATOR));
    }
    if (mOneHeader) {
        *headers = DoFormatOneHeader(IList::Probe(newCookies));
    }
    else {
        *headers = DoFormatManyHeaders(IList::Probe(newCookies));
    }
    REFCOUNT_ADD(*headers)
    return NOERROR;
}

AutoPtr<IList> RFC2109Spec::DoFormatOneHeader(
    /* [in] */ IList* cookies)
{
    Int32 version = Elastos::Core::Math::INT32_MAX_VALUE;
    // Pick the lowest common denominator
    AutoPtr<IIterator> it;
    cookies->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> value;
        it->GetNext((IInterface**)&value);
        AutoPtr<ICookie> cookie = ICookie::Probe(value);
        Int32 ver;
        if (cookie->GetVersion(&ver), ver < version) {
            version = ver;
        }
    }
    Int32 size;
    cookies->GetSize(&size);
    AutoPtr<ICharArrayBuffer> buffer;
    CCharArrayBuffer::New(40 * size, (ICharArrayBuffer**)&buffer);
    buffer->Append(ISM::COOKIE);
    buffer->Append(String(": "));
    buffer->Append(String("$Version="));
    buffer->Append(StringUtils::ToString(version));
    it = NULL;
    cookies->GetIterator((IIterator**)&it);
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> value;
        it->GetNext((IInterface**)&value);
        AutoPtr<ICookie> cookie = ICookie::Probe(value);
        buffer->Append(String("; "));
        FormatCookieAsVer(buffer, cookie, version);
    }

    AutoPtr<IArrayList> headers;
    CArrayList::New(1, (IArrayList**)&headers);
    AutoPtr<IFormattedHeader> header;
    CBufferedHeader::New(buffer, (IFormattedHeader**)&header);
    headers->Add(header);
    return IList::Probe(headers);
}

AutoPtr<IList> RFC2109Spec::DoFormatManyHeaders(
    /* [in] */ IList* cookies)
{
    Int32 size;
    cookies->GetSize(&size);
    AutoPtr<IArrayList> headers;
    CArrayList::New(size, (IArrayList**)&headers);
    AutoPtr<IIterator> it;
    cookies->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> value;
        it->GetNext((IInterface**)&value);
        AutoPtr<ICookie> cookie = ICookie::Probe(value);
        Int32 version;
        cookie->GetVersion(&version);
        AutoPtr<ICharArrayBuffer> buffer;
        CCharArrayBuffer::New(40, (ICharArrayBuffer**)&buffer);
        buffer->Append(String("Cookie: "));
        buffer->Append(String("$Version="));
        buffer->Append(StringUtils::ToString(version));
        buffer->Append(String("; "));
        FormatCookieAsVer(buffer, cookie, version);
        AutoPtr<IFormattedHeader> header;
        CBufferedHeader::New(buffer, (IFormattedHeader**)&header);
        IList::Probe(headers)->Add(header);
    }
    return IList::Probe(headers);
}

void RFC2109Spec::FormatParamAsVer(
    /* [in] */ ICharArrayBuffer* buffer,
    /* [in] */ const String& name,
    /* [in] */ const String& value,
    /* [in] */ Int32 version)
{
    buffer->Append(name);
    buffer->Append(String("="));
    if (!value.IsNull()) {
        if (version > 0) {
            buffer->Append('\"');
            buffer->Append(value);
            buffer->Append('\"');
        }
        else {
            buffer->Append(value);
        }
    }
}

void RFC2109Spec::FormatCookieAsVer(
    /* [in] */ ICharArrayBuffer* buffer,
    /* [in] */ ICookie* cookie,
    /* [in] */ Int32 version)
{
    String name, value;
    cookie->GetName(&name);
    cookie->GetValue(&value);
    FormatParamAsVer(buffer, name, value, version);
    String path;
    if (cookie->GetPath(&path), !path.IsNull()) {
        AutoPtr<IClientCookie> clientCookie = IClientCookie::Probe(cookie);
        if (clientCookie != NULL) {
            Boolean contains;
            if (clientCookie->ContainsAttribute(IClientCookie::PATH_ATTR, &contains), contains) {
                buffer->Append(String("; "));
                FormatParamAsVer(buffer, String("$Path"), path, version);
            }
        }
    }
    String domain;
    if (cookie->GetDomain(&domain), !domain.IsNull()) {
        AutoPtr<IClientCookie> clientCookie = IClientCookie::Probe(cookie);
        if (clientCookie != NULL) {
            Boolean contains;
            if (clientCookie->ContainsAttribute(IClientCookie::DOMAIN_ATTR, &contains), contains) {
                buffer->Append(String("; "));
                FormatParamAsVer(buffer, String("$Domain"), domain, version);
            }
        }
    }
}

ECode RFC2109Spec::GetVersion(
    /* [out] */ Int32* version)
{
    VALIDATE_NOT_NULL(version)
    *version = 1;
    return NOERROR;
}

ECode RFC2109Spec::GetVersionHeader(
    /* [out] */ IHeader** header)
{
    VALIDATE_NOT_NULL(header)
    *header = NULL;
    return NOERROR;
}

} // namespace Cookie
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org