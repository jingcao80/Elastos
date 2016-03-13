
#include <Elastos.CoreLibrary.IO.h>
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/utility/CAtomicFile.h"
#include "elastos/droid/os/FileUtils.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::FileUtils;
using Elastos::Utility::Logging::Logger;
using Elastos::IO::CFile;
using Elastos::IO::CFileInputStream;
using Elastos::IO::CFileOutputStream;
using Elastos::IO::ICloseable;

namespace Elastos {
namespace Droid {
namespace Utility {

CAR_INTERFACE_IMPL(CAtomicFile, Object, IAtomicFile)

CAR_OBJECT_IMPL(CAtomicFile)

CAtomicFile::CAtomicFile()
{
}

CAtomicFile::~CAtomicFile()
{
}

ECode CAtomicFile::constructor(
    /* [in] */ IFile* baseName)
{
    VALIDATE_NOT_NULL(baseName);

    mBaseName = baseName;
    String name;
    ECode ec = baseName->GetPath(&name);
    if (FAILED(ec)) {
        Logger::E("CAtomicFile", "failed to GetPath of file %s, ec=%08x", TO_CSTR(baseName), ec);
        return ec;
    }
    name += ".bak";
    return CFile::New(name, (IFile**)&mBackupName);
}

/**
 * Return the path to the base file.  You should not generally use this,
 * as the data at that path may not be valid.
 */
ECode CAtomicFile::GetBaseFile(
    /* [out] */ IFile** file)
{
    VALIDATE_NOT_NULL(file);
    *file = mBaseName;
    REFCOUNT_ADD(*file);
    return NOERROR;
}

/**
 * Delete the atomic file.  This deletes both the base and backup files.
 */
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

/**
 * Start a new write operation on the file.  This returns a FileOutputStream
 * to which you can write the new file data.  The existing file is replaced
 * with the new data.  You <em>must not</em> directly close the given
 * FileOutputStream; instead call either {@link #finishWrite(FileOutputStream)}
 * or {@link #failWrite(FileOutputStream)}.
 *
 * <p>Note that if another thread is currently performing
 * a write, this will simply replace whatever that thread is writing
 * with the new file being written by this thread, and when the other
 * thread finishes the write the new write operation will no longer be
 * safe (or will be lost).  You must do your own threading protection for
 * access to AtomicFile.
 */
ECode CAtomicFile::StartWrite(
    /* [out] */ IFileOutputStream** stream)
{
    VALIDATE_NOT_NULL(stream);
    *stream = NULL;

    // Rename the current file so it may be used as a backup during the next read
    Boolean b;
    FAIL_RETURN(mBaseName->Exists(&b));
    if (b) {
        mBackupName->Exists(&b);
        if (!b) {
            mBaseName->RenameTo(mBackupName, &b);
            if (!b) {
                Logger::W("AtomicFile", "Couldn't rename file %s to backup file %s",
                    TO_CSTR(mBaseName), TO_CSTR(mBackupName));
            }
        } else {
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
            Logger::W("AtomicFile", "Couldn't create directory %s", TO_CSTR(mBaseName));
            return E_IO_EXCEPTION;
        }
        String path;
        parent->GetPath(&path);
        FileUtils::SetPermissions(
            path, FileUtils::sS_IRWXU | FileUtils::sS_IRWXG | FileUtils::sS_IXOTH, -1, -1);
        if (CFileOutputStream::New(mBaseName, stream) == (ECode)E_FILE_NOT_FOUND_EXCEPTION) {
            Logger::W("AtomicFile", "Couldn't create %s", TO_CSTR(mBaseName));
            return E_IO_EXCEPTION;
        }
    }
    return NOERROR;
}

/**
 * Call when you have successfully finished writing to the stream
 * returned by {@link #startWrite()}.  This will close, sync, and
 * commit the new data.  The next attempt to read the atomic file
 * will return the new file stream.
 */
ECode CAtomicFile::FinishWrite(
    /* [in] */ IFileOutputStream* str)
{
    ECode ec = NOERROR;
    if (str != NULL) {
        Boolean result;
        FileUtils::Sync(str);
        //try {
            ICloseable::Probe(str)->Close();
            ec = mBackupName->Delete(&result);
            if (ec == (ECode)E_IO_EXCEPTION) {
                Logger::W("CAtomicFile", "finishWrite: Got E_IO_EXCEPTION");
            }
        //} catch (IOException e) {
        //    Log.w("AtomicFile", "finishWrite: Got exception:", e);
        //}
    }
    return ec;
}

/**
 * Call when you have failed for some reason at writing to the stream
 * returned by {@link #startWrite()}.  This will close the current
 * write stream, and roll back to the previous state of the file.
 */
ECode CAtomicFile::FailWrite(
    /* [in] */ IFileOutputStream* str)
{
    ECode ec = NOERROR;
    if (str != NULL) {
        Boolean result;
        FileUtils::Sync(str);
        //try {
            ICloseable::Probe(str)->Close();
            ec = mBaseName->Delete(&result);
            FAIL_GOTO(ec, _EXIT_)
            ec = mBackupName->RenameTo(mBaseName, &result);
            FAIL_GOTO(ec, _EXIT_)
        //} catch (IOException e) {
        //    Log.w("AtomicFile", "failWrite: Got exception:", e);
        //}
    }

_EXIT_:
    if (ec == (ECode)E_IO_EXCEPTION) {
        Logger::W("CAtomicFile", "failWrite: Got E_IO_EXCEPTION");
    }
    return ec;
}

/** @hide
 * @deprecated This is not safe.
 */
ECode CAtomicFile::Truncate()
{
    ECode ec = NOERROR;
    //try {
    AutoPtr<IFileOutputStream> fos;
    ec = CFileOutputStream::New(mBaseName, (IFileOutputStream**)&fos);
    FAIL_GOTO(ec, _EXIT_)

    FileUtils::Sync(fos);
    ICloseable::Probe(fos)->Close();
    //} catch (FileNotFoundException e) {
    //    throw new IOException("Couldn't append " + mBaseName);
    //} catch (IOException e) {
    //}
_EXIT_:
    if (ec == (ECode)E_FILE_NOT_FOUND_EXCEPTION) {
        Logger::E("CAtomicFile", "Couldn't append %s", TO_CSTR(mBaseName));
        return E_IO_EXCEPTION;
    }
    return NOERROR;
}

/** @hide
 * @deprecated This is not safe.
 */
ECode CAtomicFile::OpenAppend(
    /* [out] */ IFileOutputStream** stream)
{
    VALIDATE_NOT_NULL(stream);
    *stream = NULL;

    //try {
    ECode ec = CFileOutputStream::New(mBaseName, TRUE, stream);
    if (ec == (ECode)E_FILE_NOT_FOUND_EXCEPTION) {
        Logger::E("CAtomicFile", "Couldn't append %s", TO_CSTR(mBaseName));
        return E_IO_EXCEPTION;
    }
    return ec;
    //} catch (FileNotFoundException e) {
    //    throw new IOException("Couldn't append " + mBaseName);
    //}
}

/**
 * Open the atomic file for reading.  If there previously was an
 * incomplete write, this will roll back to the last good data before
 * opening for read.  You should call close() on the FileInputStream when
 * you are done reading from it.
 *
 * <p>Note that if another thread is currently performing
 * a write, this will incorrectly consider it to be in the state of a bad
 * write and roll back, causing the new data currently being written to
 * be dropped.  You must do your own threading protection for access to
 * AtomicFile.
 */
ECode CAtomicFile::OpenRead(
    /* [out] */ IFileInputStream** stream)
{
    VALIDATE_NOT_NULL(stream);
    *stream = NULL;
    Boolean result;
    mBackupName->Exists(&result);
    if (result) {
        mBaseName->Delete(&result);
        mBackupName->RenameTo(mBaseName, &result);
    }
    return CFileInputStream::New(mBaseName, stream);
}

ECode CAtomicFile::GetLastModifiedTime(
    /* [out] */ Int64* time)
{
    VALIDATE_NOT_NULL(time)
    *time = 0;
    Boolean result;
    mBackupName->Exists(&result);
    if (result) {
        return mBackupName->GetLastModified(time);
    }
    return mBaseName->GetLastModified(time);
}

/**
 * A convenience for {@link #openRead()} that also reads all of the
 * file contents into a byte array which is returned.
 */
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

ECode CAtomicFile::Exists(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Boolean bval;
    mBaseName->Exists(&bval);
    if (!bval) {
        mBackupName->Exists(&bval);
    }
    *result = bval;
    return NOERROR;
}

ECode CAtomicFile::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    if (mBaseName != NULL) {
        *str = Object::ToString(mBaseName);
    }
    else {
        *str = Object::ToString(mBackupName);
    }
    return NOERROR;
}

}
}
}
