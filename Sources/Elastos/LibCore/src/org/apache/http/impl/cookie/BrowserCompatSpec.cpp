
#include "org/apache/http/impl/cookie/BrowserCompatSpec.h"
#include "org/apache/http/impl/cookie/DateUtils.h"
#include "org/apache/http/impl/cookie/BasicPathHandler.h"
#include "org/apache/http/impl/cookie/BasicDomainHandler.h"
#include "org/apache/http/impl/cookie/BasicSecureHandler.h"
#include "org/apache/http/impl/cookie/BasicMaxAgeHandler.h"
#include "org/apache/http/impl/cookie/BasicCommentHandler.h"
#include "org/apache/http/impl/cookie/BasicExpiresHandler.h"
#include "org/apache/http/impl/cookie/NetscapeDraftHeaderParser.h"
#include "org/apache/http/message/CParserCursor.h"
#include "org/apache/http/message/CBufferedHeader.h"
#include "org/apache/http/utility/CCharArrayBuffer.h"
#include "elastos/utility/CArrayList.h"
#include "elastos/utility/CLocaleHelper.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Utility::ILocale;
using Elastos::Utility::ILocaleHelper;
using Elastos::Utility::CLocaleHelper;
using Elastos::Utility::IIterator;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::IHeader;
using Org::Apache::Http::IHeaderElement;
using Org::Apache::Http::IFormattedHeader;
using Org::Apache::Http::Cookie::ICookieAttributeHandler;
using Org::Apache::Http::Cookie::IClientCookie;
using Org::Apache::Http::Cookie::ISM;
using Org::Apache::Http::Message::IParserCursor;
using Org::Apache::Http::Message::CParserCursor;
using Org::Apache::Http::Message::CBufferedHeader;
using Org::Apache::Http::Utility::ICharArrayBuffer;
using Org::Apache::Http::Utility::CCharArrayBuffer;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Cookie {

static AutoPtr< ArrayOf<String> > InitDatePatterns()
{
    AutoPtr< ArrayOf<String> > patterns = ArrayOf<String>::Alloc(14);
    (*patterns)[0] = DateUtils::PATTERN_RFC1123;
    (*patterns)[1] = DateUtils::PATTERN_RFC1036;
    (*patterns)[2] = DateUtils::PATTERN_ASCTIME;
    (*patterns)[3] = String("EEE, dd-MMM-yyyy HH:mm:ss z");
    (*patterns)[4] = String("EEE, dd-MMM-yyyy HH:mm:ss z");
    (*patterns)[5] = String("EEE, dd MMM yy HH:mm:ss z");
    (*patterns)[6] = String("EEE dd-MMM-yyyy HH:mm:ss z");
    (*patterns)[7] = String("EEE dd MMM yyyy HH:mm:ss z");
    (*patterns)[8] = String("EEE dd-MMM-yyyy HH-mm-ss z");
    (*patterns)[9] = String("EEE dd-MMM-yy HH:mm:ss z");
    (*patterns)[10] = String("EEE dd MMM yy HH:mm:ss z");
    (*patterns)[11] = String("EEE,dd-MMM-yy HH:mm:ss z");
    (*patterns)[12] = String("EEE,dd-MMM-yyyy HH:mm:ss z");
    (*patterns)[12] = String("EEE, dd-MM-yyyy HH:mm:ss z");
    return patterns;
}
const AutoPtr< ArrayOf<String> > BrowserCompatSpec::DATE_PATTERNS = InitDatePatterns();

BrowserCompatSpec::BrowserCompatSpec(
    /* [in] */ ArrayOf<String>* datepatterns)
{
    Init(datepatterns);
}

BrowserCompatSpec::BrowserCompatSpec()
{
    Init(NULL);
}

void BrowserCompatSpec::Init(
    /* [in] */ ArrayOf<String>* datepatterns)
{
    if (datepatterns != NULL) {
        mDatepatterns = datepatterns->Clone();
    }
    else {
        mDatepatterns = DATE_PATTERNS;
    }
    AutoPtr<ICookieAttributeHandler> pathHandler = (ICookieAttributeHandler*)new BasicPathHandler();
    AutoPtr<ICookieAttributeHandler> domainHandler = (ICookieAttributeHandler*)new BasicDomainHandler();
    AutoPtr<ICookieAttributeHandler> maxAgeHandler = (ICookieAttributeHandler*)new BasicMaxAgeHandler();
    AutoPtr<ICookieAttributeHandler> secureHandler = (ICookieAttributeHandler*)new BasicSecureHandler();
    AutoPtr<ICookieAttributeHandler> commentHandler = (ICookieAttributeHandler*)new BasicCommentHandler();
    AutoPtr<ICookieAttributeHandler> expiresHandler = (ICookieAttributeHandler*)new BasicExpiresHandler(mDatepatterns);
    RegisterAttribHandler(IClientCookie::PATH_ATTR, pathHandler);
    RegisterAttribHandler(IClientCookie::DOMAIN_ATTR, domainHandler);
    RegisterAttribHandler(IClientCookie::MAX_AGE_ATTR, maxAgeHandler);
    RegisterAttribHandler(IClientCookie::SECURE_ATTR, secureHandler);
    RegisterAttribHandler(IClientCookie::COMMENT_ATTR, commentHandler);
    RegisterAttribHandler(IClientCookie::EXPIRES_ATTR, expiresHandler);
}

ECode BrowserCompatSpec::Parse(
    /* [in] */ IHeader* header,
    /* [in] */ ICookieOrigin* origin,
    /* [out] */ IList** cookies)
{
    VALIDATE_NOT_NULL(cookies)
    *cookies = NULL;

    if (header == NULL) {
        Logger::E("BrowserCompatSpec", "Header may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (origin == NULL) {
        Logger::E("BrowserCompatSpec", "Cookie origin may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    String headervalue;
    header->GetValue(&headervalue);
    // AutoPtr<ILocaleHelper> helper;
    // CLocaleHelper::AcquireSingleton((ILocaleHelper**)&helper);
    // AutoPtr<ILocale> ENGLISH;
    // helper->GetENGLISH((ILocale**)&ENGLISH);
    Boolean isNetscapeCookie = FALSE;
    Int32 i1 = headervalue.ToLowerCase(/*ENGLISH*/).IndexOf("expires=");
    if (i1 != -1) {
        i1 += String("expires=").GetLength();
        Int32 i2 = headervalue.IndexOf(';', i1);
        if (i2 == -1) {
            i2 = headervalue.GetLength();
        }
        // try {
        AutoPtr<IDate> date;
        ECode ec = DateUtils::ParseDate(headervalue.Substring(i1, i2), mDatepatterns, (IDate**)&date);
        if (SUCCEEDED(ec)) isNetscapeCookie = TRUE;
        // } catch (DateParseException e) {
        //     // Does not look like a valid expiry date
        // }
    }
    AutoPtr< ArrayOf<IHeaderElement*> > elems;
    if (isNetscapeCookie) {
        AutoPtr<NetscapeDraftHeaderParser> parser = NetscapeDraftHeaderParser::DEFAULT;
        AutoPtr<ICharArrayBuffer> buffer;
        AutoPtr<IParserCursor> cursor;
        AutoPtr<IFormattedHeader> formattedH = IFormattedHeader::Probe(header);
        if (formattedH != NULL) {
            formattedH->GetBuffer((ICharArrayBuffer**)&buffer);
            Int32 pos;
            formattedH->GetValuePos(&pos);
            Int32 len;
            buffer->GetLength(&len);
            CParserCursor::New(pos, len, (IParserCursor**)&cursor);
        }
        else {
            String s;
            header->GetValue(&s);
            if (s.IsNull()) {
                Logger::E("BrowserCompatSpec", "Header value is null");
                return E_MALFORMED_COOKIE_EXCEPTION;
            }
            CCharArrayBuffer::New(s.GetLength(), (ICharArrayBuffer**)&buffer);
            buffer->Append(s);
            Int32 len;
            buffer->GetLength(&len);
            CParserCursor::New(0, len, (IParserCursor**)&cursor);
        }
        AutoPtr<IHeaderElement> ele;
        parser->ParseHeader(buffer, cursor, (IHeaderElement**)&ele);
        elems = ArrayOf<IHeaderElement*>::Alloc(1);
        elems->Set(0, ele);
    }
    else {
        header->GetElements((ArrayOf<IHeaderElement*>**)&elems);
    }
    return Parse(elems, origin, cookies);
}

ECode BrowserCompatSpec::FormatCookies(
    /* [in] */ IList* cookies,
    /* [out] */ IList** _headers)
{
    VALIDATE_NOT_NULL(_headers)
    *_headers = NULL;

    if (cookies == NULL) {
        Logger::E("BrowserCompatSpec", "List of cookie may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Boolean isEmpty;
    if (cookies->IsEmpty(&isEmpty), isEmpty) {
        Logger::E("BrowserCompatSpec", "List of cookie may not be empty");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Int32 size;
    cookies->GetSize(&size);
    AutoPtr<ICharArrayBuffer> buffer;
    CCharArrayBuffer::New(20 * size, (ICharArrayBuffer**)&buffer);
    buffer->Append(ISM::COOKIE);
    buffer->Append(String(": "));
    AutoPtr<IIterator> it;
    cookies->GetIterator((IIterator**)&it);
    Int32 i = 0;
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> value;
        it->GetNext((IInterface**)&value);
        AutoPtr<ICookie> cookie = ICookie::Probe(value);
        if (i > 0) {
            buffer->Append(String("; "));
        }
        String name;
        cookie->GetName(&name);
        buffer->Append(name);
        buffer->Append(String("="));
        String s;
        cookie->GetValue(&s);
        if (!s.IsNull()) {
            buffer->Append(s);
        }
        i++;
    }
    AutoPtr<IList> headers;
    CArrayList::New(1, (IList**)&headers);
    AutoPtr<IHeader> header;
    CBufferedHeader::New(buffer, (IHeader**)&header);
    headers->Add(header);
    *_headers = headers;
    REFCOUNT_ADD(*_headers)
    return NOERROR;
}

ECode BrowserCompatSpec::GetVersion(
    /* [out] */ Int32* version)
{
    VALIDATE_NOT_NULL(version)
    *version = 0;
    return NOERROR;
}

ECode BrowserCompatSpec::GetVersionHeader(
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