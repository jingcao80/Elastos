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
#include <Elastos.CoreLibrary.Utility.Zip.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/os/FileUtils.h"
#include <elastos/droid/system/Os.h>
#include <elastos/droid/system/OsConstants.h>
#include <elastos/utility/Arrays.h>
//#include "elastos/droid/text/TextUtils.h"

// #include <sys/errno.h>
// #include <sys/stat.h>
// #include <fcntl.h>

using Elastos::Core::EIID_IComparator;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::IO::IFlushable;
using Elastos::IO::ICloseable;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IFileInputStream;
using Elastos::IO::IOutputStream;
using Elastos::IO::CFileInputStream;
using Elastos::IO::CFileOutputStream;
using Elastos::IO::IBufferedInputStream;
using Elastos::IO::CBufferedInputStream;
using Elastos::IO::IByteArrayOutputStream;
using Elastos::IO::CByteArrayOutputStream;
using Elastos::IO::IFileWriter;
using Elastos::IO::CFileWriter;
using Elastos::IO::CFile;
using Elastos::IO::IWriter;
using Elastos::Utility::Arrays;
using Elastos::Utility::Regex::IPatternHelper;
using Elastos::Utility::Regex::CPatternHelper;
using Elastos::Utility::Regex::IMatcher;
using Elastos::Utility::Zip::IChecksum;
using Elastos::Utility::Zip::ICRC32;
using Elastos::Utility::Zip::CCRC32;
using Elastos::Utility::Zip::ICheckedInputStream;
using Elastos::Utility::Zip::CCheckedInputStream;
using Elastos::Droid::System::IStructStat;
using Elastos::Droid::System::Os;
using Elastos::Droid::System::OsConstants;
//using Elastos::Droid::Text::TextUtils;

