
#include "File.h"
#include "CFile.h"
#include "IoUtils.h"
#include "ToStringArray.h"
#include <elastos/utility/etl/List.h>
#include <elastos/utility/etl/Vector.h>
#include "StringBuilder.h"
#include "CSystem.h"
#include "CRandom.h"
#include "OsConstants.h"
#include "CLibcore.h"
#include "DeleteOnExit.h"
#include "url/UrlUtils.h"
#include "CURL.h"
#include "CURI.h"

#include <dirent.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <utime.h>
#include <unistd.h>

using Elastos::Utility::Etl::List;
using Elastos::Utility::Etl::Vector;
using Elastos::Utility::CRandom;
using Elastos::Core::StringBuilder;
using Elastos::Core::CSystem;;
using Elastos::Core::ISystem;
using Elastos::Core::EIID_IComparable;
using Elastos::Net::CURL;
using Elastos::Net::CURI;
using Elastos::Droid::System::OsConstants;
using Elastos::Droid::System::IStructStat;
// using Elastos::Droid::System::IStructStatFs;
using Elastos::Droid::System::IStructStatVfs;
using Libcore::IO::ILibcore;
using Libcore::IO::CLibcore;
using Libcore::IO::IoUtils;
using Libcore::IO::IOs;
using Libcore::IO::DeleteOnExit;
using Libcore::IO::IDeleteOnExit;
using Libcore::Net::Url::UrlUtils;

