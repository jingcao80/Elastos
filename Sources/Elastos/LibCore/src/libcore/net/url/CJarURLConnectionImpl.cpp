
#include "CJarURLConnectionImpl.h"
#include "AutoLock.h"
#include "UriCodec.h"
#include "CJarFile.h"
#include "CFile.h"
#include "CFileOutputStream.h"

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Core::AutoLock;
using Elastos::IO::IFile;
using Elastos::IO::CFile;
using Elastos::IO::CFileOutputStream;
using Elastos::IO::IFileOutputStream;
using Elastos::Utility::Jar::CJarFile;
using Elastos::Utility::Zip::IZipFile;
using Elastos::Utility::Zip::IZipEntry;
using Libcore::Net::UriCodec;

namespace Libcore {
namespace Net {
namespace Url {

CAR_OBJECT_IMPL(CJarURLConnectionImpl)

CAR_INTERFACE_IMPL(CJarURLConnectionImpl, JarURLConnection, IJarURLConnectionImpl)

Object CJarURLConnectionImpl::sJarCacheLock;
HashMap<AutoPtr<IURL>, AutoPtr<IJarFile> > CJarURLConnectionImpl::sJarCache;

CJarURLConnectionImpl::JarURLConnectionInputStream::JarURLConnectionInputStream(
    /* [in] */ IInputStream* in,
    /* [in] */ IJarFile* file,
    /* [in] */ CJarURLConnectionImpl* host)
{
    FilterInputStream::constructor(in);
    mJarFile = file;
    mHost = host;
}

ECode CJarURLConnectionImpl::JarURLConnectionInputStream::Close() /*throws IOException*/
{
    FilterInputStream::Close();
    Boolean usecaches = FALSE;
    mHost->GetUseCaches(&usecaches);
    if (!usecaches) {
        mHost->mClosed = TRUE;
        ICloseable::Probe(mJarFile)->Close();
    }
    return NOERROR;
}

ECode CJarURLConnectionImpl::constructor(
    /* [in] */ IURL* url) /*throws MalformedURLException, IOException*/
{
    JarURLConnection::constructor(url);
    GetJarFileURL((IURL**)&mJarFileURL);
    return mJarFileURL->OpenConnection((IURLConnection**)&mJarFileURLConnection);
}

ECode CJarURLConnectionImpl::Connect() /*throws IOException*/
{
    if (!mConnected) {
        FAIL_RETURN(FindJarFile()); // ensure the file can be found
        FAIL_RETURN(FindJarEntry()); // ensure the entry, if any, can be found
        mConnected = TRUE;
    }
    return NOERROR;
}

ECode CJarURLConnectionImpl::GetJarFile(
    /* [out] */ IJarFile** file) /*throws IOException*/
{
    VALIDATE_NOT_NULL(file);
    *file = NULL;
    FAIL_RETURN(Connect());
    *file = mJarFile;
    REFCOUNT_ADD(*file);
    return NOERROR;
}

ECode CJarURLConnectionImpl::FindJarFile() /*throws IOException*/
{
    Boolean usecaches = FALSE;
    if (GetUseCaches(&usecaches), usecaches) {
        {    AutoLock syncLock(sJarCacheLock);
            mJarFile = sJarCache[mJarFileURL];
        }

        if (mJarFile == NULL) {
            AutoPtr<IJarFile> jar;
            FAIL_RETURN(OpenJarFile((IJarFile**)&jar));
            {    AutoLock syncLock(sJarCacheLock);
                mJarFile = sJarCache[mJarFileURL];
                if (mJarFile == NULL) {
                    sJarCache[mJarFileURL] = jar;
                    mJarFile = jar;
                } else {
                    ICloseable::Probe(jar)->Close();
                }
            }
        }
    } else {
        FAIL_RETURN(OpenJarFile((IJarFile**)&mJarFile));
    }

    if (mJarFile == NULL) {
        // throw new IOException();
        return E_IO_EXCEPTION;
    }
    return NOERROR;
}

ECode CJarURLConnectionImpl::OpenJarFile(
    /* [out] */ IJarFile** jarFile) /*throws IOException*/
{
    VALIDATE_NOT_NULL(jarFile);
    *jarFile = NULL;
    String value;
    if ((mJarFileURL->GetProtocol(&value), value).Equals("file")) {
        mJarFileURL->GetFile(&value);
        String decodedFile;
        UriCodec::Decode(value, &decodedFile);

        AutoPtr<IFile> file;
        CFile::New(decodedFile, (IFile**)&file);
        return CJarFile::New(file, TRUE, IZipFile::OPEN_READ, jarFile);
    } else {
        AutoPtr<IURLConnection> uc;
        FAIL_RETURN(mJarFileURL->OpenConnection((IURLConnection**)&uc));
        AutoPtr<IInputStream> is;
        FAIL_RETURN(uc->GetInputStream((IInputStream**)&is));
        // try {
        AutoPtr<IFileOutputStream> fos;
        AutoPtr<IJarFile> result;
        // try {
        AutoPtr<IFile> tempJar;
        FAIL_RETURN(CFile::CreateTempFile(String("hyjar_"), String(".tmp"), NULL, (IFile**)&tempJar));
        tempJar->DeleteOnExit();
        CFileOutputStream::New(tempJar, (IFileOutputStream**)&fos);
        AutoPtr<ArrayOf<Byte> > buf = ArrayOf<Byte>::Alloc(4096);
        Int32 nbytes = 0;
        FAIL_GOTO(is->Read(buf, &nbytes), Fail);
        while (nbytes > -1) {
            FAIL_GOTO(IOutputStream::Probe(fos)->Write(buf, 0, nbytes), Fail);
            FAIL_GOTO(is->Read(buf, &nbytes), Fail);
        }
        ICloseable::Probe(fos)->Close();
        return CJarFile::New(tempJar, TRUE, IZipFile::OPEN_READ | IZipFile::OPEN_DELETE, jarFile);
        // } catch (IOException e) {
        //     return null;
        // } finally {
        //     if (fos != null) {
        //         try {
        //             fos.close();
        //         } catch (IOException ex) {
        //             return null;
        //         }
        //     }
        // }
Fail:
        if (fos != NULL) {
            ICloseable::Probe(fos)->Close();
        }
        // } finally {
        if (is != NULL) {
            is->Close();
        }
        // }
    }

    return NOERROR;
}

ECode CJarURLConnectionImpl::GetJarEntry(
    /* [out] */ IJarEntry** entry) /*throws IOException*/
{
    VALIDATE_NOT_NULL(entry);
    FAIL_RETURN(Connect());
    *entry = mJarEntry;
    REFCOUNT_ADD(*entry);
    return NOERROR;
}

ECode CJarURLConnectionImpl::FindJarEntry() /*throws IOException*/
{
    String name;
    FAIL_RETURN(GetEntryName(&name));
    if (name == NULL) {
        return NOERROR;
    }

    FAIL_RETURN(mJarFile->GetJarEntry(name, (IJarEntry**)&mJarEntry));
    if (mJarEntry == NULL) {
        // throw new FileNotFoundException(getEntryName());
        return E_FILE_NOT_FOUND_EXCEPTION;
    }
    return NOERROR;
}

ECode CJarURLConnectionImpl::GetInputStream(
    /* [out] */ IInputStream** stream)/* throws IOException*/
{
    VALIDATE_NOT_NULL(stream);
    if (mClosed) {
        // throw new IllegalStateException("JarURLConnection InputStream has been closed");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    FAIL_RETURN(Connect());
    if (mJarInput != NULL) {
        *stream = mJarInput;
        REFCOUNT_ADD(*stream);
        return NOERROR;
    }

    if (mJarEntry == NULL) {
        // throw new IOException("Jar entry not specified");
        *stream = NULL;
        return E_IO_EXCEPTION;
    }

    AutoPtr<IInputStream> is;
    FAIL_RETURN(IZipFile::Probe(mJarFile)->GetInputStream(IZipEntry::Probe(mJarEntry), (IInputStream**)&is));
    mJarInput = new JarURLConnectionInputStream(is, mJarFile, this);
    *stream = mJarInput;
    REFCOUNT_ADD(*stream);
    return NOERROR;
}

ECode CJarURLConnectionImpl::GetContentType(
    /* [out] */ String* type)
{
    VALIDATE_NOT_NULL(type);
    String file;
    if ((mUrl->GetFile(&file), file).EndWith("!/")) {
        // the type for jar file itself is always "x-java/jar"
        *type = String("x-java/jar");
        return NOERROR;
    }
    String entryName;
    GetEntryName(&entryName);

    if (entryName != NULL) {
        // if there is an Jar Entry, get the content type from the name
        *type = GuessContentTypeFromName(entryName);
    } else {
        // try {
        ASSERT_SUCCEEDED(Connect());
        mJarFileURLConnection->GetContentType(type);
        // } catch (IOException ioe) {
        //     // Ignore
        // }
    }
    if (*type == NULL) {
        *type = String("content/unknown");
    }
    return NOERROR;
}

ECode CJarURLConnectionImpl::GetContentLength(
    /* [out] */ Int32* length)
{
    VALIDATE_NOT_NULL(length);
    // try {
    if (FAILED(Connect())) {
        *length = -1;
        return NOERROR;
    }
    if (mJarEntry == NULL) {
        return mJarFileURLConnection->GetContentLength(length);
    }

    Int64 size = 0;
    AutoPtr<IJarEntry> entry;
    if (FAILED(GetJarEntry((IJarEntry**)&entry))) {
        *length = -1;
        return NOERROR;
    }
    IZipEntry::Probe(entry)->GetSize(&size);
    *length = (Int32) size;
    // } catch (IOException e) {
    //     // Ignored
    // }
    return NOERROR;
}

ECode CJarURLConnectionImpl::GetContent(
    /* [out] */ IInterface** content) /*throws IOException*/
{
    VALIDATE_NOT_NULL(content);
    *content = NULL;
    FAIL_RETURN(Connect());
    // if there is no Jar Entry, return a JarFile
    if (mJarEntry == NULL) {
        *content = IObject::Probe(mJarFile);
        REFCOUNT_ADD(*content);
        return NOERROR;
    }
    return JarURLConnection::GetContent(content);
}

ECode CJarURLConnectionImpl::GetPermission(
    /* [out] */ IPermission** permission) /*throws IOException*/
{
    return mJarFileURLConnection->GetPermission(permission);
}

ECode CJarURLConnectionImpl::GetUseCaches(
    /* [out] */ Boolean* usecaches)
{
    return mJarFileURLConnection->GetUseCaches(usecaches);
}

ECode CJarURLConnectionImpl::SetUseCaches(
    /* [in] */ Boolean usecaches)
{
    return mJarFileURLConnection->SetUseCaches(usecaches);
}

ECode CJarURLConnectionImpl::GetDefaultUseCaches(
    /* [out] */ Boolean* usecaches)
{
    return mJarFileURLConnection->GetDefaultUseCaches(usecaches);
}

ECode CJarURLConnectionImpl::SetDefaultUseCaches(
    /* [in] */ Boolean defaultusecaches)
{
    return mJarFileURLConnection->SetDefaultUseCaches(defaultusecaches);
}

} // namespace Url
} // namespace Net
} // namespace Libcore
