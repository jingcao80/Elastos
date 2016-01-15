
#include "org/apache/http/impl/DefaultConnectionReuseStrategy.h"
#include "org/apache/http/CHttpVersion.h"
#include "org/apache/http/message/CBasicTokenIterator.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::IHttpEntity;
using Org::Apache::Http::CHttpVersion;
using Org::Apache::Http::IProtocolVersion;
using Org::Apache::Http::IStatusLine;
using Org::Apache::Http::IHttpMessage;
using Org::Apache::Http::Message::CBasicTokenIterator;
using Org::Apache::Http::Protocol::IHTTP;
using Org::Apache::Http::Protocol::IExecutionContext;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {

DefaultConnectionReuseStrategy::DefaultConnectionReuseStrategy()
{}

CAR_INTERFACE_IMPL(DefaultConnectionReuseStrategy, Object, IConnectionReuseStrategy)

ECode DefaultConnectionReuseStrategy::KeepAlive(
    /* [in] */ IHttpResponse* response,
    /* [in] */ IHttpContext* context,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    if (response == NULL) {
        Logger::E("DefaultConnectionReuseStrategy", "HTTP response may not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (context == NULL) {
        Logger::E("DefaultConnectionReuseStrategy", "HTTP context may not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IInterface> o;
    context->GetAttribute(IExecutionContext::HTTP_CONNECTION, (IInterface**)&o);
    AutoPtr<IHttpConnection> conn = IHttpConnection::Probe(o);

    Boolean isOpen;
    if (conn != NULL && (conn->IsOpen(&isOpen), !isOpen)) {
        *result = FALSE;
        return NOERROR;
    }
    // do NOT check for stale connection, that is an expensive operation

    // Check for a self-terminating entity. If the end of the entity will
    // be indicated by closing the connection, there is no keep-alive.
    AutoPtr<IHttpEntity> entity;
    response->GetEntity((IHttpEntity**)&entity);
    AutoPtr<IStatusLine> statusLine;
    response->GetStatusLine((IStatusLine**)&statusLine);
    AutoPtr<IProtocolVersion> ver;
    statusLine->GetProtocolVersion((IProtocolVersion**)&ver);
    if (entity != NULL) {
        Int64 len;
        if (entity->GetContentLength(&len), len < 0) {
            Boolean isChunked, lessEquals;
            if ((entity->IsChunked(&isChunked), !isChunked) ||
                    (ver->LessEquals(IProtocolVersion::Probe(CHttpVersion::HTTP_1_0), &lessEquals), lessEquals)) {
                // if the content length is not known and is not chunk
                // encoded, the connection cannot be reused
                *result = FALSE;
                return NOERROR;
            }
        }
    }

    // Check for the "Connection" header. If that is absent, check for
    // the "Proxy-Connection" header. The latter is an unspecified and
    // broken but unfortunately common extension of HTTP.
    AutoPtr<IHeaderIterator> hit;
    IHttpMessage::Probe(response)->GetHeaderIterator(IHTTP::CONN_DIRECTIVE, (IHeaderIterator**)&hit);
    Boolean hasNext;
    if (hit->HasNext(&hasNext), !hasNext) {
        hit = NULL;
        IHttpMessage::Probe(response)->GetHeaderIterator(String("Proxy-Connection"), (IHeaderIterator**)&hit);
    }

    // Experimental usage of the "Connection" header in HTTP/1.0 is
    // documented in RFC 2068, section 19.7.1. A token "keep-alive" is
    // used to indicate that the connection should be persistent.
    // Note that the final specification of HTTP/1.1 in RFC 2616 does not
    // include this information. Neither is the "Connection" header
    // mentioned in RFC 1945, which informally describes HTTP/1.0.
    //
    // RFC 2616 specifies "close" as the only connection token with a
    // specific meaning: it disables persistent connections.
    //
    // The "Proxy-Connection" header is not formally specified anywhere,
    // but is commonly used to carry one token, "close" or "keep-alive".
    // The "Connection" header, on the other hand, is defined as a
    // sequence of tokens, where each token is a header name, and the
    // token "close" has the above-mentioned additional meaning.
    //
    // To get through this mess, we treat the "Proxy-Connection" header
    // in exactly the same way as the "Connection" header, but only if
    // the latter is missing. We scan the sequence of tokens for both
    // "close" and "keep-alive". As "close" is specified by RFC 2068,
    // it takes precedence and indicates a non-persistent connection.
    // If there is no "close" but a "keep-alive", we take the hint.

    if (hit->HasNext(&hasNext), hasNext) {
        // try {
        AutoPtr<ITokenIterator> ti;
        CreateTokenIterator(hit, (ITokenIterator**)&ti);
        Boolean keepalive = FALSE;
        while (ti->HasNext(&hasNext), hasNext) {
            String token;
            ti->NextToken(&token);
            if (IHTTP::CONN_CLOSE.EqualsIgnoreCase(token)) {
                *result = FALSE;
                return NOERROR;
            }
            else if (IHTTP::CONN_KEEP_ALIVE.EqualsIgnoreCase(token)) {
                // continue the loop, there may be a "close" afterwards
                keepalive = TRUE;
            }
        }
        if (keepalive) {
            *result = TRUE;
            return NOERROR;
        }
        // neither "close" nor "keep-alive", use default policy

        // } catch (ParseException px) {
        //     // invalid connection header means no persistent connection
        //     // we don't have logging in HttpCore, so the exception is lost
        //     return false;
        // }
    }

    // default since HTTP/1.1 is persistent, before it was non-persistent
    Boolean lessEquals;
    ver->LessEquals(IProtocolVersion::Probe(CHttpVersion::HTTP_1_0), &lessEquals);
    *result = !lessEquals;
    return NOERROR;
}

ECode DefaultConnectionReuseStrategy::CreateTokenIterator(
    /* [in] */ IHeaderIterator* hit,
    /* [out] */ ITokenIterator** tit)
{
    VALIDATE_NOT_NULL(tit)
    return CBasicTokenIterator::New(hit, tit);
}

} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org