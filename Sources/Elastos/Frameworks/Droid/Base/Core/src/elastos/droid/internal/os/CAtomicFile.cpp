
#include "elastos/droid/ext/frameworkdef.h"
#include <Elastos.CoreLibrary.IO.h>
#include "elastos/droid/internal/os/CAtomicFile.h"
#include "elastos/droid/os/FileUtils.h"

using Elastos::IO::CFile;
using Elastos::IO::CFileInputStream;
using Elastos::IO::CFileOutputStream;
using Elastos::IO::ICloseable;
using Elastos::Droid::Os::FileUtils;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Os {

CAR_INTERFACE_IMPL(CAtomicFile, Object, IAtomicFile)

CAR_OBJECT_IMPL(CAtomicFile)

ECode CAtomicFile::constructor(
    /* [in] */ IFile* baseName)
{
    VALIDATE_NOT_NULL(baseName);

    mBaseName = baseName;
    String name;
    FAIL_RETURN(baseName->GetPath(&name));
    name += ".bak";
    return CFile::New(name, (IFile**)&mBackupName);
}

ECode CAtomicFile::GetBaseFile(
    /* [out] */ IFile** file)
{
    VALIDATE_NOT_NULL(file);
    *file = mBaseName;
    REFCOUNT_ADD(*file);
    return NOERROR;
}

ECode CAtomicFile::StartWrite(
    /* [out] */ IFileOutputStream** stream)
{
    VALIDATE_NOT_NULL(stream);
    *stream = NULL;

    // Rename the current file so it may be used as a backup during the next read
    Boolean b;
    mBaseName->Exists(&b);
    if (b) {
        mBackupName->Exists(&b);
        if (!b) {
            mBaseName->RenameTo(mBackupName, &b);
            if (!b) {
                //Log.w("AtomicFile", "Couldn't rename file " + mBaseName
                //        + " to backup file " + mBackupName);
            }
        }
        else {
            Boolean result;
            mBaseName->Delete(&result);
        }
    }
    if (CFileOutputStream::New(mBaseName, stream) == (ECode)E_FILE_NOT_FOUND_EXCEPTION) {
        AutoPtr<IFile> parent;
        mBaseName->GetParentFile((IFile**)&parent);
        Boolean res;
        parent->Mkdir(&res);
        if (!res) {
            // throw new IOException("Couldn't create directory " + mBaseName);
            return E_IO_EXCEPTION;
        }
        String path;
        parent->GetPath(&path);
        FileUtils::SetPermissions(
            path, FileUtils::sS_IRWXU | FileUtils::sS_IRWXG | FileUtils::sS_IXOTH, -1, -1);
        if (CFileOutputStream::New(mBaseName, stream) == (ECode)E_FILE_NOT_FOUND_EXCEPTION) {
            // throw new IOException("Couldn't create " + mBaseName);
            return E_IO_EXCEPTION;
        }
    }
    return NOERROR;
}

ECode CAtomicFile::FinishWrite(
    /* [in] */ IFileOutputStream* str)
{
    if (str != NULL) {
        Boolean result;
        FileUtils::Sync(str);
        //try {
            ICloseable::Probe(str)->Close();
            mBackupName->Delete(&result);
        //} catch (IOException e) {
        //    Log.w("AtomicFile", "finishWrite: Got exception:", e);
        //}
    }
    return NOERROR;
}

ECode CAtomicFile::FailWrite(
    /* [in] */ IFileOutputStream* str)
{
    if (str != NULL) {
        Boolean result;
        FileUtils::Sync(str);
        //try {
            ICloseable::Probe(str)->Close();
            mBaseName->Delete(&result);
            mBackupName->RenameTo(mBaseName, &result);
        //} catch (IOException e) {
        //    Log.w("AtomicFile", "failWrite: Got exception:", e);
        //}
    }
    return NOERROR;
}

ECode CAtomicFile::OpenAppend(
    /* [out] */ IFileOutputStream** stream)
{
    VALIDATE_NOT_NULL(stream);
    //try {
    return CFileOutputStream::New(mBaseName, TRUE, stream);
    //} catch (FileNotFoundException e) {
    //    throw new IOException("Couldn't append " + mBaseName);
    //}
}

ECode CAtomicFile::Truncate()
{
    //try {
        AutoPtr<IFileOutputStream> fos;
        FAIL_RETURN(CFileOutputStream::New(mBaseName, (IFileOutputStream**)&fos));
        FileUtils::Sync(fos);
        ICloseable::Probe(fos)->Close();
    //} catch (FileNotFoundException e) {
    //    throw new IOException("Couldn't append " + mBaseName);
    //} catch (IOException e) {
    //}
    return NOERROR;
}

ECode CAtomicFile::Exists(
    /* [out] */ Boolean* exists)
{
    VALIDATE_NOT_NULL(exists)
    Boolean e1, e2;
    *exists = (mBaseName->Exists(&e1), e1) || (mBackupName->Exists(&e2), e2);
    return NOERROR;
}

ECode CAtomicFile::Delete()
{
    Boolean result;
    if (mBaseName != NULL) {
        mBaseName->Delete(&result);
    }
    if (mBackupName != NULL) {
        mBackupName->Delete(&result);
    }
    return NOERROR;
}

ECode CAtomicFile::OpenRead(
    /* [out] */ IFileInputStream** stream)
{
    VALIDATE_NOT_NULL(stream);
    Boolean result;
    mBackupName->Exists(&result);
    if (result) {
        mBaseName->Delete(&result);
        mBackupName->RenameTo(mBaseName, &result);
    }
    return CFileInputStream::New(mBaseName, stream);
}

ECode CAtomicFile::ReadFully(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    AutoPtr<IFileInputStream> stream;
    FAIL_RETURN(OpenRead((IFileInputStream**)&stream));

    //try {
    Int32 pos = 0;
    Int32 avail, amt;
    IInputStream::Probe(stream)->Available(&avail);
    AutoPtr<ArrayOf<Byte> > data = ArrayOf<Byte>::Alloc(avail);

    AutoPtr<ArrayOf<Byte> > newData;
    while (TRUE) {
        ECode ec = IInputStream::Probe(stream)->Read(data, pos,data->GetLength() - pos, &amt);
        if (FAILED(ec)) {
            ICloseable::Probe(stream)->Close();
            return ec;
        }
        //Log.i("foo", "Read " + amt + " bytes at " + pos
        //        + " of avail " + data.length);
        if (amt <= 0) {
            //Log.i("foo", "**** FINISHED READING: pos=" + pos
            //        + " len=" + data.length);
            ICloseable::Probe(stream)->Close();
            *result = data;
            REFCOUNT_ADD(*result);
            return NOERROR;
        }

        pos += amt;
        IInputStream::Probe(stream)->Available(&avail);
        if (avail > data->GetLength() - pos) {
            newData = ArrayOf<Byte>::Alloc(pos + avail);
            newData->Copy(data, 0, pos);
            data = newData;
        }
    }

    return NOERROR;
}

}
}
}
}
