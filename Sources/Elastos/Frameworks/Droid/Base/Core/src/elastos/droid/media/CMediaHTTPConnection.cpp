#include "elastos/droid/media/CMediaHTTPConnection.h"
#include "elastos/droid/net/NetworkUtils.h"
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

#include <binder/MemoryDealer.h>
#include <media/stagefright/foundation/ABase.h>
#include <media/stagefright/foundation/ADebug.h>
#include <utils/RefBase.h>

using Elastos::Droid::Net::NetworkUtils;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringUtils;
using Elastos::IO::CBufferedInputStream;
using Elastos::IO::IBufferedInputStream;
using Elastos::Net::CCookieHandlerHelper;
using Elastos::Net::CCookieManager;
using Elastos::Net::CURL;
using Elastos::Net::ICookieHandler;
using Elastos::Net::ICookieHandlerHelper;
using Elastos::Net::ICookieManager;
using Elastos::Net::IInetAddress;
using Elastos::Net::IURLConnection;
using Elastos::Utility::CHashMap;
using Elastos::Utility::IHashMap;
using Elastos::Utility::IMapEntry;
using Elastos::Utility::ISet;
using Elastos::Utility::Logging::Logger;

using android::sp;

namespace Elastos {
namespace Droid {
namespace Media {

//==============================================================================
//  JMediaHTTPConnection
//==============================================================================

struct JMediaHTTPConnection : public android::RefBase {
    enum {
        kBufferSize = 32768,
    };

    JMediaHTTPConnection(CMediaHTTPConnection* obj);

    sp<android::IMemory> getIMemory();

    AutoPtr<ArrayOf<Byte> > getByteArrayObj();

protected:
    virtual ~JMediaHTTPConnection();

private:
    CMediaHTTPConnection* mObject;
    AutoPtr<ArrayOf<Byte> > mByteArrayObj;

    sp<android::MemoryDealer> mDealer;
    sp<android::IMemory> mMemory;