namespace Elastos {
namespace IO {

CAR_INTERFACE_IMPL_3(File, Object, IFile, IComparable, ISerializable)

// static {
    // separatorChar = System.getProperty("file.separator", "/").charAt(0);
    // pathSeparatorChar = System.getProperty("path.separator", ":").charAt(0);
    // separator = String.valueOf(separatorChar);
    // pathSeparator = String.valueOf(pathSeparatorChar);
// }

const Char32 File::sSeparatorChar = '/';
const Char32 File::sPathSeparatorChar = ':';
const String File::sSeparator = String("/");
const String File::sPathSeparator = String(":");

AutoPtr<IRandom> File::mTempFileRandom ;

AutoPtr<IRandom> File::GetTempFileRandom()
{
    if (mTempFileRandom == NULL) {
        AutoPtr<CRandom> random;
        CRandom::NewByFriend((CRandom**)&random);
        mTempFileRandom = (IRandom*)random.Get();
    }

    return mTempFileRandom;
}

File::File()
{}

File::~File()
{}

ECode File::constructor(
    /* [in] */ IFile* dir,
    /* [in] */ const String& name)
{
    String path;
    if (dir != NULL) {
        dir->GetPath(&path);
    }
    return constructor(path, name);
}

ECode File::constructor(
    /* [in] */ const String& path)
{
    mPath = FixSlashes(path);
    return NOERROR;
}

ECode File::constructor(
    /* [in] */ const String& dirPath,
    /* [in] */ const String& name)
{
    if (name.IsNull()) {
        // throw new NullPointerException("name == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    if (dirPath.IsNullOrEmpty()) {
        mPath = FixSlashes(name);
    }
    else if (name.IsEmpty()) {
        mPath = FixSlashes(dirPath);
    }
    else {
        mPath = FixSlashes(Join(dirPath, name));
    }
    return NOERROR;
}

ECode File::constructor(
    /* [in] */ IURI* uri)
{
     // check pre-conditions
     FAIL_RETURN(CheckURI(uri));
     String path;
     uri->GetPath(&path);
     mPath = FixSlashes(path);
     return NOERROR;
}

String File::FixSlashes(
    /* [in] */ const String& origPath)
{
    // Remove duplicate adjacent slashes.
    Boolean lastWasSlash = FALSE;
    AutoPtr< ArrayOf<Char32> > newPath = origPath.GetChars();
    Int32 length = newPath->GetLength();
    Int32 newLength = 0;
    Char32 ch;
    for (Int32 i = 0; i < length; ++i) {
        ch = (*newPath)[i];
        if (ch == '/') {
            if (!lastWasSlash) {
                (*newPath)[newLength++] = sSeparatorChar;
                lastWasSlash = TRUE;
            }
        }
        else {
            (*newPath)[newLength++] = ch;
            lastWasSlash = FALSE;
        }
    }
    // Remove any trailing slash (unless this is the root of the file system).
    if (lastWasSlash && newLength > 1) {
        newLength--;
    }
    // Reuse the original string if possible.
    return (newLength != length) ? String(*newPath.Get(), 0, newLength) : origPath;
}

String File::Join(
    /* [in] */ const String& prefix,
    /* [in] */ const String& suffix)
{
    Int32 prefixLength = prefix.GetLength();
    Boolean haveSlash = (prefixLength > 0 && prefix.GetChar(prefixLength - 1) == sSeparatorChar);
    if (!haveSlash) {
        haveSlash = (suffix.GetLength() > 0 && suffix.GetChar(0) == sSeparatorChar);
    }
    String joinStr = prefix;
    if (!haveSlash) joinStr.AppendFormat("%c", sSeparatorChar);
    joinStr += suffix;
    return joinStr;
}

ECode File::CheckURI(
    /* [in] */ IURI* uri)
{
    Boolean isAbsolute = FALSE;
    uri->IsAbsolute(&isAbsolute);
    String schemeSpecific;
    uri->GetSchemeSpecificPart(&schemeSpecific);
    if (!isAbsolute) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    else if (!schemeSpecific.StartWith("/")) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    String scheme;
    uri->GetScheme(&scheme);
    if (!String("file").Equals(scheme)) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    String rawPath;
    uri->GetRawPath(&rawPath);
    if (rawPath.IsNull() || rawPath.IsEmpty()) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    String authority;
    uri->GetRawAuthority(&authority);
    if (!authority.IsNull()) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    String query;
    uri->GetRawQuery(&query);
    if (!query.IsNull()) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    String fragment;
    uri->GetRawFragment(&fragment);
    if (!fragment.IsNull()) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode File::ListRoots(
    /* [out, callee] */ ArrayOf<IFile*>** roots)
{
    VALIDATE_NOT_NULL(roots)
    AutoPtr<IFile> file;
    CFile::New(String("/"), (IFile**)&file);
    AutoPtr< ArrayOf<IFile*> > files = ArrayOf<IFile*>::Alloc(1);
    files->Set(0, file);
    *roots = files;
    REFCOUNT_ADD(*roots);
    return NOERROR;
}

ECode File::CanExecute(
    /* [out] */ Boolean* canExecute)
{
    VALIDATE_NOT_NULL(canExecute)
    *canExecute = DoAccess(OsConstants::_X_OK);
    return NOERROR;
}

ECode File::CanRead(
    /* [out] */ Boolean* canRead)
{
    VALIDATE_NOT_NULL(canRead)
    *canRead = DoAccess(OsConstants::_R_OK);
    return NOERROR;
}

ECode File::CanWrite(
    /* [out] */ Boolean* canWrite)
{
    VALIDATE_NOT_NULL(canWrite)
    *canWrite = DoAccess(OsConstants::_W_OK);
    return NOERROR;
}

Boolean File::DoAccess(
    /* [in] */ Int32 mode)
{
    // try {
    AutoPtr<ILibcore> libcore;
    CLibcore::AcquireSingleton((ILibcore**)&libcore);
    AutoPtr<IOs> os;
    libcore->GetOs((IOs**)&os);
    Boolean result;
    if (SUCCEEDED(os->Access(mPath, mode, &result))) {
        return result;
    }
    return FALSE;
    // } catch (ErrnoException errnoException) {
    //     return false;
    // }
}

ECode File::CompareTo(
    /* [in] */ IInterface* obj,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = -1;
    VALIDATE_NOT_NULL(obj)

    IFile* afile = IFile::Probe(obj);
    if (afile == NULL) return NOERROR;

    String anotherPath;
    afile->GetPath(&anotherPath);
    //todo: the result of String.Compare() maybe not correct;
    *result = mPath.Compare(anotherPath);
    return NOERROR;
}

ECode File::CompareTo(
    /* [in] */ IFile* another,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = -1;
    VALIDATE_NOT_NULL(another)

    IFile* afile = IFile::Probe(another);
    if (afile == NULL) return E_INVALID_ARGUMENT;

    String anotherPath;
    afile->GetPath(&anotherPath);
    *result = mPath.Compare(anotherPath);
    return NOERROR;
}

ECode File::Delete(
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded)

    // try {
    AutoPtr<ILibcore> libcore;
    CLibcore::AcquireSingleton((ILibcore**)&libcore);
    AutoPtr<IOs> os;
    libcore->GetOs((IOs**)&os);
    if (SUCCEEDED(os->Remove(mPath))) {
        *succeeded = TRUE;
        return NOERROR;
    }
    *succeeded = FALSE;
    return NOERROR;
    // } catch (ErrnoException errnoException) {
    //     return false;
    // }
}

ECode File::Delete()
{
    Boolean succeeded;
    return Delete(&succeeded);
}

ECode File::DeleteOnExit()
{
    AutoPtr<IDeleteOnExit> doe;
    DeleteOnExit::GetInstance((IDeleteOnExit**)&doe);
    String path;
    GetAbsolutePath(&path);
    return doe->AddFile(path);
}

ECode File::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* isEqual)
{
    VALIDATE_NOT_NULL(isEqual);
    *isEqual = FALSE;
    VALIDATE_NOT_NULL(obj);

    IFile* file = IFile::Probe(obj);
    if (file == NULL) return NOERROR;

    String path;
    file->GetPath(&path);
    *isEqual = mPath.Equals(path);
    return NOERROR;
}

ECode File::Exists(
    /* [out] */ Boolean* isExist)
{
    VALIDATE_NOT_NULL(isExist)
    *isExist = DoAccess(OsConstants::_F_OK);
    return NOERROR;
}

ECode File::GetAbsolutePath(
    /* [out] */ String* path)
{
    VALIDATE_NOT_NULL(path);
    *path = NULL;

    Boolean isAbsolute;
    if (IsAbsolute(&isAbsolute), isAbsolute) {
        *path = mPath;
        return NOERROR;
    }

    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);

