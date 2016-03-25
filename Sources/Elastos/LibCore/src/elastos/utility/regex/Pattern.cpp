
#include "Pattern.h"
#include "CMatcher.h"
#include "CSplitter.h"
#include "CString.h"
#include "StringBuilder.h"

using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Utility {
namespace Regex {

CAR_INTERFACE_IMPL(Pattern, Object, IPattern)

Pattern::Pattern()
    : mNativePattern(NULL)
    , mFlags(0)
{ }

Pattern::~Pattern()
{
    if (mNativePattern != NULL) {
        delete mNativePattern;
    }
}

ECode Pattern::constructor(
    /* [in] */ const String& pattern,
    /* [in] */ Int32 flags)
{
    if ((flags & CANON_EQ) != 0) {
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }
    Int32 supportedFlags = CASE_INSENSITIVE | COMMENTS | DOTALL
        | LITERAL | MULTILINE | UNICODE_CASE | UNIX_LINES;
    if ((flags & ~supportedFlags) != 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mPattern = pattern;
    mFlags = flags;
    return Compile();
}

ECode Pattern::Matcher(
    /* [in] */ const String& inputString,
    /* [out] */ IMatcher** matcher)
{
    VALIDATE_NOT_NULL(matcher);
    *matcher = NULL;
    AutoPtr<ICharSequence> seq;
    FAIL_RETURN(CString::New(inputString, (ICharSequence**)&seq));
    return CMatcher::New(this, seq, matcher);
}

ECode Pattern::Matcher(
    /* [in] */ ICharSequence* input,
    /* [out] */ IMatcher** matcher)
{
    VALIDATE_NOT_NULL(matcher);
    return CMatcher::New(this, input, matcher);
}

ECode Pattern::Split(
    /* [in] */ const String& inputString,
    /* [out, callee]*/ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result);
    return Split(inputString, 0, result);
}

ECode Pattern::Split(
    /* [in] */ ICharSequence* input,
    /* [out, callee]*/ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result);
    return Split(input, 0, result);
}

ECode Pattern::Split(
    /* [in] */ ICharSequence* input,
    /* [in] */ Int32 limit,
    /* [out, callee]*/ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(input);
    String s;
    input->ToString(&s);
    return Split(s, limit, result);
}

ECode Pattern::Split(
    /* [in] */ const String& input,
    /* [in] */ Int32 limit,
    /* [out, callee]*/ ArrayOf<String>** result)
{
    AutoPtr<ISplitter> splitter;
    FAIL_RETURN(CSplitter::AcquireSingleton((ISplitter**)&splitter));
    return splitter->Split(this, mPattern, input, limit, result);
}

ECode Pattern::GetPattern(
    /* [out] */ String* pattern)
{
    VALIDATE_NOT_NULL(pattern);
    *pattern = mPattern;
    return NOERROR;
}

ECode Pattern::GetFlags(
    /* [out] */ Int32* flags)
{
    VALIDATE_NOT_NULL(flags);
    *flags = mFlags;
    return NOERROR;
}

ECode Pattern::Compile(
    /* [in] */ const String& regularExpression,
    /* [in] */ Int32 flags,
    /* [out] */ IPattern** obj)
{
    VALIDATE_NOT_NULL(obj);
    AutoPtr<Pattern> pattern = new Pattern();
    FAIL_RETURN(pattern->constructor(regularExpression, flags));
    *obj = IPattern::Probe(pattern);
    REFCOUNT_ADD(*obj);
    return NOERROR;
}

ECode Pattern::Compile(
    /* [in] */ const String& pattern,
    /* [out] */ IPattern** obj)
{
    return Compile(pattern, 0, obj);
}

ECode Pattern::Compile()
{
    if (mPattern.IsNull()) {
        return E_NULL_POINTER_EXCEPTION;
    }

    String icuPattern = mPattern;
    if ((mFlags & LITERAL) != 0) {
        icuPattern = Quote(mPattern);
    }

    // These are the flags natively supported by ICU.
    // They even have the same value in native code.
    Int32 icuFlags = mFlags & (CASE_INSENSITIVE | COMMENTS | MULTILINE
            | DOTALL | UNIX_LINES);
    assert(mNativePattern == NULL);
    return CompileImpl(icuPattern, icuFlags, &mNativePattern);
}

Boolean Pattern::Matches(
    /* [in] */ const String& regularExpression,
    /* [in] */ ICharSequence* input)
{
    AutoPtr<Pattern> pattern = new Pattern();
    ECode ec = pattern->constructor(regularExpression, 0);
    if (FAILED(ec)) return FALSE;

    AutoPtr<CMatcher> matcher;
    ec = CMatcher::NewByFriend(IPattern::Probe(pattern), input, (CMatcher**)&matcher);
    if (FAILED(ec)) return FALSE;

    Boolean result;
    matcher->Matches(&result);
    return result;
}

Boolean Pattern::Matches(
    /* [in] */ const String& regularExpression,
    /* [in] */ const String& inputString)
{
    AutoPtr<ICharSequence> input;
    ECode ec = CString::New(inputString, (ICharSequence**)&input);
    if (FAILED(ec)) return FALSE;

    return Matches(regularExpression, input);
}

String Pattern::Quote(
    /* [in] */ const String& string)
{
    AutoPtr<StringBuilder> sb = new StringBuilder();
    sb->Append("\\Q");
    Int32 apos = 0;
    Int32 k;
    while ((k = string.IndexOf("\\E", apos)) >= 0) {
        sb->Append(string.Substring(apos, k + 2));
        sb->Append("\\\\E\\Q");
        apos = k + 2;
    }
    sb->Append(string.Substring(apos));
    sb->Append("\\E");
    return sb->ToString();
}

ECode Pattern::CompileImpl(
    /* [in] */ const String& regex,
    /* [in] */ Int32 flags,
    /* [out] */ RegexPattern** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    flags |= UREGEX_ERROR_ON_UNKNOWN_ESCAPES;

    UErrorCode status = U_ZERO_ERROR;
    UParseError error;
    error.offset = -1;

    //UnicodeString regexString(temp.unicodeString());
    //UText *pUText = utext_openUTF8(NULL, regex.string(), regex.GetLength(), &status);
    //UnicodeString regexString("ad", 2, 0);
    UnicodeString regexString = UNICODE_STRING(regex.string(), regex.GetByteLength());
    RegexPattern* regexP = RegexPattern::compile(regexString, flags, error, status);
    if (!U_SUCCESS(status)) {
        if (regexP) delete regexP;
        return E_PATTERN_SYNTAX_EXCEPTION;
//        throwPatternSyntaxException(env, status, regex, error);
    }
    *result = regexP;
    return NOERROR;
}

} // namespace Regex
} // namespace Utility
} // namespace Elastos
