
#include "org/apache/http/impl/cookie/CookieSpecBase.h"
#include "org/apache/http/impl/cookie/BasicClientCookie.h"
#include "elastos/utility/CArrayList.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Core::IArrayOf;
using Elastos::Utility::IArrayList;
using Elastos::Utility::CArrayList;
using Elastos::Utility::ICollection;
using Elastos::Utility::IIterator;
using Elastos::Utility::ILocale;
using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::INameValuePair;
using Org::Apache::Http::IHeaderElement;
using Org::Apache::Http::Cookie::ICookieAttributeHandler;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Cookie {

String CookieSpecBase::GetDefaultPath(
    /* [in] */ ICookieOrigin* origin)
{
    String defaultPath;
    origin->GetPath(&defaultPath);
    Int32 lastSlashIndex = defaultPath.LastIndexOf('/');
    if (lastSlashIndex >= 0) {
        if (lastSlashIndex == 0) {
            //Do not remove the very first slash
            lastSlashIndex = 1;
        }
        defaultPath = defaultPath.Substring(0, lastSlashIndex);
    }
    return defaultPath;
}

String CookieSpecBase::GetDefaultDomain(
    /* [in] */ ICookieOrigin* origin)
{
    String host;
    origin->GetHost(&host);
    return host;
}

ECode CookieSpecBase::Parse(
    /* [in] */ ArrayOf<IHeaderElement*>* elems,
    /* [in] */ ICookieOrigin* origin,
    /* [out] */ IList** _cookies)
{
    VALIDATE_NOT_NULL(_cookies)
    *_cookies = NULL;

    AutoPtr<IArrayList> cookies;
    CArrayList::New(elems->GetLength(), (IArrayList**)&cookies);
    for (Int32 i = 0; i < elems->GetLength(); ++i) {
        AutoPtr<IHeaderElement> headerelement = (*elems)[i];
        String name, value;
        headerelement->GetName(&name);
        headerelement->GetValue(&value);
        if (name.IsNull() || name.GetLength() == 0) {
            Logger::E("CookieSpecBase", "Cookie name may not be empty");
            return E_MALFORMED_COOKIE_EXCEPTION;
        }

        AutoPtr<BasicClientCookie> cookie = new BasicClientCookie(name, value);
        cookie->SetPath(GetDefaultPath(origin));
        cookie->SetDomain(GetDefaultDomain(origin));

        // cycle through the parameters
        AutoPtr< ArrayOf<INameValuePair*> > attribs;
        headerelement->GetParameters((ArrayOf<INameValuePair*>**)&attribs);
        for (Int32 j = attribs->GetLength() - 1; j >= 0; j--) {
            AutoPtr<INameValuePair> attrib = (*attribs)[i];
            String n;
            attrib->GetName(&n);
            String s = n.ToLowerCase(/*ILocale::ENGLISH*/);

            String v;
            attrib->GetValue(&v);
            cookie->SetAttribute(s, v);

            AutoPtr<ICookieAttributeHandler> handler;
            FindAttribHandler(s, (ICookieAttributeHandler**)&handler);
            if (handler != NULL) {
                handler->Parse(cookie, v);
            }
        }
        cookies->Add(cookie->Probe(EIID_IInterface));
    }
    *_cookies = IList::Probe(cookies);
    REFCOUNT_ADD(*_cookies)
    return NOERROR;
}

ECode CookieSpecBase::Validate(
    /* [in] */ ICookie* cookie,
    /* [in] */ ICookieOrigin* origin)
{
    if (cookie == NULL) {
        Logger::E("CookieSpecBase", "Cookie may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (origin == NULL) {
        Logger::E("CookieSpecBase", "Cookie origin may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<ICollection> coll;
    GetAttribHandlers((ICollection**)&coll);
    AutoPtr<IIterator> it;
    coll->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> value;
        it->GetNext((IInterface**)&value);
        AutoPtr<ICookieAttributeHandler> handler = ICookieAttributeHandler::Probe(value);
        handler->Validate(cookie, origin);
    }
    return NOERROR;
}

ECode CookieSpecBase::Match(
    /* [in] */ ICookie* cookie,
    /* [in] */ ICookieOrigin* origin,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    if (cookie == NULL) {
        Logger::E("CookieSpecBase", "Cookie may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (origin == NULL) {
        Logger::E("CookieSpecBase", "Cookie origin may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<ICollection> coll;
    GetAttribHandlers((ICollection**)&coll);
    AutoPtr<IIterator> it;
    coll->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> value;
        it->GetNext((IInterface**)&value);
        AutoPtr<ICookieAttributeHandler> handler = ICookieAttributeHandler::Probe(value);
        Boolean isMatch;
        if (handler->Match(cookie, origin, &isMatch), !isMatch) {
            *result = FALSE;
            return NOERROR;
        }
    }
    *result = TRUE;
    return NOERROR;
}

} // namespace Cookie
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org