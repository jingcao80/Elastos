
#include "CJarEntry.h"
#include "CJarFile.h"
#include "CManifest.h"
#include "CStreams.h"
#include "CFile.h"
#include <elastos/utility/etl//List.h>
#include <cutils/log.h>

using Elastos::Utility::Etl::List;
using Elastos::IO::FilterInputStream;
using Elastos::IO::EIID_IInputStream;
using Elastos::IO::CFile;
using Libcore::IO::IStreams;
using Libcore::IO::CStreams;

namespace Elastos {
namespace Utility {
namespace Jar {

const String CJarFile::META_DIR("META-INF/");

//===========================================================
// CJarFile::JarFileInputStream
//===========================================================
CJarFile::JarFileInputStream::JarFileInputStream()
    : mCount(0)
    , mDone(FALSE)
{
}

ECode CJarFile::JarFileInputStream::constructor(
    /* [in] */ IInputStream* is,
    /* [in] */ Int64 size,
    /* [in] */ JarVerifier::VerifierEntry* e)
{
    FAIL_RETURN(FilterInputStream::constructor(is))
    mEntry = e;
    mCount = size;
    return NOERROR;
}

ECode CJarFile::JarFileInputStream::Read(
    /* [out] */ Int32* val)
{
    VALIDATE_NOT_NULL(val)
    *val = -1;

    if (mDone) {
        return NOERROR;
    }

    if (mCount > 0) {
        FAIL_RETURN(FilterInputStream::Read(val));
        if (*val != -1) {
            FAIL_RETURN(mEntry->Write(*val))
            mCount--;
        }
        else {
            mCount = 0;
        }

        if (mCount == 0) {
            mDone = TRUE;
            FAIL_RETURN(mEntry->Verify())
        }
        return NOERROR;
    }
    else {
        mDone = TRUE;
        FAIL_RETURN(mEntry->Verify())
        *val = -1;
        return NOERROR;
    }
}

ECode CJarFile::JarFileInputStream::Read(
    /* [in] */ ArrayOf<Byte>* buf,
    /* [in] */ Int32 off,
    /* [in] */ Int32 nbytes,
    /* [out] */ Int32* val)
{
    VALIDATE_NOT_NULL(val)
    *val = -1;

    if (mDone) {
        return NOERROR;
    }

    if (mCount > 0) {
        FAIL_RETURN(FilterInputStream::Read(buf, off, nbytes, val))
        if (*val != -1) {
            Int32 size = *val;
            if (mCount < size) {
                size = (Int32) mCount;
            }
            FAIL_RETURN(mEntry->Write(buf, off, size))
            mCount -= size;
        }
        else {
            mCount = 0;
        }

        if (mCount == 0) {
            mDone = TRUE;
            FAIL_RETURN(mEntry->Verify())
        }
        return NOERROR;
    }
    else {
        mDone = TRUE;
        FAIL_RETURN(mEntry->Verify())
        *val = -1;
        return NOERROR;
    }
}

ECode CJarFile::JarFileInputStream::Available(
    /* [out] */ Int32* val)
{
    VALIDATE_NOT_NULL(val)
    if (mDone) {
        *val = 0;
        return NOERROR;
    }
    return FilterInputStream::Available(val);
}

ECode CJarFile::JarFileInputStream::Skip(
    /* [in] */ Int64 byteCount,
    /* [out] */ Int64* val)
{
    VALIDATE_NOT_NULL(val)
    AutoPtr<IStreams> stream;
    FAIL_RETURN(CStreams::AcquireSingleton((IStreams**)&stream))
    return stream->SkipByReading(this, byteCount, val);
}

//===========================================================
// CJarFile::JarFileEnumerator
//===========================================================
CAR_INTERFACE_IMPL(CJarFile::JarFileEnumerator, Object, IEnumeration)

CJarFile::JarFileEnumerator::JarFileEnumerator(
    /* [in] */ IEnumeration* enu,
    /* [in] */ CJarFile* jarFile)
    : mZipEntry(enu)
    , mJarFile(jarFile)
{
}

ECode CJarFile::JarFileEnumerator::HasMoreElements(
    /* [out] */ Boolean * hasMore)
{
    return mZipEntry->HasMoreElements(hasMore);
}

ECode CJarFile::JarFileEnumerator::GetNextElement(
    /* [out] */ IInterface ** obj)
{
    AutoPtr<IInterface> i;
    mZipEntry->GetNextElement((IInterface**)&i);
    AutoPtr<IJarEntry> je;
    CJarEntry::New(String("name"), (IJarEntry**)&je);
    ((CJarEntry*)je.Get())->constructor(IZipEntry::Probe(i), mJarFile);
    *obj = je.Get();
    REFCOUNT_ADD(*obj)
    return NOERROR;
}

ECode CJarFile::JarFileEnumerator::GetNextElement(
    /* [out] */ IJarEntry ** obj)
{
    AutoPtr<IInterface> i;
    mZipEntry->GetNextElement((IInterface**)&i);
    AutoPtr<IJarEntry> je;
    CJarEntry::New(String("name"), (IJarEntry**)&je);
    ((CJarEntry*)je.Get())->constructor(IZipEntry::Probe(i), mJarFile);
    *obj = je.Get();
    REFCOUNT_ADD(*obj)
    return NOERROR;
}

//===========================================================
// CJarFile
//===========================================================
CAR_INTERFACE_IMPL(CJarFile, ZipFile, IJarFile)

CAR_OBJECT_IMPL(CJarFile)

CJarFile::CJarFile()
    : mClosed(FALSE)
{}

ECode CJarFile::constructor(
    /* [in] */ IFile* file)
{
    return constructor(file, TRUE);
}

ECode CJarFile::constructor(
    /* [in] */ IFile * file,
    /* [in] */ Boolean verify)
{
    return constructor(file, verify, IZipFile::OPEN_READ);
}

ECode CJarFile::constructor(
    /* [in] */ IFile* file,
    /* [in] */ Boolean verify,
    /* [in] */ Int32 mode)
{
    FAIL_RETURN(ZipFile::constructor(file, mode))

    // Step 1: Scan the central directory for meta entries (MANIFEST.mf
    // & possibly the signature files) and read them fully.
    AutoPtr<HashMap<String, AutoPtr<ArrayOf<Byte> > > > metaEntries;
    metaEntries = ReadMetaEntries(this, verify);

    // Step 2: Construct a verifier with the information we have.
    // Verification is possible *only* if the JAR file contains a manifest
    // *AND* it contains signing related information (signature block
    // files and the signature files).
    //
    // TODO: Is this really the behaviour we want if verify == true ?
    // We silently skip verification for files that have no manifest or
    // no signatures.
    HashMap<String, AutoPtr<ArrayOf<Byte> > >::Iterator it;
    it = metaEntries->Find(MANIFEST_NAME);
    if (verify && metaEntries->GetSize() > 1
        && it != metaEntries->End()) {
        // We create the manifest straight away, so that we can create
        // the jar verifier as well.
        CManifest::New((IManifest**)&mManifest);
        ((CManifest*)mManifest.Get())->constructor(it->mSecond, TRUE);
        String name;
        GetName(&name);
        mVerifier = new JarVerifier(name, mManifest, metaEntries);
    }
    else {
        mVerifier = NULL;
        mManifestBytes = it->mSecond;
    }

    return NOERROR;
}

ECode CJarFile::constructor(
    /* [in] */ const String& filename)
{
    return constructor(filename, TRUE);
}

ECode CJarFile::constructor(
    /* [in] */ const String& filename,
    /* [in] */ Boolean verify)
{
    AutoPtr<IFile> file;
    CFile::New(filename, (IFile**)&file);
    return constructor(file, verify, IZipFile::OPEN_READ);
}

ECode CJarFile::GetEntries(
    /* [out] */ IEnumeration** entries)
{
    VALIDATE_NOT_NULL(entries)
    AutoPtr<IEnumeration> e;
    ZipFile::GetEntries((IEnumeration**)&e);
    AutoPtr<JarFileEnumerator> jfe = new JarFileEnumerator(e, this);
    *entries = jfe.Get();
    REFCOUNT_ADD(*entries)
    return NOERROR;
}

ECode CJarFile::GetJarEntry(
    /* [in] */ const String& name,
    /* [out] */ IJarEntry** jarEntry)
{
    VALIDATE_NOT_NULL(jarEntry)
    *jarEntry = NULL;

    AutoPtr<IZipEntry> ze;
    FAIL_RETURN(GetEntry(name, (IZipEntry**)&ze))
    *jarEntry = IJarEntry::Probe(ze);
    REFCOUNT_ADD(*jarEntry)
    return NOERROR;
}

ECode CJarFile::GetManifest(
    /* [out] */ IManifest** manifest)
{
    VALIDATE_NOT_NULL(manifest)
    *manifest = NULL;

    if (mClosed) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    if (mManifest != NULL) {
        *manifest = mManifest;
        REFCOUNT_ADD(*manifest)
        return NOERROR;
    }

    // If manifest == null && manifestBytes == null, there's no manifest.
    if (mManifestBytes == NULL) {
        return NOERROR;
    }

    // We hit this code path only if the verification isn't necessary. If
    // we did decide to verify this file, we'd have created the Manifest and
    // the associated Verifier in the constructor itself.
    CManifest::New((IManifest**)&mManifest);
    ((CManifest*)mManifest.Get())->constructor(mManifestBytes, FALSE);
    mManifestBytes = NULL;
    *manifest = mManifest;
    REFCOUNT_ADD(*manifest)
    return NOERROR;
}

AutoPtr<HashMap<String, AutoPtr<ArrayOf<Byte> > > > CJarFile::ReadMetaEntries(
    /* [in] */ ZipFile* zipFile,
    /* [in] */ Boolean verificationRequired)
{
    AutoPtr<HashMap<String, AutoPtr<ArrayOf<Byte> > > > metaEntriesMap;
    metaEntriesMap = new HashMap<String, AutoPtr<ArrayOf<Byte> > >();

    // Get all meta directory entries
    AutoPtr<ArrayOf<IZipEntry*> > metaEntries = GetMetaEntries(zipFile);
    if (metaEntries == NULL) {
        return metaEntriesMap;
    }

    AutoPtr<IStreams> streams;
    CStreams::AcquireSingleton((IStreams**)&streams);
    HashMap<String, AutoPtr<ArrayOf<Byte> > >::Iterator it;
    for (Int32 i = 0; i < metaEntries->GetLength(); ++i) {
        IZipEntry* entry = (*metaEntries)[i];
        String entryName;
        entry->GetName(&entryName);
        // Is this the entry for META-INF/MANIFEST.MF ?
        //
        // TODO: Why do we need the containsKey check ? Shouldn't we discard
        // files that contain duplicate entries like this as invalid ?.
        it = metaEntriesMap->Find(MANIFEST_NAME);
        if (entryName.EqualsIgnoreCase(MANIFEST_NAME) &&
            it == metaEntriesMap->End()) {

            AutoPtr<IInputStream> is;
            zipFile->GetInputStream(entry, (IInputStream**)&is);
            AutoPtr<ArrayOf<Byte> > bytes;
            ECode ec = streams->ReadFully(is, (ArrayOf<Byte>**)&bytes);
            if (FAILED(ec)) {
                ALOGE("CJarFile::ReadMetaEntries failed!");
                break;
            }
            (*metaEntriesMap)[MANIFEST_NAME] = bytes;

            // If there is no verifier then we don't need to look any further.
            if (!verificationRequired) {
                break;
            }
        }
        else if (verificationRequired) {
            // Is this an entry that the verifier needs?
            if (EndsWithIgnoreCase(entryName, String(".SF"))
                || EndsWithIgnoreCase(entryName, String(".DSA"))
                || EndsWithIgnoreCase(entryName, String(".RSA"))
                || EndsWithIgnoreCase(entryName, String(".EC"))) {
                AutoPtr<IInputStream> is;
                zipFile->GetInputStream(entry, (IInputStream**)&is);
                AutoPtr<ArrayOf<Byte> > bytes;
                ECode ec = streams->ReadFully(is, (ArrayOf<Byte>**)&bytes);
                if (FAILED(ec)) {
                    ALOGE("CJarFile::ReadMetaEntries failed!");
                    break;
                }

                (*metaEntriesMap)[entryName.ToUpperCase()] = bytes;
            }
        }
    }

    return metaEntriesMap;
}

Boolean CJarFile::EndsWithIgnoreCase(
    /* [in] */ const String& s,
    /* [in] */ const String& suffix)
{
    return s.RegionMatches(s.GetLength() - suffix.GetLength(), suffix, 0, suffix.GetLength());
}

ECode CJarFile::GetInputStream(
    /* [in] */ IZipEntry* ze,
    /* [out] */ IInputStream** is)
{
    VALIDATE_NOT_NULL(is)
    *is = NULL;
    VALIDATE_NOT_NULL(ze)

    if (mManifestBytes != NULL) {
        AutoPtr<IManifest> m;
        GetManifest((IManifest**)&m);
    }

    if (mVerifier != NULL) {
        Boolean bval;
        if (mVerifier->ReadCertificates(&bval), bval) {
            mVerifier->RemoveMetaEntries();
            ((CManifest*)mManifest.Get())->RemoveChunks();

            if (mVerifier->IsSignedJar(&bval), !bval) {
                mVerifier = NULL;
            }
        }
    }

    AutoPtr<IInputStream> in;
    FAIL_RETURN(ZipFile::GetInputStream(ze, (IInputStream**)&in))
    if (in == NULL) {
        return NOERROR;
    }

    Int64 size;
    if (mVerifier == NULL || (ze->GetSize(&size), size) == -1) {
        *is = in;
        REFCOUNT_ADD(*is)
        return NOERROR;
    }

    AutoPtr<JarVerifier::VerifierEntry> entry;
    String name;
    ze->GetName(&name);
    FAIL_RETURN(mVerifier->InitEntry(name, (JarVerifier::VerifierEntry**)&entry))
    if (entry == NULL) {
        *is = in;
        REFCOUNT_ADD(*is)
        return NOERROR;
    }

    AutoPtr<JarFileInputStream> jfis = new JarFileInputStream();
    jfis->constructor(in, size, entry);
    *is = (IInputStream*)jfis.Get();
    REFCOUNT_ADD(*is)
    return NOERROR;
}

ECode CJarFile::GetEntry(
    /* [in] */ const String& entryName,
    /* [out] */ IZipEntry** entry)
{
    VALIDATE_NOT_NULL(entry)
    AutoPtr<IZipEntry> ze;
    FAIL_RETURN(ZipFile::GetEntry(entryName, (IZipEntry**)&ze));
    if (ze == NULL) {
        *entry = ze;
        REFCOUNT_ADD(*entry)
        return NOERROR;
    }

    AutoPtr<IJarEntry> je;
    FAIL_RETURN(CJarEntry::New((IZipEntry*)ze.Get(), (IJarEntry**)&je));
    ((CJarEntry*)je.Get())->mParentJar = this;
    *entry = IZipEntry::Probe(je);
    REFCOUNT_ADD(*entry)
    return NOERROR;
}

AutoPtr<ArrayOf<IZipEntry*> > CJarFile::GetMetaEntries(
    /* [in] */ IZipFile* zipFile)
{
    List<AutoPtr<IZipEntry> > list;

    AutoPtr<IEnumeration> allEntries;
    zipFile->GetEntries((IEnumeration**)&allEntries);
    Boolean hasMore;
    while (allEntries->HasMoreElements(&hasMore), hasMore) {
        AutoPtr<IInterface> obj;
        allEntries->GetNextElement((IInterface**)&obj);
        AutoPtr<IZipEntry> ze = IZipEntry::Probe(obj);
        String name;
        ze->GetName(&name);
        if (name.StartWith(META_DIR)
                && name.GetLength() > META_DIR.GetLength()) {
            list.PushBack(ze);
        }
    }

    AutoPtr<ArrayOf<IZipEntry*> > array = ArrayOf<IZipEntry*>::Alloc(list.GetSize());
    Int32 i = 0;
    List<AutoPtr<IZipEntry> >::Iterator it;
    for (it = list.Begin(); it != list.End(); ++it) {
        array->Set(i++, *it);
    }
    return array;
}

ECode CJarFile::Close()
{
    FAIL_RETURN(ZipFile::Close())
    mClosed = TRUE;
    return NOERROR;
}

} // namespace Jar
} // namespace Utility
} // namespace Elastos

