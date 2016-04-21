
#include "CFormatter.h"
#include "CLocale.h"
#include "LocaleData.h"
#include "CFile.h"
#include "CFileOutputStream.h"
#include "COutputStreamWriter.h"
#include "CBufferedWriter.h"
#include "IoUtils.h"
#include "Charset.h"
#include "CString.h"
#include "CBigInteger.h"
#include "Date.h"
#include "Calendar.h"
#include "CDouble.h"
#include "CBigDecimal.h"
#include "CMathContext.h"
#include "IntegralToString.h"
#include "Character.h"
#include "StringUtils.h"
#include "Math.h"
#include "CSystem.h"
#include "CDate.h"
//#include "CNativeDecimalFormat.h"

//using Libcore::ICU::CNativeDecimalFormat;
using Libcore::ICU::LocaleData;
using Libcore::IO::IoUtils;
using Elastos::Core::EIID_IAppendable;
using Elastos::Core::IStringBuilder;
using Elastos::Core::EIID_IStringBuilder;
using Elastos::Core::CString;
using Elastos::Core::IntegralToString;
using Elastos::Core::StringUtils;
using Elastos::Core::Character;
using Elastos::Core::IBoolean;
using Elastos::Core::EIID_IBoolean;
using Elastos::Core::IInteger16;
using Elastos::Core::EIID_IInteger16;
using Elastos::Core::IInteger32;
using Elastos::Core::EIID_IInteger32;
using Elastos::Core::IInteger64;
using Elastos::Core::EIID_IInteger64;
using Elastos::Core::IByte;
using Elastos::Core::EIID_IByte;
using Elastos::Core::Math;
using Elastos::Core::IDouble;
using Elastos::Core::CDouble;
using Elastos::Core::IFloat;
using Elastos::Core::IChar32;
using Elastos::Core::INumber;
using Elastos::Core::EIID_INumber;
using Elastos::Core::EIID_IFloat;
using Elastos::Core::EIID_IDouble;
using Elastos::Core::ISystem;
using Elastos::IO::CFile;
using Elastos::IO::IFileOutputStream;
using Elastos::IO::CFileOutputStream;
using Elastos::IO::IOutputStreamWriter;
using Elastos::IO::COutputStreamWriter;
using Elastos::IO::IBufferedWriter;
using Elastos::IO::CBufferedWriter;
using Elastos::IO::Charset::ICharset;
using Elastos::IO::Charset::Charset;
using Elastos::IO::EIID_ICloseable;
using Elastos::IO::EIID_IFlushable;
using Elastos::IO::IWriter;
using Elastos::Math::IBigInteger;
using Elastos::Math::EIID_IBigInteger;
using Elastos::Math::CBigInteger;
using Elastos::Math::EIID_IBigDecimal;
using Elastos::Math::IBigDecimal;
using Elastos::Math::CBigDecimal;
using Elastos::Math::IMathContext;
using Elastos::Math::CMathContext;
using Elastos::Utility::Calendar;
using Elastos::Utility::CDate;
using Elastos::Utility::CLocale;

namespace Elastos {
namespace Utility {

//==========================================================
//       CFormatter
//==========================================================

static void CachedDecimalFormatDestructor(void* st)
{
    CFormatter::CachedDecimalFormat* obj = static_cast<CFormatter::CachedDecimalFormat*>(st);
    if (obj) {
        obj->Release();
    }
}

static Boolean InitTLS()
{
    Int32 UNUSED(result) = pthread_key_create(&CFormatter::sCachedDecimalFormat, CachedDecimalFormatDestructor);
    assert(result == 0);
    return TRUE;
}

pthread_key_t CFormatter::sCachedDecimalFormat;
Boolean CFormatter::sHaveKey = InitTLS();

AutoPtr<ArrayOf<Char32> > InitZEROS()
{
    Int32 len = 9;
    AutoPtr<ArrayOf<Char32> > zeros = ArrayOf<Char32>::Alloc(len);
    for (Int32 i = 0; i < len; i++)
        (*zeros)[i] = '0';

    return zeros;
}
const AutoPtr<ArrayOf<Char32> > CFormatter::ZEROS = InitZEROS(); //ZEROS[] = { '0', '0', '0', '0', '0', '0', '0', '0', '0' };

CAR_INTERFACE_IMPL_3(CFormatter, Object, IFormatter, ICloseable, IFlushable)

CAR_OBJECT_IMPL(CFormatter)

CFormatter::CFormatter()
    : mClosed(FALSE)
    , mLastIOException(NOERROR)
{
}

ECode CFormatter::constructor()
{
    AutoPtr<StringBuilder> strbuild = new StringBuilder();
    AutoPtr<ILocale> deloc = CLocale::GetDefault();
    return this->constructor((IAppendable*)strbuild->Probe(EIID_IAppendable), deloc);
}

ECode CFormatter::constructor(
    /* [in] */ IAppendable* a)
{
    AutoPtr<ILocale> deloc = CLocale::GetDefault();
    return this->constructor(a, deloc);
}

ECode CFormatter::constructor(
    /* [in] */ ILocale* l)
{
    AutoPtr<StringBuilder> strbuild = new StringBuilder();
    return this->constructor((IAppendable*)strbuild->Probe(EIID_IAppendable), l);
}

ECode CFormatter::constructor(
    /* [in] */ IAppendable* a,
    /* [in] */ ILocale* l)
{
    if (a == NULL) {
        AutoPtr<StringBuilder> strbuild = new StringBuilder();
        mOut = (IAppendable*)strbuild->Probe(EIID_IAppendable);
    }
    else {
        mOut = a;
    }
    mLocale = l;
    return NOERROR;
}

ECode CFormatter::constructor(
    /* [in] */ const String& fileName)
{
    AutoPtr<IFile> file;
    FAIL_RETURN(CFile::New(fileName, (IFile**)&file));
    return this->constructor(file);
}

ECode CFormatter::constructor(
    /* [in] */ const String& fileName,
    /* [in] */ const String& csn)
{
    AutoPtr<IFile> file;
    FAIL_RETURN(CFile::New(fileName, (IFile**)&file));
    return this->constructor(file, csn);
}

ECode CFormatter::constructor(
    /* [in] */ const String& fileName,
    /* [in] */ const String& csn,
    /* [in] */ ILocale* l)
{
    AutoPtr<IFile> file;
    FAIL_RETURN(CFile::New(fileName, (IFile**)&file));
    return this->constructor(file, csn, l);
}

ECode CFormatter::constructor(
    /* [in] */ IFile* file)
{
    AutoPtr<IFileOutputStream> ops;
    FAIL_RETURN(CFileOutputStream::New(file, (IFileOutputStream**)&ops));
    return this->constructor(IOutputStream::Probe(ops));
}

ECode CFormatter::constructor(
    /* [in] */ IFile* file,
    /* [in] */ const String& csn)
{
    AutoPtr<ILocale> deloc = CLocale::GetDefault();
    return this->constructor(file, csn, deloc);
}

ECode CFormatter::constructor(
    /* [in] */ IFile* file,
    /* [in] */ const String& csn,
    /* [in] */ ILocale* l)
{
    AutoPtr<IFileOutputStream> fout;
    AutoPtr<IOutputStreamWriter> osw;
    // try {
    ECode ec = CFileOutputStream::New(file, (IFileOutputStream**)&fout);
    FAIL_GOTO(ec, _EXIT_)

    ec = COutputStreamWriter::New(IOutputStream::Probe(fout), csn, (IOutputStreamWriter**)&osw);
    FAIL_GOTO(ec, _EXIT_)

    ec = CBufferedWriter::New(IWriter::Probe(osw), (IAppendable**)&mOut);
    FAIL_GOTO(ec, _EXIT_)
    // } catch (RuntimeException e) {
    //     IoUtils.closeQuietly(fout);
    //     throw e;
    // } catch (UnsupportedEncodingException e) {
    //     IoUtils.closeQuietly(fout);
    //     throw e;
    // }
    mLocale = l;
    return NOERROR;

_EXIT_:
    IoUtils::CloseQuietly(ICloseable::Probe(fout));
    return ec;
}

ECode CFormatter::constructor(
    /* [in] */ IOutputStream* os)
{
    AutoPtr<ICharset> defchar;
    Charset::DefaultCharset((ICharset**)&defchar);
    String charname;
    defchar->GetName(&charname);
    AutoPtr<IOutputStreamWriter> osw;
    FAIL_RETURN(COutputStreamWriter::New(os, charname, (IOutputStreamWriter**)&osw));
    FAIL_RETURN(CBufferedWriter::New(IWriter::Probe(osw), (IAppendable**)&mOut));
    mLocale = CLocale::GetDefault();
    return NOERROR;
}

ECode CFormatter::constructor(
    /* [in] */ IOutputStream* os,
    /* [in] */ const String& csn)
{
    AutoPtr<ILocale> defloc = CLocale::GetDefault();
    return this->constructor(os, csn, defloc);
}

ECode CFormatter::constructor(
    /* [in] */ IOutputStream* os,
    /* [in] */ const String& csn,
    /* [in] */ ILocale* l)
{
    AutoPtr<IOutputStreamWriter> osw;
    FAIL_RETURN(COutputStreamWriter::New(os, csn, (IOutputStreamWriter**)&osw));
    FAIL_RETURN(CBufferedWriter::New(IWriter::Probe(osw), (IAppendable**)&mOut));
    mLocale = l;
    return NOERROR;
}

ECode CFormatter::constructor(
    /* [in] */ IPrintStream* ps)
{
    if (ps == NULL) {
        // throw new NullPointerException("ps == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    mOut = (IAppendable*)ps->Probe(EIID_IAppendable);
    mLocale = CLocale::GetDefault();
    return NOERROR;
}

AutoPtr<INativeDecimalFormat> CFormatter::GetDecimalFormat(
    /* [in] */ const String& pattern)
{
    AutoPtr<CachedDecimalFormat> cdf = (CachedDecimalFormat*)pthread_getspecific(sCachedDecimalFormat);
    if (cdf == NULL) {
        cdf = new CachedDecimalFormat();
        pthread_setspecific(sCachedDecimalFormat, cdf.Get());
        cdf->AddRef();
    }
    return cdf->Update(mLocaleData, pattern);
}

ECode CFormatter::CheckNotClosed()
{
    if (mClosed) {
        return E_FORMATTER_CLOSED_EXCEPTION;
    }
    return NOERROR;
}

ECode CFormatter::GetLocale(
    /* [out] */ ILocale** outlocale)
{
    VALIDATE_NOT_NULL(outlocale)

    FAIL_RETURN(CheckNotClosed());
    *outlocale = mLocale;
    REFCOUNT_ADD(*outlocale)

    return NOERROR;
}

ECode CFormatter::GetOut(
    /* [out] */ IAppendable** outapp)
{
    VALIDATE_NOT_NULL(outapp)

    FAIL_RETURN(CheckNotClosed());
    *outapp = mOut;
    REFCOUNT_ADD(*outapp)

    return NOERROR;
}

ECode CFormatter::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)

