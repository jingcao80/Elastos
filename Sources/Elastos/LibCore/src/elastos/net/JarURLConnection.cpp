
#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.CoreLibrary.Utility.Zip.h"
#include "JarURLConnection.h"
#include "UriCodec.h"
#include "CURL.h"

using Elastos::Core::ICloneable;
using Libcore::Net::UriCodec;

namespace Elastos {
namespace Net {

CAR_INTERFACE_IMPL(JarURLConnection, URLConnection, IJarURLConnection)

ECode JarURLConnection::constructor(
    /* [in] */ IURL* url)
{
    VALIDATE_NOT_NULL(url)

    FAIL_RETURN(URLConnection::constructor(url))

    String filestr;
    url->GetFile(&filestr);
    FAIL_RETURN(Decode(filestr, &mFile))

    Int32 sepIdx;
    if ((sepIdx = mFile.IndexOf("!/")) < 0) {
        return E_MALFORMED_URL_EXCEPTION;
    }

    CURL::New(mFile.Substring(0, sepIdx), (IURL**)&mFileURL);
    sepIdx += 2;
    if (mFile.GetLength() == sepIdx) {
        return NOERROR;
    }
    mEntryName = mFile.Substring(sepIdx, mFile.GetLength());
    String ref;
    url->GetRef(&ref);
    if (!ref.IsNull()) {
        mEntryName += "#";
        mEntryName += ref;
    }
    return NOERROR;
}

ECode JarURLConnection::GetAttributes(
    /* [out] */ IAttributes** attr)
{
    VALIDATE_NOT_NULL(attr)
    *attr = NULL;

    AutoPtr<IJarEntry> jEntry;
    GetJarEntry((IJarEntry**)&jEntry);
    if (jEntry != NULL) {
        return jEntry->GetAttributes(attr);
    }

    return NOERROR;
}

ECode JarURLConnection::GetCertificates(
    /* [out, callee] */ ArrayOf<ICertificate *> ** certs)
{
    VALIDATE_NOT_NULL(certs)
    *certs = NULL;

    AutoPtr<IJarEntry> jEntry;
    GetJarEntry((IJarEntry**)&jEntry);
    if (jEntry != NULL) {
        return jEntry->GetCertificates(certs);
    }

    return NOERROR;
}

ECode JarURLConnection::GetEntryName(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = mEntryName;
    return NOERROR;
}

ECode JarURLConnection::GetJarEntry(
    /* [out] */ IJarEntry** entry)
{
    VALIDATE_NOT_NULL(entry)
    *entry = NULL;

    if (!mConnected) {
        FAIL_RETURN(Connect());
    }
    if (mEntryName.IsNull()) {
        return NOERROR;
    }
    // The entry must exist since the connect succeeded
    AutoPtr<IJarFile> jarFile;
    GetJarFile((IJarFile**)&jarFile);
    return jarFile->GetJarEntry(mEntryName, entry);
}

ECode JarURLConnection::GetManifest(
    /* [out] */ IManifest** manifest)
{
    VALIDATE_NOT_NULL(manifest)
    *manifest = NULL;

    AutoPtr<IJarFile> jarFile;
    GetJarFile((IJarFile**)&jarFile);
    AutoPtr<IManifest> m;
    jarFile->GetManifest((IManifest**)&m);

    if (m) {
        AutoPtr<IInterface> obj;
        ICloneable::Probe(m)->Clone((IInterface**)&obj);
        *manifest = IManifest::Probe(obj);
        REFCOUNT_ADD(*manifest);
    }
    return NOERROR;
}

ECode JarURLConnection::GetJarFile(
    /* [out] */ IJarFile** file)
{
    assert(0 && "Sub class must implement this method.");
    return E_NOT_IMPLEMENTED;
}

ECode JarURLConnection::GetJarFileURL(
    /* [out] */ IURL** url)
{
    VALIDATE_NOT_NULL(url)
    *url = mFileURL;
    REFCOUNT_ADD(*url)
    return NOERROR;
}

ECode JarURLConnection::GetMainAttributes(
    /* [out] */ IAttributes** attr)
{
    VALIDATE_NOT_NULL(attr)
    *attr = NULL;

    AutoPtr<IJarFile> jarFile;
    GetJarFile((IJarFile**)&jarFile);
    AutoPtr<IManifest> m;
    jarFile->GetManifest((IManifest**)&m);

    if (m) {
        return m->GetMainAttributes(attr);
    }
    return NOERROR;
}

ECode JarURLConnection::Decode(
    /* [in] */ const String& encoded,
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value)
    *value = String(NULL);

    //try {
        // "+" means "+" in URLs. i.e. like RFC 3986, not like
        // MIME application/x-www-form-urlencoded
        Boolean convertPlus = FALSE;
        AutoPtr<ICharset> charset;// = StandardCharsets::UTF_8
        ECode ec = UriCodec::Decode(encoded, convertPlus, charset, TRUE /* throwOnFailure */, value);
        if (ec == (ECode)E_ILLEGAL_ARGUMENT_EXCEPTION) {
            ALOGE("E_MALFORMED_URL_EXCEPTION: JarURLConnection: Unable to decode URL %s",
                encoded.string());
            return E_MALFORMED_URL_EXCEPTION;
        }
    //} catch (IllegalArgumentException e) {
    //    throw new MalformedURLException("Unable to decode URL", e);
    //}
    return NOERROR;
}

} // namespace Net
} // namespace Elastos