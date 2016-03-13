
#include "Elastos.Droid.Utility.h"
#include "elastos/droid/internal/utility/FastPrintWriter.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::AutoLock;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::IO::IBuffer;
using Elastos::IO::CByteBufferHelper;
using Elastos::IO::IByteBufferHelper;
using Elastos::IO::ICharBuffer;
using Elastos::IO::CCharBufferHelper;
using Elastos::IO::ICharBufferHelper;
using Elastos::IO::ICloseable;
using Elastos::IO::IFlushable;
using Elastos::IO::Charset::ICharset;
using Elastos::IO::Charset::CCharsetHelper;
using Elastos::IO::Charset::ICharsetHelper;
using Elastos::IO::Charset::ICodingErrorAction;
using Elastos::IO::Charset::CCodingErrorActionHelper;
using Elastos::IO::Charset::ICodingErrorActionHelper;
using Elastos::IO::Charset::ICoderResult;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {

AutoPtr<IWriter> FastPrintWriter::sDummyWriter = new DummyWriter();

ECode FastPrintWriter::DummyWriter::Close()
{
    Logger::E("FastPrintWriter", "Shouldn't be here");
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode FastPrintWriter::DummyWriter::Flush()
{
    return Close();
}

ECode FastPrintWriter::DummyWriter::Write(
    /* [in] */ ArrayOf<Char32>* buffer,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 count)
{
    return Close();
}

CAR_INTERFACE_IMPL(FastPrintWriter, PrintWriter, IFastPrintWriter)

FastPrintWriter::FastPrintWriter()
    : mBufferLen(0)
    , mPos(0)
    , mAutoFlush(FALSE)
    , mIoError(FALSE)
{
}

ECode FastPrintWriter::constructor(
    /* [in] */ IOutputStream* out)
{
    return constructor(out, FALSE, 8192);
}

ECode FastPrintWriter::constructor(
    /* [in] */ IOutputStream* out,
    /* [in] */ Boolean autoFlush)
{
    return constructor(out, autoFlush, 8192);
}

ECode FastPrintWriter::constructor(
    /* [in] */ IOutputStream* out,
    /* [in] */ Boolean autoFlush,
    /* [in] */ Int32 bufferLen)
{
    if (out == NULL) {
        Logger::D("FastPrintWriter", "out is NULL");
        return E_NULL_POINTER_EXCEPTION;
    }

    FAIL_RETURN(PrintWriter::constructor(sDummyWriter, autoFlush))

    mBufferLen = bufferLen;
    mText = ArrayOf<Char32>::Alloc(bufferLen);
    AutoPtr<IByteBufferHelper> bbHelper;
    CByteBufferHelper::AcquireSingleton((IByteBufferHelper**)&bbHelper);
    FAIL_RETURN(bbHelper->Allocate(mBufferLen, (IByteBuffer**)&mBytes))

    mOutputStream = out;
    mWriter = NULL;
    mPrinter = NULL;
    mAutoFlush = autoFlush;
    AutoPtr<ISystem> sys;
    CSystem::AcquireSingleton((ISystem**)&sys);
    sys->GetLineSeparator(&mSeparator);
    return InitDefaultEncoder();
}

ECode FastPrintWriter::constructor(
    /* [in] */ IWriter* wr)
{
    return constructor(wr, FALSE, 8192);
}

ECode FastPrintWriter::constructor(
    /* [in] */ IWriter* wr,
    /* [in] */ Boolean autoFlush)
{
    return constructor(wr, autoFlush, 8192);
}

ECode FastPrintWriter::constructor(
    /* [in] */ IWriter* wr,
    /* [in] */ Boolean autoFlush,
    /* [in] */ Int32 bufferLen)
{
    FAIL_RETURN(PrintWriter::constructor(sDummyWriter, autoFlush))
    if (wr == NULL) {
        Logger::E("FastPrintWriter", "wr is NULL");
        return E_NULL_POINTER_EXCEPTION;
    }
    mBufferLen = bufferLen;
    mText = ArrayOf<Char32>::Alloc(bufferLen);
    mBytes = NULL;
    mOutputStream = NULL;
    mWriter = wr;
    mPrinter = NULL;
    mAutoFlush = autoFlush;
    AutoPtr<ISystem> sys;
    CSystem::AcquireSingleton((ISystem**)&sys);
    sys->GetLineSeparator(&mSeparator);
    return InitDefaultEncoder();
}

ECode FastPrintWriter::constructor(
    /* [in] */ IPrinter* pr)
{
    return constructor(pr, 512);
}

ECode FastPrintWriter::constructor(
    /* [in] */ IPrinter* pr,
    /* [in] */ Int32 bufferLen)
{
    FAIL_RETURN(PrintWriter::constructor(sDummyWriter, TRUE));
    if (pr == NULL) {
        Logger::E("FastPrintWriter", "pr is NULL");
        return E_NULL_POINTER_EXCEPTION;
    }
    mBufferLen = bufferLen;
    mText = ArrayOf<Char32>::Alloc(bufferLen);
    mBytes = NULL;
    mOutputStream = NULL;
    mWriter = NULL;
    mPrinter = pr;
    mAutoFlush = TRUE;
    AutoPtr<ISystem> sys;
    CSystem::AcquireSingleton((ISystem**)&sys);
    sys->GetLineSeparator(&mSeparator);
    return InitDefaultEncoder();
}

ECode FastPrintWriter::InitEncoder(
    /* [in] */ const String& csn)
{
    ECode ec = NOERROR;
    do {
        AutoPtr<ICharsetHelper> cHelper;
        CCharsetHelper::AcquireSingleton((ICharsetHelper**)&cHelper);
        AutoPtr<ICharset> charset;
        ec = cHelper->ForName(csn, (ICharset**)&charset);
        if (FAILED(ec))
            break;
        mCharset = NULL;
        ec = charset->NewEncoder((ICharsetEncoder**)&mCharset);
    } while (0);
    if (FAILED(ec)) {
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }

    AutoPtr<ICodingErrorActionHelper> ceaHelper;
    CCodingErrorActionHelper::AcquireSingleton((ICodingErrorActionHelper**)&ceaHelper);
    AutoPtr<ICodingErrorAction> replace;
    ceaHelper->GetREPLACE((ICodingErrorAction**)&replace);
    FAIL_RETURN(mCharset->OnMalformedInput(replace))
    return mCharset->OnUnmappableCharacter(replace);
}

ECode FastPrintWriter::CheckError(
    /* [out] */ Boolean* hasError)
{
    VALIDATE_NOT_NULL(hasError)
    Flush();
    AutoLock lock(mLock);
    *hasError = mIoError;
    return NOERROR;
}

ECode FastPrintWriter::ClearError()
{
    AutoLock lock(mLock);
    mIoError = FALSE;
    return NOERROR;
}

void FastPrintWriter::SetError()
{
    AutoLock lock(mLock);
    mIoError = TRUE;
}

ECode FastPrintWriter::InitDefaultEncoder()
{
    AutoPtr<ICharsetHelper> cHelper;
    CCharsetHelper::AcquireSingleton((ICharsetHelper**)&cHelper);
    AutoPtr<ICharset> charset;
    FAIL_RETURN(cHelper->DefaultCharset((ICharset**)&charset))
    mCharset = NULL;
    FAIL_RETURN(charset->NewEncoder((ICharsetEncoder**)&mCharset))

    AutoPtr<ICodingErrorActionHelper> ceaHelper;
    CCodingErrorActionHelper::AcquireSingleton((ICodingErrorActionHelper**)&ceaHelper);
    AutoPtr<ICodingErrorAction> replace;
    ceaHelper->GetREPLACE((ICodingErrorAction**)&replace);
    FAIL_RETURN(mCharset->OnMalformedInput(replace))
    return mCharset->OnUnmappableCharacter(replace);
}

ECode FastPrintWriter::AppendLocked(
    /* [in] */ Char32 c)
{
    Int32 pos = mPos;
    if (pos >= (mBufferLen - 1)) {
        FAIL_RETURN(FlushLocked())
        pos = mPos;
    }
    (*mText)[pos] = c;
    mPos = pos + 1;
    return NOERROR;
}

ECode FastPrintWriter::AppendLocked(
    /* [in] */ const String& str,
    /* [in] */ Int32 i,
    /* [in] */ Int32 length)
{
    Int32 BUFFER_LEN = mBufferLen;
    if (length > BUFFER_LEN) {
        Int32 end = i + length;
        while (i < end) {
            Int32 next = i + BUFFER_LEN;
            FAIL_RETURN(AppendLocked(str, i, next < end ? BUFFER_LEN : (end - i)))
            i = next;
        }
        return NOERROR;
    }
    Int32 pos = mPos;
    if ((pos+length) > BUFFER_LEN) {
        FAIL_RETURN(FlushLocked())
        pos = mPos;
    }
    AutoPtr<ArrayOf<Char32> > chars = str.GetChars(i, i + length);
    mText->Copy(pos, chars);
    mPos = pos + length;
    return NOERROR;
}

ECode FastPrintWriter::AppendLocked(
    /* [in] */ ArrayOf<Char32>* buf,
    /* [in] */ Int32 i,
    /* [in] */ Int32 length)
{
    Int32 BUFFER_LEN = mBufferLen;
    if (length > BUFFER_LEN) {
        Int32 end = i + length;
        while (i < end) {
            Int32 next = i + BUFFER_LEN;
            FAIL_RETURN(AppendLocked(buf, i, next < end ? BUFFER_LEN : (end - i)))
            i = next;
        }
        return NOERROR;
    }
    Int32 pos = mPos;
    if ((pos+length) > BUFFER_LEN) {
        FAIL_RETURN(FlushLocked())
        pos = mPos;
    }
    mText->Copy(pos, buf, i, length);
    mPos = pos + length;
    return NOERROR;
}

ECode FastPrintWriter::FlushBytesLocked()
{
    AutoPtr<IBuffer> buffer = IBuffer::Probe(mBytes);
    Int32 position;
    FAIL_RETURN(buffer->GetPosition(&position))
    if (position > 0) {
        FAIL_RETURN(buffer->Flip())
        AutoPtr<ArrayOf<Byte> > bytes;
        FAIL_RETURN(mBytes->GetArray((ArrayOf<Byte>**)&bytes))
        FAIL_RETURN(mOutputStream->Write(bytes, 0, position))
        FAIL_RETURN(buffer->Clear())
    }
    return NOERROR;
}

ECode FastPrintWriter::FlushLocked()
{
    //Log.i("PackageManager", "flush mPos=" + mPos);
    if (mPos > 0) {
        if (mOutputStream != NULL) {
            AutoPtr<ICharBufferHelper> cbHelper;
            CCharBufferHelper::AcquireSingleton((ICharBufferHelper**)&cbHelper);
            AutoPtr<ICharBuffer> charBuffer;
            FAIL_RETURN(cbHelper->Wrap(mText, 0, mPos, (ICharBuffer**)&charBuffer))
            AutoPtr<ICoderResult> result;
            FAIL_RETURN(mCharset->Encode(charBuffer, mBytes, TRUE, (ICoderResult**)&result))
            while (TRUE) {
                Boolean isError, isOverflow;
                result->IsError(&isError);
                if (isError) {
                    String str;
                    IObject::Probe(result)->ToString(&str);
                    Logger::E("FastPrintWriter", str);
                    return E_IO_EXCEPTION;
                }
                else if (result->IsOverflow(&isOverflow), isOverflow) {
                    FAIL_RETURN(FlushBytesLocked())
                    result = NULL;
                    FAIL_RETURN(mCharset->Encode(charBuffer, mBytes, TRUE, (ICoderResult**)&result))
                    continue;
                }
                break;
            }
            FAIL_RETURN(FlushBytesLocked())
            FAIL_RETURN(IFlushable::Probe(mOutputStream)->Flush())
        }
        else if (mWriter != NULL) {
            FAIL_RETURN(mWriter->Write(mText, 0, mPos))
            FAIL_RETURN(IFlushable::Probe(mWriter)->Flush())
        }
        else {
            Int32 nonEolOff = 0;
            Int32 sepLen = mSeparator.GetLength();
            Int32 len = sepLen < mPos ? sepLen : mPos;
            while (nonEolOff < len && (*mText)[mPos - 1 - nonEolOff]
                    == mSeparator.GetChar(mSeparator.GetLength() - 1 - nonEolOff)) {
                nonEolOff++;
            }
            if (nonEolOff >= mPos) {
                mPrinter->Println(String(""));
            }
            else {
                mPrinter->Println(String(*mText, 0, mPos-nonEolOff));
            }
        }
        mPos = 0;
    }
    return NOERROR;
}

ECode FastPrintWriter::Flush()
{
    AutoLock lock(mLock);
    ECode ec = NOERROR;
    do {
        ec = FlushLocked();
        if (FAILED(ec))
            break;
        if (mOutputStream != NULL) {
            ec =IFlushable::Probe(mOutputStream)->Flush();
        }
        else if (mWriter != NULL) {
            ec = IFlushable::Probe(mWriter)->Flush();
        }
    } while (0);
    if (FAILED(ec)) {
        SetError();
    }
    return NOERROR;
}

ECode FastPrintWriter::Close()
{
    AutoLock lock(mLock);
    ECode ec = NOERROR;
    do {
        ec = FlushLocked();
        if (FAILED(ec))
            break;
        if (mOutputStream != NULL) {
            ec = ICloseable::Probe(mOutputStream)->Close();
        }
        else if (mWriter != NULL) {
            ec = ICloseable::Probe(mWriter)->Close();
        }
    } while (0);
    if (FAILED(ec)) {
        SetError();
    }
    return NOERROR;
}

ECode FastPrintWriter::Print(
    /* [in] */ ArrayOf<Char32>* charArray)
{
    AutoLock lock(mLock);
    AppendLocked(charArray, 0, charArray->GetLength());
    return NOERROR;
}

ECode FastPrintWriter::PrintChar(
    /* [in] */ Char32 ch)
{
    AutoLock lock(mLock);
    AppendLocked(ch);
    return NOERROR;
}

ECode FastPrintWriter::Print(
    /* [in] */ const String& _str)
{
    String str = _str;
    if (str == NULL) {
        str = String("NULL"); //String.valueOf((Object) NULL);
    }
    AutoLock lock(mLock);
    if (FAILED(AppendLocked(str, 0, str.GetLength()))) {
        SetError();
    }
    return NOERROR;
}

ECode FastPrintWriter::Print(
    /* [in] */ Int32 inum)
{
    if (inum == 0) {
        return Print(String("0").GetChars());
    }
    else {
        return PrintWriter::Print(inum);
    }
}

ECode FastPrintWriter::Print(
    /* [in] */ Int64 lnum)
{
    if (lnum == 0) {
        return Print(String("0").GetChars());
    }
    else {
        return PrintWriter::Print(lnum);
    }
}

ECode FastPrintWriter::Println()
{
    AutoLock lock(mLock);
    if (FAILED(AppendLocked(mSeparator, 0, mSeparator.GetLength()))) {
        SetError();
    }
    if (mAutoFlush) {
        if (FAILED(FlushLocked())) {
            SetError();
        }
    }
    return NOERROR;
}

ECode FastPrintWriter::Println(
    /* [in] */ Int32 inum)
{
    if (inum == 0) {
        return Println(String("0").GetChars());
    }
    else {
        return PrintWriter::Println(inum);
    }
}

ECode FastPrintWriter::Println(
    /* [in] */ Int64 lnum)
{
    if (lnum == 0) {
        return Println(String("0").GetChars());
    }
    else {
        return PrintWriter::Println(lnum);
    }
}

ECode FastPrintWriter::Println(
    /* [in] */ ArrayOf<Char32>* chars)
{
    Print(chars);
    return Println();
}

ECode FastPrintWriter::PrintCharln(
    /* [in] */ Char32 c)
{
    PrintChar(c);
    return Println();
}

ECode FastPrintWriter::Write(
    /* [in] */ ArrayOf<Char32>* buf,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 count)
{
    AutoLock lock(mLock);
    AppendLocked(buf, offset, count);
    return NOERROR;
}

ECode FastPrintWriter::Write(
    /* [in] */ Int32 oneChar)
{
    AutoLock lock(mLock);
    AppendLocked((Char32) oneChar);
    return NOERROR;
}

ECode FastPrintWriter::Write(
    /* [in] */ const String& str)
{
    AutoLock lock(mLock);
    AppendLocked(str, 0, str.GetLength());
    return NOERROR;
}

ECode FastPrintWriter::Write(
    /* [in] */ const String& str,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 count)
{
    AutoLock lock(mLock);
    AppendLocked(str, offset, count);
    return NOERROR;
}

ECode FastPrintWriter::Append(
    /* [in] */ ICharSequence* csq,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    String str("NULL");
    if (NULL != csq) {
        csq->ToString(&str);
    }

    String output = str.Substring(start, end);
    return Write(output, 0, output.GetLength());
}
} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos
