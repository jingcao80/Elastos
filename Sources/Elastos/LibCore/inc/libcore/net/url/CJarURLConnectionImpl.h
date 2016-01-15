
#ifndef __LIBCORE_NET_URL_LIBCORE_NET_URI_JARURLCONNECTIONIMPL_H__
#define __LIBCORE_NET_URL_LIBCORE_NET_URI_JARURLCONNECTIONIMPL_H__

#include "Elastos.CoreLibrary.Utility.Zip.h"
#include "_Libcore_Net_Url_CJarURLConnectionImpl.h"
#include "JarURLConnection.h"
#include "FilterInputStream.h"

using Elastos::Net::IURL;
using Elastos::Net::JarURLConnection;
using Elastos::IO::FilterInputStream;

DEFINE_OBJECT_HASH_FUNC_FOR(IURL)

namespace Libcore {
namespace Net {
namespace Url {

/**
 * This subclass extends {@code URLConnection}.
 * <p>
 *
 * This class is responsible for connecting and retrieving resources from a Jar
 * file which can be anywhere that can be referred to by an URL.
 */
CarClass(CJarURLConnectionImpl)
    , public JarURLConnection
    , public IJarURLConnectionImpl
{
private:
    class JarURLConnectionInputStream
        : public FilterInputStream
    {
    public:
        JarURLConnectionInputStream(
            /* [in] */ IInputStream* in,
            /* [in] */ IJarFile* file,
            /* [in] */ CJarURLConnectionImpl* host);

        // @Override
        CARAPI Close() /*throws IOException*/;

    public:
        AutoPtr<IJarFile> mJarFile;
        AutoPtr<CJarURLConnectionImpl> mHost;
    };

public:
    CAR_INTERFACE_DECL();

    /**
     * @param url
     *            the URL of the JAR
     * @throws MalformedURLException
     *             if the URL is malformed
     * @throws IOException
     *             if there is a problem opening the connection.
     */
    CARAPI constructor(
        /* [in] */ IURL* url) /*throws MalformedURLException, IOException*/;

    /**
     * @see java.net.URLConnection#connect()
     */
    // @Override
    CARAPI Connect() /*throws IOException*/;

    /**
     * Returns the Jar file referred by this {@code URLConnection}.
     *
     * @throws IOException
     *             thrown if an IO error occurs while connecting to the
     *             resource.
     */
    // @Override
    CARAPI GetJarFile(
        /* [out] */ IJarFile** file) /*throws IOException*/;

    /**
     * Returns the JarEntry of the entry referenced by this {@code
     * URLConnection}.
     *
     * @return the JarEntry referenced
     *
     * @throws IOException
     *             if an IO error occurs while getting the entry
     */
    // @Override
    CARAPI GetJarEntry(
        /* [out] */ IJarEntry** entry) /*throws IOException*/;

private:
    /**
     * Returns the Jar file referred by this {@code URLConnection}
     *
     * @throws IOException
     *             if an IO error occurs while connecting to the resource.
     */
    CARAPI FindJarFile() /*throws IOException*/;

    CARAPI OpenJarFile(
        /* [out] */ IJarFile** file) /*throws IOException*/;

    /**
     * Look up the JarEntry of the entry referenced by this {@code
     * URLConnection}.
     */
    CARAPI FindJarEntry() /*throws IOException*/;

    /**
     * Creates an input stream for reading from this URL Connection.
     *
     * @return the input stream
     *
     * @throws IOException
     *             if an IO error occurs while connecting to the resource.
     */
    // @Override
    CARAPI GetInputStream(
        /* [out] */ IInputStream** stream)/* throws IOException*/;

    /**
     * Returns the content type of the resource. For jar file itself
     * "x-java/jar" should be returned, for jar entries the content type of the
     * entry should be returned. Returns non-null results ("content/unknown" for
     * unknown types).
     *
     * @return the content type
     */
    // @Override
    CARAPI GetContentType(
        /* [out] */ String* type);

    /**
     * Returns the content length of the resource. Test cases reveal that if the URL is referring to
     * a Jar file, this method answers a content-length returned by URLConnection. For a jar entry
     * it returns the entry's size if it can be represented as an {@code int}. Otherwise, it will
     * return -1.
     */
    // @Override
    CARAPI GetContentLength(
        /* [out] */ Int32* length);

    /**
     * Returns the object pointed by this {@code URL}. If this URLConnection is
     * pointing to a Jar File (no Jar Entry), this method will return a {@code
     * JarFile} If there is a Jar Entry, it will return the object corresponding
     * to the Jar entry content type.
     *
     * @return a non-null object
     *
     * @throws IOException
     *             if an IO error occurred
     *
     * @see ContentHandler
     * @see ContentHandlerFactory
     * @see java.io.IOException
     * @see #setContentHandlerFactory(ContentHandlerFactory)
     */
    // @Override
    CARAPI GetContent(
        /* [out] */ IInterface** content) /*throws IOException*/;

    /**
     * Returns the permission, in this case the subclass, FilePermission object
     * which represents the permission necessary for this URLConnection to
     * establish the connection.
     *
     * @return the permission required for this URLConnection.
     *
     * @throws IOException
     *             thrown when an IO exception occurs while creating the
     *             permission.
     */
    // @Override
    CARAPI GetPermission(
        /* [out] */ IPermission** permission) /*throws IOException*/;

    // @Override
    CARAPI GetUseCaches(
        /* [out] */ Boolean* usecaches);

    // @Override
    CARAPI SetUseCaches(
        /* [in] */ Boolean usecaches);

    // @Override
    CARAPI GetDefaultUseCaches(
        /* [out] */ Boolean* usecaches);

    // @Override
    CARAPI SetDefaultUseCaches(
        /* [in] */ Boolean defaultusecaches);

private:
    static HashMap<AutoPtr<IURL>, AutoPtr<IJarFile> > sJarCache/* = new HashMap<URL, JarFile>()*/;
    static Object sJarCacheLock;

    AutoPtr<IURL> mJarFileURL;

    AutoPtr<IInputStream> mJarInput;

    AutoPtr<IJarFile> mJarFile;

    AutoPtr<IJarEntry> mJarEntry;

    Boolean mClosed;
};

} // namespace Url
} // namespace Net
} // namespace Libcore

#endif //__LIBCORE_NET_URL_LIBCORE_NET_URI_JARURLCONNECTIONIMPL_H__