    DISALLOW_EVIL_CONSTRUCTORS(JMediaHTTPConnection);
};

JMediaHTTPConnection::JMediaHTTPConnection(CMediaHTTPConnection* obj)
    : mObject(obj),
      mByteArrayObj(NULL)
{
    mDealer = new android::MemoryDealer(kBufferSize, "MediaHTTPConnection");
    mMemory = mDealer->allocate(kBufferSize);

    mByteArrayObj = ArrayOf<Byte>::Alloc(JMediaHTTPConnection::kBufferSize);
}

JMediaHTTPConnection::~JMediaHTTPConnection()
{
    mByteArrayObj = NULL;
    mObject = NULL;
}

sp<android::IMemory> JMediaHTTPConnection::getIMemory()
{
    return mMemory;
}

AutoPtr<ArrayOf<Byte> > JMediaHTTPConnection::getByteArrayObj()
{
    return mByteArrayObj;
}

//==============================================================================
//  CMediaHTTPConnection
//==============================================================================

const String CMediaHTTPConnection::TAG("MediaHTTPConnection");
Boolean CMediaHTTPConnection::VERBOSE = FALSE;
Int32 CMediaHTTPConnection::HTTP_TEMP_REDIRECT = 307;
Int32 CMediaHTTPConnection::MAX_REDIRECTS = 20;

CAR_INTERFACE_IMPL_2(CMediaHTTPConnection, Object, IIMediaHTTPConnection, IBinder)

CAR_OBJECT_IMPL(CMediaHTTPConnection)

CMediaHTTPConnection::CMediaHTTPConnection()
    : mCurrentOffset(-1)
    , mTotalSize(-1)
    , mAllowCrossDomainRedirect(TRUE)
    , mAllowCrossProtocolRedirect(TRUE)
{
}

CMediaHTTPConnection::~CMediaHTTPConnection()
{
    NativeFinalize();
}

ECode CMediaHTTPConnection::constructor()
{
    AutoPtr<ICookieHandlerHelper> helper;
    CCookieHandlerHelper::AcquireSingleton((ICookieHandlerHelper**)&helper);
    AutoPtr<ICookieHandler> ch;
    helper->GetDefault((ICookieHandler**)&ch);
    if (ch == NULL) {
        AutoPtr<ICookieManager> cm;
        CCookieManager::New((ICookieManager**)&cm);
        helper->SetDefault(ICookieHandler::Probe(cm));
    }

    NativeSetup();
    return NOERROR;
}

ECode CMediaHTTPConnection::Connect(
    /* [in] */ const String& uri,
    /* [in] */ const String& headers,
    /* [out] */ IBinder** result)
{
    if (VERBOSE) {
        Logger::D(TAG, "connect: uri=%s, headers=%s",
                uri.string(), headers.string());
    }

    // try {
    Disconnect();
    mAllowCrossDomainRedirect = TRUE;
    CURL::New(uri, (IURL**)&mURL);
    mHeaders = ConvertHeaderStringToMap(headers);
    // } catch (MalformedURLException e) {
    //     return null;
    // }

    *result = NativeGetIMemory();
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CMediaHTTPConnection::Disconnect()
{
    TeardownConnection();
    mHeaders = NULL;
    mURL = NULL;
    return NOERROR;
}

ECode CMediaHTTPConnection::ReadAt(
    /* [in] */ Int64 offset,
    /* [in] */ Int32 size,
    /* [out] */ Int32* result)
{
    *result = NativeReadAt(offset, size);
    return NOERROR;
}

ECode CMediaHTTPConnection::GetSize(
    /* [out] */ Int64* result)
{
    if (mConnection == NULL) {
        // try {
        SeekTo(0);
        // } catch (IOException e) {
        //     return -1;
        // }
    }

    *result = mTotalSize;
    return NOERROR;
}

ECode CMediaHTTPConnection::GetMIMEType(
    /* [out] */ String* result)
{
    if (mConnection == NULL) {
        // try {
        SeekTo(0);
        // } catch (IOException e) {
        //     return "application/octet-stream";
        // }
    }

    return IURLConnection::Probe(mConnection)->GetContentType(result);
}

ECode CMediaHTTPConnection::GetUri(
    /* [out] */ String* result)
{
    return mURL->ToString(result);
}

ECode CMediaHTTPConnection::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = "CMediaHTTPConnection";
    return NOERROR;
}

Boolean CMediaHTTPConnection::ParseBoolean(
    /* [in] */ const String& val)
{
    // try {
    return StringUtils::ParseInt64(val) != 0;
    // } catch (NumberFormatException e) {
    //     return "true".equalsIgnoreCase(val) ||
    //         "yes".equalsIgnoreCase(val);
    // }
}

Boolean CMediaHTTPConnection::FilterOutInternalHeaders(
    /* [in] */ const String& key,
    /* [in] */ const String& val)
{
    if (key.EqualsIgnoreCase("android-allow-cross-domain-redirect")) {
        mAllowCrossDomainRedirect = ParseBoolean(val);
        // cross-protocol redirects are also controlled by this flag
        mAllowCrossProtocolRedirect = mAllowCrossDomainRedirect;
    }
    else {
        return FALSE;
    }
    return TRUE;
}

AutoPtr<IMap> CMediaHTTPConnection::ConvertHeaderStringToMap(
    /* [in] */ const String& headers)
{
    AutoPtr<IHashMap> map;
    CHashMap::New((IHashMap**)&map);

    AutoPtr<ArrayOf<String> > pairs;
    StringUtils::Split(headers, String("\r\n"), (ArrayOf<String>**)&pairs);
    for (Int32 i = 0; i < pairs->GetLength(); i++) {
        String pair = (*pairs)[i];
        Int32 colonPos = pair.IndexOf(":");
        if (colonPos >= 0) {
            String key = pair.Substring(0, colonPos);
            String val = pair.Substring(colonPos + 1);

            if (!FilterOutInternalHeaders(key, val)) {
                AutoPtr<ICharSequence> cs1, cs2;
                CString::New(key, (ICharSequence**)&cs1);
                CString::New(val, (ICharSequence**)&cs2);
                map->Put(cs1, cs2);
            }
        }
    }
    AutoPtr<IMap> m = IMap::Probe(map);
    return m;
}

void CMediaHTTPConnection::TeardownConnection()
{
    if (mConnection != NULL) {
        mInputStream = NULL;

        mConnection->Disconnect();
        mConnection = NULL;

        mCurrentOffset = -1;
    }
}

Boolean CMediaHTTPConnection::IsLocalHost(
    /* [in] */ IURL* url)
{
    if (url == NULL) {
        return FALSE;
    }

    String host;
    url->GetHost(&host);

    if (host == NULL) {
        return FALSE;
    }

    // try {
    if (host.EqualsIgnoreCase("localhost")) {
        return TRUE;
    }
    AutoPtr<IInetAddress> addr;
    NetworkUtils::NumericToInetAddress(host, (IInetAddress**)&addr);
    Boolean b;
    if ((addr->IsLoopbackAddress(&b), b)) {
        return TRUE;
    }
    // } catch (IllegalArgumentException iex) {
    // }
    return FALSE;
}

ECode CMediaHTTPConnection::SeekTo(
    /* [in] */ Int64 offset)
{
    TeardownConnection();

    // try {
    Int32 response;
    Int32 redirectCount = 0;

    AutoPtr<IURL> url = mURL;

    // do not use any proxy for localhost (127.0.0.1)
    Boolean noProxy = IsLocalHost(url);

    while (TRUE) {
        if (noProxy) {
            AutoPtr<Elastos::Net::IProxyHelper> proxyhelper;
            Elastos::Net::CProxyHelper::AcquireSingleton((Elastos::Net::IProxyHelper**)&proxyhelper);
            AutoPtr<Elastos::Net::IProxy> proxyTemp;
            proxyhelper->GetNO_PROXY((Elastos::Net::IProxy**)&proxyTemp);

            AutoPtr<IURLConnection> urlConnection;
            url->OpenConnection(proxyTemp, (IURLConnection**)&urlConnection);
            mConnection = IHttpURLConnection::Probe(urlConnection);
        }
        else {
            AutoPtr<IURLConnection> urlConnection;
            url->OpenConnection((IURLConnection**)&urlConnection);
            mConnection = IHttpURLConnection::Probe(urlConnection);
        }

        // handle redirects ourselves if we do not allow cross-domain redirect
        mConnection->SetInstanceFollowRedirects(mAllowCrossDomainRedirect);

        if (mHeaders != NULL) {
            AutoPtr<ISet> entrySet;
            mHeaders->GetEntrySet((ISet**)&entrySet);
            AutoPtr<ArrayOf<IInterface*> > array;
            entrySet->ToArray((ArrayOf<IInterface*>**)&array);

            for (Int32 i = 0; i < array->GetLength(); ++i) {
                AutoPtr<IMapEntry> entry;
                entry = IMapEntry::Probe((*array)[i]);
                AutoPtr<IInterface> iKey;
                entry->GetKey((IInterface**)&iKey);
                String key;
                ICharSequence::Probe(iKey)->ToString(&key);
                AutoPtr<IInterface> iValue;
                entry->GetValue((IInterface**)&iValue);
                String value;
                ICharSequence::Probe(iValue)->ToString(&value);

                IURLConnection::Probe(mConnection)->SetRequestProperty(
                        key, value);
            }
        }

        if (offset > 0) {
            IURLConnection::Probe(mConnection)->SetRequestProperty(
                    String("Range"), String("bytes=") + offset + "-");
        }

        mConnection->GetResponseCode(&response);
        if (response != IHttpURLConnection::HTTP_MULT_CHOICE &&
                response != IHttpURLConnection::HTTP_MOVED_PERM &&
                response != IHttpURLConnection::HTTP_MOVED_TEMP &&
                response != IHttpURLConnection::HTTP_SEE_OTHER &&
                response != HTTP_TEMP_REDIRECT) {
            // not a redirect, or redirect handled by HttpURLConnection
            break;
        }

        if (++redirectCount > MAX_REDIRECTS) {
            // throw new NoRouteToHostException("Too many redirects: " + redirectCount);
            return E_NO_ROUTE_TO_HOST_EXCEPTION;
        }

        String method;
        mConnection->GetRequestMethod(&method);
        if (response == HTTP_TEMP_REDIRECT &&
                !method.Equals("GET") && !method.Equals("HEAD")) {
            // "If the 307 status code is received in response to a
            // request other than GET or HEAD, the user agent MUST NOT
            // automatically redirect the request"
            // throw new NoRouteToHostException("Invalid redirect");
        }
        String location;
        IURLConnection::Probe(mConnection)->GetHeaderField(String("Location"), &location);
        if (location == NULL) {
            // throw new NoRouteToHostException("Invalid redirect");
            return E_NO_ROUTE_TO_HOST_EXCEPTION;
        }
        CURL::New(mURL /* TRICKY: don't use url! */, location, (IURL**)&url);
        String protocol;
        url->GetProtocol(&protocol);
        if (!protocol.Equals("https") &&
                !protocol.Equals("http")) {
            // throw new NoRouteToHostException("Unsupported protocol redirect");
            return E_NO_ROUTE_TO_HOST_EXCEPTION;
        }
        String p;
        mURL->GetProtocol(&p);
        Boolean sameProtocol = p.Equals(protocol);
        if (!mAllowCrossProtocolRedirect && !sameProtocol) {
            // throw new NoRouteToHostException("Cross-protocol redirects are disallowed");
            return E_NO_ROUTE_TO_HOST_EXCEPTION;
        }
        String host1, host2;
        mURL->GetHost(&host1);
        url->GetHost(&host2);
        Boolean sameHost = host1.Equals(host2);
        if (!mAllowCrossDomainRedirect && !sameHost) {
            // throw new NoRouteToHostException("Cross-domain redirects are disallowed");
            return E_NO_ROUTE_TO_HOST_EXCEPTION;
        }

        if (response != HTTP_TEMP_REDIRECT) {
            // update effective URL, unless it is a Temporary Redirect
            mURL = url;
        }
    }

    if (mAllowCrossDomainRedirect) {
        // remember the current, potentially redirected URL if redirects
        // were handled by HttpURLConnection
        IURLConnection::Probe(mConnection)->GetURL((IURL**)&mURL);
    }

    if (response == IHttpURLConnection::HTTP_PARTIAL) {
        // Partial content, we cannot just use getContentLength
        // because what we want is not just the length of the range
        // returned but the size of the full content if available.

        String contentRange;
        IURLConnection::Probe(mConnection)->GetHeaderField(String("Content-Range"), &contentRange);

        mTotalSize = -1;
        if (contentRange != NULL) {
            // format is "bytes xxx-yyy/zzz
            // where "zzz" is the total number of bytes of the
            // content or '*' if unknown.

            Int32 lastSlashPos = contentRange.LastIndexOf('/');
            if (lastSlashPos >= 0) {
                String total =
                    contentRange.Substring(lastSlashPos + 1);

                // try {
                mTotalSize = StringUtils::ParseInt64(total);
                // } catch (NumberFormatException e) {
                // }
            }
        }
    }
    else if (response != IHttpURLConnection::HTTP_OK) {
        // throw new IOException();
        return E_IO_EXCEPTION;
    }
    else {
        Int32 length;
        IURLConnection::Probe(mConnection)->GetContentLength(&length);
        mTotalSize = length;
    }

    if (offset > 0 && response != IHttpURLConnection::HTTP_PARTIAL) {
        // Some servers simply ignore "Range" requests and serve
        // data from the start of the content.
        // throw new IOException();
        return E_IO_EXCEPTION;
    }

    AutoPtr<IInputStream> is;
    IURLConnection::Probe(mConnection)->GetInputStream((IInputStream**)&is);
    AutoPtr<IBufferedInputStream> bis;
    CBufferedInputStream::New(is, (IBufferedInputStream**)&bis);
    mInputStream = IInputStream::Probe(bis);

    mCurrentOffset = offset;
    // } catch (IOException e) {
    //     mTotalSize = -1;
    //     mInputStream = null;
    //     mConnection = null;
    //     mCurrentOffset = -1;

    //     throw e;
    // }
    return NOERROR;
}

Int32 CMediaHTTPConnection::ReadAt(
    /* [in] */ Int64 offset,
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 size)
{
// TODO: Need StrictMode
    // StrictMode.ThreadPolicy policy =
    //     new StrictMode.ThreadPolicy.Builder().permitAll().build();

    // StrictMode.setThreadPolicy(policy);

    // try {
    if (offset != mCurrentOffset) {
        SeekTo(offset);
    }

    Int32 n;
    mInputStream->Read(data, 0, size, &n);

    if (n == -1) {
        // InputStream signals EOS using a -1 result, our semantics
        // are to return a 0-length read.
        n = 0;
    }

    mCurrentOffset += n;

    if (VERBOSE) {
        Logger::D(TAG, "readAt %d / %d => %d", offset, size, n);
    }

    return n;
    // } catch (NoRouteToHostException e) {
    //     Log.w(TAG, "readAt " + offset + " / " + size + " => " + e);
    //     return MEDIA_ERROR_UNSUPPORTED;
    // } catch (IOException e) {
    //     if (VERBOSE) {
    //         Log.d(TAG, "readAt " + offset + " / " + size + " => -1");
    //     }
    //     return -1;
    // } catch (Exception e) {
    //     if (VERBOSE) {
    //         Log.d(TAG, "unknown exception " + e);
    //         Log.d(TAG, "readAt " + offset + " / " + size + " => -1");
    //     }
    //     return -1;
    // }
}

void CMediaHTTPConnection::NativeInit()
{
    return;
}

void CMediaHTTPConnection::NativeSetup()
{
    sp<JMediaHTTPConnection> conn = new JMediaHTTPConnection(this);

    mNativeContext = (Int64)conn.get();
}

void CMediaHTTPConnection::NativeFinalize()
{
    mNativeContext = 0;
}

AutoPtr<IBinder> CMediaHTTPConnection::NativeGetIMemory()
{
    sp<JMediaHTTPConnection> conn = (JMediaHTTPConnection*)mNativeContext;

//TODO: javaObjectForIBinder
    // return javaObjectForIBinder(env, conn->getIMemory()->asBinder());
    return NULL;
}

Int32 CMediaHTTPConnection::NativeReadAt(
    /* [in] */ Int64 offset,
    /* [in] */ Int32 size)
{
    sp<JMediaHTTPConnection> conn = (JMediaHTTPConnection*)mNativeContext;

    if (size > JMediaHTTPConnection::kBufferSize) {
        size = JMediaHTTPConnection::kBufferSize;
    }

    AutoPtr<ArrayOf<Byte> > byteArrayObj = conn->getByteArrayObj();

    Int32 n = ReadAt(offset, byteArrayObj, size);

    if (n > 0) {
//TODO: GetByteArrayRegion
        // env->GetByteArrayRegion(
        //         byteArrayObj, 0, n, (jbyte *)conn->getIMemory()->pointer());
    }

    return n;
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