namespace Elastos {
namespace Droid {
namespace Os {

static AutoPtr<IPattern> InitSAFE_FILENAME_PATTERN()
{
    const String str("[\\w%+,./=_-]+");
    AutoPtr<IPatternHelper> helper;
    CPatternHelper::AcquireSingleton((IPatternHelper**)&helper);
    AutoPtr<IPattern> pattern;
    helper->Compile(str, (IPattern**)&pattern);
    return pattern;
}

AutoPtr<IPattern> FileUtils::SAFE_FILENAME_PATTERN = InitSAFE_FILENAME_PATTERN();

const Int32 FileUtils::sS_IRWXU;
const Int32 FileUtils::sS_IRUSR;
const Int32 FileUtils::sS_IWUSR;
const Int32 FileUtils::sS_IXUSR;
const Int32 FileUtils::sS_IRWXG;
const Int32 FileUtils::sS_IRGRP;
const Int32 FileUtils::sS_IWGRP;
const Int32 FileUtils::sS_IXGRP;
const Int32 FileUtils::sS_IRWXO;
const Int32 FileUtils::sS_IROTH;
const Int32 FileUtils::sS_IWOTH;
const Int32 FileUtils::sS_IXOTH;

Int32 FileUtils::SetPermissions(
    /* [in] */ IFile* file,
    /* [in] */ Int32 mode,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 gid)
{
    String path;
    if (file) file->GetAbsolutePath(&path);
    return SetPermissions(path, mode, uid, gid);
}

Int32 FileUtils::SetPermissions(
    /* [in] */ const String& path,
    /* [in] */ Int32 mode,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 gid)
{
    // try {
    ECode ec = Elastos::Droid::System::Os::Chmod(path, mode);
    if (FAILED(ec)) {
        return (Int32)ec;
    }
    // } catch (ErrnoException e) {
    //     Slog.w(TAG, "Failed to chmod(" + path + "): " + e);
    //     return e.errno;
    // }

    if (uid >= 0 || gid >= 0) {
        ec = Elastos::Droid::System::Os::Chown(path, uid, gid);
        if (FAILED(ec)) {
            return (Int32)ec;
        }
    }
    return 0;
}

Int32 FileUtils::SetPermissions(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 mode,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 gid)
{
    // try {
    ECode ec = Elastos::Droid::System::Os::Fchmod(fd, mode);
    if (FAILED(ec)) {
        return (Int32)ec;
    }
    // } catch (ErrnoException e) {
    //     Slog.w(TAG, "Failed to chmod(" + path + "): " + e);
    //     return e.errno;
    // }

    if (uid >= 0 || gid >= 0) {
        ec = Elastos::Droid::System::Os::Fchown(fd, uid, gid);
        if (FAILED(ec)) {
            return (Int32)ec;
        }
    }
    return 0;
}

Int32 FileUtils::GetUid(
    /* [in] */ const String& mountPoint)
{
    // try {
    AutoPtr<IStructStat> ss;
    ECode ec = Elastos::Droid::System::Os::Stat(mountPoint, (IStructStat**)&ss);
    if (FAILED(ec)) {
        return -1;
    }

    Int32 result;
    ss->GetUid(&result);
    return result;
    // } catch (ErrnoException e) {
        // return -1;
    // }
}

Boolean FileUtils::Sync(
    /* [in] */ IFileOutputStream* stream)
{
    if (stream != NULL) {
        AutoPtr<IFileDescriptor> fd;
        ECode ec = stream->GetFD((IFileDescriptor**)&fd);
        FAIL_GOTO(ec, _EXIT_)

        ec = fd->Sync();
        FAIL_GOTO(ec, _EXIT_)

        return TRUE;
    }

_EXIT_:
    return FALSE;
}

Boolean FileUtils::CopyFile(
    /* [in] */ IFile* srcFile,
    /* [in] */ IFile* destFile)
{
    Boolean result = FALSE;

    AutoPtr<IInputStream> in;
    ECode ec = CFileInputStream::New(srcFile, (IInputStream**)&in);
    FAIL_GOTO(ec, _EXIT_);
    result = CopyToFile(in, destFile);

_EXIT_:
    if (in)
        in->Close();
    return result;
}

Boolean FileUtils::CopyToFile(
    /* [in] */ IInputStream* inputStream,
    /* [in] */ IFile* destFile)
{
    if (inputStream == NULL || destFile == NULL) {
        return FALSE;
    }

    Boolean result = FALSE;

    Boolean temp;
    destFile->Exists(&temp);
    if (temp) {
        destFile->Delete(&temp);
    }

    AutoPtr<IFileDescriptor> fd;
    AutoPtr<ArrayOf<Byte> > buffer;
    Int32 bytesRead;

    IOutputStream* os;
    AutoPtr<IFileOutputStream> out;
    ECode ec = CFileOutputStream::New(destFile, (IFileOutputStream**)&out);
    FAIL_GOTO(ec, _EXIT_);

    os = IOutputStream::Probe(out);
    buffer = ArrayOf<Byte>::Alloc(4096);
    while (inputStream->Read(buffer, &bytesRead), bytesRead >= 0) {
        os->Write(buffer, 0, bytesRead);
    }

    IFlushable::Probe(out)->Flush();
    out->GetFD((IFileDescriptor**)&fd);
    fd->Sync();

    result = TRUE;

_EXIT_:
    if (out) {
        ICloseable::Probe(out)->Close();
    }

    return result;
}


Boolean FileUtils::IsFilenameSafe(
    /* [in] */ IFile* file)
{
    Boolean result = FALSE;

    // Note, we check whether it matches what's known to be safe,
    // rather than what's known to be unsafe.  Non-ASCII, control
    // characters, etc. are all unsafe by default.
    String filepath;
    file->GetPath(&filepath);
    AutoPtr<IMatcher> matcher;
    SAFE_FILENAME_PATTERN->Matcher(filepath, (IMatcher**)&matcher);
    matcher->Matches(&result);
    return result;
}

ECode FileUtils::ReadTextFile(
    /* [in] */ IFile* file,
    /* [in] */ Int32 max,
    /* [in] */ const String& ellipsis,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = String("");
    VALIDATE_NOT_NULL(file);

    AutoPtr<IInputStream> input;
    AutoPtr<IBufferedInputStream> bis;
    Int64 size;
    Int32 length;
    AutoPtr<ArrayOf<Byte> > data, last, tmp;
    Boolean rolled = FALSE;
    AutoPtr<IByteArrayOutputStream> contents;
    IInputStream* is;
    IOutputStream* os;

    ECode ec = CFileInputStream::New(file, (IInputStream**)&input);
    FAIL_GOTO(ec, _EXIT_);

    // wrapping a BufferedInputStream around it because when reading /proc with unbuffered
    // input stream, bytes read not equal to buffer size is not necessarily the correct
    // indication for EOF; but it is true for BufferedInputStream due to its implementation.
    ec = CBufferedInputStream::New(input, (IBufferedInputStream**)&bis);
    FAIL_GOTO(ec, _EXIT_);

    is = IInputStream::Probe(bis);
    file->GetLength(&size);
    if (max > 0 || (size > 0 && max == 0)) {  // "head" mode: read the first N bytes
        if (size > 0 && (max == 0 || size < max)) max = (Int32) size;
        data = ArrayOf<Byte>::Alloc(max + 1);
        is->Read(data, &length);
        if (length <= 0) {
            *result = String("");
        }
        else if (length <= max) {
            *result = String((const char *)(data->GetPayload()), length);
        }
        else if (ellipsis.IsNull()) {
            *result = String((const char *)(data->GetPayload()), max);
        }
        else {
            *result = String((const char *)(data->GetPayload()), max);
            *result += ellipsis;
        }
    }
    else if (max < 0) {  // "tail" mode: keep the last N
        do {
            if (last != NULL) rolled = true;
            tmp = last; last = data; data = tmp;
            if (data == NULL) data = ArrayOf<Byte>::Alloc(-max);
            is->Read(data, &length);
        } while (length == data->GetLength());

        if (last == NULL && length <= 0) {
            *result = String("");
            goto _EXIT_;
        }
        if (last == NULL) {
            *result = String((const char *)(data->GetPayload()), length);
            goto _EXIT_;
        }

        if (length > 0) {
            rolled = true;
            last->Copy(length, last, 0, last->GetLength() - length);
            last->Copy(last->GetLength() - length, data, 0, length);
//            System.arraycopy(last, length, last, 0, last.length - length);
//            System.arraycopy(data, 0, last, last.length - len, length);
        }

        if (ellipsis == NULL || !rolled) {
            *result = String((const char *)(last->GetPayload()), last->GetLength());
        }
        else {
            *result =  ellipsis;
            *result += String((const char *)(last->GetPayload()), last->GetLength());
        }
    }
    else {  // "cat" mode: size unknown, read it all in streaming fashion
        CByteArrayOutputStream::New((IByteArrayOutputStream**)&contents);
        os = IOutputStream::Probe(contents);
        data = ArrayOf<Byte>::Alloc(1024);
        do {
            is->Read(data, &length);
            if (length > 0) os->Write(data, 0, length);
        } while (length == data->GetLength());

        contents->ToString(result);
    }

_EXIT_:
    if (bis) ICloseable::Probe(bis)->Close();
    if (input) ICloseable::Probe(input)->Close();

    return ec;
}

ECode FileUtils::StringToFile(
    /* [in] */ const String& filename,
    /* [in] */ const String& string)
{
    AutoPtr<IFileWriter> out;
    ECode ec = CFileWriter::New(filename, (IFileWriter**)&out);
    FAIL_GOTO(ec, _EXIT_);

    ec = IWriter::Probe(out)->Write(string);

_EXIT_:
    if (ICloseable::Probe(out))
        ICloseable::Probe(out)->Close();

    return ec;
}

ECode FileUtils::ChecksumCrc32(
    /* [in] */ IFile* file,
    /* [out] */ Int64* summer)
{
    VALIDATE_NOT_NULL(file);
    VALIDATE_NOT_NULL(summer);
    *summer = 0;

    AutoPtr<ArrayOf<Byte> > buf;
    Int32 length;

    AutoPtr<ICRC32> checkSummer;
    CCRC32::New((ICRC32**)&checkSummer);

    AutoPtr<IFileInputStream> input;
    CFileInputStream::New(file, (IFileInputStream**)&input);

    IInputStream* is;
    AutoPtr<ICheckedInputStream> cis;
    ECode ec = CCheckedInputStream::New(
        IInputStream::Probe(input),
        IChecksum::Probe(checkSummer), (ICheckedInputStream**)&cis);
    FAIL_GOTO(ec, _EXIT_);

    is = IInputStream::Probe(cis);
    buf = ArrayOf<Byte>::Alloc(128);

    while(is->Read(buf, &length), length >= 0) {
        // Just read for checksum to get calculated.
    }

    IChecksum::Probe(checkSummer)->GetValue(summer);

_EXIT_:
    if (cis != NULL) {
        ICloseable::Probe(cis)->Close();
    }

    return ec;
}

CAR_INTERFACE_IMPL(FileUtils::FileComparator, Object, IComparator)

ECode FileUtils::FileComparator::Compare(
    /* [in] */ IInterface* inlhs,
    /* [in] */ IInterface* inrhs,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    IFile* lhs = IFile::Probe(inlhs);
    IFile* rhs = IFile::Probe(inrhs);
    if (lhs == NULL && rhs == NULL) {
        *result = 0;
        return NOERROR;
    }

    if (lhs != NULL && rhs == NULL) {
        *result = -1;
        return NOERROR;
    }
    else if (lhs == NULL && rhs != NULL) {
        *result = 1;
        return NOERROR;
    }

    Int64 l, r;
    lhs->GetLastModified(&l);
    rhs->GetLastModified(&r);

    *result = (Int32)(r - l);
    return NOERROR;
}

Boolean FileUtils::DeleteOlderFiles(
    /* [in] */ IFile* dir,
    /* [in] */ Int32 minCount,
    /* [in] */ Int64 minAge)
{
    if (minCount < 0 || minAge < 0) {
        // throw new IllegalArgumentException("Constraints must be positive or 0");
        return FALSE;
    }

    AutoPtr<ArrayOf<IFile*> > files;
    dir->ListFiles((ArrayOf<IFile*>**)&files);
    if (files == NULL) return FALSE;

    // Sort with newest files first
    AutoPtr<IComparator> cmp = new FileComparator();
    Arrays::Sort(files.Get(), cmp.Get());

    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);

