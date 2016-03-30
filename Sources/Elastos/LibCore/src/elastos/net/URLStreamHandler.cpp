
#include "URLStreamHandler.h"
#include "CURL.h"
#include "CURI.h"
#include "InetAddress.h"
#include "StringBuilder.h"
#include "StringUtils.h"
#include "Math.h"
#include "url/UrlUtils.h"

namespace Elastos {
namespace Net {

using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Core::Math;
using Libcore::Net::Url::UrlUtils;


CAR_INTERFACE_IMPL(URLStreamHandler, Object, IURLStreamHandler)

URLStreamHandler::URLStreamHandler()
{
}

URLStreamHandler::~URLStreamHandler()
{
}

ECode URLStreamHandler::OpenConnection(
    /* [in] */ IURL* u,
    /* [in] */ IProxy* proxy,
    /* [out] */ IURLConnection** urlConnection)
{
//    throw new UnsupportedOperationException();
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode URLStreamHandler::ParseURL(
    /* [in] */ IURL* url,
    /* [in] */ const String& spec,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    CURL* curl = (CURL*)url;
    if (this != curl->mStreamHandler) {
       return E_SECURITY_EXCEPTION;
    }

    if (end < start) {
        return E_STRING_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    Int32 fileStart;
    String authority;
    String userInfo;
    String host;
    Int32 port = -1;
    String path;
    String query;
    String ref;
    if (spec.RegionMatches(start, String("//"), 0, 2)) {
        // Parse the authority from the spec.
        Int32 authorityStart = start + 2;
        fileStart = UrlUtils::FindFirstOf(spec, String("/?#"), authorityStart, end);
        authority = spec.Substring(authorityStart, fileStart);
        Int32 userInfoEnd = UrlUtils::FindFirstOf(spec, String("@"), authorityStart, fileStart);
        Int32 hostStart;
        if (userInfoEnd != fileStart) {
            userInfo = spec.Substring(authorityStart, userInfoEnd);
            hostStart = userInfoEnd + 1;
        } else {
            userInfo = NULL;
            hostStart = authorityStart;
        }

        /*
         * Extract the host and port. The host may be an IPv6 address with
         * colons like "[::1]", in which case we look for the port delimiter
         * colon after the ']' character.
         */
        Int32 colonSearchFrom = hostStart;
        Int32 ipv6End = UrlUtils::FindFirstOf(spec, String("]"), hostStart, fileStart);
        if (ipv6End != fileStart) {
            if (UrlUtils::FindFirstOf(spec, String(":"), hostStart, ipv6End) == ipv6End) {
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
            colonSearchFrom = ipv6End;
        }
        Int32 hostEnd = UrlUtils::FindFirstOf(spec, String(":"), colonSearchFrom, fileStart);
        host = spec.Substring(hostStart, hostEnd);
        Int32 portStart = hostEnd + 1;
        if (portStart < fileStart) {

            Char32 firstPortChar = spec.GetChar(portStart);
            if (firstPortChar >= '0' && firstPortChar <= '9') {
                port = StringUtils::ParseInt32(spec.Substring(portStart, fileStart));
            }
            else {
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
                // throw new IllegalArgumentException("invalid port: " + port);
            }
        }
        path = NULL;
        query = NULL;
        ref = NULL;
    }
    else {
        // Get the authority from the context URL.
        fileStart = start;
        url->GetAuthority(&authority);
        url->GetUserInfo(&userInfo);
        url->GetHost(&host);
        if (host == NULL) {
            host = "";
        }
        url->GetPort(&port);
        url->GetPath(&path);
        url->GetQuery(&query);
        url->GetRef(&ref);
    }

    /*
     * Extract the path, query and fragment. Each part has its own leading
     * delimiter character. The query can contain slashes and the fragment
     * can contain slashes and question marks.
     *    / path ? query # fragment
     */
    Int32 pos = fileStart;
    Int32 nextPos;
    AutoPtr<ArrayOf<Char32> > char32Array = spec.GetChars();
    while (pos < end) {
        switch ((*char32Array)[pos]) {
        case '#':
            nextPos = end;
            ref = spec.Substring(pos + 1, nextPos);
            break;
        case '?':
            nextPos = UrlUtils::FindFirstOf(spec, String("#"), pos, end);
            query = spec.Substring(pos + 1, nextPos);
            ref = NULL;
            break;
        default:
            nextPos = UrlUtils::FindFirstOf(spec, String("?#"), pos, end);
            path = RelativePath(path, spec.Substring(pos, nextPos));
            query = NULL;
            ref = NULL;
            break;
        }
        pos = nextPos;
    }

    if (path.IsNull()) {
        path = "";
    }

    path = UrlUtils::AuthoritySafePath(authority, path);

    String protocol;
    url->GetProtocol(&protocol);
    SetURL(url, protocol, host, port, authority, userInfo, path, query, ref);
    return NOERROR;
}

ECode URLStreamHandler::SetURL(
    /* [in] */ IURL* url,
    /* [in] */ const String& protocol,
    /* [in] */ const String& host,
    /* [in] */ Int32 port,
    /* [in] */ const String& file,
    /* [in] */ const String& ref)
{
    CURL* curl = (CURL*)url;
    if (this != curl->mStreamHandler) {
       return E_SECURITY_EXCEPTION;
    }

    curl->Set(protocol, host, port, file, ref);
    return NOERROR;
}

ECode URLStreamHandler::SetURL(
    /* [in] */ IURL* url,
    /* [in] */ const String& protocol,
    /* [in] */ const String& host,
    /* [in] */ Int32 port,
    /* [in] */ const String& authority,
    /* [in] */ const String& userInfo,
    /* [in] */ const String& file,
    /* [in] */ const String& query,
    /* [in] */ const String& ref)
{
    CURL* curl = (CURL*)url;
    if (this != curl->mStreamHandler) {
       return E_SECURITY_EXCEPTION;
    }
    curl->Set(protocol, host, port, authority, userInfo, file, query, ref);
    return NOERROR;
}

ECode URLStreamHandler::ToExternalForm(
    /* [in] */ IURL* url,
    /* [in] */ Boolean escapeIllegalCharacters,
    /* [out] */ String* s)
{
    VALIDATE_NOT_NULL(s)

    StringBuilder answer;
    String protocol;
    url->GetProtocol(&protocol);
    answer += protocol;
    answer.AppendChar(':');
    String authority;
    url->GetAuthority(&authority);
    if (!authority.IsNull()) {
        answer += "//";
        if(escapeIllegalCharacters) {
            CURI::AUTHORITY_ENCODER->AppendPartiallyEncoded(answer, authority);
        }
        else{
            answer += authority;
        }
    }

    String fileAndQuery, ref;
    url->GetFile(&fileAndQuery);
    url->GetRef(&ref);
    if (!fileAndQuery.IsNull()) {
        if(escapeIllegalCharacters) {
            CURI::FILE_AND_QUERY_ENCODER->AppendPartiallyEncoded(answer, fileAndQuery);
        }
        else {
            answer += fileAndQuery;
        }
    }
    if (!ref.IsNull()) {
        answer.AppendChar('#');
        if (escapeIllegalCharacters) {
            CURI::ALL_LEGAL_ENCODER->AppendPartiallyEncoded(answer, ref);
        }
        else {
            answer += ref;
        }
    }
    answer.ToString(s);
    return NOERROR;
}

ECode URLStreamHandler::ToExternalForm(
    /* [in] */ IURL* url,
    /* [out] */ String* s)
{
    return ToExternalForm(url, FALSE, s);
}

ECode URLStreamHandler::Equals(
    /* [in] */ IURL* url1,
    /* [in] */ IURL* url2,
    /* [out] */ Boolean* isEquals)
{
    VALIDATE_NOT_NULL(isEquals)

    Boolean isSame;
    if (SameFile(url1, url2, &isSame), !isSame) {
        *isEquals = FALSE;
        return NOERROR;
    }
    String ref1, ref2, query1, query2;
    url1->GetRef(&ref1);
    url2->GetRef(&ref2);
    url1->GetQuery(&query1);
    url2->GetQuery(&query2);
    *isEquals = ref1.Equals(ref2)
            && query1.Equals(query2);
    return NOERROR;
}

ECode URLStreamHandler::GetDefaultPort(
    /* [out] */ Int32* portNum)
{
    VALIDATE_NOT_NULL(portNum)

    *portNum = -1;
    return NOERROR;
}

ECode URLStreamHandler::GetHostAddress(
    /* [in] */ IURL* url,
    /* [out] */ IInetAddress** hostAddress)
{
    VALIDATE_NOT_NULL(hostAddress)
//    try {
    String host;
    url->GetHost(&host);
    if (host.IsNullOrEmpty()) {
        *hostAddress = NULL;
        return NOERROR;
    }
    return InetAddress::GetByName(host, hostAddress);
//    } catch (UnknownHostException e) {
//        return null;
//    }
}

ECode URLStreamHandler::GetHashCode(
    /* [in] */ IURL* url,
    /* [out] */ Int32* code)
{
    VALIDATE_NOT_NULL(code)

    String s;
    ToExternalForm(url, &s);
    *code = s.GetHashCode();
    return NOERROR;
}

ECode URLStreamHandler::HostsEqual(
    /* [in] */ IURL* a,
    /* [in] */ IURL* b,
    /* [out] */ Boolean* isEqual)
{
    VALIDATE_NOT_NULL(isEqual)

    // URLs with the same case-insensitive host name have equal hosts
    String aHost, bHost;
    a->GetHost(&aHost);
    b->GetHost(&bHost);
    *isEqual = ((aHost == bHost) || (!aHost.IsNull() && aHost.EqualsIgnoreCase(bHost)));
    return NOERROR;
}

ECode URLStreamHandler::SameFile(
    /* [in] */ IURL* url1,
    /* [in] */ IURL* url2,
    /* [out] */ Boolean* isSame)
{
    VALIDATE_NOT_NULL(isSame)

    String protocol1, protocol2, file1, file2;
    url1->GetProtocol(&protocol1);
    url2->GetProtocol(&protocol2);
    url1->GetFile(&file1);
    url2->GetFile(&file2);

    Boolean isEqual;
    HostsEqual(url1, url2, &isEqual);

    Int32 port1, port2;
    url1->GetEffectivePort(&port1);
    url2->GetEffectivePort(&port2);

    *isSame = protocol1.Equals(protocol2)
            && file1.Equals(file2)
            && isEqual
            && port1 == port2;

    // return Objects.equal(url1.getProtocol(), url2.getProtocol())
    //         && Objects.equal(url1.getFile(), url2.getFile())
    //         && hostsEqual(url1, url2)
    //         && url1.getEffectivePort() == url2.getEffectivePort();
    return NOERROR;
}

String URLStreamHandler::RelativePath(
    /* [in] */ const String& base,
    /* [in] */ const String& path)
{
    if (path.StartWith("/")) {
        return UrlUtils::CanonicalizePath(path, TRUE);
    }
    else if (base != NULL) {
        String combined = base.Substring(0, base.LastIndexOf('/') + 1) + path;
        return UrlUtils::CanonicalizePath(combined, TRUE);
    }
    else {
        return path;
    }
}

} // namespace Net
} // namespace Elastos