    String userDir;
    system->GetProperty(String("user.dir"), &userDir);
    *path = mPath.IsNullOrEmpty() ? userDir : Join(userDir, mPath);
    return NOERROR;
}

ECode File::GetAbsoluteFile(
    /* [out] */ IFile** file)
{
    VALIDATE_NOT_NULL(file)

    String absolutePath;
    GetAbsolutePath(&absolutePath);
    return CFile::New(absolutePath, file);
}

ECode File::GetCanonicalPath(
    /* [out] */ String* path)
{
    VALIDATE_NOT_NULL(path)

    String absolutePath;
    GetAbsolutePath(&absolutePath);
    *path = UrlUtils::CanonicalizePath(absolutePath, TRUE);
    return NOERROR;
}

ECode File::GetCanonicalFile(
    /* [out] */ IFile** file)
{
    VALIDATE_NOT_NULL(file)
    String path;
    GetCanonicalPath(&path);
    return CFile::New(path, file);
}

ECode File::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)
    Int32 separatorIndex = mPath.LastIndexOf(sSeparator);
    *name = (separatorIndex < 0) ? mPath : mPath.Substring(separatorIndex + 1, mPath.GetLength());
    return NOERROR;
}

ECode File::GetParent(
    /* [out] */ String* parent)
{
    VALIDATE_NOT_NULL(parent)
    Int32 length = mPath.GetLength(), firstInPath = 0;
    if (sSeparatorChar == '\\' && length > 2 && mPath.GetChar(1) == ':') {
        firstInPath = 2;
    }
    Int32 index = mPath.LastIndexOf(sSeparatorChar);
    if (index == -1 && firstInPath > 0) {
        index = 2;
    }
    if (index == -1 || mPath.GetChar(length - 1) == sSeparatorChar) {
        *parent = NULL;
        return NOERROR;
    }
    if (mPath.IndexOf(sSeparatorChar) == index
            && mPath.GetChar(firstInPath) == sSeparatorChar) {
        *parent = mPath.Substring(0, index + 1);
        return NOERROR;
    }
    *parent = mPath.Substring(0, index);
    return NOERROR;
}

ECode File::GetParentFile(
    /* [out] */ IFile** file)
{
    VALIDATE_NOT_NULL(file)
    String tempParent;
    GetParent(&tempParent);
    if (tempParent.IsNull()) {
        *file = NULL;
        return NOERROR;
    }
    return CFile::New(tempParent, file);
}

ECode File::GetPath(
    /* [out] */ String* path)
{
    VALIDATE_NOT_NULL(path)
    *path = mPath;
    return NOERROR;
}

ECode File::GetHashCode(
    /* [out] */ Int32* code)
{
    VALIDATE_NOT_NULL(code)
    *code = mPath.GetHashCode() ^ 1234321;
    return NOERROR;
}

ECode File::IsAbsolute(
    /* [out] */ Boolean* isAbsolute)
{
    VALIDATE_NOT_NULL(isAbsolute)
    *isAbsolute = mPath.GetLength() > 0 && mPath.GetChar(0) == sSeparatorChar;
    return NOERROR;
}

ECode File::IsDirectory(
    /* [out] */ Boolean* isDirectory)
{
    VALIDATE_NOT_NULL(isDirectory)

    // try {
    AutoPtr<ILibcore> libcore;
    CLibcore::AcquireSingleton((ILibcore**)&libcore);
    AutoPtr<IOs> os;
    libcore->GetOs((IOs**)&os);
    AutoPtr<IStructStat> stat;
    if (FAILED(os->Stat(mPath, (IStructStat**)&stat))) {
        *isDirectory = FALSE;
        return NOERROR;
    }
    Int32 mode;
    stat->GetMode(&mode);
    *isDirectory = OsConstants::S_IsDIR(mode);
    return NOERROR;
    // } catch (ErrnoException errnoException) {
    //     // The RI returns false on error. (Even for errors like EACCES or ELOOP.)
    //     return false;
    // }
}

