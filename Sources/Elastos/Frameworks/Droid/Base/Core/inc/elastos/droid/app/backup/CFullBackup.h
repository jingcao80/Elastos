//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOS_DROID_APP_BACKUP_CFULLBACKUP_H__
#define __ELASTOS_DROID_APP_BACKUP_CFULLBACKUP_H__

#include "_Elastos_Droid_App_Backup_CFullBackup.h"
#include <elastos/core/Singleton.h>

using Elastos::IO::IFile;
using Elastos::Droid::Os::IParcelFileDescriptor;

namespace Elastos {
namespace Droid {
namespace App {
namespace Backup {

CarClass(CFullBackup)
    , public Singleton
    , public IFullBackup
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI BackupToTar(
        /* [in] */ const String& packageName,
        /* [in] */ const String& domainName,
        /* [in] */ const String& linkdomain,
        /* [in] */ const String& rootpath,
        /* [in] */ const String& path,
        /* [in] */ IBackupDataOutput* output,
        /* [out] */ Int32* result);

    /**
     * Copy data from a socket to the given File location on permanent storage.  The
     * modification time and access mode of the resulting file will be set if desired,
     * although group/all rwx modes will be stripped: the restored file will not be
     * accessible from outside the target application even if the original file was.
     * If the {@code type} parameter indicates that the result should be a directory,
     * the socket parameter may be {@code null}; even if it is valid, no data will be
     * read from it in this case.
     * <p>
     * If the {@code mode} argument is negative, then the resulting output file will not
     * have its access mode or last modification time reset as part of this operation.
     *
     * @param data Socket supplying the data to be copied to the output file.  If the
     *    output is a directory, this may be {@code null}.
     * @param size Number of bytes of data to copy from the socket to the file.  At least
     *    this much data must be available through the {@code data} parameter.
     * @param type Must be either {@link BackupAgent#TYPE_FILE} for ordinary file data
     *    or {@link BackupAgent#TYPE_DIRECTORY} for a directory.
     * @param mode Unix-style file mode (as used by the chmod(2) syscall) to be set on
     *    the output file or directory.  group/all rwx modes are stripped even if set
     *    in this parameter.  If this parameter is negative then neither
     *    the mode nor the mtime values will be applied to the restored file.
     * @param mtime A timestamp in the standard Unix epoch that will be imposed as the
     *    last modification time of the output file.  if the {@code mode} parameter is
     *    negative then this parameter will be ignored.
     * @param outFile Location within the filesystem to place the data.  This must point
     *    to a location that is writeable by the caller, prefereably using an absolute path.
     * @throws IOException
     */
    CARAPI RestoreFile(
        /* [in] */ IParcelFileDescriptor * data,
        /* [in] */ Int64 size,
        /* [in] */ Int32 type,
        /* [in] */ Int64 mode,
        /* [in] */ Int64 mtime,
        /* [in] */ IFile * outFile);

private:
    static const String TAG;
};

} // namespace Backup
} // namespace App
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_APP_BACKUP_CFULLBACKUP_H__
