
#include "elastos/droid/internal/os/LoggingPrintStream.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/Character.h>
#include <elastos/core/Math.h>
#include <elastos/core/StringUtils.h>

using Elastos::Core::Character;
using Elastos::Core::StringUtils;
using Elastos::IO::IBuffer;
using Elastos::IO::CByteBufferHelper;
using Elastos::IO::IByteBufferHelper;
using Elastos::IO::CCharBufferHelper;
using Elastos::IO::ICharBufferHelper;
using Elastos::IO::Charset::ICharset;
using Elastos::IO::Charset::CCharsetHelper;
using Elastos::IO::Charset::ICharsetHelper;
using Elastos::IO::Charset::ICodingErrorAction;
using Elastos::IO::Charset::CCodingErrorActionHelper;
using Elastos::IO::Charset::ICodingErrorActionHelper;
using Elastos::IO::Charset::ICoderResult;
using Elastos::Utility::CFormatter;
using Elastos::Utility::CLocaleHelper;
using Elastos::Utility::ILocaleHelper;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Os {

ECode LoggingPrintStream::MyOutputStream::Write(
    /* [in] */ Int32 oneByte)
{
    return E_ASSERTION_ERROR;
}

LoggingPrintStream::LoggingPrintStream()
{
    mBuilder = new StringBuilder();
    AutoPtr<OutputStream> outputStream = new MyOutputStream();
    PrintStream::constructor(outputStream);
    CFormatter::New(mBuilder, NULL, (IFormatter**)&mFormatter);
}

ECode LoggingPrintStream::Flush()
{
    AutoLock lock(this);
    Flush(TRUE);

    return NOERROR;
}

void LoggingPrintStream::Flush(
    /* [in] */ Boolean completely)
{
    Int32 length = mBuilder->GetLength();
    Int32 start = 0;
    Int32 nextBreak;

    // Log one line for each line break.
    while (start < length
        && (mBuilder->IndexOf(String("\n"), start, &nextBreak), nextBreak) != -1) {
        String subStr;
        mBuilder->Substring(start, nextBreak, &subStr);
        Log(subStr);
        start = nextBreak + 1;
    }

    if (completely) {
        // Log the remainder of the buffer.
        if (start < length) {
            String subStr;
            mBuilder->Substring(start, &subStr);
            Log(subStr);
        }
        mBuilder->SetLength(0);
    }
    else {
        // Delete characters leading up to the next starting point.
        mBuilder->Delete(0, start);
    }
}

ECode LoggingPrintStream::Write(
    /* [in] */ Int32 oneByte)
{
    AutoPtr<ArrayOf<Byte> > bytes = ArrayOf<Byte>::Alloc(1);
    (*bytes)[0] = oneByte;

    return Write(bytes, 0, 1);
}

ECode LoggingPrintStream::Write(
    /* [in] */ ArrayOf<Byte>* buffer)
{
    return Write(buffer, 0, buffer->GetLength());
}

ECode LoggingPrintStream::Write(
    /* [in] */ ArrayOf<Byte>* bytes,
    /* [in] */ Int32 start,
    /* [in] */ Int32 count)
{
    AutoLock lock(this);

    if (mDecoder == NULL) {
        AutoPtr<IByteBufferHelper> bbHelper;
        CByteBufferHelper::AcquireSingleton((IByteBufferHelper**)&bbHelper);
        bbHelper->Allocate(80, (IByteBuffer**)&mEncodedBytes);
        AutoPtr<ICharBufferHelper> cbHelper;
        CCharBufferHelper::AcquireSingleton((ICharBufferHelper**)&cbHelper);
        cbHelper->Allocate(80, (ICharBuffer**)&mDecodedChars);
        AutoPtr<ICharsetHelper> cHelper;
        CCharsetHelper::AcquireSingleton((ICharsetHelper**)&cHelper);
        AutoPtr<ICharset> charset;
        FAIL_RETURN(cHelper->DefaultCharset((ICharset**)&charset))
        FAIL_RETURN(charset->NewDecoder((ICharsetDecoder**)&mDecoder))

        AutoPtr<ICodingErrorActionHelper> ceaHelper;
        CCodingErrorActionHelper::AcquireSingleton((ICodingErrorActionHelper**)&ceaHelper);
        AutoPtr<ICodingErrorAction> replace;
        ceaHelper->GetREPLACE((ICodingErrorAction**)&replace);
        FAIL_RETURN(mDecoder->OnMalformedInput(replace))
        FAIL_RETURN(mDecoder->OnUnmappableCharacter(replace))
    }

    Int32 end = start + count;
    while (start < end) {
        // copy some bytes from the array to the long-lived buffer. This
        // way, if we end with a partial character we don't lose it.
        Int32 remaining;
        IBuffer::Probe(mEncodedBytes)->GetRemaining(&remaining);
        Int32 numBytes = Elastos::Core::Math::Min(remaining, end - start);
        mEncodedBytes->Put(bytes, start, numBytes);
        start += numBytes;

        IBuffer::Probe(mEncodedBytes)->Flip();
        AutoPtr<ICoderResult> coderResult;
        Boolean isOverflow = FALSE;
        do {
            // decode bytes from the byte buffer into the char buffer
            mDecoder->Decode(mEncodedBytes, mDecodedChars, FALSE, (ICoderResult**)&coderResult);

            // copy chars from the char buffer into our string builder
            IBuffer::Probe(mDecodedChars)->Flip();
            mBuilder->Append(mDecodedChars);
            IBuffer::Probe(mDecodedChars)->Clear();
        } while ((coderResult->IsOverflow(&isOverflow), isOverflow));
        mEncodedBytes->Compact();
    }
    Flush(FALSE);

    return NOERROR;
}

ECode LoggingPrintStream::CheckError(
    /* [out] */ Boolean* hasError)
{
    VALIDATE_NOT_NULL(hasError);
    *hasError = FALSE;
    return NOERROR;
}

ECode LoggingPrintStream::SetError()
{
    /* ignored */
    return NOERROR;
}

ECode LoggingPrintStream::Close()
{
    /* ignored */
    return NOERROR;
}

ECode LoggingPrintStream::Format(
    /* [in] */ const String& format,
    /* [in] */ ArrayOf<IInterface*>* args,
    /* [out] */ IPrintStream** pw)
{
    VALIDATE_NOT_NULL(pw)
    AutoPtr<ILocaleHelper> localeHelper;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&localeHelper);
    AutoPtr<ILocale> ouloc;
    localeHelper->GetDefault((ILocale**)&ouloc);
    return Format(ouloc, format, args, pw);
}