ECode File::IsFile(
    /* [out] */ Boolean* isFile)
{
    VALIDATE_NOT_NULL(isFile)
    // try {
    AutoPtr<ILibcore> libcore;
    CLibcore::AcquireSingleton((ILibcore**)&libcore);
    AutoPtr<IOs> os;
    libcore->GetOs((IOs**)&os);
    AutoPtr<IStructStat> stat;
    if (FAILED(os->Stat(mPath, (IStructStat**)&stat))) {
        *isFile = FALSE;
        return NOERROR;
    }
    Int32 mode;
    stat->GetMode(&mode);
    *isFile = OsConstants::S_IsREG(mode);
    return NOERROR;
    // } catch (ErrnoException errnoException) {
    //     // The RI returns false on error. (Even for errors like EACCES or ELOOP.)
    //     return false;
    // }
}

ECode File::IsHidden(
    /* [out] */ Boolean* isHidden)
{
    VALIDATE_NOT_NULL(isHidden)
    if (mPath.IsEmpty()) {
        *isHidden = FALSE;
        return NOERROR;
    }
    String name;
    GetName(&name);
    *isHidden = name.StartWith(".");
    return NOERROR;
}

ECode File::GetLastModified(
    /* [out] */ Int64* time)
{
    VALIDATE_NOT_NULL(time)
    // try {
    AutoPtr<ILibcore> libcore;
    CLibcore::AcquireSingleton((ILibcore**)&libcore);
    AutoPtr<IOs> os;
    libcore->GetOs((IOs**)&os);
    AutoPtr<IStructStat> stat;
    if (FAILED(os->Stat(mPath, (IStructStat**)&stat))) {
        *time = 0;
        return NOERROR;
    }
    Int64 mtime;
    stat->GetMtime(&mtime);
    *time = mtime * 1000LL;
    return NOERROR;
    // } catch (ErrnoException errnoException) {
    //     // The RI returns 0 on error. (Even for errors like EACCES or ELOOP.)
    //     return 0;
    // }
}

ECode File::SetLastModified(
    /* [in] */ Int64 time,
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded)
    if (time < 0) {
//        throw new IllegalArgumentException("time < 0");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    *succeeded = SetLastModifiedImpl(mPath, time);
    return NOERROR;
}

Boolean File::SetLastModifiedImpl(
    /* [in] */ const String& path,
    /* [in] */ Int64 ms)
{
    if (path.IsNull()) {
        return FALSE;
    }

    // We want to preserve the access time.
    struct stat sb;
    if (stat(path.string(), &sb) == -1) {
        return FALSE;
    }

    // TODO: we could get microsecond resolution with utimes(3), "legacy" though it is.
    utimbuf times;
    times.actime = sb.st_atime;
    times.modtime = static_cast<time_t>(ms / 1000);
    return (utime(path.string(), &times) == 0);
}

ECode File::SetReadOnly(
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded)

    return SetWritable(FALSE, FALSE, succeeded);
}

ECode File::SetExecutable(
    /* [in] */ Boolean executable,
    /* [in] */ Boolean ownerOnly,
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded)
    *succeeded = DoChmod(
        ownerOnly ? OsConstants::_S_IXUSR
            : (OsConstants::_S_IXUSR | OsConstants::_S_IXGRP | OsConstants::_S_IXOTH)
            , executable);
    return NOERROR;
}

ECode File::SetExecutable(
    /* [in] */ Boolean executable,
    /* [out] */ Boolean* succeeded)
{
    return SetExecutable(executable, TRUE, succeeded);
}

ECode File::SetReadable(
    /* [in] */ Boolean readable,
    /* [in] */ Boolean ownerOnly,
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded)
    *succeeded = DoChmod(ownerOnly ? OsConstants::_S_IRUSR
        : (OsConstants::_S_IRUSR | OsConstants::_S_IRGRP | OsConstants::_S_IROTH)
        , readable);
    return NOERROR;
}

ECode File::SetReadable(
    /* [in] */ Boolean readable,
    /* [out] */ Boolean* succeeded)
{
    return SetReadable(readable, TRUE, succeeded);
}

ECode File::SetWritable(
    /* [in] */ Boolean writable,
    /* [in] */ Boolean ownerOnly,
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded)
    *succeeded = DoChmod(ownerOnly ? OsConstants::_S_IWUSR
        : (OsConstants::_S_IWUSR | OsConstants::_S_IWGRP | OsConstants::_S_IWOTH)
        , writable);
    return NOERROR;
}