    FAIL_RETURN(CheckNotClosed());
    AutoPtr<StringBuilder> sb = (StringBuilder*)(mOut.Get());
    return sb->ToString(str);
}

ECode CFormatter::Flush()
{
    FAIL_RETURN(CheckNotClosed());
    AutoPtr<IFlushable> fa = (IFlushable*)mOut->Probe(EIID_IFlushable);
    if (fa != NULL) {
        // try {
        ECode ec = fa->Flush();
        if (ec != NOERROR) {
            mLastIOException = ec;
        }
        // } catch (IOException e) {
        //     lastIOException = e;
        // }
    }
    return NOERROR;
}

ECode CFormatter::Close()
{
    if (!mClosed) {
        mClosed = TRUE;
        // try {
        AutoPtr<ICloseable> ca = (ICloseable*)mOut->Probe(EIID_ICloseable);
        ECode ec = NOERROR;
        if (ca != NULL) {
            ec = ca->Close();
        }

        if (ec != NOERROR) {
            mLastIOException = ec;
        }
        // } catch (IOException e) {
        //     lastIOException = e;
        // }
    }
    return NOERROR;
}

ECode CFormatter::GetIoException()
{
    return mLastIOException;
}

ECode CFormatter::Format(
    /* [in] */ const String& format,
    /* [in] */ ArrayOf<IInterface*>* args)
{
    return Format(mLocale, format, args);
}

ECode CFormatter::Format(
    /* [in] */ ILocale* l,
    /* [in] */ const String& format,
    /* [in] */ ArrayOf<IInterface*>* args)
{
    AutoPtr<ILocale> originalLocale = mLocale;
    // try {
    mLocale = (l == NULL ? (CLocale::US).Get() : l);
    mLocaleData = LocaleData::Get(mLocale);
    FAIL_RETURN(DoFormat(format, args));
    // } finally {
    mLocale = originalLocale;
    // }
    return NOERROR;
}

ECode CFormatter::DoFormat(
    /* [in] */ const String& format,
    /* [in] */ ArrayOf<IInterface*>* args)
{
    FAIL_RETURN(CheckNotClosed());

    FormatSpecifierParser fsp(format);
    Int32 currentObjectIndex = 0;
    AutoPtr<IInterface> lastArgument;
    Boolean hasLastArgumentSet = FALSE;

    Int32 length = format.GetLength();
    Int32 i = 0;
    while (i < length) {
        // Find the maximal plain-text sequence...
        Int32 plainTextStart = i;
        Int32 nextPercent = format.IndexOf('%', i);
        Int32 plainTextEnd = (nextPercent == -1) ? length : nextPercent;
        // ...and output it.
        if (plainTextEnd > plainTextStart) {
            AutoPtr<ICharSequence> seq;
            CString::New(format, (ICharSequence**)&seq);
            OutputCharSequence(seq, plainTextStart, plainTextEnd);
        }
        i = plainTextEnd;
        // Do we have a format specifier?
        if (i < length) {
            AutoPtr<FormatToken> token = fsp.ParseFormatToken(i + 1);

            AutoPtr<IInterface> mArgument;
            if (token->RequireArgument()) {
                Int32 index = token->GetArgIndex() == FormatToken::UNSET ? currentObjectIndex++ : token->GetArgIndex();
                mArgument = NULL;
                FAIL_RETURN(GetArgument(args, index, &fsp, lastArgument, hasLastArgumentSet, (IInterface**)&mArgument));
                lastArgument = mArgument;
                hasLastArgumentSet = TRUE;
            }

            AutoPtr<ICharSequence> substitution = Transform(token, mArgument);
            // The substitution is null if we called Formattable.formatTo.
            Int32 seqlen = 0;
            if (substitution != NULL) {
                OutputCharSequence(substitution, 0, (substitution->GetLength(&seqlen), seqlen));
            }
            i = fsp.mI;
        }
    }

    return NOERROR;
}

void CFormatter::OutputCharSequence(
    /* [in] */ ICharSequence* cs,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    // try {
    ECode ec = mOut->Append(cs, start, end);
    if (ec != NOERROR) {
        mLastIOException = ec;
    }
    // } catch (IOException e) {
    //     lastIOException = e;
    // }
}

ECode CFormatter::GetArgument(
    /* [in] */ ArrayOf<IInterface*>* args,
    /* [in] */ Int32 index,
    /* [in] */ FormatSpecifierParser* fsp,
    /* [in] */ IInterface* lastArgument,
    /* [in] */ Boolean hasLastArgumentSet,
    /* [out] */ IInterface** argument)
{
    VALIDATE_NOT_NULL(argument);
    *argument = NULL;
    if (index == FormatToken::LAST_ARGUMENT_INDEX && !hasLastArgumentSet) {
        // throw new MissingFormatArgumentException("<");
        return E_MISSING_FORMAT_WIDTH_EXCEPTION;
    }

    if (args == NULL) {
        return NOERROR;
    }

    if (index >= args->GetLength()) {
        // throw new MissingFormatArgumentException(fsp.getFormatSpecifierText());
        return E_MISSING_FORMAT_WIDTH_EXCEPTION;
    }

    if (index == FormatToken::LAST_ARGUMENT_INDEX) {
        *argument = lastArgument;
        REFCOUNT_ADD(*argument);
        return NOERROR;
    }
    *argument = (*args)[index];
    REFCOUNT_ADD(*argument);
    return NOERROR;
}

AutoPtr<ICharSequence> CFormatter::Transform(
    /* [in] */ FormatToken* token,
    /* [in] */ IInterface* argument)
{
    mFormatToken = token;
    mArg = argument;

    // There are only two format specifiers that matter: "%d" and "%s".
    // Nothing else is common in the wild. We fast-path these two to
    // avoid the heavyweight machinery needed to cope with flags, width,
    // and precision.
    AutoPtr<ICharSequence> outseq;
    if (token->IsDefault()) {
        switch (token->GetConversionType()) {
        case 's':
            if (mArg == NULL) {
                CString::New(String("NULL"), (ICharSequence**)&outseq);
                return outseq;
            }
            else if (!(IFormattable::Probe(mArg))) {
                CString::New(Object::ToString(mArg), (ICharSequence**)&outseq);
                return outseq;
            }
            break;
        case 'd':
            {
                Boolean isNumber = IInteger64::Probe(mArg) || IInteger32::Probe(mArg)
                    || IInteger16::Probe(mArg) || IByte::Probe(mArg);
                String argValue;
                if (isNumber) {
                    INumber* number = INumber::Probe(mArg);
                    argValue = Object::ToString(number);
                }

                Char32 zeroDigit;
                mLocaleData->GetZeroDigit(&zeroDigit);
                Boolean needLocalizedDigits = zeroDigit != '0';
                if (IStringBuilder::Probe(mOut) && !needLocalizedDigits) {
                    IStringBuilder* sb = IStringBuilder::Probe(mOut);
                    if (isNumber) {
                        sb->Append(argValue);
                        return NULL;
                    }
                }

                if (isNumber) {
                    CString::New(argValue, (ICharSequence**)&outseq);
                    return needLocalizedDigits ? LocalizeDigits(outseq) : outseq;
                }
            }
        }
    }

    FAIL_RETURN_NULL(mFormatToken->CheckFlags(mArg));

    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    String separator;
    system->GetLineSeparator(&separator);

    AutoPtr<ICharSequence> result;
    switch (token->GetConversionType()) {
    case 'B': case 'b':
        result = TransformFromBoolean();
        break;
    case 'H': case 'h':
        result = TransformFromHashCode();
        break;
    case 'S': case 's':
        result = TransformFromString();
        break;
    case 'C': case 'c':
        result = TransformFromCharacter();
        break;
    case 'd': case 'o': case 'x': case 'X':
        if (mArg == NULL || IBigInteger::Probe(mArg)) {
            result = TransformFromBigInteger();
        }
        else {
            result = TransformFromInteger();
        }
        break;
    case 'A': case 'a': case 'E': case 'e': case 'f': case 'G': case 'g':
        result = TransformFromFloat();
        break;
    case '%':
        result = TransformFromPercent();
        break;
    case 'n':
        CString::New(separator, (ICharSequence**)&result);
        break;
    case 't': case 'T':
        result = TransformFromDateTime();
        break;
    default:
        assert(0);
        // throw token.unknownFormatConversionException();
        return NULL;
    }

    if (Character::IsUpperCase(token->GetConversionType())) {
        if (result != NULL) {
            // result = result.toString().toUpperCase(locale);
            String str;
            result->ToString(&str);
            str = str.ToUpperCase();
            result = NULL;
            CString::New(str, (ICharSequence**)&result);
        }
    }
    return result;
}

ECode CFormatter::BadArgumentType()
{
    // throw new IllegalFormatConversionException(formatToken.getConversionType(), mArg.getClass());
    return E_ILLEGAL_FORMAT_CONVERSION_EXCEPTION;
}

AutoPtr<ICharSequence> CFormatter::LocalizeDigits(
    /* [in] */ ICharSequence* s)
{
    Int32 length = 0;
    s->GetLength(&length);
    Char32 zeroDigit;
    mLocaleData->GetZeroDigit(&zeroDigit);
    Int32 offsetToLocalizedDigits = zeroDigit - '0';
    StringBuilder result;
    for (Int32 i = 0; i < length; ++i) {
        Char32 ch;
        s->GetCharAt(i, &ch);
        if (ch >= '0' && ch <= '9') {
            ch += offsetToLocalizedDigits;
        }
        result.AppendChar(ch);
    }
    String str;
    result.ToString(&str);
    AutoPtr<ICharSequence> seq;
    CString::New(str, (ICharSequence**)&seq);
    return seq;
}

AutoPtr<ICharSequence> CFormatter::InsertGrouping(
    /* [in] */ ICharSequence* s)
{
    Int32 sLen;
    s->GetLength(&sLen);
    AutoPtr<StringBuilder> result = new StringBuilder(sLen + sLen/3);

    // A leading '-' doesn't want to be included in the grouping.
    Int32 digitsLength = 0;
    s->GetLength(&digitsLength);
    Int32 i = 0;
    Char32 ch = 0;

    if ((s->GetCharAt(0, &ch), ch) == '-') {
        --digitsLength;
        ++i;
        result->AppendChar('-');
    }

    // Append the digits that come before the first separator.
    Int32 headLength = digitsLength % 3;
    if (headLength == 0) {
        headLength = 3;
    }
    result->Append(s, i, i + headLength);
    i += headLength;

    // Append the remaining groups.
    s->GetLength(&digitsLength);
    for (; i < digitsLength; i += 3) {
        Char32 groupingSeparator;
        mLocaleData->GetGroupingSeparator(&groupingSeparator);
        result->AppendChar(groupingSeparator);
        result->Append(s, i, i + 3);
    }
    return result;
}

AutoPtr<ICharSequence> CFormatter::TransformFromBoolean()
{
    AutoPtr<ICharSequence> cs;
    String result;
    AutoPtr<IBoolean> res = (IBoolean*)mArg->Probe(EIID_IBoolean);
    if (res != NULL) {
        result = Object::ToString(res);
    }
    else if (mArg == NULL) {
        result = "FALSE";
    }
    else {
        result = "TRUE";
    }
    CString::New(result, (ICharSequence**)&cs);
    return Padding(cs, 0);
}

AutoPtr<ICharSequence> CFormatter::TransformFromHashCode()
{
    AutoPtr<ICharSequence> cs;
    String result;
    if (mArg == NULL) {
        result = "NULL";
    }
    else {
        Int32 codevalue = Object::GetHashCode(mArg);
        result = StringUtils::ToHexString(codevalue);
    }
    CString::New(result, (ICharSequence**)&cs);
    return Padding(cs, 0);
}

AutoPtr<ICharSequence> CFormatter::TransformFromString()
{
    AutoPtr<IFormattable> res = (IFormattable*)mArg->Probe(EIID_IFormattable);
    if (res != NULL) {
        Int32 flags = 0;
        if (mFormatToken->mFlagMinus) {
            flags |= IFormattableFlags::LEFT_JUSTIFY;
        }
        if (mFormatToken->mFlagSharp) {
            flags |= IFormattableFlags::ALTERNATE;
        }
        if (Character::IsUpperCase(mFormatToken->GetConversionType())) {
            flags |= IFormattableFlags::UPPERCASE;
        }
        res->FormatTo(this, flags, mFormatToken->GetWidth(),
                mFormatToken->GetPrecision());
        // all actions have been taken out in the
        // Formattable.formatTo, thus there is nothing to do, just
        // returns null, which tells the Parser to add nothing to the
        // output.
        return NULL;
    }
    AutoPtr<ICharSequence> result;
    String midstr;
    String outstr = mArg != NULL ? Object::ToString(mArg) : String("NULL");
    CString::New(outstr, (ICharSequence**)&result);
    return Padding(result, 0);
}

AutoPtr<ICharSequence> CFormatter::TransformFromCharacter()
{
    if (mArg == NULL) {
        AutoPtr<ICharSequence> cs;
        CString::New(String("NULL"), (ICharSequence**)&cs);
        return Padding(cs, 0);
    }
    if (IChar32::Probe(mArg) /*mArg instanceof Character*/) {
        String str = Object::ToString(mArg);
        AutoPtr<ICharSequence> cs;
        CString::New(str, (ICharSequence**)&cs);
        return Padding(cs, 0);
    }
    else if (IByte::Probe(mArg) || IInteger16::Probe(mArg) || IInteger32::Probe(mArg)) {
        Int32 codePoint = 0;
        INumber::Probe(mArg)->Int32Value(&codePoint);
        if (!Character::IsValidCodePoint(codePoint)) {
            // throw new IllegalFormatCodePointException(codePoint);
            return NULL;
        }
        String str = Object::ToString(mArg);
        AutoPtr<ICharSequence> cs;
        CString::New(str, (ICharSequence**)&cs);
        return Padding(cs, 0);
    }
    // else {
    //     throw badArgumentType();
    // }
    return NULL;
}

AutoPtr<ICharSequence> CFormatter::TransformFromPercent()
{
    AutoPtr<ICharSequence> cs;
    CString::New(String("%"), (ICharSequence**)&cs);
    return Padding(cs, 0);
}

AutoPtr<ICharSequence> CFormatter::Padding(
    /* [in] */ ICharSequence* inSource,
    /* [in] */ Int32 startIndex)
{
    Int32 start = startIndex;
    Int32 width = mFormatToken->GetWidth();
    Int32 precision = mFormatToken->GetPrecision();

    AutoPtr<ICharSequence> source = inSource;
    Int32 length = 0;
    source->GetLength(&length);
    if (precision >= 0) {
        length = Elastos::Core::Math::Min(length, precision);
        if (IStringBuilder::Probe(source)) {
            IStringBuilder::Probe(source)->SetLength(length);
        }
        else {
            source = NULL;
            inSource->SubSequence(0, length, (ICharSequence**)&source);
        }
    }
    if (width > 0) {
        width = Elastos::Core::Math::Max((source->GetLength(&length), length), width);
    }
    if (length >= width) {
        return source;
    }

    Char32 paddingChar = 0x0020; // space as padding char.
    if (mFormatToken->mFlagZero) {
        if (mFormatToken->GetConversionType() == 'd') {
            mLocaleData->GetZeroDigit(&paddingChar);
        }
        else {
            paddingChar = '0'; // No localized digits for bases other than decimal.
        }
    }
    else {
        // if padding char is space, always pad from the start.
        start = 0;
    }
    AutoPtr<ArrayOf<Char32> > paddingChars = ArrayOf<Char32>::Alloc(width - length);
    for (Int32 i = 0; i < width - length; ++i) {
        (*paddingChars)[i] = paddingChar;
    }

    Boolean paddingRight = mFormatToken->mFlagMinus;
    AutoPtr<StringBuilder> result = ToStringBuilder(source);
    if (paddingRight) {
        result->Append(*paddingChars);
    }
    else {
        result->Insert(start, *paddingChars);
    }
    return result;
}

AutoPtr<StringBuilder> CFormatter::ToStringBuilder(
    /* [in] */ ICharSequence* cs)
{
    AutoPtr<IStringBuilder> sb = (IStringBuilder*)cs->Probe(EIID_IStringBuilder);
    if (sb != NULL) {
        return (StringBuilder*)sb.Get();
    }
    else {
        String str;
        cs->ToString(&str);
        sb = new StringBuilder(str);
        return (StringBuilder*)sb.Get();
    }
}

AutoPtr<StringBuilder> CFormatter::WrapParentheses(
    /* [in] */ StringBuilder* _result)
{
    AutoPtr<StringBuilder> result = _result;
    result->SetCharAt(0, '('); // Replace the '-'.
    if (mFormatToken->mFlagZero) {
        mFormatToken->SetWidth(mFormatToken->GetWidth() - 1);
        result = (StringBuilder*)IStringBuilder::Probe(Padding(result, 1));
        result->AppendChar(')');
    }
    else {
        result->AppendChar(')');
        result = (StringBuilder*)IStringBuilder::Probe(Padding(result, 0));
    }
    return result;
}

AutoPtr<ICharSequence> CFormatter::TransformFromInteger()
{
    Int32 startIndex = 0;
    AutoPtr<StringBuilder> result = new StringBuilder;
    Char32 currentConversionType = mFormatToken->GetConversionType();

    Int64 value = 0;
    if (IInteger64::Probe(mArg)) {
        ((IInteger64*) mArg->Probe(EIID_IInteger64))->GetValue(&value);
    }
    else if (IInteger32::Probe(mArg)) {
        (INumber::Probe(mArg))->Int64Value(&value);
    }
    else if (IInteger16::Probe(mArg)) {
        (INumber::Probe(mArg))->Int64Value(&value);
    }
    else if (IByte::Probe(mArg)) {
        (INumber::Probe(mArg))->Int64Value(&value);
    }
    else {
        // throw badArgumentType();
        return NULL;
    }

    if (mFormatToken->mFlagSharp) {
        if (currentConversionType == 'o') {
            result->Append(String("0"));
            startIndex += 1;
        }
        else {
            result->Append(String("0x"));
            startIndex += 2;
        }
    }

    if (currentConversionType == 'd') {
        AutoPtr<ICharSequence> digits;
        CString::New(StringUtils::ToString(value), (ICharSequence**)&digits);
        if (mFormatToken->mFlagComma) {
            digits = InsertGrouping(digits);
        }
        Char32 zeroDigit;
        mLocaleData->GetZeroDigit(&zeroDigit);
        if (zeroDigit != '0') {
            digits = LocalizeDigits(digits);
        }
        result->Append(digits);

        if (value < 0) {
            if (mFormatToken->mFlagParenthesis) {
                return WrapParentheses(result);
            }
            else if (mFormatToken->mFlagZero) {
                startIndex++;
            }
        }
        else {
            if (mFormatToken->mFlagPlus) {
                result->InsertChar(0, '+');
                startIndex += 1;
            }
            else if (mFormatToken->mFlagSpace) {
                result->InsertChar(0, ' ');
                startIndex += 1;
            }
        }
    }
    else {
        // Undo sign-extension, since we'll be using Long.to(Octal|Hex)String.
        if (IByte::Probe(mArg)) {
            value &= 0xffLL;
        }
        else if (IInteger16::Probe(mArg)) {
            value &= 0xffffLL;
        }
        else if (IInteger32::Probe(mArg)) {
            value &= 0xffffffffLL;
        }
        if (currentConversionType == 'o') {
            result->Append(StringUtils::ToOctalString(value));
        }
        else {
            result->Append(StringUtils::ToHexString(value));
        }
    }

    return Padding(result, startIndex);
}

AutoPtr<ICharSequence> CFormatter::TransformFromNull()
{
    mFormatToken->mFlagZero = FALSE;
    AutoPtr<ICharSequence> cs;
    CString::New(String("NULL"), (ICharSequence**)&cs);
    return Padding(cs, 0);
}

AutoPtr<ICharSequence> CFormatter::TransformFromBigInteger()
{
    Int32 startIndex = 0;
    AutoPtr<StringBuilder> result = new StringBuilder;
    AutoPtr<IBigInteger> bigInt = (IBigInteger*) mArg->Probe(EIID_IBigInteger);
    Char32 currentConversionType = mFormatToken->GetConversionType();

    if (bigInt == NULL) {
        return TransformFromNull();
    }

    Int32 comvalue = 0;
    (IComparable::Probe(bigInt))->CompareTo(CBigInteger::ZERO, &comvalue);
    Boolean isNegative = (comvalue < 0);

    if (currentConversionType == 'd') {
        String str;
        bigInt->ToString(10, &str);
        AutoPtr<ICharSequence> digits;
        CString::New(str, (ICharSequence**)&digits);
        if (mFormatToken->mFlagComma) {
            digits = InsertGrouping(digits);
        }
        result->Append(digits);
    }
    else if (currentConversionType == 'o') {
        // convert BigInteger to a string presentation using radix 8
        String str;
        bigInt->ToString(8, &str);
        result->Append(str);
    }
    else {
        // convert BigInteger to a string presentation using radix 16
        String str;
        bigInt->ToString(16, &str);
        result->Append(str);
    }
    if (mFormatToken->mFlagSharp) {
        startIndex = isNegative ? 1 : 0;
        if (currentConversionType == 'o') {
            result->Insert(startIndex, String("0"));
            startIndex += 1;
        }
        else if (currentConversionType == 'x' || currentConversionType == 'X') {
            result->Insert(startIndex, String("0x"));
            startIndex += 2;
        }
    }

    if (!isNegative) {
        if (mFormatToken->mFlagPlus) {
            result->InsertChar(0, '+');
            startIndex += 1;
        }
        if (mFormatToken->mFlagSpace) {
            result->InsertChar(0, ' ');
            startIndex += 1;
        }
    }

    /* pad paddingChar to the output */
    if (isNegative && mFormatToken->mFlagParenthesis) {
        return WrapParentheses(result);
    }
    if (isNegative && mFormatToken->mFlagZero) {
        startIndex++;
    }
    return Padding(result, startIndex);
}

AutoPtr<ICharSequence> CFormatter::TransformFromDateTime()
{
    if (mArg == NULL) {
        return TransformFromNull();
    }

    AutoPtr<ICalendar> calendar = (ICalendar*)mArg->Probe(EIID_ICalendar);
    if (calendar == NULL) {
        AutoPtr<IDate> date;
        if (IInteger64::Probe(mArg)) {
            Int64 longvalue = 0;
            ((IInteger64*) mArg->Probe(EIID_IInteger64))->GetValue(&longvalue);
            CDate::New(longvalue, (IDate**)&date);
        }
        else if (IDate::Probe(mArg)) {
            date = (IDate*)mArg->Probe(EIID_IDate);
        }
        else {
            // throw badArgumentType();
            return NULL;
        }
        Calendar::GetInstance(mLocale, (ICalendar**)&calendar);
        calendar->SetTime(date);
    }

    AutoPtr<StringBuilder> result = new StringBuilder;
    if (!AppendT(result, mFormatToken->GetDateSuffix(), calendar)) {
        // throw formatToken.unknownFormatConversionException();
        return NULL;
    }
    return Padding(result, 0);
}

Boolean CFormatter::AppendT(
    /* [in] */ StringBuilder* result,
    /* [in] */ Char32 conversion,
    /* [in] */ ICalendar* calendar)
{
    switch (conversion) {
        case 'A':
        {
            Int32 calvalue = 0;
            calendar->Get(ICalendar::DAY_OF_WEEK, &calvalue);
            AutoPtr<ArrayOf<String> > longWeekdayNames;
            mLocaleData->GetLongWeekdayNames((ArrayOf<String>**)&longWeekdayNames);
            result->Append((*longWeekdayNames)[calvalue]);
            return TRUE;
        }
        case 'a':
        {
            Int32 calvalue = 0;
            calendar->Get(ICalendar::DAY_OF_WEEK, &calvalue);
            AutoPtr<ArrayOf<String> > shortWeekdayNames;
            mLocaleData->GetShortWeekdayNames((ArrayOf<String>**)&shortWeekdayNames);
            result->Append((*shortWeekdayNames)[calvalue]);
            return TRUE;
        }
        case 'B':
        {
            Int32 calvalue = 0;
            calendar->Get(ICalendar::MONTH, &calvalue);
            AutoPtr<ArrayOf<String> > longMonthNames;
            mLocaleData->GetLongMonthNames((ArrayOf<String>**)&longMonthNames);
            result->Append((*longMonthNames)[calvalue]);
            return TRUE;
        }
        case 'b': case 'h':
        {
            Int32 calvalue = 0;
            calendar->Get(ICalendar::MONTH, &calvalue);
            AutoPtr<ArrayOf<String> > shortMonthNames;
            mLocaleData->GetShortMonthNames((ArrayOf<String>**)&shortMonthNames);
            result->Append((*shortMonthNames)[calvalue]);
            return TRUE;
        }
        case 'C':
        {
            Int32 calvalue = 0;
            calendar->Get(ICalendar::YEAR, &calvalue);
            AppendLocalized(result, calvalue / 100, 2);
            return TRUE;
        }
        case 'D':
            AppendT(result, 'm', calendar);
            result->AppendChar('/');
            AppendT(result, 'd', calendar);
            result->AppendChar('/');
            AppendT(result, 'y', calendar);
            return TRUE;
        case 'F':
            AppendT(result, 'Y', calendar);
            result->AppendChar('-');
            AppendT(result, 'm', calendar);
            result->AppendChar('-');
            AppendT(result, 'd', calendar);
            return TRUE;
        case 'H':
        {
            Int32 calvalue = 0;
            calendar->Get(ICalendar::HOUR_OF_DAY, &calvalue);
            AppendLocalized(result, calvalue, 2);
            return TRUE;
        }
        case 'I':
        {
            Int32 calvalue = 0;
            calendar->Get(ICalendar::HOUR, &calvalue);
            AppendLocalized(result, To12Hour(calvalue), 2);
            return TRUE;
        }
        case 'L':
        {
            Int32 calvalue = 0;
            calendar->Get(ICalendar::MILLISECOND, &calvalue);
            AppendLocalized(result, calvalue, 3);
            return TRUE;
        }
        case 'M':
        {
            Int32 calvalue = 0;
            calendar->Get(ICalendar::MINUTE, &calvalue);
            AppendLocalized(result, calvalue, 2);
            return TRUE;
        }
        case 'N':
        {
            Int32 calvalue = 0;
            calendar->Get(ICalendar::MILLISECOND, &calvalue);
            AppendLocalized(result, calvalue * 1000000L, 9);
            return TRUE;
        }
        case 'Q':
        {
            Int64 calvalue = 0;
            calendar->GetTimeInMillis(&calvalue);
            AppendLocalized(result, calvalue, 0);
            return TRUE;
        }
        case 'R':
            AppendT(result, 'H', calendar);
            result->AppendChar(':');
            AppendT(result, 'M', calendar);
            return TRUE;
        case 'S':
        {
            Int32 calvalue = 0;
            calendar->Get(ICalendar::SECOND, &calvalue);
            AppendLocalized(result, calvalue, 2);
            return TRUE;
        }
        case 'T':
            AppendT(result, 'H', calendar);
            result->AppendChar(':');
            AppendT(result, 'M', calendar);
            result->AppendChar(':');
            AppendT(result, 'S', calendar);
            return TRUE;
        case 'Y':
        {
            Int32 calvalue = 0;
            calendar->Get(ICalendar::YEAR, &calvalue);
            AppendLocalized(result, calvalue, 4);
            return TRUE;
        }
        case 'Z':
        {
            AutoPtr<ITimeZone> timeZone;
            calendar->GetTimeZone((ITimeZone**)&timeZone);
            Boolean isflag = FALSE;
            AutoPtr<IDate> adate;
            calendar->GetTime((IDate**)&adate);
            timeZone->InDaylightTime(adate, &isflag);
            String str;
            timeZone->GetDisplayName(isflag, ITimeZone::SHORT, mLocale, &str);
            result->Append(str);
            return TRUE;
        }
        case 'c':
            AppendT(result, 'a', calendar);
            result->AppendChar(' ');
            AppendT(result, 'b', calendar);
            result->AppendChar(' ');
            AppendT(result, 'd', calendar);
            result->AppendChar(' ');
            AppendT(result, 'T', calendar);
            result->AppendChar(' ');
            AppendT(result, 'Z', calendar);
            result->AppendChar(' ');
            AppendT(result, 'Y', calendar);
            return TRUE;
        case 'd':
        {
            Int32 calvalue = 0;
            calendar->Get(ICalendar::DAY_OF_MONTH, &calvalue);
            AppendLocalized(result, calvalue, 2);
            return TRUE;
        }
        case 'e':
        {
            Int32 calvalue = 0;
            calendar->Get(ICalendar::DAY_OF_MONTH, &calvalue);
            AppendLocalized(result, calvalue, 0);
            return TRUE;
        }
        case 'j':
        {
            Int32 calvalue = 0;
            calendar->Get(ICalendar::DAY_OF_YEAR, &calvalue);
            AppendLocalized(result, calvalue, 3);
            return TRUE;
        }
        case 'k':
        {
            Int32 calvalue = 0;
            calendar->Get(ICalendar::HOUR_OF_DAY, &calvalue);
            AppendLocalized(result, calvalue, 0);
            return TRUE;
        }
        case 'l':
        {
            Int32 calvalue = 0;
            calendar->Get(ICalendar::HOUR, &calvalue);
            AppendLocalized(result, To12Hour(calvalue), 0);
            return TRUE;
        }
        case 'm':
        {
            // Calendar.JANUARY is 0; humans want January represented as 1.
            Int32 calvalue = 0;
            calendar->Get(ICalendar::MONTH, &calvalue);
            AppendLocalized(result, calvalue + 1, 2);
            return TRUE;
        }
        case 'p':
        {
            Int32 calvalue = 0;
            calendar->Get(ICalendar::AM_PM, &calvalue);
            AutoPtr<ArrayOf<String> > amPm;
            mLocaleData->GetAmPm((ArrayOf<String>**)&amPm);
            result->Append((*amPm)[calvalue].ToLowerCase()); // ToLowerCase(mLocale)
            return TRUE;
        }
        case 'r':
        {
            AppendT(result, 'I', calendar);
            result->AppendChar(':');
            AppendT(result, 'M', calendar);
            result->AppendChar(':');
            AppendT(result, 'S', calendar);
            result->AppendChar(' ');
            Int32 calvalue = 0;
            calendar->Get(ICalendar::AM_PM, &calvalue);
            AutoPtr<ArrayOf<String> > amPm;
            mLocaleData->GetAmPm((ArrayOf<String>**)&amPm);
            result->Append((*amPm)[calvalue]);
            return TRUE;
        }
        case 's':
        {
            Int64 calvalue = 0;
            calendar->GetTimeInMillis(&calvalue);
            AppendLocalized(result, calvalue / 1000, 0);
            return TRUE;
        }
        case 'y':
        {
            Int32 calvalue = 0;
            calendar->Get(ICalendar::YEAR, &calvalue);
            AppendLocalized(result, calvalue % 100, 2);
            return TRUE;
        }
        case 'z':
        {
            Int32 calvalue1 = 0;
            Int32 calvalue2 = 0;
            calendar->Get(ICalendar::ZONE_OFFSET, &calvalue1);
            calendar->Get(ICalendar::DST_OFFSET, &calvalue2);
            Int64 offset = calvalue1 + calvalue2;
            Char32 sign = '+';
            if (offset < 0) {
                sign = '-';
                offset = -offset;
            }
            result->AppendChar(sign);
            AppendLocalized(result, offset / 3600000, 2);
            AppendLocalized(result, (offset % 3600000) / 60000, 2);
            return TRUE;
        }
    }
    return FALSE;
}

Int32 CFormatter::To12Hour(
    /* [in] */ Int32 hour)
{
    return hour == 0 ? 12 : hour;
}

void CFormatter::AppendLocalized(
    /* [in] */ StringBuilder* result,
    /* [in] */ Int64 value,
    /* [in] */ Int32 width)
{
    Int32 paddingIndex = 0;
    result->GetLength(&paddingIndex);
    Char32 zeroDigit;
    mLocaleData->GetZeroDigit(&zeroDigit);
    if (zeroDigit == '0') {
        result->Append(value);
    }
    else {
        AutoPtr<ICharSequence> cs;
        CString::New(StringUtils::ToString(value), (ICharSequence**)&cs);
        result->Append(LocalizeDigits(cs));
    }
    Int32 len = 0;
    Int32 zeroCount = width - ((result->GetLength(&len), len) - paddingIndex);
    if (zeroCount <= 0) {
        return;
    }
    if (zeroDigit == '0') {
        result->Insert(paddingIndex, *ZEROS, 0, zeroCount);
    }
    else {
        for (Int32 i = 0; i < zeroCount; ++i) {
            result->InsertChar(paddingIndex, zeroDigit);
        }
    }
}

AutoPtr<ICharSequence> CFormatter::TransformFromSpecialNumber(
    /* [in] */ Double d)
{
    String source(NULL);
    AutoPtr<IDouble> adouble;
    CDouble::New(d, (IDouble**)&adouble);
    Boolean isflag = FALSE;
    if (adouble->IsNaN(&isflag), isflag) {
        source = "NaN";
    }
    else if (d == Elastos::Core::Math::DOUBLE_POSITIVE_INFINITY) {
        if (mFormatToken->mFlagPlus) {
            source = "+Infinity";
        }
        else if (mFormatToken->mFlagSpace) {
            source = " Infinity";
        }
        else {
            source = "Infinity";
        }
    }
    else if (d == Elastos::Core::Math::DOUBLE_NEGATIVE_INFINITY) {
        if (mFormatToken->mFlagParenthesis) {
            source = "(Infinity)";
        }
        else {
            source = "-Infinity";
        }
    }
    else {
        return NULL;
    }

    mFormatToken->SetPrecision(FormatToken::UNSET);
    mFormatToken->mFlagZero = FALSE;
    AutoPtr<ICharSequence> cs;
    CString::New(source, (ICharSequence**)&cs);
    return Padding(cs, 0);
}

AutoPtr<ICharSequence> CFormatter::TransformFromFloat()
{
    if (mArg == NULL) {
        return TransformFromNull();
    }
    else if (IFloat::Probe(mArg) || IDouble::Probe(mArg)) {
        AutoPtr<INumber> number = (INumber*) mArg->Probe(EIID_INumber);
        Double d = 0;
        number->DoubleValue(&d);
        if (d != d
                || d == Elastos::Core::Math::DOUBLE_POSITIVE_INFINITY
                || d == Elastos::Core::Math::DOUBLE_NEGATIVE_INFINITY) {
            return TransformFromSpecialNumber(d);
        }
    }
    else if (IBigDecimal::Probe(mArg)) {
        // BigDecimal can't represent NaN or infinities, but its doubleValue method will return
        // infinities if the BigDecimal is too big for a double.
    }
    else {
        // throw badArgumentType();
        return NULL;
    }

    Char32 mConversionType = mFormatToken->GetConversionType();
    if (mConversionType != 'a' && mConversionType != 'A' && !mFormatToken->IsPrecisionSet()) {
        mFormatToken->SetPrecision(FormatToken::DEFAULT_PRECISION);
    }

    AutoPtr<StringBuilder> result = new StringBuilder();
    switch (mConversionType) {
        case 'a': case 'A':
            TransformA(result);
            break;
        case 'e': case 'E':
            TransformE(result);
            break;
        case 'f':
            TransformF(result);
            break;
        case 'g':
        case 'G':
            TransformG(result);
            break;
        default:
            // throw formatToken.unknownFormatConversionException();
            return NULL;
    }

    mFormatToken->SetPrecision(FormatToken::UNSET);

    Int32 startIndex = 0;
    String minusSign;
    mLocaleData->GetMinusSign(&minusSign);
    if (StartsWithMinusSign(result->ToString(), minusSign)) {
        if (mFormatToken->mFlagParenthesis) {
            return WrapParentheses(result);
        }
    }
    else {
        if (mFormatToken->mFlagSpace) {
            result->InsertChar(0, ' ');
            startIndex++;
        }
        if (mFormatToken->mFlagPlus) {
            result->InsertChar(0, '+');
            startIndex++;
        }
    }

    Char32 firstChar = result->GetCharAt(0);
    if (mFormatToken->mFlagZero && (firstChar == '+'
        || StartsWithMinusSign(result->ToString(), minusSign))) {
        startIndex = minusSign.GetLength();
    }

    if (mConversionType == 'a' || mConversionType == 'A') {
        startIndex += 2;
    }
    return Padding(result, startIndex);
}

Boolean CFormatter::StartsWithMinusSign(
    /* [in] */ const String& cs,
    /* [in] */ const String& minusSign)
{
    if (cs.GetByteLength() < minusSign.GetByteLength()) {
        return FALSE;
    }

    AutoPtr<ArrayOf<Char32> > lc = cs.GetChars();
    AutoPtr<ArrayOf<Char32> > rc = minusSign.GetChars();

    for (Int32 i = 0; i < lc->GetLength(); ++i) {
        if ((*lc)[i] != (*rc)[i]) {
            return FALSE;
        }
    }

    return TRUE;
}

void CFormatter::TransformE(
    /* [in] */ StringBuilder* result)
{
    // All zeros in this method are *pattern* characters, so no localization.
    Int32 precision = mFormatToken->GetPrecision();
    String pattern = String("0E+00");
    if (precision > 0) {
        StringBuilder sb("0.");
        AutoPtr<ArrayOf<Char32> > zeros = ArrayOf<Char32>::Alloc(precision);
        for (Int32 i = 0; i < precision; ++i) {
            (*zeros)[i] = '0';
        }
        sb.Append(*ZEROS);
        sb.Append("E+00");
        pattern = sb.ToString();
    }

    AutoPtr<INativeDecimalFormat> nf = GetDecimalFormat(pattern);
    AutoPtr<ArrayOf<Char32> > chars;
    if (IBigDecimal::Probe(mArg)) {
        nf->FormatBigDecimal((IBigDecimal*) mArg->Probe(EIID_IBigDecimal), NULL, (ArrayOf<Char32>**)&chars);
    }
    else {
        Double doublevalue = 0;
        ((INumber*) mArg->Probe(EIID_INumber))->DoubleValue(&doublevalue);
        nf->FormatDouble(doublevalue, NULL, (ArrayOf<Char32>**)&chars);
    }
    // Unlike %f, %e uses 'e' (regardless of what the DecimalFormatSymbols would have us use).
    for (Int32 i = 0; i < chars->GetLength(); ++i) {
        if ((*chars)[i] == 'E') {
            (*chars)[i] = 'e';
        }
    }
    result->Append(*chars);
    // The # flag requires that we always output a decimal separator.
    if (mFormatToken->mFlagSharp && precision == 0) {
        Int32 indexOfE = 0;
        result->IndexOf(String("e"), &indexOfE);
        Char32 decimalSeparator;
        mLocaleData->GetDecimalSeparator(&decimalSeparator);
        result->Insert(indexOfE, (Int32)decimalSeparator);
    }
}

void CFormatter::TransformG(
    /* [in] */ StringBuilder* result)
{
    Int32 precision = mFormatToken->GetPrecision();
    if (precision == 0) {
        precision = 1;
    }
    mFormatToken->SetPrecision(precision);

    Double d = 0;
    ((INumber*) mArg->Probe(EIID_INumber))->DoubleValue(&d);
    if (d == 0.0) {
        precision--;
        mFormatToken->SetPrecision(precision);
        TransformF(result);
        return;
    }

    Boolean requireScientificRepresentation = TRUE;
    d = Elastos::Core::Math::Abs(d);
    AutoPtr<IDouble> adouble;
    CDouble::New(d, (IDouble**)&adouble);
    Boolean isflag = FALSE;
    if (adouble->IsInfinite(&isflag)) {
        precision = mFormatToken->GetPrecision();
        precision--;
        mFormatToken->SetPrecision(precision);
        TransformE(result);
        return;
    }
    AutoPtr<IBigDecimal> b;
    AutoPtr<IMathContext> mc;
    CMathContext::New(precision, (IMathContext**)&mc);
    CBigDecimal::New(d, mc, (IBigDecimal**)&b);
    INumber::Probe(b)->DoubleValue(&d);
    Int64 l = 0;
    INumber::Probe(b)->Int64Value(&l);

    if (d >= 1 && d < Elastos::Core::Math::Pow(10, precision)) {
        if (l < Elastos::Core::Math::Pow(10, precision)) {
            requireScientificRepresentation = FALSE;
            precision -= StringUtils::ToString(l).GetLength();
            precision = precision < 0 ? 0 : precision;
            l = Elastos::Core::Math::Round(d * Elastos::Core::Math::Pow(10, precision + 1));
            if (StringUtils::ToString(l).GetLength() <= mFormatToken->GetPrecision()) {
                precision++;
            }
            mFormatToken->SetPrecision(precision);
        }
    }
    else {
        AutoPtr<IBigDecimal> bmove;
        b->MovePointRight(4, (IBigDecimal**)&bmove);
        INumber::Probe(bmove)->Int64Value(&l);
        if (d >= Elastos::Core::Math::Pow(10, -4) && d < 1) {
            requireScientificRepresentation = FALSE;
            precision += 4 - StringUtils::ToString(l).GetLength();
            bmove = NULL;
            b->MovePointRight(precision + 1, (IBigDecimal**)&bmove);
            INumber::Probe(bmove)->Int64Value(&l);
            if (StringUtils::ToString(l).GetLength() <= mFormatToken->GetPrecision()) {
                precision++;
            }
            bmove = NULL;
            b->MovePointRight(precision, (IBigDecimal**)&bmove);
            INumber::Probe(bmove)->Int64Value(&l);
            if (l >= Elastos::Core::Math::Pow(10, precision - 4)) {
                mFormatToken->SetPrecision(precision);
            }
        }
    }
    if (requireScientificRepresentation) {
        precision = mFormatToken->GetPrecision();
        precision--;
        mFormatToken->SetPrecision(precision);
        TransformE(result);
    }
    else {
        TransformF(result);
    }
}

void CFormatter::TransformF(
    /* [in] */ StringBuilder* result)
{
    // All zeros in this method are *pattern* characters, so no localization.
    String pattern("0.000000");
    Int32 precision = mFormatToken->GetPrecision();
    if (mFormatToken->mFlagComma || precision != FormatToken::DEFAULT_PRECISION) {
        StringBuilder patternBuilder;
        if (mFormatToken->mFlagComma) {
            patternBuilder.AppendChar(',');
            Int32 groupingSize = 3;
            AutoPtr<ArrayOf<Char32> > sharps = ArrayOf<Char32>::Alloc(groupingSize - 1);
            for (Int32 i = 0; i < sharps->GetLength(); ++i) {
                (*sharps)[i] = '#';
            }
            patternBuilder.Append(*sharps);
        }
        patternBuilder.AppendChar('0');
        if (precision > 0) {
            patternBuilder.AppendChar('.');
            for (Int32 i = 0; i < precision; ++i) {
                patternBuilder.AppendChar('0');
            }
        }
        pattern = patternBuilder.ToString();
    }

    AutoPtr<INativeDecimalFormat> nf = GetDecimalFormat(pattern);
    if (IBigDecimal::Probe(mArg)) {
        AutoPtr<ArrayOf<Char32> > chars;
        nf->FormatBigDecimal((IBigDecimal*) mArg->Probe(EIID_IBigDecimal), NULL, (ArrayOf<Char32>**)&chars);
        result->Append(*chars);
    }
    else {
        AutoPtr<ArrayOf<Char32> > chars;
        Double dvalue = 0;
        ((INumber*) mArg->Probe(EIID_INumber))->DoubleValue(&dvalue);
        nf->FormatDouble(dvalue, NULL, (ArrayOf<Char32>**)&chars);
        result->Append(*chars);
    }
    // The # flag requires that we always output a decimal separator.
    if (mFormatToken->mFlagSharp && precision == 0) {
        Char32 decimalSeparator;
        mLocaleData->GetDecimalSeparator(&decimalSeparator);
        result->Append((Int32)decimalSeparator);
    }
}

void CFormatter::TransformA(
    /* [in] */ StringBuilder* result)
{
    if (IFloat::Probe(mArg)) {
        Float fvalue = 0;
        ((IFloat*) mArg->Probe(EIID_IFloat))->GetValue(&fvalue);
        result->Append(StringUtils::ToHexString(fvalue));
    }
    else if (IDouble::Probe(mArg)) {
        Double dvalue = 0;
        ((IDouble*) mArg->Probe(EIID_IDouble))->GetValue(&dvalue);
        result->Append(StringUtils::ToHexString(dvalue));
    }
    else {
        // throw badArgumentType();
        return;
    }

    if (!mFormatToken->IsPrecisionSet()) {
        return;
    }

    Int32 precision = mFormatToken->GetPrecision();
    if (precision == 0) {
        precision = 1;
    }
    Int32 indexOfFirstFractionalDigit = 0;
    result->IndexOf(String("."), &indexOfFirstFractionalDigit);
    indexOfFirstFractionalDigit += 1;
    Int32 indexOfP = 0;
    result->IndexOf(String("p"), &indexOfP);
    Int32 fractionalLength = indexOfP - indexOfFirstFractionalDigit;

    if (fractionalLength == precision) {
        return;
    }

    if (fractionalLength < precision) {
        AutoPtr<ArrayOf<Char32> > zeros = ArrayOf<Char32>::Alloc(precision - fractionalLength);
        for (Int32 i = 0; i < zeros->GetLength(); ++i) {
            (*zeros)[i] = '0'; // %a shouldn't be localized.
        }
        result->Insert(indexOfP, *zeros);
        return;
    }
    result->Delete(indexOfFirstFractionalDigit + precision, indexOfP);
}


//==========================================================
//       CFormatter::CachedDecimalFormat
//==========================================================
CFormatter::CachedDecimalFormat::CachedDecimalFormat()
{
}

AutoPtr<INativeDecimalFormat> CFormatter::CachedDecimalFormat::Update(
    /* [in] */ ILocaleData* localeData,
    /* [in] */ const String& pattern)
{
    if (mDecimalFormat == NULL) {
        mCurrentPattern = pattern;
        mCurrentLocaleData = localeData;
//        CNativeDecimalFormat::New(mCurrentPattern, mCurrentLocaleData, (INativeDecimalFormat**)&mDecimalFormat);
    }
    if (!pattern.Equals(mCurrentPattern)) {
        mDecimalFormat->ApplyPattern(pattern);
        mCurrentPattern = pattern;
    }
    if (localeData != mCurrentLocaleData) {
        mDecimalFormat->SetDecimalFormatSymbols(localeData);
        mCurrentLocaleData = localeData;
    }
    return mDecimalFormat;
}


//==========================================================
//       CFormatter::FormatToken
//==========================================================

const Int32 CFormatter::FormatToken::LAST_ARGUMENT_INDEX = -2;

const Int32 CFormatter::FormatToken::UNSET = -1;

const Int32 CFormatter::FormatToken::FLAGS_UNSET = 0;

const Int32 CFormatter::FormatToken::DEFAULT_PRECISION = 6;

const Int32 CFormatter::FormatToken::FLAG_ZERO = 1 << 4;

CFormatter::FormatToken::FormatToken()
{
    mFlagComma = FALSE;
    mFlagMinus = FALSE;
    mFlagParenthesis = FALSE;
    mFlagPlus = FALSE;
    mFlagSharp = FALSE;
    mFlagSpace = FALSE;
    mFlagZero = FALSE;
    mArgIndex = UNSET;
    mConversionType = UNSET;
    mDateSuffix = 0;
    mPrecision = UNSET;
    mWidth = UNSET;
}

Boolean CFormatter::FormatToken::IsDefault()
{
    return !mFlagComma && !mFlagMinus && !mFlagParenthesis && !mFlagPlus && !mFlagSharp &&
                    !mFlagSpace && !mFlagZero && mWidth == UNSET && mPrecision == UNSET;
}

Boolean CFormatter::FormatToken::IsPrecisionSet()
{
    return mPrecision != UNSET;
}

Int32 CFormatter::FormatToken::GetArgIndex()
{
    return mArgIndex;
}

void CFormatter::FormatToken::SetArgIndex(
    /* [in] */ Int32 index)
{
    mArgIndex = index;
}

Int32 CFormatter::FormatToken::GetWidth()
{
    return mWidth;
}

void CFormatter::FormatToken::SetWidth(
    /* [in] */ Int32 width)
{
    mWidth = width;
}

Int32 CFormatter::FormatToken::GetPrecision()
{
    return mPrecision;
}

void CFormatter::FormatToken::SetPrecision(
    /* [in] */ Int32 precise)
{
    mPrecision = precise;
}

String CFormatter::FormatToken::GetStrFlags()
{
    return (mStrFlags.GetLength()) ? mStrFlags.ToString() : String("");
}

Boolean CFormatter::FormatToken::SetFlag(
    /* [in] */ Int32 ch)
{
    Boolean dupe = FALSE;
    switch (ch) {
        case ',':
            dupe = mFlagComma;
            mFlagComma = TRUE;
            break;
        case '-':
            dupe = mFlagMinus;
            mFlagMinus = TRUE;
            break;
        case '(':
            dupe = mFlagParenthesis;
            mFlagParenthesis = TRUE;
            break;
        case '+':
            dupe = mFlagPlus;
            mFlagPlus = TRUE;
            break;
        case '#':
            dupe = mFlagSharp;
            mFlagSharp = TRUE;
            break;
        case ' ':
            dupe = mFlagSpace;
            mFlagSpace = TRUE;
            break;
        case '0':
            dupe = mFlagZero;
            mFlagZero = TRUE;
            break;
        default:
            return FALSE;
    }
    if (dupe) {
        // The RI documentation implies we're supposed to report all the flags, not just
        // the first duplicate, but the RI behaves the same as we do.
        // throw new DuplicateFormatFlagsException(String.valueOf(ch));
        return FALSE;
    }
    // if (mStrFlags == null) {
    //     strFlags = new StringBuilder(7); // There are seven possible flags.
    // }
    mStrFlags.AppendChar(ch);
    return TRUE;
}

Char32 CFormatter::FormatToken::GetConversionType()
{
    return mConversionType;
}

void CFormatter::FormatToken::SetConversionType(
    /* [in] */ Char32 c)
{
    mConversionType = c;
}

Char32 CFormatter::FormatToken::GetDateSuffix()
{
    return mDateSuffix;
}

void CFormatter::FormatToken::SetDateSuffix(
    /* [in] */ Char32 c)
{
    mDateSuffix = c;
}

Boolean CFormatter::FormatToken::RequireArgument()
{
    return mConversionType != '%' && mConversionType != 'n';
}

ECode CFormatter::FormatToken::CheckFlags(
    /* [in] */ IInterface* mArg)
{
    // Work out which flags are allowed.
    Boolean allowComma = FALSE;
    Boolean allowMinus = TRUE;
    Boolean allowParenthesis = FALSE;
    Boolean allowPlus = FALSE;
    Boolean allowSharp = FALSE;
    Boolean allowSpace = FALSE;
    Boolean allowZero = FALSE;
    // Precision and width?
    Boolean allowPrecision = TRUE;
    Boolean allowWidth = TRUE;
    // Argument?
    Boolean allowArgument = TRUE;
    switch (mConversionType) {
        // Character and date/time.
        case 'c':
        case 'C':
        case 't':
        case 'T':
            // Only '-' is allowed.
            allowPrecision = FALSE;
            break;

        // String.
        case 's':
        case 'S':
            if (IFormattable::Probe(mArg)) {
                allowSharp = TRUE;
            }
            break;

        // Floating point.
        case 'g':
        case 'G':
            allowComma = allowParenthesis = allowPlus = allowSpace = allowZero = TRUE;
            break;

        case 'f':
            allowComma = allowParenthesis = allowPlus = allowSharp = allowSpace = allowZero = TRUE;
            break;

        case 'e':
        case 'E':
            allowParenthesis = allowPlus = allowSharp = allowSpace = allowZero = TRUE;
            break;

        case 'a':
        case 'A':
            allowPlus = allowSharp = allowSpace = allowZero = TRUE;
            break;

        // Integral.
        case 'd':
            allowComma = allowParenthesis = allowPlus = allowSpace = allowZero = TRUE;
            allowPrecision = FALSE;
            break;

        case 'o':
        case 'x':
        case 'X':
            allowSharp = allowZero = TRUE;
            if (mArg == NULL || IBigInteger::Probe(mArg)) {
                allowParenthesis = allowPlus = allowSpace = TRUE;
            }
            allowPrecision = FALSE;
            break;

        // Special.
        case 'n':
            // Nothing is allowed.
            allowMinus = FALSE;
            allowArgument = allowPrecision = allowWidth = FALSE;
            break;

        case '%':
            // The only flag allowed is '-', and no mArgument or precision is allowed.
            allowArgument = FALSE;
            allowPrecision = FALSE;
            break;

        // Booleans and hash codes.
        case 'b':
        case 'B':
        case 'h':
        case 'H':
            break;

        default:
            // throw unknownFormatConversionException();
            return E_UNKNOWN_FORMAT_CONVERSION_EXCEPTION;
    }

    // Check for disallowed flags.
    String mismatch(NULL);
    if (!allowComma && mFlagComma) {
        mismatch = ",";
    }
    else if (!allowMinus && mFlagMinus) {
        mismatch = "-";
    }
    else if (!allowParenthesis && mFlagParenthesis) {
        mismatch = "(";
    }
    else if (!allowPlus && mFlagPlus) {
        mismatch = "+";
    }
    else if (!allowSharp && mFlagSharp) {
        mismatch = "#";
    }
    else if (!allowSpace && mFlagSpace) {
        mismatch = " ";
    }
    else if (!allowZero && mFlagZero) {
        mismatch = "0";
    }
    if (!mismatch.IsNull()) {
        if (mConversionType == 'n') {
            // For no good reason, %n is a special case...
            // throw new IllegalFormatFlagsException(mismatch);
            return E_ILLEGAL_FORMAT_FLAGS_EXCEPTION;
        }
        else {
            // throw new FormatFlagsConversionMismatchException(mismatch, mConversionType);
            return E_FORMAT_FLAGS_CONVERSION_MISMATCH_EXCEPTION;
        }
    }

    // Check for a missing width with flags that require a width.
    if ((mFlagMinus || mFlagZero) && mWidth == UNSET) {
        // throw new MissingFormatWidthException("-" + mConversionType);
        return E_MISSING_FORMAT_WIDTH_EXCEPTION;
    }

    // Check that no-mArgument conversion types don't have an mArgument.
    // Note: the RI doesn't enforce this.
    if (!allowArgument && mArgIndex != UNSET) {
        // throw new IllegalFormatFlagsException("%" + mConversionType + " doesn't take an mArgument");
        return E_ILLEGAL_FORMAT_FLAGS_EXCEPTION;
    }

    // Check that we don't have a precision or width where they're not allowed.
    if (!allowPrecision && mPrecision != UNSET) {
        // throw new IllegalFormatPrecisionException(precision);
        return E_ILLEGAL_FORMAT_PRECISION_EXCEPTION;
    }
    if (!allowWidth && mWidth != UNSET) {
        // throw new IllegalFormatWidthException(width);
        return E_ILLEGAL_FORMAT_WIDTH_EXCEPTION;
    }

    // Some combinations make no sense...
    if (mFlagPlus && mFlagSpace) {
        // throw new IllegalFormatFlagsException("the '+' and ' ' flags are incompatible");
        return E_ILLEGAL_FORMAT_FLAGS_EXCEPTION;
    }
    if (mFlagMinus && mFlagZero) {
        // throw new IllegalFormatFlagsException("the '-' and '0' flags are incompatible");
        return E_ILLEGAL_FORMAT_FLAGS_EXCEPTION;
    }
    return NOERROR;
}

ECode CFormatter::FormatToken::UnknownFormatConversionException()
{
    if (mConversionType == 't' || mConversionType == 'T') {
        // throw new UnknownFormatConversionException(String.format("%c%c", mConversionType, dateSuffix));
        return E_UNKNOWN_FORMAT_CONVERSION_EXCEPTION;
    }
    // throw new UnknownFormatConversionException(String.valueOf(mConversionType));
    return E_UNKNOWN_FORMAT_CONVERSION_EXCEPTION;
}


//==========================================================
//       CFormatter::FormatSpecifierParser
//==========================================================
CFormatter::FormatSpecifierParser::FormatSpecifierParser(
    /* [in] */ const String& format)
    : mFormat(format)
    , mLength(format.GetLength())
    , mStartIndex(0)
    , mI(0)
{
}

AutoPtr<CFormatter::FormatToken> CFormatter::FormatSpecifierParser::ParseFormatToken(
    /* [in] */ Int32 offset)
{
    mStartIndex = offset;
    mI = offset;
    AutoPtr<FormatToken> ft = new FormatToken();
    return ParseArgumentIndexAndFlags(ft);
}

String CFormatter::FormatSpecifierParser::GetFormatSpecifierText()
{
    return mFormat.Substring(mStartIndex, mI);
}

Int32 CFormatter::FormatSpecifierParser::Peek()
{
    return (mI < mLength) ? mFormat.GetChar(mI) : -1;
}

Char32 CFormatter::FormatSpecifierParser::Advance()
{
    if (mI >= mLength) {
        // throw unknownFormatConversionException();
        return E_UNKNOWN_FORMAT_CONVERSION_EXCEPTION;
    }
    return mFormat.GetChar(mI++);
}

ECode CFormatter::FormatSpecifierParser::UnknownFormatConversionException()
{
    // throw new UnknownFormatConversionException(getFormatSpecifierText());
    return E_UNKNOWN_FORMAT_CONVERSION_EXCEPTION;
}

AutoPtr<CFormatter::FormatToken> CFormatter::FormatSpecifierParser::ParseArgumentIndexAndFlags(
    /* [in] */ CFormatter::FormatToken* token)
{
    // Parse the mArgument index, if there is one.
    Int32 position = mI;
    Int32 ch = Peek();
    if (Character::IsDigit(ch)) {
        Int32 number = NextInt();
        if (Peek() == '$') {
            // The number was an mArgument index.
            Advance(); // Swallow the '$'.
            if (number == FormatToken::UNSET) {
                // throw new MissingFormatArgumentException(getFormatSpecifierText());
                return NULL;
            }
            // k$ stands for the mArgument whose index is k-1 except that
            // 0$ and 1$ both stand for the first element.
            token->SetArgIndex(Elastos::Core::Math::Max(0, number - 1));
        }
        else {
            if (ch == '0') {
                // The digit zero is a format flag, so reparse it as such.
                mI = position;
            }
            else {
                // The number was a width. This means there are no flags to parse.
                return ParseWidth(token, number);
            }
        }
    }
    else if (ch == '<') {
        token->SetArgIndex(FormatToken::LAST_ARGUMENT_INDEX);
        Advance();
    }

    // Parse the flags.
    while (token->SetFlag(Peek())) {
        Advance();
    }

    // What comes next?
    ch = Peek();
    if (Character::IsDigit(ch)) {
        return ParseWidth(token, NextInt());
    }
    else if (ch == '.') {
        return ParsePrecision(token);
    }
    else {
        return ParseConversionType(token);
    }
}

AutoPtr<CFormatter::FormatToken> CFormatter::FormatSpecifierParser::ParseWidth(
    /* [in] */ CFormatter::FormatToken* token,
    /* [in] */ Int32 width)
{
    token->SetWidth(width);
    Int32 ch = Peek();
    if (ch == '.') {
        return ParsePrecision(token);
    }
    else {
        return ParseConversionType(token);
    }
}

AutoPtr<CFormatter::FormatToken> CFormatter::FormatSpecifierParser::ParsePrecision(
    /* [in] */ CFormatter::FormatToken* token)
{
    Advance(); // Swallow the '.'.
    Int32 ch = Peek();
    if (Character::IsDigit(ch)) {
        token->SetPrecision(NextInt());
        return ParseConversionType(token);
    }
    else {
        // The precision is required but not given by the format string.
        // throw unknownFormatConversionException();
        return NULL;
    }
}

AutoPtr<CFormatter::FormatToken> CFormatter::FormatSpecifierParser::ParseConversionType(
    /* [in] */ CFormatter::FormatToken* token)
{
    Char32 conversionType = Advance(); // A conversion type is mandatory.
    token->SetConversionType(conversionType);
    if (conversionType == 't' || conversionType == 'T') {
        Char32 dateSuffix = Advance(); // A date suffix is mandatory for 't' or 'T'.
        token->SetDateSuffix(dateSuffix);
    }
    return token;
}

Int32 CFormatter::FormatSpecifierParser::NextInt()
{
    Int64 value = 0;
    AutoPtr<ArrayOf<Char32> > char32Array = mFormat.GetChars();
    while (mI < mLength && Character::IsDigit((*char32Array)[mI])) {
        value = 10 * value + ((*char32Array)[mI++] - '0');
        if (value > Elastos::Core::Math::INT32_MAX_VALUE) {
            return FailNextInt();
        }
    }
    return (Int32) value;
}

Int32 CFormatter::FormatSpecifierParser::FailNextInt()
{
    while (Character::IsDigit(Peek())) {
        Advance();
    }
    return FormatToken::UNSET;
}

} // namespace Utility
} // namespace Elastos
