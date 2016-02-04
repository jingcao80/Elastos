#include "Elastos.Droid.Net.h"
#include "Elastos.CoreLibrary.Apache.h"
#include "Elastos.CoreLibrary.IO.h"
// #include "elastos/droid/net/http/AndroidHttpClient.h"
#include "elastos/droid/server/location/GpsXtraDownloader.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Net::CProxy;
// using Elastos::Droid::Net::IProxy;
// using Elastos::Droid::Net::Http::AndroidHttpClient;
// using Elastos::Droid::Net::Http::IAndroidHttpClient;
using Elastos::Droid::Text::TextUtils;
using Elastos::IO::ICloseable;
using Elastos::IO::IDataInput;
using Elastos::IO::IDataInputStream;
using Elastos::IO::IInputStream;
using Elastos::Utility::CRandom;
using Elastos::Utility::IRandom;
using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::CHttpHost;
using Org::Apache::Http::IHttpEntity;
using Org::Apache::Http::IHttpHost;
using Org::Apache::Http::IHttpMessage;
using Org::Apache::Http::IHttpResponse;
using Org::Apache::Http::IStatusLine;
using Org::Apache::Http::Client::Methods::CHttpGet;
using Org::Apache::Http::Client::Methods::IHttpUriRequest;
using Org::Apache::Http::Conn::Params::CConnRouteParams;
using Org::Apache::Http::Conn::Params::IConnRouteParams;
using Org::Apache::Http::Params::IHttpParams;