ECode File::SetWritable(
    /* [in] */ Boolean writable,
    /* [out] */ Boolean* succeeded)
{
    return SetWritable(writable, TRUE, succeeded);
}

Boolean File::DoChmod(
    /* [in] */ Int32 mask,
    /* [in] */ Boolean set)
{
    // try {
    AutoPtr<ILibcore> libcore;
    CLibcore::AcquireSingleton((ILibcore**)&libcore);
    AutoPtr<IOs> os;
    libcore->GetOs((IOs**)&os);
    AutoPtr<IStructStat> stat;
    if (FAILED(os->Stat(mPath, (IStructStat**)&stat))) {
        return FALSE;
    }
    Int32 mode;
    stat->GetMode(&mode);
    Int32 newMode = set ? (mode | mask) : (mode & ~mask);
    if (FAILED(os->Chmod(mPath, newMode))) {
        return FALSE;
    }
    return TRUE;
    // } catch (ErrnoException errnoException) {
    //     return false;
    // }
}

ECode File::GetLength(
    /* [out] */ Int64* length)
{
    VALIDATE_NOT_NULL(length)
    // try {
    AutoPtr<ILibcore> libcore;
    CLibcore::AcquireSingleton((ILibcore**)&libcore);
    AutoPtr<IOs> os;
    libcore->GetOs((IOs**)&os);
    AutoPtr<IStructStat> stat;
    if (FAILED(os->Stat(mPath, (IStructStat**)&stat))) {
        *length = 0;
        return NOERROR;
    }
    Int64 size;
    stat->GetSize(&size);
    *length = size;
    return NOERROR;
    // } catch (ErrnoException errnoException) {
    //     // The RI returns 0 on error. (Even for errors like EACCES or ELOOP.)
    //     return 0;
    // }
}

ECode File::List(
    /* [out, callee] */ ArrayOf<String>** files)
{
    VALIDATE_NOT_NULL(files)
    AutoPtr< ArrayOf<String> > arr = ListImpl(mPath);
    *files = arr;
    REFCOUNT_ADD(*files);
    return NOERROR;
}

// Iterates over the filenames in the given directory.
class ScopedReaddir
{
public:
    ScopedReaddir(const char* path)
    {
        mDirStream = opendir(path);
        mIsBad = (mDirStream == NULL);
    }

    ~ScopedReaddir()
    {
        if (mDirStream != NULL) {
            closedir(mDirStream);
        }
    }

    // Returns the next filename, or NULL.
    const char* next()
    {
        if (mIsBad) {
            return NULL;
        }
        errno = 0;
        dirent* result = readdir(mDirStream);
        if (result != 0) {
            return result->d_name;
        }
        if (errno != 0) {
            mIsBad = true;
        }
        return NULL;
    }

    // Has an error occurred on this stream?
    Boolean isBad() const
    {
        return mIsBad;
    }

private:
    DIR* mDirStream;
    Boolean mIsBad;

    // Disallow copy and assignment.
    ScopedReaddir(const ScopedReaddir&);
    void operator=(const ScopedReaddir&);
};

typedef Vector<String> DirEntries;

// Reads the directory referred to by 'pathBytes', adding each directory entry
// to 'entries'.
static Boolean ReadDirectory(const String& path, DirEntries& entries)
{
    if (path.IsNull()) {
        return FALSE;
    }

    ScopedReaddir dir(path.string());
    const char* filename;
    while ((filename = dir.next()) != NULL) {
        if (strcmp(filename, ".") != 0 && strcmp(filename, "..") != 0) {
            // TODO: this hides allocation failures from us. Push directory iteration up into Java?
            entries.PushBack(String(filename));
        }
    }
    return !dir.isBad();
}

AutoPtr< ArrayOf<String> > File::ListImpl(
    /* [in] */ const String& path)
{
    // Read the directory entries into an intermediate form.
    DirEntries entries;
    if (!ReadDirectory(path, entries)) {
        return NULL;
    }
    // Translate the intermediate form into a Java String[].
    return Libcore::IO::ToStringArray(entries);
}