    // Keep at least minCount files
    Boolean deleted = FALSE, bval;
    Int64 age, lastModified;
    for (Int32 i = minCount; i < files->GetLength(); i++) {
        IFile* file = (*files)[i];

        // Keep files newer than minAge
        system->GetCurrentTimeMillis(&age);
        file->GetLastModified(&lastModified);
        age -= lastModified;
        if (age > minAge) {
            if (file->Delete(&bval), bval) {
                // Logger::D(TAG, "Deleted old file %s", file);
                deleted = true;
            }
        }
    }
    return deleted;
}

Boolean FileUtils::Contains(
    /* [in] */ IFile* dir,
    /* [in] */ IFile* file)
{
    if (file == NULL) return FALSE;

    String dirPath,filePath;
    dir->GetAbsolutePath(&dirPath);
    file->GetAbsolutePath(&filePath);

    if (dirPath.Equals(filePath)) {
        return TRUE;
    }

    if (!dirPath.EndWith("/")) {
        dirPath += "/";
    }
    return filePath.StartWith(dirPath);
}

Boolean FileUtils::DeleteContents(
    /* [in] */ IFile* dir)
{
    AutoPtr<ArrayOf<IFile*> > files;
    dir->ListFiles((ArrayOf<IFile*>**)&files);

    Boolean success = TRUE;
    if (files != NULL) {
        Boolean bval;
        for (Int32 i = 0; i < files->GetLength(); ++i) {
            IFile* file = (*files)[i];
            if (file->IsDirectory(&bval), bval) {
                success &= DeleteContents(file);
            }
            if (file->Delete(&bval), !bval) {
                //Log.w(TAG, "Failed to delete " + file);
                success = FALSE;
            }
        }
    }
    return success;
}

Boolean FileUtils::IsValidExtFilenameChar(
        /* [in] */ Char32 c)
{
    switch (c) {
        case '\0':
        case '/':
            return FALSE;
        default:
            return TRUE;
    }
}

Boolean FileUtils::IsValidExtFilename(
    /* [in] */ const String& name)
{
    return !name.IsNull() && name.Equals(BuildValidExtFilename(name));
}

String FileUtils::BuildValidExtFilename(
    /* [in] */ const String& name)
{
    if (name.IsEmpty() || name.Equals(String(".")) || name.Equals(String(".."))) {
        return String("(invalid)");
    }

    Int32 length = name.GetLength();
    StringBuilder res(length);
    for (Int32 i = 0; i < length; i++) {
        Char32 c = name.GetChar(i);
        if (IsValidExtFilenameChar(c)) {
            res.AppendChar(c);
        }
        else {
            res.AppendChar('_');
        }
    }
    return res.ToString();
}

Boolean FileUtils::IsValidFatFilenameChar(
    /* [in] */ Char32 c)
{
    if (0x00 <= c && c <= 0x1f) {
        return FALSE;
    }
    switch (c) {
        case '"':
        case '*':
        case '/':
        case ':':
        case '<':
        case '>':
        case '?':
        case '\\':
        case '|':
        case 0x7F:
            return FALSE;
        default:
            return TRUE;
    }
}

Boolean FileUtils::IsValidFatFilename(
    /* [in] */ const String& name)
{
    return !name.IsNull() && name.Equals(BuildValidFatFilename(name));
}

String FileUtils::BuildValidFatFilename(
    /* [in] */ const String& name)
{
    if (name.IsEmpty() || name.Equals(String(".")) || name.Equals(String(".."))) {
        return String("(invalid)");
    }

    Int32 length = name.GetLength();
    StringBuilder res(length);
    for (Int32 i = 0; i < length; i++) {
        Char32 c = name.GetChar(i);
        if (IsValidFatFilenameChar(c)) {
            res.AppendChar(c);
        }
        else {
            res.AppendChar('_');
        }
    }
    return res.ToString();
}

String FileUtils::RewriteAfterRename(
    /* [in] */ IFile* beforeDir,
    /* [in] */ IFile* afterDir,
    /* [in] */ const String& path)
{
    if (path.IsNull()) return String(NULL);

    AutoPtr<IFile> file;
    CFile::New(path, (IFile**)&file);
    AutoPtr<IFile> result = RewriteAfterRename(beforeDir, afterDir, file);
    String str;
    if (result) {
        result->GetAbsolutePath(&str);
    }
    return str;
}

AutoPtr<ArrayOf<String> > FileUtils::RewriteAfterRename(
    /* [in] */ IFile* beforeDir,
    /* [in] */ IFile* afterDir,
    /* [in] */ ArrayOf<String>* paths)
{
    if (paths == NULL) return NULL;
    AutoPtr<ArrayOf<String> > result = ArrayOf<String>::Alloc(paths->GetLength());
    for (Int32 i = 0; i < paths->GetLength(); i++) {
        result->Set(i, RewriteAfterRename(beforeDir, afterDir, (*paths)[i]));
    }
    return result;
}

AutoPtr<IFile> FileUtils::RewriteAfterRename(
    /* [in] */ IFile* beforeDir,
    /* [in] */ IFile* afterDir,
    /* [in] */ IFile* file)
{
    if (file == NULL) return NULL;
    if (Contains(beforeDir, file)) {
        String ap, bap;
        file->GetAbsolutePath(&ap);
        beforeDir->GetAbsolutePath(&bap);
        String splice = ap.Substring(bap.GetLength());

        AutoPtr<IFile> result;
        CFile::New(afterDir, splice, (IFile**)&result);
        return result;
    }
    return NULL;
}


} // namespace Os
} // namespace Droid
} // namespace Elastos
