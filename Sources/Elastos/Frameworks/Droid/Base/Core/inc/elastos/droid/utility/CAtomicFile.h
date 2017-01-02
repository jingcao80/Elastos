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

#ifndef __ELASTOS_DROID_UTILITY_CATOMICFILE_H__
#define __ELASTOS_DROID_UTILITY_CATOMICFILE_H__

#include "_Elastos_Droid_Utility_CAtomicFile.h"
#include "elastos/core/Object.h"

using Elastos::IO::IFile;
using Elastos::IO::IFileOutputStream;
using Elastos::IO::IFileInputStream;

namespace Elastos {
namespace Droid {
namespace Utility {

CarClass(CAtomicFile)
    , public Object
    , public IAtomicFile
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CAtomicFile();

    ~CAtomicFile();

    CARAPI constructor(
        /* [in] */ IFile* baseName);

    /**
     * Return the path to the base file.  You should not generally use this,
     * as the data at that path may not be valid.
     */
    CARAPI GetBaseFile(
        /* [out] */ IFile** file);

    /**
     * Delete the atomic file.  This deletes both the base and backup files.
     */
    CARAPI Delete();

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
    CARAPI StartWrite(
        /* [out] */ IFileOutputStream** stream);

    /**
     * Call when you have successfully finished writing to the stream
     * returned by {@link #startWrite()}.  This will close, sync, and
     * commit the new data.  The next attempt to read the atomic file
     * will return the new file stream.
     */
    CARAPI FinishWrite(
        /* [in] */ IFileOutputStream* str);

    /**
     * Call when you have failed for some reason at writing to the stream
     * returned by {@link #startWrite()}.  This will close the current
     * write stream, and roll back to the previous state of the file.
     */
    CARAPI FailWrite(
        /* [in] */ IFileOutputStream* str);

    /** @hide
     * @deprecated This is not safe.
     */
    CARAPI Truncate();

    /** @hide
     * @deprecated This is not safe.
     */
    CARAPI OpenAppend(
        /* [out] */ IFileOutputStream** stream);

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
    CARAPI OpenRead(
        /* [out] */ IFileInputStream** stream);

   /**
     * Gets the last modified time of the atomic file.
     * {@hide}
     *
     * @return last modified time in milliseconds since epoch.
     * @throws IOException
     */
    CARAPI GetLastModifiedTime(
        /* [out] */ Int64* time);

    /**
     * A convenience for {@link #openRead()} that also reads all of the
     * file contents into a byte array which is returned.
     */
    CARAPI ReadFully(
        /* [out] */ ArrayOf<Byte>** data);

    CARAPI Exists(
        /* [out] */ Boolean* result);

    CARAPI ToString(
        /* [out] */ String* str);

public:
    AutoPtr<IFile> mBaseName;
    AutoPtr<IFile> mBackupName;
};

} // Utility
} // Droid
} // Elastos

#endif // __ELASTOS_DROID_UTILITY_CATOMICFILE_H__