ECode File::List(
    /* [in] */ IFilenameFilter* filter,
    /* [out, callee] */ ArrayOf<String>** files)
{
    VALIDATE_NOT_NULL(files)
    *files = NULL;
    VALIDATE_NOT_NULL(filter)

    AutoPtr< ArrayOf<String> > filenames = ListImpl(mPath);
    if (filter == NULL || filenames == NULL) {
        *files = filenames;
        REFCOUNT_ADD(*files);
        return NOERROR;
    }
    Elastos::Utility::Etl::List<String> result;
    for (Int32 i = 0; i < filenames->GetLength(); ++i) {
        String filename = (*filenames)[i];
        Boolean isAccept;
        if (filter->Accept(this,
                filename, &isAccept), isAccept) {
            result.PushBack(filename);
        }
    }
    AutoPtr< ArrayOf<String> > _files = ArrayOf<String>::Alloc(result.GetSize());
    Elastos::Utility::Etl::List<String>::Iterator it;
    Int32 index;
    for (it = result.Begin(), index = 0; it != result.End(); ++it, ++index) {
        (*_files)[index] = *it;
    }
    *files = _files;
    REFCOUNT_ADD(*files);
    return NOERROR;
}

ECode File::ListFiles(
    /* [out, callee] */ ArrayOf<IFile*>** files)
{
    VALIDATE_NOT_NULL(files)

    AutoPtr< ArrayOf<String> > filenames = ListImpl(mPath);
    AutoPtr< ArrayOf<IFile*> > _files = FilenamesToFiles(filenames);
    *files = _files;
    REFCOUNT_ADD(*files);
    return NOERROR;
}

ECode File::ListFiles(
    /* [in] */ IFilenameFilter* filter,
    /* [out, callee] */ ArrayOf<IFile*>** files)
{
    VALIDATE_NOT_NULL(files)
    *files = NULL;
    VALIDATE_NOT_NULL(filter)

    AutoPtr< ArrayOf<String> > filenames;
    List(filter, (ArrayOf<String>**)&filenames);
    AutoPtr< ArrayOf<IFile*> > _files = FilenamesToFiles(filenames);
    *files = _files;
    REFCOUNT_ADD(*files);
    return NOERROR;
}

ECode File::ListFiles(
    /* [in] */ IFileFilter* filter,
    /* [out, callee] */ ArrayOf<IFile*>** files)
{
    VALIDATE_NOT_NULL(files)
    *files = NULL;
    VALIDATE_NOT_NULL(filter)

    AutoPtr< ArrayOf<IFile*> > _files;
    ListFiles((ArrayOf<IFile*>**)&_files);
    if (filter == NULL || _files == NULL) {
        *files = _files;
        REFCOUNT_ADD(*files);
        return NOERROR;
    }
    Elastos::Utility::Etl::List< AutoPtr<IFile> > result;
    for (Int32 i = 0; i < _files->GetLength(); ++i) {
        IFile* file = (*_files)[i];
        Boolean isAccept;
        if (filter->Accept(file, &isAccept), isAccept) {
            result.PushBack(file);
        }
    }
    _files = ArrayOf<IFile*>::Alloc(result.GetSize());
    Elastos::Utility::Etl::List< AutoPtr<IFile> >::Iterator it;
    Int32 index;
    for (it = result.Begin(), index = 0; it != result.End(); ++it, ++index) {
        _files->Set(index, *it);
    }
    *files = _files;
    REFCOUNT_ADD(*files);
    return NOERROR;
}

AutoPtr< ArrayOf<IFile*> > File::FilenamesToFiles(
    /* [in] */ ArrayOf<String>* filenames)
{
    if (filenames == NULL) {
        return NULL;
    }
    Int32 count = filenames->GetLength();
    AutoPtr< ArrayOf<IFile*> > result = ArrayOf<IFile*>::Alloc(count);
    for (Int32 i = 0; i < count; ++i) {
        AutoPtr<IFile> file;
        CFile::New(this,
                (*filenames)[i], (IFile**)&file);
        result->Set(i, file);
    }
    return result;
}

ECode File::Mkdir(
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded)
    // try {
    *succeeded = SUCCEEDED(MkdirErrno()) ? TRUE : FALSE;
    return NOERROR;
    // } catch (ErrnoException errnoException) {
    //     return false;
    // }
}

ECode File::MkdirErrno()
{
    //On Android, we don't want default permissions to allow global access.
    AutoPtr<ILibcore> libcore;
    CLibcore::AcquireSingleton((ILibcore**)&libcore);
    AutoPtr<IOs> os;
    libcore->GetOs((IOs**)&os);
    return os->Mkdir(mPath, OsConstants::_S_IRWXU);
}

ECode File::Mkdirs(
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded)
    *succeeded = Mkdirs(FALSE);
    return NOERROR;
}

