
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.CoreLibrary.IO.h>
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Webkit.h"
#include "elastos/droid/providers/downloads/CHelpers.h"
#include "elastos/droid/providers/downloads/Constants.h"
#include "elastos/droid/providers/downloads/CDownloadDrmHelper.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/os/Environment.h"
#include "elastos/droid/os/FileUtils.h"
#include "elastos/utility/regex/Pattern.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::IEnvironment;
using Elastos::Droid::Os::Environment;
using Elastos::Droid::Os::FileUtils;
using Elastos::Droid::Provider::IDownloadsImpl;
using Elastos::Droid::Provider::CDownloadsImpl;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Webkit::IMimeTypeMap;
using Elastos::Droid::Webkit::IMimeTypeMapHelper;
using Elastos::Droid::Webkit::CMimeTypeMapHelper;
using Elastos::Core::CString;
using Elastos::IO::CFile;
using Elastos::Utility::CRandom;
using Elastos::Utility::Regex::IPattern;
using Elastos::Utility::Regex::Pattern;
using Elastos::Utility::Regex::IMatcher;
using Elastos::Utility::Regex::IMatchResult;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Providers {
namespace Downloads {

//===============================================================
// CHelpers::Lexer::
//===============================================================
const Int32 CHelpers::Lexer::TOKEN_START = 0;
const Int32 CHelpers::Lexer::TOKEN_OPEN_PAREN = 1;
const Int32 CHelpers::Lexer::TOKEN_CLOSE_PAREN = 2;
const Int32 CHelpers::Lexer::TOKEN_AND_OR = 3;
const Int32 CHelpers::Lexer::TOKEN_COLUMN = 4;
const Int32 CHelpers::Lexer::TOKEN_COMPARE = 5;
const Int32 CHelpers::Lexer::TOKEN_VALUE = 6;
const Int32 CHelpers::Lexer::TOKEN_IS = 7;
const Int32 CHelpers::Lexer::TOKEN_NULL = 8;
const Int32 CHelpers::Lexer::TOKEN_END = 9;

CHelpers::Lexer::Lexer(
    /* [in] */ String selection,
    /* [in] */ ISet* allowedColumns)
{
    mSelection = selection;
    mAllowedColumns = allowedColumns;
    mChars = ArrayOf<Char32>::Alloc(mSelection.GetLength());
    mChars = mSelection.GetChars(0, mChars->GetLength());
    Advance();
}

Int32 CHelpers::Lexer::CurrentToken()
{
    return mCurrentToken;
}

void CHelpers::Lexer::Advance()
{
    AutoPtr<ArrayOf<Char32> > chars = mChars;

    // consume whitespace
    while (mOffset < chars->GetLength() && (*chars)[mOffset] == ' ') {
        ++mOffset;
    }

    // end of input
    if (mOffset == chars->GetLength()) {
        mCurrentToken = TOKEN_END;
        return;
    }

    // "("
    if ((*chars)[mOffset] == '(') {
        ++mOffset;
        mCurrentToken = TOKEN_OPEN_PAREN;
        return;
    }

    // ")"
    if ((*chars)[mOffset] == ')') {
        ++mOffset;
        mCurrentToken = TOKEN_CLOSE_PAREN;
        return;
    }

    // "?"
    if ((*chars)[mOffset] == '?') {
        ++mOffset;
        mCurrentToken = TOKEN_VALUE;
        return;
    }

    // "=" and "=="
    if ((*chars)[mOffset] == '=') {
        ++mOffset;
        mCurrentToken = TOKEN_COMPARE;
        if (mOffset < chars->GetLength() && (*chars)[mOffset] == '=') {
            ++mOffset;
        }
        return;
    }

    // ">" and ">="
    if ((*chars)[mOffset] == '>') {
        ++mOffset;
        mCurrentToken = TOKEN_COMPARE;
        if (mOffset < chars->GetLength() && (*chars)[mOffset] == '=') {
            ++mOffset;
        }
        return;
    }

    // "<", "<=" and "<>"
    if ((*chars)[mOffset] == '<') {
        ++mOffset;
        mCurrentToken = TOKEN_COMPARE;
        if (mOffset < chars->GetLength() && ((*chars)[mOffset] == '=' || (*chars)[mOffset] == '>')) {
            ++mOffset;
        }
        return;
    }

    // "!="
    if ((*chars)[mOffset] == '!') {
        ++mOffset;
        mCurrentToken = TOKEN_COMPARE;
        if (mOffset < chars->GetLength() && (*chars)[mOffset] == '=') {
            ++mOffset;
            return;
        }
        // throw new IllegalArgumentException("Unexpected character after !");
    }

    // columns and keywords
    // first look for anything that looks like an identifier or a keyword
    //     and then recognize the individual words.
    // no attempt is made at discarding sequences of underscores with no alphanumeric
    //     characters, even though it's not clear that they'd be legal column names.
    if (IsIdentifierStart((*chars)[mOffset])) {
        Int32 startOffset = mOffset;
        ++mOffset;
        while (mOffset < chars->GetLength() && IsIdentifierChar((*chars)[mOffset])) {
            ++mOffset;
        }
        String word = mSelection.Substring(startOffset, mOffset);
        if (mOffset - startOffset <= 4) {
            if (word.Equals("IS")) {
                mCurrentToken = TOKEN_IS;
                return;
            }
            if (word.Equals("OR") || word.Equals("AND")) {
                mCurrentToken = TOKEN_AND_OR;
                return;
            }
            if (word.Equals("NULL")) {
                mCurrentToken = TOKEN_NULL;
                return;
            }
        }
        AutoPtr<ICharSequence> pWord;
        CString::New(word, (ICharSequence**)&pWord);
        Boolean bCt = FALSE;
        if ((mAllowedColumns->Contains(pWord, &bCt), bCt)) {
            mCurrentToken = TOKEN_COLUMN;
            return;
        }
        // throw new IllegalArgumentException("unrecognized column or keyword");
    }

    // quoted strings
    if ((*chars)[mOffset] == '\'') {
        ++mOffset;
        while (mOffset < chars->GetLength()) {
            if ((*chars)[mOffset] == '\'') {
                if (mOffset + 1 < chars->GetLength() && (*chars)[mOffset + 1] == '\'') {
                    ++mOffset;
                }
                else {
                    break;
                }
            }
            ++mOffset;
        }
        if (mOffset == chars->GetLength()) {
            // throw new IllegalArgumentException("unterminated string");
        }
        ++mOffset;
        mCurrentToken = TOKEN_VALUE;
        return;
    }

    // anything we don't recognize
    // throw new IllegalArgumentException("illegal character: " + chars[mOffset]);
}

Boolean CHelpers::Lexer::IsIdentifierStart(
    /* [in] */ Char32 c)
{
    return c == '_' ||
            (c >= 'A' && c <= 'Z') ||
            (c >= 'a' && c <= 'z');
}

Boolean CHelpers::Lexer::IsIdentifierChar(
    /* [in] */ Char32 c)
{
    return c == '_' ||
            (c >= 'A' && c <= 'Z') ||
            (c >= 'a' && c <= 'z') ||
            (c >= '0' && c <= '9');
}

//===============================================================
// CHelpers::
//===============================================================
static AutoPtr<IRandom> InitRandom()
{
    Int64 mils = SystemClock::GetUptimeMillis();
    AutoPtr<IRandom> p;
    CRandom::New(mils, (IRandom**)&p);
    return p;
}
AutoPtr<IRandom> CHelpers::sRandom = InitRandom();

static AutoPtr<IPattern> InitPat()
{
    AutoPtr<IPattern> p;
    Pattern::Compile(String("attachment;\\s*filename\\s*=\\s*\"([^\"]*)\""), (IPattern**)&p);
    return p;
}

AutoPtr<IPattern> CHelpers::CONTENT_DISPOSITION_PATTERN = InitPat();

Object CHelpers::sUniqueLock;

CAR_SINGLETON_IMPL(CHelpers)

CAR_INTERFACE_IMPL(CHelpers, Singleton, IHelpers)

CHelpers::CHelpers()
{
}

String CHelpers::ParseContentDisposition(
    /* [in] */ String contentDisposition)
{
    AutoPtr<IMatcher> m;
    CONTENT_DISPOSITION_PATTERN->Matcher(contentDisposition, (IMatcher**)&m);
    Boolean bFind = FALSE;
    if ((m->Find(&bFind), bFind)) {
        String res;
        IMatchResult::Probe(m)->Group(1, &res);
        return res;
    }
    return String(NULL);
}

ECode CHelpers::GenerateSaveFile(
    /* [in] */ IContext* context,
    /* [in] */ const String& url,
    /* [in] */ const String& hint,
    /* [in] */ const String& contentDisposition,
    /* [in] */ const String& contentLocation,
    /* [in] */ const String& mimeType,
    /* [in] */ Int32 destination,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IFile> parent;
    AutoPtr<ArrayOf<IFile*> > parentTest;
    String name(NULL);

    if (destination == IDownloadsImpl::DESTINATION_FILE_URI) {
        AutoPtr<IUriHelper> hlp;
        CUriHelper::AcquireSingleton((IUriHelper**)&hlp);
        AutoPtr<IUri> uri;
        hlp->Parse(hint, (IUri**)&uri);
        String path;
        uri->GetPath(&path);
        AutoPtr<IFile> file;
        CFile::New(path, (IFile**)&file);
        AutoPtr<IFile> parFile;
        file->GetParentFile((IFile**)&parFile);
        parFile->GetAbsoluteFile((IFile**)&parent);
        parentTest = ArrayOf<IFile*>::Alloc(1);
        (*parentTest)[0] = parent;
        file->GetName(&name);
    }
    else {
        GetRunningDestinationDirectory(context, destination, (IFile**)&parent);
        parentTest = ArrayOf<IFile*>::Alloc(1);
        (*parentTest)[0] = parent;
        GetSuccessDestinationDirectory(context, destination, (IFile**)&((*parentTest)[1]));
        name = ChooseFilename(url, hint, contentDisposition, contentLocation);
    }

    // Ensure target directories are ready
    for (Int32 i = 0;i < parentTest->GetLength();i++) {
        AutoPtr<IFile> test = (*parentTest)[i];
        Boolean bIsDir = FALSE, bMDir = FALSE;
        test->IsDirectory(&bIsDir);
        test->Mkdirs(&bMDir);
        if (!(bIsDir || bMDir)) {
            // throw new IOException("Failed to create parent for " + test);
        }
    }

    AutoPtr<IDownloadDrmHelper> hlp;
    CDownloadDrmHelper::AcquireSingleton((IDownloadDrmHelper**)&hlp);
    Boolean bNeed = FALSE;
    if ((hlp->IsDrmConvertNeeded(mimeType, &bNeed), bNeed)) {
        hlp->ModifyDrmFwLockFileExtension(name, &name);
    }

    String prefix;
    String suffix;
    Int32 dotIndex = name.LastIndexOf('.');
    Boolean missingExtension = dotIndex < 0;
    if (destination == IDownloadsImpl::DESTINATION_FILE_URI) {
        // Destination is explicitly set - do not change the extension
        if (missingExtension) {
            prefix = name;
            suffix = "";
        }
        else {
            prefix = name.Substring(0, dotIndex);
            suffix = name.Substring(dotIndex);
        }
    }
    else {
        // Split filename between base and extension
        // Add an extension if filename does not have one
        if (missingExtension) {
            prefix = name;
            suffix = ChooseExtensionFromMimeType(mimeType, TRUE);
        }
        else {
            prefix = name.Substring(0, dotIndex);
            suffix = ChooseExtensionFromFilename(mimeType, destination, name, dotIndex);
        }
    }

    {    AutoLock syncLock(sUniqueLock);
        name = GenerateAvailableFilenameLocked(parentTest, prefix, suffix);

        // Claim this filename inside lock to prevent other threads from
        // clobbering us. We're not paranoid enough to use O_EXCL.
        AutoPtr<IFile> file;
        CFile::New(parent, name, (IFile**)&file);
        Boolean bCF = FALSE;
        file->CreateNewFile(&bCF);
        return file->GetAbsolutePath(result);
    }
    return NOERROR;
}

String CHelpers::ChooseFilename(
    /* [in] */ String url,
    /* [in] */ String hint,
    /* [in] */ String contentDisposition,
    /* [in] */ String contentLocation)
{
    String filename(NULL);

    // First, try to use the hint from the application, if there's one
    if (filename.IsNull() && !hint.IsNull() && !hint.EndWith("/")) {
        if (Constants::LOGVV) {
            Logger::V(Constants::TAG, "getting filename from hint");
        }
        Int32 index = hint.LastIndexOf('/') + 1;
        if (index > 0) {
            filename = hint.Substring(index);
        }
        else {
            filename = hint;
        }
    }

    // If we couldn't do anything with the hint, move toward the content disposition
    if (filename.IsNull() && !contentDisposition.IsNull()) {
        filename = ParseContentDisposition(contentDisposition);
        if (!filename.IsNull()) {
            if (Constants::LOGVV) {
                Logger::V(Constants::TAG, "getting filename from content-disposition");
            }
            Int32 index = filename.LastIndexOf('/') + 1;
            if (index > 0) {
                filename = filename.Substring(index);
            }
        }
    }

    // If we still have nothing at this point, try the content location
    if (filename.IsNull() && !contentLocation.IsNull()) {
        AutoPtr<IUriHelper> hlp;
        CUriHelper::AcquireSingleton((IUriHelper**)&hlp);
        String decodedContentLocation;
        hlp->Decode(contentLocation, &decodedContentLocation);
        if (!decodedContentLocation.IsNull()
                && !decodedContentLocation.EndWith("/")
                && decodedContentLocation.IndexOf('?') < 0) {
            if (Constants::LOGVV) {
                Logger::V(Constants::TAG, "getting filename from content-location");
            }
            Int32 index = decodedContentLocation.LastIndexOf('/') + 1;
            if (index > 0) {
                filename = decodedContentLocation.Substring(index);
            }
            else {
                filename = decodedContentLocation;
            }
        }
    }

    // If all the other http-related approaches failed, use the plain uri
    if (filename.IsNull()) {
        AutoPtr<IUriHelper> hlp;
        CUriHelper::AcquireSingleton((IUriHelper**)&hlp);
        String decodedUrl;
        hlp->Decode(url, &decodedUrl);
        if (!decodedUrl.IsNull()
                && !decodedUrl.EndWith("/") && decodedUrl.IndexOf('?') < 0) {
            Int32 index = decodedUrl.LastIndexOf('/') + 1;
            if (index > 0) {
                if (Constants::LOGVV) {
                    Logger::V(Constants::TAG, "getting filename from uri");
                }
                filename = decodedUrl.Substring(index);
            }
        }
    }

    // Finally, if couldn't get filename from URI, get a generic filename
    if (filename.IsNull()) {
        if (Constants::LOGVV) {
            Logger::V(Constants::TAG, "using default filename");
        }
        filename = Constants::DEFAULT_DL_FILENAME;
    }

    // The VFAT file system is assumed as target for downloads.
    // Replace invalid characters according to the specifications of VFAT.
    filename = ReplaceInvalidVfatCharacters(filename);

    return filename;
}

String CHelpers::ChooseExtensionFromMimeType(
    /* [in] */ String mimeType,
    /* [in] */ Boolean useDefaults)
{
    String extension(NULL);
    if (!mimeType.IsNull()) {
        AutoPtr<IMimeTypeMapHelper> hlp;
        CMimeTypeMapHelper::AcquireSingleton((IMimeTypeMapHelper**)&hlp);
        AutoPtr<IMimeTypeMap> mm;
        hlp->GetSingleton((IMimeTypeMap**)&mm);
        mm->GetExtensionFromMimeType(mimeType, &extension);
        if (!extension.IsNull()) {
            if (Constants::LOGVV) {
                Logger::V(Constants::TAG, "adding extension from type");
            }
            String str(".");
            str += extension;
            extension = str;
        }
        else {
            if (Constants::LOGVV) {
                String logg("couldn't find extension for ");
                logg += mimeType;
                Logger::V(Constants::TAG, logg);
            }
        }
    }
    if (extension.IsNull()) {
        if (!mimeType.IsNull() && mimeType.ToLowerCase().StartWith("text/")) {
            if (mimeType.EqualsIgnoreCase("text/html")) {
                if (Constants::LOGVV) {
                    Logger::V(Constants::TAG, "adding default html extension");
                }
                extension = Constants::DEFAULT_DL_HTML_EXTENSION;
            }
            else if (useDefaults) {
                if (Constants::LOGVV) {
                    Logger::V(Constants::TAG, "adding default text extension");
                }
                extension = Constants::DEFAULT_DL_TEXT_EXTENSION;
            }
        }
        else if (useDefaults) {
            if (Constants::LOGVV) {
                Logger::V(Constants::TAG, "adding default binary extension");
            }
            extension = Constants::DEFAULT_DL_BINARY_EXTENSION;
        }
    }
    return extension;
}

String CHelpers::ChooseExtensionFromFilename(
    /* [in] */ String mimeType,
    /* [in] */ Int32 destination,
    /* [in] */ String filename,
    /* [in] */ Int32 lastDotIndex)
{
    String extension(NULL);
    if (!mimeType.IsNull()) {
        // Compare the last segment of the extension against the mime type.
        // If there's a mismatch, discard the entire extension.
        AutoPtr<IMimeTypeMapHelper> hlp;
        CMimeTypeMapHelper::AcquireSingleton((IMimeTypeMapHelper**)&hlp);
        AutoPtr<IMimeTypeMap> mm;
        hlp->GetSingleton((IMimeTypeMap**)&mm);
        String typeFromExt;
        mm->GetMimeTypeFromExtension(
                filename.Substring(lastDotIndex + 1), &typeFromExt);
        if (typeFromExt.IsNull() || !typeFromExt.EqualsIgnoreCase(mimeType)) {
            extension = ChooseExtensionFromMimeType(mimeType, FALSE);
            if (!extension.IsNull()) {
                if (Constants::LOGVV) {
                    Logger::V(Constants::TAG, "substituting extension from type");
                }
            }
            else {
                if (Constants::LOGVV) {
                    String logg("couldn't find extension for ");
                    logg += mimeType;
                    Logger::V(Constants::TAG, logg);
                }
            }
        }
    }
    if (extension.IsNull()) {
        if (Constants::LOGVV) {
            Logger::V(Constants::TAG, "keeping extension");
        }
        extension = filename.Substring(lastDotIndex);
    }
    return extension;
}

Boolean CHelpers::IsFilenameAvailableLocked(
    /* [in] */ ArrayOf<IFile*>* parents,
    /* [in] */ String name)
{
    if (Constants::RECOVERY_DIRECTORY.EqualsIgnoreCase(name)) return FALSE;

    for (Int32 i = 0; i < parents->GetLength(); i++) {
        AutoPtr<IFile> parent = (*parents)[i];
        AutoPtr<IFile> f;
        CFile::New(parent, name, (IFile**)&f);
        Boolean bExt = FALSE;
        if ((f->Exists(&bExt), bExt)) {
            return FALSE;
        }
    }

    return TRUE;
}

String CHelpers::GenerateAvailableFilenameLocked(
    /* [in] */ ArrayOf<IFile*>* parents,
    /* [in] */ String prefix,
    /* [in] */ String suffix)
{
    String name = prefix + suffix;
    if (IsFilenameAvailableLocked(parents, name)) {
        return name;
    }

    /*
    * This number is used to generate partially randomized filenames to avoid
    * collisions.
    * It starts at 1.
    * The next 9 iterations increment it by 1 at a time (up to 10).
    * The next 9 iterations increment it by 1 to 10 (random) at a time.
    * The next 9 iterations increment it by 1 to 100 (random) at a time.
    * ... Up to the point where it increases by 100000000 at a time.
    * (the maximum value that can be reached is 1000000000)
    * As soon as a number is reached that generates a filename that doesn't exist,
    *     that filename is used.
    * If the filename coming in is [base].[ext], the generated filenames are
    *     [base]-[sequence].[ext].
    */
    Int32 sequence = 1;
    for (Int32 magnitude = 1; magnitude < 1000000000; magnitude *= 10) {
        for (Int32 iteration = 0; iteration < 9; ++iteration) {
            name = prefix;
            name += Constants::FILENAME_SEQUENCE_SEPARATOR;
            name += sequence;
            name += suffix;
            if (IsFilenameAvailableLocked(parents, name)) {
                return name;
            }
            Int32 rdm = 0;
            sRandom->NextInt32(magnitude, &rdm);
            sequence += (rdm + 1);
        }
    }
    return String(NULL);
    // throw new IOException("Failed to generate an available filename");
}

ECode CHelpers::IsFilenameValid(
    /* [in] */ IContext* context,
    /* [in] */ IFile* file,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ArrayOf<IFile*> > whitelist;
    // try {
        file->GetCanonicalFile((IFile**)&file);
        whitelist = ArrayOf<IFile*>::Alloc(4);
        AutoPtr<IFile> fd;
        context->GetFilesDir((IFile**)&fd);
        fd->GetCanonicalFile(&((*whitelist)[0]));
        AutoPtr<IFile> cd;
        context->GetCacheDir((IFile**)&cd);
        cd->GetCanonicalFile(&((*whitelist)[1]));
        Environment::GetDownloadCacheDirectory()->GetCanonicalFile(&((*whitelist)[2]));
        Environment::GetExternalStorageDirectory()->GetCanonicalFile(&((*whitelist)[3]));

    // } catch (IOException e) {
    //     Log.w(TAG, "Failed to resolve canonical path: " + e);
    //     return false;
    // }

    for (Int32 i = 0; i < whitelist->GetLength(); i++) {
        AutoPtr<IFile> testDir = (*whitelist)[i];
        if (FileUtils::Contains(testDir, file)) {
            *result = TRUE;
            return NOERROR;
        }
    }

    *result = FALSE;
    return NOERROR;
}

ECode CHelpers::GetRunningDestinationDirectory(
    /* [in] */ IContext* context,
    /* [in] */ Int32 destination,
    /* [out] */ IFile** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IFile> res = GetDestinationDirectory(context, destination, TRUE);
    *result = res;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CHelpers::GetSuccessDestinationDirectory(
    /* [in] */ IContext* context,
    /* [in] */ Int32 destination,
    /* [out] */ IFile** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IFile> res = GetDestinationDirectory(context, destination, FALSE);
    *result = res;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

AutoPtr<IFile> CHelpers::GetDestinationDirectory(
    /* [in] */ IContext* context,
    /* [in] */ Int32 destination,
    /* [in] */ Boolean running)
{
    switch (destination) {
        case IDownloadsImpl::DESTINATION_CACHE_PARTITION:
        case IDownloadsImpl::DESTINATION_CACHE_PARTITION_PURGEABLE:
        case IDownloadsImpl::DESTINATION_CACHE_PARTITION_NOROAMING:
            if (running) {
                AutoPtr<IFile> f;
                context->GetFilesDir((IFile**)&f);
                return f;
            }
            else {
                AutoPtr<IFile> f;
                context->GetCacheDir((IFile**)&f);
                return f;
            }

        case IDownloadsImpl::DESTINATION_SYSTEMCACHE_PARTITION:
            if (running) {
                AutoPtr<IFile> f;
                CFile::New(Environment::GetDownloadCacheDirectory(),
                        Constants::DIRECTORY_CACHE_RUNNING, (IFile**)&f);
                return f;
            }
            else {
                return Environment::GetDownloadCacheDirectory();
            }

        case IDownloadsImpl::DESTINATION_EXTERNAL:
        {
            AutoPtr<IFile> target;
            CFile::New(
                    Environment::GetExternalStorageDirectory(), Environment::DIRECTORY_DOWNLOADS, (IFile**)&target);
            Boolean bIsDir = FALSE, bMDir = FALSE;
            if (!(target->IsDirectory(&bIsDir), bIsDir) && (target->Mkdirs(&bMDir), bMDir)) {
                // throw new IOException("unable to create external downloads directory");
                return NULL;
            }
            return target;
        }
        default:
            return NULL;
            // throw new IllegalStateException("unexpected destination: " + destination);
    }
}

ECode CHelpers::ValidateSelection(
    /* [in] */ const String& selection,
    /* [in] */ ISet* allowedColumns)
{
    // try {
        if (selection.IsNull() || selection.IsEmpty()) {
            return NOERROR;
        }
        AutoPtr<Lexer> lexer = new Lexer(selection, allowedColumns);
        ParseExpression(lexer);
        if (lexer->CurrentToken() != Lexer::TOKEN_END) {
            // throw new IllegalArgumentException("syntax error");
        }
    // } catch (RuntimeException ex) {
    //     if (Constants.LOGV) {
    //         Log.d(Constants.TAG, "invalid selection [" + selection + "] triggered " + ex);
    //     } else if (false) {
    //         Log.d(Constants.TAG, "invalid selection triggered " + ex);
    //     }
    //     throw ex;
    // }
    return NOERROR;
}

void CHelpers::ParseExpression(
    /* [in] */ Lexer* lexer)
{
    for (;;) {
        // ( expression )
        if (lexer->CurrentToken() == Lexer::TOKEN_OPEN_PAREN) {
            lexer->Advance();
            ParseExpression(lexer);
            if (lexer->CurrentToken() != Lexer::TOKEN_CLOSE_PAREN) {
                // throw new IllegalArgumentException("syntax error, unmatched parenthese");
            }
            lexer->Advance();
        }
        else {
            // statement
            ParseStatement(lexer);
        }
        if (lexer->CurrentToken() != Lexer::TOKEN_AND_OR) {
            break;
        }
        lexer->Advance();
    }
}

// statement <- COLUMN COMPARE VALUE
//            | COLUMN IS NULL
void CHelpers::ParseStatement(
    /* [in] */ Lexer* lexer)
{
    // both possibilities start with COLUMN
    if (lexer->CurrentToken() != Lexer::TOKEN_COLUMN) {
        // throw new IllegalArgumentException("syntax error, expected column name");
        return;
    }
    lexer->Advance();

    // statement <- COLUMN COMPARE VALUE
    if (lexer->CurrentToken() == Lexer::TOKEN_COMPARE) {
        lexer->Advance();
        if (lexer->CurrentToken() != Lexer::TOKEN_VALUE) {
            // throw new IllegalArgumentException("syntax error, expected quoted string");
            return;
        }
        lexer->Advance();
        return;
    }

    // statement <- COLUMN IS NULL
    if (lexer->CurrentToken() == Lexer::TOKEN_IS) {
        lexer->Advance();
        if (lexer->CurrentToken() != Lexer::TOKEN_NULL) {
            // throw new IllegalArgumentException("syntax error, expected NULL");
            return;
        }
        lexer->Advance();
        return;
    }

    // didn't get anything good after COLUMN
    // throw new IllegalArgumentException("syntax error after column name");
}

String CHelpers::ReplaceInvalidVfatCharacters(
    /* [in] */ String filename)
{
    Char32 START_CTRLCODE = 0x00;
    Char32 END_CTRLCODE = 0x1f;
    Char32 QUOTEDBL = 0x22;
    Char32 ASTERISK = 0x2A;
    Char32 SLASH = 0x2F;
    Char32 COLON = 0x3A;
    Char32 LESS = 0x3C;
    Char32 GREATER = 0x3E;
    Char32 QUESTION = 0x3F;
    Char32 BACKSLASH = 0x5C;
    Char32 BAR = 0x7C;
    Char32 DEL = 0x7F;
    Char32 UNDERSCORE = 0x5F;

    String str;
    Char32 ch;
    Boolean isRepetition = FALSE;
    for (Int32 i = 0; i < filename.GetLength(); i++) {
        ch = filename.GetChar(i);
        if ((START_CTRLCODE <= ch &&
            ch <= END_CTRLCODE) ||
            ch == QUOTEDBL ||
            ch == ASTERISK ||
            ch == SLASH ||
            ch == COLON ||
            ch == LESS ||
            ch == GREATER ||
            ch == QUESTION ||
            ch == BACKSLASH ||
            ch == BAR ||
            ch == DEL){
            if (!isRepetition) {
                str += UNDERSCORE;
                isRepetition = TRUE;
            }
        }
        else {
            str += ch;
            isRepetition = FALSE;
        }
    }
    return str;
}

} // namespace Downloads
} // namespace Providers
} // namespace Droid
} // namespace Elastos