ECode LoggingPrintStream::Printf(
    /* [in] */ const String& format,
    /* [in] */ ArrayOf<IInterface*>* args,
    /* [out] */ IPrintStream** pw)
{
    return Format(format, args, pw);
}

ECode LoggingPrintStream::Printf(
    /* [in] */ ILocale* l,
    /* [in] */ const String& format,
    /* [in] */ ArrayOf<IInterface*>* args,
    /* [out] */ IPrintStream** ps)
{
    return Format(l, format, args, ps);
}

ECode LoggingPrintStream::Format(
    /* [in] */ ILocale* l,
    /* [in] */ const String& format,
    /* [in] */ ArrayOf<IInterface*>* args,
    /* [out] */ IPrintStream** pw)
{
    VALIDATE_NOT_NULL(pw)
    *pw = NULL;

    if (format.IsNull()) {
        return E_NULL_POINTER_EXCEPTION;
    }

    AutoLock lock(this);
    mFormatter->Format(l, format, args);
    Flush(FALSE);
    *pw = this;
    REFCOUNT_ADD(*pw)
    return NOERROR;
}

ECode LoggingPrintStream::Print(
    /* [in] */ ArrayOf<Char32>* charArray)
{
    AutoLock lock(this);
    mBuilder->Append(*charArray);
    Flush(FALSE);
    return NOERROR;
}

ECode LoggingPrintStream::PrintChar(
    /* [in] */ Char32 ch)
{
    AutoLock lock(this);
    mBuilder->AppendChar(ch);
    if (ch == '\n')
        Flush(FALSE);
    return NOERROR;
}

ECode LoggingPrintStream::Print(
    /* [in] */ Double dnum)
{
    AutoLock lock(this);
    mBuilder->Append(dnum);
    return NOERROR;
}

ECode LoggingPrintStream::Print(
    /* [in] */ Float fnum)
{
    AutoLock lock(this);
    mBuilder->Append(fnum);
    return NOERROR;
}