namespace Elastos {
namespace Droid {
namespace Server {
namespace Location {

const String GpsXtraDownloader::TAG("GpsXtraDownloader");
const Boolean GpsXtraDownloader::DEBUG = Logger::IsLoggable(TAG, Logger::___DEBUG);
const String GpsXtraDownloader::DEFAULT_USER_AGENT("Android");

GpsXtraDownloader::GpsXtraDownloader(
    /* [in] */ IContext* context,
    /* [in] */ IProperties* properties)
    : mContext(context)
{
    // read XTRA servers from the Properties object
    Int32 count = 0;
    String server1;
    String server2;
    String server3;

    properties->GetProperty(String("XTRA_SERVER_1"), &server1);
    properties->GetProperty(String("XTRA_SERVER_2"), &server2);
    properties->GetProperty(String("XTRA_SERVER_3"), &server3);

    if (!server1.IsNull()) count++;
    if (!server2.IsNull()) count++;
    if (!server3.IsNull()) count++;

    // Set User Agent from properties, if possible.
    String agent;
    properties->GetProperty(String("XTRA_USER_AGENT"), &agent);
    if (TextUtils::IsEmpty(agent)) {
        mUserAgent = DEFAULT_USER_AGENT;
    }
    else {
        mUserAgent = agent;
    }

    if (count == 0) {
        Logger::E(TAG, "No XTRA servers were specified in the GPS configuration");
        mXtraServers = NULL;
        return;
    }
    else {
        mXtraServers = ArrayOf<String>::Alloc(count);
        count = 0;
        if (!server1.IsNull()) (*mXtraServers)[count++] = server1;
        if (!server2.IsNull()) (*mXtraServers)[count++] = server2;
        if (!server3.IsNull()) (*mXtraServers)[count++] = server3;

        // randomize first server
        AutoPtr<IRandom> random;
        CRandom::New((IRandom**)&random);
        random->NextInt32(count, &mNextServerIndex);
    }
}

AutoPtr<ArrayOf<Byte> > GpsXtraDownloader::DownloadXtraData()
{
#if 0
    AutoPtr<IProxy> proxy;
    CProxy::New((IProxy**)&proxy);
    String proxyHost;
    Int32 proxyPort;
    proxy->GetHost(mContext, &proxyHost);
    proxy->GetPort(mContext, &proxyPort);
    Boolean useProxy = (!proxyHost.IsNull() && proxyPort != -1);
    AutoPtr<ArrayOf<Byte> > result;
    Int32 startIndex = mNextServerIndex;

    if (mXtraServers == NULL) {
        return NULL;
    }

    // load balance our requests among the available servers
    while (result == NULL) {
        result = DoDownload((*mXtraServers)[mNextServerIndex], useProxy, proxyHost, proxyPort);

        // increment mNextServerIndex and wrap around if necessary
        mNextServerIndex++;
        if (mNextServerIndex == mXtraServers->GetLength()) {
            mNextServerIndex = 0;
        }
        // break if we have tried all the servers
        if (mNextServerIndex == startIndex) break;
    }

    return result;
#endif
    return NULL;
}

AutoPtr<ArrayOf<Byte> > GpsXtraDownloader::DoDownload(
    /* [in] */ const String& url,
    /* [in] */ Boolean isProxySet,
    /* [in] */ const String& proxyHost,
    /* [in] */ Int32 proxyPort)
{
#if 0 //TODO AndroidHttpClient
    if (DEBUG) Logger::D(TAG, "Downloading XTRA data from %s", url.string());

    AutoPtr<IAndroidHttpClient> client;
    // try {
    if (DEBUG) Logger::D(TAG, "XTRA user agent: %s", mUserAgent.string());
    client = AndroidHttpClient::NewInstance(mUserAgent);
    AutoPtr<IHttpUriRequest> req;
    CHttpGet::New(url, (IHttpUriRequest**)&req);

    AutoPtr<IHttpMessage> msg = IHttpMessage::Probe(req);
    if (isProxySet) {
        AutoPtr<IHttpHost> proxy;
        CHttpHost::New(proxyHost, proxyPort, (IHttpHost**)&proxy);
        AutoPtr<IConnRouteParams> crp;
        CConnRouteParams::AcquireSingleton((IConnRouteParams**)&crp);

        AutoPtr<IHttpParams> params;
        msg->GetParams((IHttpParams**)&params);
        crp->SetDefaultProxy(params, proxy);
    }

    msg->AddHeader(
        String("Accept"),
        String("*/*, application/vnd.wap.mms-message, application/vnd.wap.sic"));

    msg->AddHeader(
        String("x-wap-profile"),
        String("http://www.openmobilealliance.org/tech/profiles/UAPROF/ccppschema-20021212#"));

    AutoPtr<IHttpResponse> response;
    client->Execute(req, (IHttpResponse**)&response);
    AutoPtr<IStatusLine> status;
    response->GetStatusLine((IStatusLine**)&status);
    Int32 code;
    status->GetStatusCode(&code);
    if (code != 200) { // HTTP 200 is success.
        String phrase;
        status->GetReasonPhrase(&phrase);
        if (DEBUG) Logger::D(TAG, "HTTP error: %s", phrase.string());
        return NULL;
    }

    AutoPtr<IHttpEntity> entity;
    response->GetEntity((IHttpEntity**)&entity);
    AutoPtr<ArrayOf<Byte> > body;
    if (entity != NULL) {
        Int64 len;
        entity->GetContentLength(&len);
        if (len > 0) {
            body = ArrayOf<Byte>::Alloc((Int32)len);
            AutoPtr<IInputStream> is;
            entity->GetContent((IInputStream**)&is);
            AutoPtr<IDataInputStream> dis = IDataInputStream::Probe(is);
            IDataInput::Probe(dis)->ReadFully(body);
            ECode ec = ICloseable::Probe(dis)->Close();
            if (FAILED(ec)) {
                Logger::E(TAG, "Unexpected IOException.%08x", ec);
            }
        }
        if (entity != NULL) {
            entity->ConsumeContent();
        }
    }
    return body;
    // } catch (Exception e) {
    //     if (DEBUG) Log.d(TAG, "error " + e);
    // } finally {
    //     if (client != null) {
    //         client.close();
    //     }
    // }
    // return null;
#endif
    return NULL;
}

} // namespace Location
} // namespace Server
} // namespace Droid
} // namespace Elastos
