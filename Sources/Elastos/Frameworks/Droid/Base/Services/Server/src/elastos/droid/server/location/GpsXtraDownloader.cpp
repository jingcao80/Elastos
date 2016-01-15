
#include "location/GpsXtraDownloader.h"

using Elastos::Droid::Net::CProxy;
using Elastos::Droid::Net::IProxy;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Location {

const String GpsXtraDownloader::TAG("GpsXtraDownloader");

const Boolean GpsXtraDownloader::DEBUG = FALSE;

GpsXtraDownloader::GpsXtraDownloader(
    /* [in] */ IContext* context,
    /* [in] */ IProperties* properties)
{
    mContext = context;

    // read XTRA servers from the Properties object
    Int32 count = 0;
    String server1;
    String server2;
    String server3;

    properties->GetProperty("XTRA_SERVER_1", &server1);
    properties->GetProperty("XTRA_SERVER_2", &server2);
    properties->GetProperty("XTRA_SERVER_3", &server3);

    if (server1.GetLength() != 0) count++;
    if (server2.GetLength() != 0) count++;
    if (server3.GetLength() != 0) count++;

    if (count == 0) {
//        Log.e(TAG, "No XTRA servers were specified in the GPS configuration");
        return;
    } else {
        mXtraServers = ArrayOf<String>::Alloc(count);
        count = 0;
        if (server1.GetLength() != 0) mXtraServers[count++] = server1;
        if (server2.GetLength() != 0) mXtraServers[count++] = server2;
        if (server3.GetLength() != 0) mXtraServers[count++] = server3;

        // randomize first server
        //Random random = new Random();
        CRandom::New((IRandom**)&random);
        //mNextServerIndex = random.nextInt(count);
        random->NextInt(count, &mNextServerIndex);
    }
}

AutoPtr<ArrayOf<Byte> > GpsXtraDownloader::DownloadXtraData()
{
    AutoPtr<IProxy> proxy;
    CProxy::AcquireSingleton((IProxy**)&proxy);
    String proxyHost;
    Int32 proxyPort;
    proxy->GetHost(mContext, &proxyHost);
    proxy->GetPort(mContext, &proxyPort);
    Boolean useProxy = (!proxyHost.IsNUllOrEmpty() && proxyPort != -1);
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
}

AutoPtr<ArrayOf<Byte> > GpsXtraDownloader::DoDownload(
    /* [in] */ const String& url,
    /* [in] */ Boolean isProxySet,
    /* [in] */ const String& proxyHost,
    /* [in] */ Int32 proxyPort)
{
////    if (DEBUG) Log.d(TAG, "Downloading XTRA data from " + url);
//    AutoPtr<IAndroidHttpClientHelper> atcHelper;
//    AutoPtr<IAndroidHttpClient> client;
//
//    CAndroidHttpClientHelper::AcquireSingleton((IAndroidHttpClientHelper**)&atcHelper)
//
//    //try {
//        atcHelper->NewInstance(String("Android"), (IAndroidHttpClient**)&client);
//        HttpUriRequest req = new HttpGet(url);
//
//        if (isProxySet) {
//            AutoPtr<IHttpHost> proxy;
//            CHttpHost::New(proxyHost, proxyPort, (IHttpHost**)&proxy);
//            ConnRouteParams->SetDefaultProxy(req.getParams(), proxy);
//        }
//
//        req->AddHeader(
//                "Accept",
//                "*/*, application/vnd.wap.mms-message, application/vnd.wap.sic");
//
//        req->AddHeader(
//                "x-wap-profile",
//                "http://www.openmobilealliance.org/tech/profiles/UAPROF/ccppschema-20021212#");
//
//        AutoPtr<IHttpResponse> response;
//        client->Execute(req, (IHttpResponse**)&response);
//        AutoPtr<IStatusLine> status;
//        response->GetStatusLine(&status);
//        Int32 statusCode = 0;
//        status->GetStatusCode(&statusCode);
//        if (statusCode != 200) { // HTTP 200 is success.
////            if (DEBUG) Log.d(TAG, "HTTP error: " + status.getReasonPhrase());
//            return NULL;
//        }
//
//        AutoPtr<IHttpEntity> entity;
//        response->GetEntity((IHttpEntity**)&entity);
//        AutoPtr<ArrayOf<Byte> > body;
//        if (entity != NULL) {
//            //try {
//                Int32 length;
//                entity->GetContentLength(&length);
//                if (length > 0) {
//                    body = ArrayOf<Byte>::Alloc(length);
////                    DataInputStream dis = new DataInputStream(entity.getContent());
//                    //try {
//                        dis->ReadFully(body);
//                    //} finally {
//                    //    try {
//                    //        dis.close();
//                    //    } catch (IOException e) {
//                    //        Log.e(TAG, "Unexpected IOException.", e);
//                    //    }
//                    //}
//            //    }
//            //} finally {
//            //    if (entity != null) {
//            //        entity.consumeContent();
//            //    }
//            //}
//        }
//        return body;
//    //} catch (Exception e) {
//    //    if (DEBUG) Log.d(TAG, "error " + e);
//    //} finally {
//    //    if (client != null) {
//    //        client.close();
//    //    }
//    //}
    PFL_EX("TODO: Apache Http Net")
    return NULL;
}

} // namespace Location
} // namespace Server
} // namespace Droid
} // namespace Elastos