Boolean File::Mkdirs(
    /* [in] */ Boolean resultIfExists)
{
    // Try to create the directory directly.
    if (SUCCEEDED(MkdirErrno())) {
        return TRUE;
    }
    else {
        // If the parent was missing, try to create it and then try again.
        AutoPtr<IFile> parent;
        if (SUCCEEDED(GetParentFile((IFile**)&parent))) {
            Boolean succeeded;
            if (parent && SUCCEEDED(parent->Mkdirs(&succeeded)) && succeeded) {
                Mkdir(&succeeded);
                return succeeded;
            }
        }
        return FALSE;
    }
}

ECode File::CreateNewFile(
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded)
    *succeeded = FALSE;

    // try {
    // On Android, we don't want default permissions to allow global access.

    AutoPtr<ILibcore> libcore;
    CLibcore::AcquireSingleton((ILibcore**)&libcore);
    AutoPtr<IOs> os;
    libcore->GetOs((IOs**)&os);
    AutoPtr<IFileDescriptor> fd;
    ECode ec = os->Open(mPath,
        OsConstants::_O_RDWR | OsConstants::_O_CREAT | OsConstants::_O_EXCL
        , 0600, (IFileDescriptor**)&fd);

    if (fd != NULL) os->Close(fd);

    if (FAILED(ec)) {
        if (errno == EEXIST) return NOERROR;
        return E_IO_EXCEPTION;
    }

    *succeeded = TRUE;
    // } catch (ErrnoException errnoException) {
    //     if (errnoException.errno == EEXIST) {
    //         // The file already exists.
    //         return false;
    //     }
    //     throw errnoException.rethrowAsIOException();
    // } finally {
    IoUtils::Close(fd); // TODO: should we suppress IOExceptions thrown here?
    // }
    return NOERROR;
}

ECode File::CreateTempFile(
    /* [in] */ const String& prefix,
    /* [in] */ const String& suffix,
    /* [out] */ IFile** file)
{
    VALIDATE_NOT_NULL(file)
    return CreateTempFile(prefix, suffix, NULL, file);
}

ECode File::CreateTempFile(
    /* [in] */ const String& prefix,
    /* [in] */ const String& suffix,
    /* [in] */ IFile* directory,
    /* [out] */ IFile** file)
{
    // Force a prefix null check first
    String _suffix = suffix;
    if (prefix.GetLength() < 3) {
        //throw new IllegalArgumentException("prefix must be at least 3 characters");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (_suffix.IsNull()) {
        _suffix = ".tmp";
    }
    AutoPtr<IFile> tmpDirFile = directory;
    if (tmpDirFile == NULL) {
        AutoPtr<ISystem> system;
        Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);

        String tmpDir;
        system->GetProperty(String("elastos.io.tmpdir"), String("."), &tmpDir);
        CFile::New(tmpDir, (IFile**)&tmpDirFile);
    }
    AutoPtr<IFile> result;
    Boolean succeeded;
    ECode ec = NOERROR;
    AutoPtr<IRandom> random = GetTempFileRandom();
    do {
        Int32 randomInt32;
        random->NextInt32(&randomInt32);
        StringBuilder sb(prefix);
        sb += randomInt32;
        sb += _suffix;
        result = NULL;
        CFile::New(tmpDirFile, sb.ToString(), (IFile**)&result);
    } while (ec = result->CreateNewFile(&succeeded), SUCCEEDED(ec) && !succeeded);

    if (succeeded) {
        *file = result;
        REFCOUNT_ADD(*file);
    }
    return ec;
}

ECode File::RenameTo(
    /* [in] */ IFile* newPath,
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(newPath)
    VALIDATE_NOT_NULL(succeeded)
    *succeeded = FALSE;

    // try {
    String path;
    newPath->GetPath(&path);
    AutoPtr<ILibcore> libcore;
    CLibcore::AcquireSingleton((ILibcore**)&libcore);
    AutoPtr<IOs> os;
    libcore->GetOs((IOs**)&os);
    *succeeded = SUCCEEDED(os->Rename(mPath, path)) ? TRUE : FALSE;
    return NOERROR;
    // } catch (ErrnoException errnoException) {
    //     return false;
    // }
}

ECode File::ToString(
    /* [out] */ String* path)
{
    VALIDATE_NOT_NULL(path)
    *path = mPath;
    return NOERROR;
}

