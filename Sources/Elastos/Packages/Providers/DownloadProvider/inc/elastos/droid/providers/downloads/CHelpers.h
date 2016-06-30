
#ifndef __ELASTOS_DROID_PROVIDERS_DOWNLOADS_CHELPERS_H__
#define __ELASTOS_DROID_PROVIDERS_DOWNLOADS_CHELPERS_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos_Droid_Providers_Downloads_CHelpers.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContext;
using Elastos::Utility::ISet;
using Elastos::Utility::IRandom;
using Elastos::Utility::Regex::IPattern;
using Elastos::IO::IFile;

namespace Elastos {
namespace Droid {
namespace Providers {
namespace Downloads {

/**
 * Some helper functions for the download manager
 */
CarClass(CHelpers)
    , public Singleton
    , public IHelpers
{
private:
    /**
     * A simple lexer that recognizes the words of our restricted subset of SQL where clauses
     */
    class Lexer
        : public Object
    {
    public:
        Lexer(
            /* [in] */ String selection,
            /* [in] */ ISet* allowedColumns);

        CARAPI_(Int32) CurrentToken();

        CARAPI_(void) Advance();

    private:
        static CARAPI_(Boolean) IsIdentifierStart(
            /* [in] */ Char32 c);

        static CARAPI_(Boolean) IsIdentifierChar(
            /* [in] */ Char32 c);

    public:
        static const Int32 TOKEN_START;
        static const Int32 TOKEN_OPEN_PAREN;
        static const Int32 TOKEN_CLOSE_PAREN;
        static const Int32 TOKEN_AND_OR;
        static const Int32 TOKEN_COLUMN;
        static const Int32 TOKEN_COMPARE;
        static const Int32 TOKEN_VALUE;
        static const Int32 TOKEN_IS;
        static const Int32 TOKEN_NULL;
        static const Int32 TOKEN_END;

    private:
        String mSelection;
        AutoPtr<ISet> mAllowedColumns;
        Int32 mOffset;
        Int32 mCurrentToken;
        AutoPtr<ArrayOf<Char32> > mChars;
    };

public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CHelpers();

    /**
     * Creates a filename (where the file should be saved) from info about a download.
     * This file will be touched to reserve it.
     */
    CARAPI GenerateSaveFile(
        /* [in] */ IContext* context,
        /* [in] */ const String& url,
        /* [in] */ const String& hint,
        /* [in] */ const String& contentDisposition,
        /* [in] */ const String& contentLocation,
        /* [in] */ const String& mimeType,
        /* [in] */ Int32 destination,
        /* [out] */ String* result);

    /**
     * Checks whether the filename looks legitimate for security purposes. This
     * prevents us from opening files that aren't actually downloads.
     */
    CARAPI IsFilenameValid(
        /* [in] */ IContext* context,
        /* [in] */ IFile* file,
        /* [out] */ Boolean* result);

    CARAPI GetRunningDestinationDirectory(
        /* [in] */ IContext* context,
        /* [in] */ Int32 destination,
        /* [out] */ IFile** result);

    CARAPI GetSuccessDestinationDirectory(
        /* [in] */ IContext* context,
        /* [in] */ Int32 destination,
        /* [out] */ IFile** result);

    /**
     * Checks whether this looks like a legitimate selection parameter
     */
    CARAPI ValidateSelection(
        /* [in] */ const String& selection,
        /* [in] */ ISet* allowedColumns);

private:

    /*
     * Parse the Content-Disposition HTTP Header. The format of the header
     * is defined here: http://www.w3.org/Protocols/rfc2616/rfc2616-sec19.html
     * This header provides a filename for content that is going to be
     * downloaded to the file system. We only support the attachment type.
     */
    static CARAPI_(String) ParseContentDisposition(
        /* [in] */ String contentDisposition);

    static CARAPI_(String) ChooseFilename(
        /* [in] */ String url,
        /* [in] */ String hint,
        /* [in] */ String contentDisposition,
        /* [in] */ String contentLocation);

    static CARAPI_(String) ChooseExtensionFromMimeType(
        /* [in] */ String mimeType,
        /* [in] */ Boolean useDefaults);

    static CARAPI_(String) ChooseExtensionFromFilename(
        /* [in] */ String mimeType,
        /* [in] */ Int32 destination,
        /* [in] */ String filename,
        /* [in] */ Int32 lastDotIndex);

    static CARAPI_(Boolean) IsFilenameAvailableLocked(
        /* [in] */ ArrayOf<IFile*>* parents,
        /* [in] */ String name);

    static CARAPI_(String) GenerateAvailableFilenameLocked(
        /* [in] */ ArrayOf<IFile*>* parents,
        /* [in] */ String prefix,
        /* [in] */ String suffix);

    static CARAPI_(AutoPtr<IFile>) GetDestinationDirectory(
        /* [in] */ IContext* context,
        /* [in] */ Int32 destination,
        /* [in] */ Boolean running);

    // expression <- ( expression ) | statement [AND_OR ( expression ) | statement] *
    //             | statement [AND_OR expression]*
    static CARAPI_(void) ParseExpression(
        /* [in] */ Lexer* lexer);

    // statement <- COLUMN COMPARE VALUE
    //            | COLUMN IS NULL
    static CARAPI_(void) ParseStatement(
        /* [in] */ Lexer* lexer);

    /**
     * Replace invalid filename characters according to
     * specifications of the VFAT.
     * @note Package-private due to testing.
     */
    static CARAPI_(String) ReplaceInvalidVfatCharacters(
        /* [in] */ String filename);

public:
    static AutoPtr<IRandom> sRandom;

private:
    /** Regex used to parse content-disposition headers */
    static AutoPtr<IPattern> CONTENT_DISPOSITION_PATTERN;

    static Object sUniqueLock;
};

} // namespace Downloads
} // namespace Providers
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_PROVIDERS_DOWNLOADS_CHELPERS_H__
