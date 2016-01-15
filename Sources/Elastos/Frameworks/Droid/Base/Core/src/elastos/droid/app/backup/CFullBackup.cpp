
#include "elastos/droid/app/backup/CFullBackup.h"
#include "elastos/droid/app/backup/CBackupDataOutput.h"
#include <elastos/utility/logging/Logger.h>
#include <androidfw/BackupHelpers.h>

using Elastos::IO::CFileInputStream;
using Elastos::IO::CFileOutputStream;
using Elastos::IO::IFileInputStream;
using Elastos::IO::IFileOutputStream;
using Elastos::Utility::Logging::Logger;
using Libcore::IO::CLibcore;
using Libcore::IO::ILibcore;
using Libcore::IO::IOs;

namespace Elastos {
namespace Droid {
namespace App {
namespace Backup {

const String CFullBackup::TAG("CFullBackup");

CAR_INTERFACE_IMPL(CFullBackup, Singleton, IFullBackup)

CAR_SINGLETON_IMPL(CFullBackup)

ECode CFullBackup::BackupToTar(
    /* [in] */ const String& packageName,
    /* [in] */ const String& domainName,
    /* [in] */ const String& linkdomain,
    /* [in] */ const String& rootpath,
    /* [in] */ const String& path,
    /* [in] */ IBackupDataOutput* output,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    android::String8 NPackageName(!packageName.IsNull() ? packageName.string() : "");
    android::String8 NRootpath(!rootpath.IsNull() ? rootpath.string() : "");
    android::String8 NPath(!path.IsNull() ? path.string() : "");
    android::String8 NDomain(!domainName.IsNull() ? domainName.string() : "");

    android::BackupDataWriter* writer = (android::BackupDataWriter*)
            ((CBackupDataOutput*)output)->GetNativeBackupDataWriter();

    if (!writer) {
        Logger::E(TAG, "NO output stream provided [%s]", path.string());
        *result = -1;
        return NOERROR;
    }

    if (path.GetLength() < rootpath.GetLength()) {
        Logger::E(TAG, "file path [%s] shorter than root path [%s]", path.string(), rootpath.string());
        *result = -1;
        return NOERROR;
    }

    *result = write_tarfile(NPackageName, NDomain, NRootpath, NPath, writer);
    return NOERROR;
}

ECode CFullBackup::RestoreFile(
    /* [in] */ IParcelFileDescriptor * data,
    /* [in] */ Int64 size,
    /* [in] */ Int32 type,
    /* [in] */ Int64 mode,
    /* [in] */ Int64 mtime,
    /* [in] */ IFile * outFile)
{
    if (type == IBackupAgent::TYPE_DIRECTORY) {
        // Canonically a directory has no associated content, so we don't need to read
        // anything from the pipe in this case.  Just create the directory here and
        // drop down to the metadata adjustment.
        Boolean result;
        if (outFile != NULL) outFile->Mkdirs(&result);
    }
    else {
        AutoPtr<IFileOutputStream> out;

        // Pull the data from the pipe, copying it to the output file, until we're done
        //try {
        if (outFile != NULL) {
            AutoPtr<IFile> parent;
            outFile->GetParentFile((IFile**)&parent);
            Boolean exist;
            if (parent->Exists(&exist), !exist) {
                // in practice this will only be for the default semantic directories,
                // and using the default mode for those is appropriate.
                Boolean result;
                parent->Mkdirs(&result);
            }
            CFileOutputStream::New(outFile, (IFileOutputStream**)&out);
        }
        //} catch (IOException e) {
        //    Log.e(TAG, "Unable to create/open file " + outFile.getPath(), e);
        //}

        AutoPtr<ArrayOf<Byte> > buffer = ArrayOf<Byte>::Alloc(32 * 1024);
        Int64 origSize = size;
        AutoPtr<IFileDescriptor> fd;
        data->GetFileDescriptor((IFileDescriptor**)&fd);
        AutoPtr<IFileInputStream> in;
        CFileInputStream::New(fd, (IFileInputStream**)&in);
        while (size > 0) {
            Int32 toRead = (size > buffer->GetLength()) ? buffer->GetLength() : (Int32)size;
            Int32 got = 0;
            in->ReadBytes(buffer, 0, toRead, &got);
            if (got <= 0) {
                Logger::W(TAG, "Incomplete read: expected:%d, but got:%d", size, (origSize - size));
                break;
            }
            if (out != NULL) {
                //try {
                ECode ec = out->WriteBytes(*buffer, 0, got);
                if (FAILED(ec)) {
                    String path;
                    outFile->GetPath(&path);
                    Logger::E("Unable to write to file %s", path.string());
                    out->Close();
                    Boolean result;
                    outFile->Delete(&result);
                }
                //} catch (IOException e) {
                    // Problem writing to the file.  Quit copying data and delete
                    // the file, but of course keep consuming the input stream.
                //     Log.e(TAG, "Unable to write to file " + outFile.getPath(), e);
                //     out.close();
                //     out = NULL;
                //     outFile.delete();
                // }
            }
            size -= got;
        }
        if (out != NULL) out->Close();
    }

    // Now twiddle the state to match the backup, assuming all went well
    if (mode >= 0 && outFile != NULL) {
        //try {
            // explicitly prevent emplacement of files accessible by outside apps
        mode &= 0700;
        String path;
        outFile->GetPath(&path);
        AutoPtr<ILibcore> libcore;
        CLibcore::AcquireSingleton((ILibcore**)&libcore);
        AutoPtr<IOs> os;
        libcore->GetOs((IOs**)&os);
        os->Chmod(path, (Int32)mode);
        //} catch (ErrnoException e) {
        //    e.rethrowAsIOException();
        //}
        Boolean result;
        outFile->SetLastModified(mtime, &result);
    }
    return NOERROR;
}

} // namespace Backup
} // namespace App
} // namespace Droid
} // namespace Elastos
