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

#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Libcore.h>
#include <Elastos.CoreLibrary.Utility.Zip.h>
#include "elastos/droid/internal/utility/CFileRotator.h"
#include "elastos/droid/os/FileUtils.h"
#include <elastos/core/StringUtils.h>
#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Os::FileUtils;
using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::IO::ICloseable;
using Elastos::IO::CFile;
using Elastos::IO::IFileInputStream;
using Elastos::IO::CFileInputStream;
using Elastos::IO::IBufferedInputStream;
using Elastos::IO::CBufferedInputStream;
using Elastos::IO::CFileOutputStream;
using Elastos::IO::IBufferedOutputStream;
using Elastos::IO::CBufferedOutputStream;
using Elastos::IO::IFlushable;
using Elastos::Utility::Logging::Slogger;
using Elastos::Utility::Zip::IZipOutputStream;
using Elastos::Utility::Zip::CZipOutputStream;
using Elastos::Utility::Zip::IZipEntry;
using Elastos::Utility::Zip::CZipEntry;
using Libcore::IO::IIoUtils;
using Libcore::IO::CIoUtils;
using Libcore::IO::IStreams;
using Libcore::IO::CStreams;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {

CFileRotator::FileInfo::FileInfo(
    /* [in] */ const String& prefix)
    : mStartMillis(0)
    , mEndMillis(0)
{
    assert(!prefix.IsNull());
    mPrefix = prefix;
}

Boolean CFileRotator::FileInfo::Parse(
    /* [in] */ const String& name)
{
    mStartMillis = mEndMillis = -1;

    Int32 dotIndex = name.LastIndexOf('.');
    Int32 dashIndex = name.LastIndexOf('-');

    // skip when missing time section
    if (dotIndex == -1 || dashIndex == -1) return FALSE;

    // skip when prefix doesn't match
    if (!mPrefix.Equals(name.Substring(0, dotIndex))) return FALSE;

    // try {
    mStartMillis = StringUtils::ParseInt64(name.Substring(dotIndex + 1, dashIndex));

    if (name.GetLength() - dashIndex == 1) {
        mEndMillis = Elastos::Core::Math::INT64_MAX_VALUE;
    }
    else {
        mEndMillis = StringUtils::ParseInt64(name.Substring(dashIndex + 1));
    }

    return TRUE;
    // } catch (NumberFormatException e) {
    //     return false;
    // }
}

String CFileRotator::FileInfo::Build()
{
    StringBuilder name(mPrefix);
    name.AppendChar('.');
    name.Append(mStartMillis);
    name.AppendChar('-');
    if (mEndMillis != Elastos::Core::Math::INT64_MAX_VALUE) {
        name.Append(mEndMillis);
    }
    return name.ToString();
}

Boolean CFileRotator::FileInfo::IsActive()
{
    return mEndMillis == Elastos::Core::Math::INT64_MAX_VALUE;
}

CAR_INTERFACE_IMPL_3(CFileRotator::CombineActiveRewriter, Object,
    IFileRotatorReader, IFileRotatorWriter, IFileRotatorRewriter)

CFileRotator::CombineActiveRewriter::CombineActiveRewriter(
    /* [in] */ IFileRotatorReader* reader,
    /* [in] */ IFileRotatorWriter* writer)
    : mReader(reader)
    , mWriter(writer)
{}

ECode CFileRotator::CombineActiveRewriter::Reset()
{
    // ignored
    return NOERROR;
}

ECode CFileRotator::CombineActiveRewriter::Read(
    /* [in] */ IInputStream* in)
{
    return mReader->Read(in);
}

ECode CFileRotator::CombineActiveRewriter::ShouldWrite(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = TRUE;
    return NOERROR;
}

ECode CFileRotator::CombineActiveRewriter::Write(
    /* [in] */ IOutputStream* out)
{
    return mWriter->Write(out);
}

const String CFileRotator::TAG("CFileRotator");
const Boolean CFileRotator::LOGD = FALSE;
const String CFileRotator::SUFFIX_BACKUP(".backup");
const String CFileRotator::SUFFIX_NO_BACKUP(".no_backup");

CAR_INTERFACE_IMPL(CFileRotator, Object, IFileRotator)
CAR_OBJECT_IMPL(CFileRotator)

ECode CFileRotator::constructor(
    /* [in] */ IFile* basePath,
    /* [in] */ const String& prefix,
    /* [in] */ Int64 rotateAgeMillis,
    /* [in] */ Int64 deleteAgeMillis)
{
    if (basePath == NULL || prefix.IsNull())
        return E_NULL_POINTER_EXCEPTION;
    mBasePath = basePath;
    mPrefix = prefix;
    mRotateAgeMillis = rotateAgeMillis;
    mDeleteAgeMillis = deleteAgeMillis;

    // ensure that base path exists
    Boolean result;
    FAIL_RETURN(mBasePath->Mkdirs(&result))

    // recover any backup files
    AutoPtr< ArrayOf<String> > files;
    FAIL_RETURN(mBasePath->List((ArrayOf<String>**)&files))
    for (Int32 i = 0; i < files->GetLength(); ++i) {
        String name = (*files)[0];
        if (!name.StartWith(mPrefix)) continue;

        if (name.EndWith(SUFFIX_BACKUP)) {
            if (LOGD) Slogger::D(TAG, "recovering %s", name.string());

            AutoPtr<IFile> backupFile, file;
            CFile::New(mBasePath, name, (IFile**)&backupFile);
            CFile::New(mBasePath, name.Substring(0, name.GetLength() - SUFFIX_BACKUP.GetLength()), (IFile**)&file);

            // write failed with backup; recover last file
            Boolean result;
            FAIL_RETURN(backupFile->RenameTo(file, &result))
        }
        else if (name.EndWith(SUFFIX_NO_BACKUP)) {
            if (LOGD) Slogger::D(TAG, "recovering %s", name.string());

            AutoPtr<IFile> noBackupFile, file;
            CFile::New(mBasePath, name, (IFile**)&noBackupFile);
            CFile::New(mBasePath, name.Substring(0, name.GetLength() - SUFFIX_NO_BACKUP.GetLength()), (IFile**)&file);

            // write failed without backup; delete both
            Boolean result;
            FAIL_RETURN(noBackupFile->Delete(&result))
            FAIL_RETURN(file->Delete(&result))
        }
    }
    return NOERROR;
}

ECode CFileRotator::DeleteAll()
{
    AutoPtr<FileInfo> info = new FileInfo(mPrefix);
    AutoPtr< ArrayOf<String> > files;
    FAIL_RETURN(mBasePath->List((ArrayOf<String>**)&files))
    for (Int32 i = 0; i < files->GetLength(); ++i) {
        String name = (*files)[i];
        if (info->Parse(name)) {
            // delete each file that matches parser
            AutoPtr<IFile> file;
            CFile::New(mBasePath, name, (IFile**)&file);
            Boolean result;
            FAIL_RETURN(file->Delete(&result));
        }
    }
    return NOERROR;
}

ECode CFileRotator::DumpAll(
    /* [in] */ IOutputStream* os)
{
    AutoPtr<IZipOutputStream> zos;
    CZipOutputStream::New(os, (IZipOutputStream**)&zos);
    // try {
    AutoPtr<FileInfo> info = new FileInfo(mPrefix);
    AutoPtr< ArrayOf<String> > files;
    ECode ec = mBasePath->List((ArrayOf<String>**)&files);
    FAIL_GOTO(ec, failed)
    for (Int32 i = 0; i < files->GetLength(); ++i) {
        String name = (*files)[i];
        if (info->Parse(name)) {
            AutoPtr<IZipEntry> entry;
            CZipEntry::New(name, (IZipEntry**)&entry);
            ec = zos->PutNextEntry(entry);
            FAIL_GOTO(ec, failed);

            AutoPtr<IFile> file;
            CFile::New(mBasePath, name, (IFile**)&file);
            AutoPtr<IFileInputStream> is;
            CFileInputStream::New(file, (IFileInputStream**)&is);
            // try {
            AutoPtr<IStreams> stream;
            CStreams::AcquireSingleton((IStreams**)&stream);
            Int32 result;
            stream->Copy(IInputStream::Probe(is), IOutputStream::Probe(zos), &result);
            AutoPtr<IIoUtils> ioutils;
            CIoUtils::AcquireSingleton((IIoUtils**)&ioutils);
            ioutils->CloseQuietly(ICloseable::Probe(is));
            // } finally {
            //     IoUtils.closeQuietly(is);
            // }

            zos->CloseEntry();
        }
    }
    // } finally {
    //     IoUtils.closeQuietly(zos);
    // }
failed:
    AutoPtr<IIoUtils> ioutils;
    CIoUtils::AcquireSingleton((IIoUtils**)&ioutils);
    ioutils->CloseQuietly(ICloseable::Probe(zos));
    return ec;
}

ECode CFileRotator::RewriteActive(
    /* [in] */ IFileRotatorRewriter* rewriter,
    /* [in] */ Int64 currentTimeMillis)
{
    String activeName = GetActiveName(currentTimeMillis);
    return RewriteSingle(rewriter, activeName);
}

ECode CFileRotator::CombineActive(
    /* [in] */ IFileRotatorReader* reader,
    /* [in] */ IFileRotatorWriter* writer,
    /* [in] */ Int64 currentTimeMillis)
{
    AutoPtr<CombineActiveRewriter> rewriter = new CombineActiveRewriter(reader, writer);
    return RewriteActive(rewriter, currentTimeMillis);
}

ECode CFileRotator::RewriteAll(
    /* [in] */ IFileRotatorRewriter* rewriter)
{
    AutoPtr<FileInfo> info = new FileInfo(mPrefix);
    AutoPtr< ArrayOf<String> > files;
    FAIL_RETURN(mBasePath->List((ArrayOf<String>**)&files))
    for (Int32 i = 0; i < files->GetLength(); ++i) {
        String name = (*files)[i];
        if (!info->Parse(name)) continue;

        // process each file that matches parser
        FAIL_RETURN(RewriteSingle(rewriter, name))
    }
    return NOERROR;
}

ECode CFileRotator::RewriteSingle(
    /* [in] */ IFileRotatorRewriter* rewriter,
    /* [in] */ const String& name)
{
    if (LOGD) Slogger::D(TAG, "rewriting %s", name.string());

    AutoPtr<IFile> file;
    CFile::New(mBasePath, name, (IFile**)&file);
    AutoPtr<IFile> backupFile;

    rewriter->Reset();

    Boolean isExist;
    if (file->Exists(&isExist), isExist) {
        // read existing data
        FAIL_RETURN(ReadFile(file, IFileRotatorReader::Probe(rewriter)))

        // skip when rewriter has nothing to write
        Boolean shouldWrite;
        if (rewriter->ShouldWrite(&shouldWrite), !shouldWrite) return NOERROR;

        // backup existing data during write
        CFile::New(mBasePath, name + SUFFIX_BACKUP, (IFile**)&backupFile);
        Boolean result;
        file->RenameTo(backupFile, &result);

        // try {
        ECode ec = WriteFile(file, IFileRotatorWriter::Probe(rewriter));
        if (FAILED(ec)) {
            // write failed, delete file and restore backup
            file->Delete(&result);
            backupFile->RenameTo(file, &result);
            return E_IO_EXCEPTION;
        }

        // write success, delete backup
        backupFile->Delete(&result);
        // } catch (Throwable t) {
        //     // write failed, delete file and restore backup
        //     file.delete();
        //     backupFile.renameTo(file);
        //     throw rethrowAsIoException(t);
        // }
    }
    else {
        // create empty backup during write
        CFile::New(mBasePath, name + SUFFIX_NO_BACKUP, (IFile**)&backupFile);
        Boolean result;
        backupFile->CreateNewFile(&result);

        // try {
        ECode ec = WriteFile(file, IFileRotatorWriter::Probe(rewriter));
        if (FAILED(ec)) {
            // write failed, delete file and restore backup
            file->Delete(&result);
            backupFile->Delete(&result);
            return E_IO_EXCEPTION;
        }

        // write success, delete empty backup
        backupFile->Delete(&result);
        // } catch (Throwable t) {
        //     // write failed, delete file and empty backup
        //     file.delete();
        //     backupFile.delete();
        //     throw rethrowAsIoException(t);
        // }
    }
    return NOERROR;
}

ECode CFileRotator::ReadMatching(
    /* [in] */ IFileRotatorReader* reader,
    /* [in] */ Int64 matchStartMillis,
    /* [in] */ Int64 matchEndMillis)
{
    AutoPtr<FileInfo> info = new FileInfo(mPrefix);
    AutoPtr< ArrayOf<String> > files;
    FAIL_RETURN(mBasePath->List((ArrayOf<String>**)&files))
    for (Int32 i = 0; i < files->GetLength(); ++i) {
        String name = (*files)[i];
        if (!info->Parse(name)) continue;

        // read file when it overlaps
        if (info->mStartMillis <= matchEndMillis && matchStartMillis <= info->mEndMillis) {
            if (LOGD) Slogger::D(TAG, "reading matching %s", name.string());

            AutoPtr<IFile> file;
            CFile::New(mBasePath, name, (IFile**)&file);
            FAIL_RETURN(ReadFile(file, reader))
        }
    }
    return NOERROR;
}

String CFileRotator::GetActiveName(
    /* [in] */ Int64 currentTimeMillis)
{
    String oldestActiveName;
    Int64 oldestActiveStart = Elastos::Core::Math::INT64_MAX_VALUE;

    AutoPtr<FileInfo> info = new FileInfo(mPrefix);
    AutoPtr< ArrayOf<String> > files;
    ASSERT_SUCCEEDED(mBasePath->List((ArrayOf<String>**)&files))
    for (Int32 i = 0; i < files->GetLength(); ++i) {
        String name = (*files)[i];
        if (!info->Parse(name)) continue;

        // pick the oldest active file which covers current time
        if (info->IsActive() && info->mStartMillis < currentTimeMillis
                && info->mStartMillis < oldestActiveStart) {
            oldestActiveName = name;
            oldestActiveStart = info->mStartMillis;
        }
    }

    if (!oldestActiveName.IsNull()) {
        return oldestActiveName;
    }
    else {
        // no active file found above; create one starting now
        info->mStartMillis = currentTimeMillis;
        info->mEndMillis = Elastos::Core::Math::INT64_MAX_VALUE;
        return info->Build();
    }
}

ECode CFileRotator::MaybeRotate(
    /* [in] */ Int64 currentTimeMillis)
{
    Int64 rotateBefore = currentTimeMillis - mRotateAgeMillis;
    Int64 deleteBefore = currentTimeMillis - mDeleteAgeMillis;

    AutoPtr<FileInfo> info = new FileInfo(mPrefix);
    AutoPtr< ArrayOf<String> > baseFiles;
    FAIL_RETURN(mBasePath->List((ArrayOf<String>**)&baseFiles))
    if (baseFiles == NULL)
        return NOERROR;
    for (Int32 i = 0; i < baseFiles->GetLength(); ++i) {
        String name = (*baseFiles)[i];
        if (!info->Parse(name)) continue;

        if (info->IsActive()) {
            if (info->mStartMillis <= rotateBefore) {
                // found active file; rotate if old enough
                if (LOGD) Slogger::D(TAG, "rotating %s", name.string());

                info->mEndMillis = currentTimeMillis;

                AutoPtr<IFile> file;
                CFile::New(mBasePath, name, (IFile**)&file);
                AutoPtr<IFile> destFile;
                CFile::New(mBasePath, info->Build(), (IFile**)&destFile);
                Boolean result;
                FAIL_RETURN(file->RenameTo(destFile, &result))
            }
        }
        else if (info->mEndMillis <= deleteBefore) {
            // found rotated file; delete if old enough
            if (LOGD) Slogger::D(TAG, "deleting %s", name.string());

            AutoPtr<IFile> file;
            CFile::New(mBasePath, name, (IFile**)&file);
            Boolean result;
            FAIL_RETURN(file->Delete(&result))
        }
    }
    return NOERROR;
}

ECode CFileRotator::ReadFile(
    /* [in] */ IFile* file,
    /* [in] */ IFileRotatorReader* reader)
{
    AutoPtr<IFileInputStream> fis;
    CFileInputStream::New(file, (IFileInputStream**)&fis);
    AutoPtr<IBufferedInputStream> bis;
    CBufferedInputStream::New(IInputStream::Probe(fis), (IBufferedInputStream**)&bis);
    // try {
    ECode ec = reader->Read(IInputStream::Probe(bis));
    // } finally {
    AutoPtr<IIoUtils> ioutils;
    CIoUtils::AcquireSingleton((IIoUtils**)&ioutils);
    ioutils->CloseQuietly(ICloseable::Probe(bis));
    // }
    return ec;
}

ECode CFileRotator::WriteFile(
    /* [in] */ IFile* file,
    /* [in] */ IFileRotatorWriter* writer)
{
    AutoPtr<IFileOutputStream> fos;
    CFileOutputStream::New(file, (IFileOutputStream**)&fos);
    AutoPtr<IBufferedOutputStream> bos;
    CBufferedOutputStream::New(IOutputStream::Probe(fos), (IBufferedOutputStream**)&bos);
    // try {
    ECode ec = writer->Write(IOutputStream::Probe(bos));
    FAIL_GOTO(ec, failed)
    IFlushable::Probe(bos)->Flush();
    // } finally {
    //     FileUtils.sync(fos);
    //     IoUtils.closeQuietly(bos);
    // }
failed:
    FileUtils::Sync(fos);
    AutoPtr<IIoUtils> ioutils;
    CIoUtils::AcquireSingleton((IIoUtils**)&ioutils);
    ioutils->CloseQuietly(ICloseable::Probe(bos));
    return ec;
}

} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos
