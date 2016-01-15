
#include "FileHandler.h"
#include "CURL.h"
#include "CFileURLConnection.h"

using Elastos::Net::CURL;

namespace Libcore {
namespace Net {
namespace Url {

CAR_INTERFACE_IMPL(FileHandler, URLStreamHandler, IFileHandler)

ECode FileHandler::constructor()
{
    return NOERROR;
}

ECode FileHandler::OpenConnection(
    /* [in] */ IURL* u,
    /* [out] */ IURLConnection** urlConnection)
{
    return OpenConnection(u, NULL, urlConnection);
}

ECode FileHandler::OpenConnection(
    /* [in] */ IURL* u,
    /* [in] */ Elastos::Net::IProxy* proxy,
    /* [out] */ IURLConnection** urlConnection)
{
    VALIDATE_NOT_NULL(urlConnection)

    if (u == NULL) {
        // throw new IllegalArgumentException("url == null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    String host;
    u->GetHost(&host);
    if (host.IsNull() || host.IsEmpty() || host.EqualsIgnoreCase("localhost")) {
        AutoPtr<IFileURLConnection> furl;
        CFileURLConnection::New(u, (IFileURLConnection**)&furl);
        *urlConnection = IURLConnection::Probe(furl);
        REFCOUNT_ADD(*urlConnection)
        return NOERROR;
    }

    // If a hostname is specified try to get the resource using FTP
    String filename;
    AutoPtr<IURL> ftpURL;
    FAIL_RETURN(CURL::New(String("ftp"), host, (u->GetFile(&filename), filename), (IURL**)&ftpURL));
    AutoPtr<IURLConnection> outurl;
    *urlConnection = (proxy == NULL)
                    ? (ftpURL->OpenConnection((IURLConnection**)&outurl), outurl)
                    : (ftpURL->OpenConnection(proxy, (IURLConnection**)&outurl), outurl);
    REFCOUNT_ADD(*urlConnection)

    return NOERROR;
}

ECode FileHandler::ParseURL(
    /* [in] */ IURL* url,
    /* [in] */ const String& spec,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    if (end < start) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    String parseString = String("");
    if (start < end) {
        parseString = spec.Substring(start, end).Replace('\\', '/');
    }
    return URLStreamHandler::ParseURL(url, parseString, 0, parseString.GetLength());
}

} // namespace Url
} // namespace Net
} // namespace Libcore
