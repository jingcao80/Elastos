
#ifndef __ELASTOS_NET_CURL_H__
#define __ELASTOS_NET_CURL_H__

#include "_Elastos_Net_CURL.h"
#include "elastos/core/Object.h"
#include <elastos/utility/etl/HashMap.h>

using Elastos::Core::Object;
using Elastos::Core::IComparable;
using Elastos::IO::IInputStream;
using Elastos::IO::ISerializable;
using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Net {

class URLStreamHandler;

CarClass(CURL)
    , public Object
    , public IURL
    , public ISerializable
{
    friend class URLStreamHandler;

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CURL();

    CARAPI constructor(
        /* [in] */ const String& spec);

    CARAPI constructor(
        /* [in] */ IURL* context,
        /* [in] */ const String& spec);

    CARAPI constructor(
        /* [in] */ IURL* context,
        /* [in] */ const String& spec,
        /* [in] */ IURLStreamHandler* handler);

    CARAPI constructor(
        /* [in] */ const String& protocol,
        /* [in] */ const String& host,
        /* [in] */ const String& file);

    CARAPI constructor(
        /* [in] */ const String& protocol,
        /* [in] */ const String& host,
        /* [in] */ Int32 port,
        /* [in] */ const String& file);

    CARAPI constructor(
        /* [in] */ const String& protocol,
        /* [in] */ const String& host,
        /* [in] */ Int32 port,
        /* [in] */ const String& file,
        /* [in] */ IURLStreamHandler* handler);

    /**
     * Sets the stream handler factory for this VM.
     *
     * @throws Error if a URLStreamHandlerFactory has already been installed
     *     for the current VM.
     */
    static CARAPI SetURLStreamHandlerFactory(
        /* [in] */ IURLStreamHandlerFactory* streamFactory);

    CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean* result);

    /**
     * Returns true if this URL refers to the same resource as {@code otherURL}.
     * All URL components except the reference field are compared.
     */
    CARAPI SameFile(
        /* [in] */ IURL* otherURL,
        /* [out] */ Boolean* isSame);

    CARAPI GetHashCode(
         /* [out] */ Int32 * result);

    /**
     * Returns the content of the resource which is referred by this URL. By
     * default this returns an {@code InputStream}, or null if the content type
     * of the response is unknown.
     */
    CARAPI GetContent(
        /* [out] */ IInterface** content);

//        public final Object getContent(Class[] types)

    /**
     * Equivalent to {@code openConnection().getInputStream(types)}.
     */
    CARAPI OpenStream(
        /* [out] */ IInputStream** is);

    /**
     * Returns a new connection to the resource referred to by this URL.
     *
     * @throws IOException if an error occurs while opening the connection.
     */
    CARAPI OpenConnection(
        /* [out] */ IURLConnection** connection);

    /**
     * Returns a new connection to the resource referred to by this URL.
     *
     * @param proxy the proxy through which the connection will be established.
     * @throws IOException if an I/O error occurs while opening the connection.
     * @throws IllegalArgumentException if the argument proxy is null or of is
     *     an invalid type.
     * @throws UnsupportedOperationException if the protocol handler does not
     *     support opening connections through proxies.
     */
    CARAPI OpenConnection(
        /* [in] */ IProxy* proxy,
        /* [out] */ IURLConnection** connection);

    /**
     * Returns the URI equivalent to this URL.
     *
     * @throws URISyntaxException if this URL cannot be converted into a URI.
     */
    CARAPI ToURI(
        /* [out] */ IURI** uri);

    /**
     * Encodes this URL to the equivalent URI after escaping characters that are
     * not permitted by URI.
     *
     * @hide
     */
    CARAPI ToURILenient(
            /* [out] */ IURI** uri);

    /**
     * Returns a string containing a concise, human-readable representation of
     * this URL. The returned string is the same as the result of the method
     * {@code toExternalForm()}.
     */
    CARAPI ToString(
        /* [out] */ String* str);

    /**
     * Returns a string containing a concise, human-readable representation of
     * this URL.
     */
    CARAPI ToExternalForm(
        /* [out] */ String* url);

    /** @hide */
    CARAPI GetEffectivePort(
        /* [out] */ Int32* port);

    CARAPI GetProtocol(
        /* [out] */ String* protocol);

    CARAPI GetAuthority(
        /* [out] */ String* authority);

    CARAPI GetUserInfo(
        /* [out] */ String* userInfo);

    CARAPI GetHost(
        /* [out] */ String* hostName);

    CARAPI GetPort(
        /* [out] */ Int32* port);

    CARAPI GetDefaultPort(
        /* [out] */ Int32* port);

    CARAPI GetFile(
        /* [out] */ String* fileName);

    CARAPI GetPath(
        /* [out] */ String* path);

    CARAPI GetQuery(
        /* [out] */ String* query);

    CARAPI GetRef(
        /* [out] */ String* ref);

private:
    // private void readObject(java.io.ObjectInputStream stream)

    // private void writeObject(ObjectOutputStream s)

    CARAPI_(void) FixURL(
        /* [in] */ Boolean fixHost);

    /**
     * Sets the properties of this URL using the provided arguments. Only a
     * {@code URLStreamHandler} can use this method to set fields of the
     * existing URL instance. A URL is generally constant.
     */
    CARAPI Set(
        /* [in] */ const String& protocol,
        /* [in] */ const String& host,
        /* [in] */ Int32 port,
        /* [in] */ const String& file,
        /* [in] */ const String& ref);

    /**
     * Sets the receiver's stream handler to one which is appropriate for its
     * protocol.
     *
     * <p>Note that this will overwrite any existing stream handler with the new
     * one. Senders must check if the streamHandler is null before calling the
     * method if they do not want this behavior (a speed optimization).
     *
     * @throws MalformedURLException if no reasonable handler is available.
     */
    CARAPI_(void) SetupStreamHandler();

    CARAPI Set(
        /* [in] */ const String& protocol,
        /* [in] */ const String& host,
        /* [in] */ Int32 port,
        /* [in] */ const String& authority,
        /* [in] */ const String& userInfo,
        /* [in] */ const String& path,
        /* [in] */ const String& query,
        /* [in] */ const String& ref);

private:
    static AutoPtr<IURLStreamHandlerFactory> sStreamHandlerFactory;

    /** Cache of protocols to their handlers */
    static HashMap<String, AutoPtr<IURLStreamHandler> > sStreamHandlers;
    static Object sLock;

    String mProtocol;
    String mAuthority;
    String mHost;
    Int32 mPort;
    String mFile;
    String mRef;

    /* transient */ String mUserInfo;
    /* transient */ String mPath;
    /* transient */ String mQuery;

    /* transient */ AutoPtr<IURLStreamHandler> mStreamHandler;

   /**
     * The cached hash code, or 0 if it hasn't been computed yet. Unlike the RI,
     * this implementation's hashCode is transient because the hash code is
     * unspecified and may vary between VMs or versions.
     */
    /* transient */ Int32 mHashCode;
};

} // namespace Net
} // namespace Elastos

#endif //__ELASTOS_NET_CURL_H__
