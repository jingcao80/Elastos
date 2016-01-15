
#ifndef __ELASTOS_NET_URLCONNECTION_H__
#define __ELASTOS_NET_URLCONNECTION_H__

#include "Elastos.CoreLibrary.Net.h"
#include "elastos/core/Object.h"
#include <elastos/utility/etl/HashMap.h>

using Elastos::IO::IInputStream;
using Elastos::IO::IOutputStream;
using Elastos::Security::IPermission;
using Elastos::Utility::IMap;
using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Net {

class URLConnection
    : public Object
    , public IURLConnection
{
public:
    class DefaultContentHandler
        : public Object
        , public IContentHandler
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI GetContent(
            /* [in] */ IURLConnection* uConn,
            /* [out] */ IInterface** obj);

        CARAPI GetContent(
            /* [in] */ IURLConnection* uConn,
            /* [in] */ ArrayOf<InterfaceID> * types,
            /* [out] */ IInterface** obj);
    };

public:
    CAR_INTERFACE_DECL()

    URLConnection();

    virtual ~URLConnection();

    CARAPI constructor(
        /* [in] */ IURL* url);

    virtual CARAPI Connect() = 0;

    CARAPI GetAllowUserInteraction(
        /* [out] */ Boolean* value);

    CARAPI GetContent(
        /* [out] */ IInterface** content);

    CARAPI GetContent(
        /* [in] */ ArrayOf<InterfaceID> * types,
        /* [out] */ IInterface** obj);

    CARAPI GetContentEncoding(
        /* [out] */ String* encoding);

    CARAPI GetContentLength(
        /* [out] */ Int32* length);

    CARAPI GetContentType(
        /* [out] */ String* type);

    CARAPI GetDate(
        /* [out] */ Int64* date);

    static CARAPI_(Boolean) GetDefaultAllowUserInteraction();

    static CARAPI_(String) GetDefaultRequestProperty(
        /* [in] */ const String& field);

    CARAPI GetDefaultUseCaches(
        /* [out] */ Boolean* value);

    CARAPI GetDoInput(
        /* [out] */ Boolean* allowed);

    CARAPI GetDoOutput(
        /* [out] */ Boolean* allowed);

    CARAPI GetExpiration(
        /* [out] */ Int64* expiration);

    static CARAPI_(AutoPtr<IFileNameMap>) GetFileNameMap();

    CARAPI GetHeaderField(
        /* [in] */ const String& key,
        /* [out] */ String* value);

    CARAPI GetHeaderField(
        /* [in] */ Int32 pos,
        /* [out] */ String* value);

    CARAPI GetHeaderFields(
        /* [out] */ IMap** headerFields);

    CARAPI GetRequestProperties(
        /* [out] */ IMap** properties);

    CARAPI AddRequestProperty(
        /* [in] */ const String& field,
        /* [in] */ const String& newValue);

    CARAPI GetHeaderFieldByKey(
        /* [in] */ const String& key,
        /* [out] */ String* value);

    CARAPI GetHeaderFieldDate(
        /* [in] */ const String& field,
        /* [in] */ Int64 defaultValue,
        /* [out] */ Int64* value);

    CARAPI GetHeaderFieldInt32(
        /* [in] */ const String& field,
        /* [in] */ Int32 defaultValue,
        /* [out] */ Int32* value);

    CARAPI GetHeaderFieldKey(
        /* [in] */ Int32 posn,
        /* [out] */ String* key);

    CARAPI GetIfModifiedSince(
        /* [out] */ Int64* time);

    CARAPI GetInputStream(
        /* [out] */ IInputStream** is);

    CARAPI GetLastModified(
        /* [out] */ Int64* value);

    CARAPI GetOutputStream(
        /* [out] */ IOutputStream** os);

    CARAPI GetPermission(
        /* [out] */ IPermission** perm);

    CARAPI GetRequestProperty(
        /* [in] */ const String& field,
        /* [out] */ String* property);

    CARAPI GetURL(
        /* [out] */ IURL** url);

    CARAPI GetUseCaches(
        /* [out] */ Boolean* allowed);

    static CARAPI_(String) GuessContentTypeFromName(
        /* [in] */ const String& url);

    static CARAPI_(String) GuessContentTypeFromStream(
        /* [in] */ IInputStream* is);

    CARAPI SetAllowUserInteraction(
        /* [in] */ Boolean newValue);

    static CARAPI SetContentHandlerFactory(
        /* [in] */ IContentHandlerFactory* contentFactory);

    static CARAPI_(void) SetDefaultAllowUserInteraction(
        /* [in] */ Boolean allows);

    static CARAPI_(void) SetDefaultRequestProperty(
        /* [in] */ const String& field,
        /* [in] */ const String& value);

    CARAPI SetDefaultUseCaches(
        /* [in] */ Boolean newValue);

    CARAPI SetDoInput(
        /* [in] */ Boolean newValue);

    CARAPI SetDoOutput(
        /* [in] */ Boolean newValue);

    static CARAPI_(void) SetFileNameMap(
        /* [in] */ IFileNameMap* map);

    CARAPI SetIfModifiedSince(
        /* [in] */ Int64 newValue);

    CARAPI SetRequestProperty(
        /* [in] */ const String& field,
        /* [in] */ const String& newValue);

    CARAPI SetUseCaches(
        /* [in] */ Boolean newValue);

    CARAPI SetConnectTimeout(
        /* [in] */ Int32 timeout);

    CARAPI GetConnectTimeout(
        /* [out] */ Int32* timeout);

    CARAPI SetReadTimeout(
        /* [in] */ Int32 timeout);

    CARAPI GetReadTimeout(
        /* [out] */ Int32* timeout);

private:
    CARAPI GetContentHandler(
        /* [in] */ const String& type,
        /* [out] */ IContentHandler** contentHandler);

    CARAPI CheckNotConnected();

    CARAPI_(String) ParseTypeString(
        /* [in] */ const String& typeString);

public:
    AutoPtr<IContentHandler> mDefaultHandler;

    // static Hashtable<String, Object> contentHandlers = new Hashtable<String, Object>();
    static HashMap<String, AutoPtr<IContentHandler> > sContentHandlers;

protected:
    AutoPtr<IURL> mUrl;

    Int64 mIfModifiedSince;

    Boolean mUseCaches;// = defaultUseCaches;

    Boolean mConnected;

    Boolean mDoOutput;

    Boolean mDoInput;

    Boolean mAllowUserInteraction;

private:
    String mContentType;

    static Boolean sDefaultAllowUserInteraction;

    static Boolean sDefaultUseCaches;

    static Object sLock;

    Int64 mLastModified;

    static AutoPtr<IContentHandlerFactory> sContentHandlerFactory;

    Int32 mReadTimeout;

    Int32 mConnectTimeout;

    static AutoPtr<IFileNameMap> sFileNameMap;
};

} // namespace Net
} // namespace Elastos

#endif //__ELASTOS_NET_URLCONNECTION_H__
