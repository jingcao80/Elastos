
#ifndef __ELASTOS_NET_JARURLCONNECTION_H__
#define __ELASTOS_NET_JARURLCONNECTION_H__

#include "URLConnection.h"

using Elastos::Security::Cert::ICertificate;
using Elastos::Utility::Jar::IJarEntry;
using Elastos::Utility::Jar::IJarFile;
using Elastos::Utility::Jar::IManifest;
using Elastos::Utility::Jar::IAttributes;

namespace Elastos {
namespace Net {

class JarURLConnection
    : public URLConnection
    , public IJarURLConnection
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Constructs an instance of {@code JarURLConnection} that refers to the
     * specified URL.
     *
     * @param url
     *            the URL that contains the location to connect to.
     * @throws MalformedURLException
     *             if an invalid URL has been entered.
     */
    CARAPI constructor(
        /* [in] */ IURL* url);

    /**
     * Returns all attributes of the {@code JarEntry} referenced by this {@code
     * JarURLConnection}.
     *
     * @return the attributes of the referenced {@code JarEntry}.
     * @throws IOException
     *                if an I/O exception occurs while retrieving the
     *                JAR-entries.
     */
    CARAPI GetAttributes(
        /* [out] */ IAttributes** attr);

    /**
     * Returns all certificates of the {@code JarEntry} referenced by this
     * {@code JarURLConnection} instance. This method will return {@code null}
     * until the {@code InputStream} has been completely verified.
     *
     * @return the certificates of the {@code JarEntry} as an array.
     * @throws IOException
     *                if there is an I/O exception occurs while getting the
     *                {@code JarEntry}.
     */
    CARAPI GetCertificates(
        /* [out, callee] */ ArrayOf<ICertificate *> ** certs);

    /**
     * Gets the name of the entry referenced by this {@code JarURLConnection}.
     * The return value will be {@code null} if this instance refers to a JAR
     * file rather than an JAR file entry.
     *
     * @return the {@code JarEntry} name this instance refers to.
     */
    CARAPI GetEntryName(
        /* [out] */ String* str);

    /**
     * Gets the {@code JarEntry} object of the entry referenced by this {@code
     * JarURLConnection}.
     *
     * @return the referenced {@code JarEntry} object or {@code null} if no
     *         entry name is specified.
     * @throws IOException
     *             if an error occurs while getting the file or file-entry.
     */
    CARAPI GetJarEntry(
        /* [out] */ IJarEntry** entry);

    /**
     * Gets the manifest file associated with this JAR-URL.
     *
     * @return the manifest of the referenced JAR-file.
     * @throws IOException
     *             if an error occurs while getting the manifest file.
     */
    CARAPI GetManifest(
        /* [out] */ IManifest** manifest);

    /**
     * Gets the {@code JarFile} object referenced by this {@code
     * JarURLConnection}.
     *
     * @return the referenced JarFile object.
     * @throws IOException
     *                if an I/O exception occurs while retrieving the JAR-file.
     */
    CARAPI GetJarFile(
        /* [out] */ IJarFile** file);

    /**
     * Gets the URL to the JAR-file referenced by this {@code JarURLConnection}.
     *
     * @return the URL to the JAR-file or {@code null} if there was an error
     *         retrieving the URL.
     */
    CARAPI GetJarFileURL(
        /* [out] */ IURL** url);

    /**
     * Gets all attributes of the manifest file referenced by this {@code
     * JarURLConnection}. If this instance refers to a JAR-file rather than a
     * JAR-file entry, {@code null} will be returned.
     *
     * @return the attributes of the manifest file or {@code null}.
     * @throws IOException
     *                if an I/O exception occurs while retrieving the {@code
     *                JarFile}.
     */
    CARAPI GetMainAttributes(
        /* [out] */ IAttributes** attr);

private:
    static CARAPI Decode(
        /* [in] */ const String& encoded,
        /* [out] */ String* value);

protected:
    /**
     * The location part of the represented URL.
     */
    AutoPtr<IURLConnection> mJarFileURLConnection;

private:
    String mEntryName;

    AutoPtr<IURL> mFileURL;

    // the file component of the URL
    String mFile;
};

} // namespace Net
} // namespace Elastos

#endif //__ELASTOS_NET_JARURLCONNECTION_H__