ECode LoggingPrintStream::Print(
    /* [in] */ Int32 inum)
{
    AutoLock lock(this);
    mBuilder->Append(inum);
    return NOERROR;
}

ECode LoggingPrintStream::Print(
    /* [in] */ Int64 lnum)
{
    AutoLock lock(this);
    mBuilder->Append(lnum);
    return NOERROR;
}

ECode LoggingPrintStream::Print(
    /* [in] */ IInterface* obj)
{
    AutoLock lock(this);
    mBuilder->Append(obj);
    Flush(FALSE);
    return NOERROR;
}

ECode LoggingPrintStream::Print(
    /* [in] */ const String& str)
{
    AutoLock lock(this);
    mBuilder->Append(str);
    Flush(FALSE);
    return NOERROR;
}

ECode LoggingPrintStream::Print(
    /* [in] */ Boolean result)
{
    AutoLock lock(this);
    mBuilder->Append(result);
    return NOERROR;
}

ECode LoggingPrintStream::Println()
{
    AutoLock lock(this);
    Flush(TRUE);
    return NOERROR;
}

ECode LoggingPrintStream::Println(
    /* [in] */ ArrayOf<Char32>* charArray)
{
    AutoLock lock(this);
    mBuilder->Append(*charArray);
    Flush(TRUE);
    return NOERROR;
}

ECode LoggingPrintStream::PrintCharln(
    /* [in] */ Char32 ch)
{
    AutoLock lock(this);
    mBuilder->AppendChar(ch);
    Flush(TRUE);
    return NOERROR;
}

ECode LoggingPrintStream::Println(
    /* [in] */ Double dnum)
{
    AutoLock lock(this);
    mBuilder->Append(dnum);
    Flush(TRUE);
    return NOERROR;
}

ECode LoggingPrintStream::Println(
    /* [in] */ Float fnum)
{
    AutoLock lock(this);
    mBuilder->Append(fnum);
    Flush(TRUE);
    return NOERROR;
}

ECode LoggingPrintStream::Println(
    /* [in] */ Int32 inum)
{
    AutoLock lock(this);
    mBuilder->Append(inum);
    Flush(TRUE);
    return NOERROR;
}

ECode LoggingPrintStream::Println(
    /* [in] */ Int64 lnum)
{
    AutoLock lock(this);
    mBuilder->Append(lnum);
    Flush(TRUE);
    return NOERROR;
}

ECode LoggingPrintStream::Println(
    /* [in] */ IInterface* obj)
{
    AutoLock lock(this);
    mBuilder->Append(obj);
    Flush(TRUE);
    return NOERROR;
}

ECode LoggingPrintStream::Println(
    /* [in] */ const String& s)
{
    if (mBuilder->GetLength() == 0 && s != NULL) {
        // Optimization for a simple println.
        Int32 length = s.GetLength();

        Int32 start = 0;
        Int32 nextBreak;

        // Log one line for each line break.
        while (start < length
            && (nextBreak = s.IndexOf('\n', start)) != -1) {
            Log(s.Substring(start, nextBreak));
            start = nextBreak + 1;
        }

        if (start < length) {
            Log(s.Substring(start));
        }
    }
    else {
        mBuilder->Append(s);
        Flush(TRUE);
    }
    return NOERROR;
}

ECode LoggingPrintStream::Println(
    /* [in] */ Boolean result)
{
    AutoLock lock(this);
    mBuilder->Append(result);
    Flush(TRUE);
    return NOERROR;
}

ECode LoggingPrintStream::AppendChar(
    /* [in] */ Char32 c)
{
    AutoLock lock(this);
    mBuilder->AppendChar(c);
    return NOERROR;
}

ECode LoggingPrintStream::Append(
    /* [in] */ ICharSequence* csq)
{
    AutoLock lock(this);
    mBuilder->Append(csq);
    Flush(FALSE);
    return NOERROR;
}

ECode LoggingPrintStream::Append(
    /* [in] */ ICharSequence* csq,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    AutoLock lock(this);
    mBuilder->Append(csq, start, end);
    Flush(FALSE);
    return NOERROR;
}

} // namespace Os
} // namespace Internal
} // namespace Droid
} // namespace Elastos