ECode File::ToURI(
    /* [out] */ IURI** result)
{
    String name = GetAbsoluteName();
    // try {
    String nullStr;
    if (!name.StartWith("/")) {
        // start with sep.
        AutoPtr<IURI> uri;
        CURI::New(String("file"), nullStr, String("/") + name, nullStr, nullStr, (IURI**)&uri);
        *result = uri;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    else if (name.StartWith("//")) {
        AutoPtr<IURI> uri;
        CURI::New(String("file"), String(""),  name, nullStr, nullStr, (IURI**)&uri);// UNC path
        *result = uri;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }

    AutoPtr<IURI> uri;
    CURI::New(String("file"), String(NULL),  name, nullStr, nullStr, (IURI**)&uri);// UNC path
    *result = uri;
    REFCOUNT_ADD(*result)
    return NOERROR;
    // } catch (URISyntaxException e) {
    //     // this should never happen
    //     return null;
    // }
}

ECode File::ToURL(
    /* [out] */ IURL** result)
{
    VALIDATE_NOT_NULL(result)

    String name = GetAbsoluteName();
    if (!name.StartWith("/")) {
        // start with sep.
        AutoPtr<IURL> url;
        CURL::New(String("file"), String(""), -1, String("/") + name, NULL, (IURL**)&url);
        *result = url;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    else if (name.StartWith("//")) {
        AutoPtr<IURL> url;
        CURL::New(String("file:") + name, (IURL**)&url);// UNC path
        *result = url;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }

    AutoPtr<IURL> url;
    CURL::New(String("file"), String(""), -1, name, NULL, (IURL**)&url);
    *result = url;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

// TODO: is this really necessary, or can it be replaced with getAbsolutePath?
String File::GetAbsoluteName()
{
    AutoPtr<IFile> f;
    GetAbsoluteFile((IFile**)&f);
    String name;
    f->GetPath(&name);
    Boolean isDir;
    f->IsDirectory(&isDir);
    if (isDir && name.GetChar(name.GetLength() - 1) != sSeparatorChar) {
        // Directories must end with a slash
        name = name + "/";
    }
    if (sSeparatorChar != '/') { // Must convert slashes.
        name = name.Replace(sSeparatorChar, '/');
    }
    return name;
}

// private void writeObject(ObjectOutputStream stream) throws IOException {
//     stream.defaultWriteObject();
//     stream.writeChar(separatorChar);
// }

// private void readObject(ObjectInputStream stream) throws IOException, ClassNotFoundException {
//     stream.defaultReadObject();
//     char inSeparator = stream.readChar();
//     this.path = fixSlashes(path.replace(inSeparator, separatorChar));
// }

ECode File::GetTotalSpace(
    /* [out] */ Int64* space)
{
    VALIDATE_NOT_NULL(space)
    // try {
    AutoPtr<ILibcore> libcore;
    CLibcore::AcquireSingleton((ILibcore**)&libcore);
    AutoPtr<IOs> os;
    libcore->GetOs((IOs**)&os);
    AutoPtr<IStructStatVfs> sb;
    if (FAILED(os->StatVfs(mPath, (IStructStatVfs**)&sb))) {
        *space = 0;
        return NOERROR;
    }
    Int64 blocks, bsize;
    sb->GetBlocks(&blocks);
    sb->GetBsize(&bsize);
    *space = blocks * bsize; // total block count * block size in bytes.
    return NOERROR;
    // } catch (ErrnoException errnoException) {
    //     return 0;
    // }
}

ECode File::GetUsableSpace(
    /* [out] */ Int64* space)
{
    VALIDATE_NOT_NULL(space)
    // try {
    AutoPtr<ILibcore> libcore;
    CLibcore::AcquireSingleton((ILibcore**)&libcore);
    AutoPtr<IOs> os;
    libcore->GetOs((IOs**)&os);
    AutoPtr<IStructStatVfs> sb;
    if (FAILED(os->StatVfs(mPath, (IStructStatVfs**)&sb))) {
        *space = 0;
        return NOERROR;
    }
    Int64 bavail, bsize;
    sb->GetBavail(&bavail);
    sb->GetBsize(&bsize);
    *space = bavail * bsize; // non-root free block count * block size in bytes.
    return NOERROR;
    // } catch (ErrnoException errnoException) {
    //     return 0;
    // }
}

ECode File::GetFreeSpace(
    /* [out] */ Int64* space)
{
    VALIDATE_NOT_NULL(space)
    // try {
    AutoPtr<ILibcore> libcore;
    CLibcore::AcquireSingleton((ILibcore**)&libcore);
    AutoPtr<IOs> os;
    libcore->GetOs((IOs**)&os);
    AutoPtr<IStructStatVfs> sb;
    if (FAILED(os->StatVfs(mPath, (IStructStatVfs**)&sb))) {
        *space = 0;
        return NOERROR;
    }
    Int64 bfree, bsize;
    sb->GetBfree(&bfree);
    sb->GetBsize(&bsize);
    *space = bfree * bsize; // free block count * block size in bytes.
    return NOERROR;
    // } catch (ErrnoException errnoException) {
    //     return 0;
    // }
}

} // namespace IO
} // namespace Elastos
