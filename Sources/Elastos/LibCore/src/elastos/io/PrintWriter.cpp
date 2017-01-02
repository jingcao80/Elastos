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

#include "PrintWriter.h"
#include "COutputStreamWriter.h"
#include "CFileOutputStream.h"
#include "CBufferedOutputStream.h"
#include "CString.h"
#include "CLocaleHelper.h"
#include "CLocale.h"
#include "CFormatter.h"
#include "CSystem.h"
#include "utility/logging/Slogger.h"
#include "AutoLock.h"

using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Core::StringUtils;
using Elastos::Core::CString;
using Elastos::Core::IAppendable;
using Elastos::Core::EIID_IAppendable;
using Elastos::IO::COutputStreamWriter;
using Elastos::IO::CFileOutputStream;
using Elastos::IO::CBufferedOutputStream;
using Elastos::Utility::IFormatter;
using Elastos::Utility::CFormatter;
using Elastos::Utility::Logging::Slogger;
using Elastos::Utility::CLocale;
using Elastos::Utility::ILocaleHelper;
using Elastos::Utility::CLocaleHelper;

namespace Elastos {
namespace IO {

CAR_INTERFACE_IMPL(PrintWriter, Writer, IPrintWriter)

PrintWriter::PrintWriter()
    : mIoError(FALSE)
    , mAutoflush(FALSE)
    , mLineSeparator("\n")
{}

PrintWriter::~PrintWriter()
{}

ECode PrintWriter::constructor(
    /* [in] */ IOutputStream* outs)
{
    AutoPtr<IOutputStreamWriter> opsw;
    FAIL_RETURN(COutputStreamWriter::New(outs, (IOutputStreamWriter**)&opsw));
    return constructor(IWriter::Probe(opsw), FALSE);
}

ECode PrintWriter::constructor(
    /* [in] */ IOutputStream* outs,
    /* [in] */ Boolean autoflush)
{
    AutoPtr<IOutputStreamWriter> opsw;
    FAIL_RETURN(COutputStreamWriter::New(outs, (IOutputStreamWriter**)&opsw));
    return constructor(IWriter::Probe(opsw), autoflush);
}

ECode PrintWriter::constructor(
    /* [in] */ IWriter* wr)
{
    return constructor(wr, FALSE);
}

ECode PrintWriter::constructor(
    /* [in] */ IWriter* wr,
    /* [in] */ Boolean autoflush)
{
    FAIL_RETURN(Writer::constructor(ISynchronize::Probe(wr)))

    mAutoflush = autoflush;
    mOut = wr;
    return NOERROR;
}

ECode PrintWriter::constructor(
    /* [in] */ IFile* file)
{
    AutoPtr<IOutputStream> fops;
    FAIL_RETURN(CFileOutputStream::New(file, (IOutputStream**)&fops));
    AutoPtr<IOutputStream> bops;
    FAIL_RETURN(CBufferedOutputStream::New(fops, (IOutputStream**)&bops));
    AutoPtr<IWriter> opsr;
    FAIL_RETURN(COutputStreamWriter::New(bops, (IWriter**)&opsr));
    return constructor(opsr, FALSE);
}

ECode PrintWriter::constructor(
    /* [in] */ IFile* file,
    /* [in] */ const String& csn)
{
    AutoPtr<IOutputStream> fops;
    FAIL_RETURN(CFileOutputStream::New(file, (IOutputStream**)&fops));
    AutoPtr<IOutputStream> bops;
    FAIL_RETURN(CBufferedOutputStream::New(fops, (IOutputStream**)&bops));
    AutoPtr<IWriter> opsr;
    FAIL_RETURN(COutputStreamWriter::New(bops, csn, (IWriter**)&opsr));
    return constructor(opsr, FALSE);
}

ECode PrintWriter::constructor(
    /* [in] */ const String& fileName)
{
    AutoPtr<IOutputStream> fops;
    FAIL_RETURN(CFileOutputStream::New(fileName, (IOutputStream**)&fops));
    AutoPtr<IOutputStream> bops;
    FAIL_RETURN(CBufferedOutputStream::New(fops, (IOutputStream**)&bops));
    AutoPtr<IWriter> opsr;
    FAIL_RETURN(COutputStreamWriter::New(bops, (IWriter**)&opsr));
    return constructor(opsr, FALSE);
}

ECode PrintWriter::constructor(
    /* [in] */ const String& fileName,
    /* [in] */ const String& csn)
{
    AutoPtr<IOutputStream> fops;
    FAIL_RETURN(CFileOutputStream::New(fileName, (IOutputStream**)&fops));
    AutoPtr<IOutputStream> bops;
    FAIL_RETURN(CBufferedOutputStream::New(fops, (IOutputStream**)&bops));
    AutoPtr<IWriter> opsr;
    FAIL_RETURN(COutputStreamWriter::New(bops, csn, (IWriter**)&opsr));
    return constructor(opsr, FALSE);
}

ECode PrintWriter::CheckError(
    /* [out] */ Boolean* hasError)
{
    VALIDATE_NOT_NULL(hasError);

    AutoPtr<IWriter> delegate = mOut;
    if (delegate == NULL) {
        *hasError = mIoError;
        return NOERROR;
    }

    Flush();
    Boolean tmp;
    FAIL_RETURN(delegate->CheckError(&tmp))
    *hasError = mIoError || tmp;
    return NOERROR;
}

ECode PrintWriter::ClearError()
{
    AutoLock lock(mLock);
    mIoError = FALSE;

    return NOERROR;
}

ECode PrintWriter::Close()
{
    AutoLock lock(mLock);
    if (mOut != NULL) {
        if (FAILED(ICloseable::Probe(mOut)->Close())) {
            SetError();
        }
        mOut = NULL;
    }

    return NOERROR;
}

ECode PrintWriter::Flush()
{
    AutoLock lock(mLock);
    if (mOut != NULL) {
        if (FAILED(IFlushable::Probe(mOut)->Flush())) {
            SetError();
        }
    }
    else {
        SetError();
    }

    return NOERROR;
}

ECode PrintWriter::Format(
    /* [in] */ const String& format,
    /* [in] */ ArrayOf<IInterface*>* args)
{
    AutoPtr<ILocaleHelper> helper;
    FAIL_RETURN(CLocaleHelper::AcquireSingleton((ILocaleHelper**)&helper))
    AutoPtr<ILocale> locale;
    FAIL_RETURN(helper->GetDefault((ILocale**)&locale))
    return Format(locale, format, args);
}

ECode PrintWriter::Format(
    /* [in] */ ILocale* l,
    /* [in] */ const String& format,
    /* [in] */ ArrayOf<IInterface*>* args)
{
    if (format.IsNull()) {
        //throw new NullPointerException("format == null");
        return E_NULL_POINTER_EXCEPTION;
    }

    AutoPtr<IFormatter> formatter;
    FAIL_RETURN(CFormatter::New(this, l, (IFormatter**)&formatter))
    FAIL_RETURN(formatter->Format(format, args))
    if (mAutoflush) {
        Flush();
    }

    return NOERROR;
}

ECode PrintWriter::Printf(
    /* [in] */ const String& format,
    /* [in] */ ArrayOf<IInterface*>* args)
{
    return Format(format, args);
}

ECode PrintWriter::Printf(
    /* [in] */ ILocale* l,
    /* [in] */ const String& format,
    /* [in] */ ArrayOf<IInterface*>* args)
{
    return Format(l, format, args);
}

ECode PrintWriter::Print(
    /* [in] */ ArrayOf<Char32>* charArray)
{
    String s(*charArray);
    return Print(s);
}

ECode PrintWriter::PrintChar(
    /* [in] */ Char32 ch)
{
    String str;
    str.Append(ch);
    return Print(str);
}

ECode PrintWriter::Print(
    /* [in] */ Double dnum)
{
    return Print(StringUtils::ToString(dnum));
}

ECode PrintWriter::Print(
    /* [in] */ Float fnum)
{
    //Print(String.valueOf(fnum));
    return Print(StringUtils::ToString(fnum));
}

ECode PrintWriter::Print(
    /* [in] */ Int32 inum)
{
    return Print(StringUtils::ToString(inum));
}

ECode PrintWriter::Print(
    /* [in] */ Int64 lnum)
{
    return Print(StringUtils::ToString(lnum));
}

ECode PrintWriter::Print(
    /* [in] */ IInterface* obj)
{
    return Print(Object::ToString(obj));
}

ECode PrintWriter::Print(
    /* [in] */ const String& str)
{
    return Write(!str.IsNull() ? str : String("NULL"));
}

ECode PrintWriter::Print(
    /* [in] */ Boolean result)
{
    return Print(StringUtils::BooleanToString(result));
}

ECode PrintWriter::Println()
{
    AutoLock lock(mLock);

    AutoPtr<Elastos::Core::CSystem> cs;
    Elastos::Core::CSystem::AcquireSingletonByFriend((Elastos::Core::CSystem**)&cs);
    AutoPtr<ISystem> system = (ISystem*)cs.Get();

    String separator;
    system->GetLineSeparator(&separator);
    FAIL_RETURN(Print(separator))
    if (mAutoflush) {
        Flush();
    }

    return NOERROR;
}

ECode PrintWriter::Println(
    /* [in] */ ArrayOf<Char32>* charArray)
{
    return Println(String(*charArray, 0));
}

ECode PrintWriter::PrintCharln(
    /* [in] */ Char32 ch)
{
    String str;
    str.Append(ch);
    return Println(str);
}

ECode PrintWriter::Println(
    /* [in] */ Double dnum)
{
    return Println(StringUtils::ToString(dnum));
}

ECode PrintWriter::Println(
    /* [in] */ Float fnum)
{
    //Println(String.valueOf(fnum));
    return Println(StringUtils::ToString(fnum));
}

ECode PrintWriter::Println(
    /* [in] */ Int32 inum)
{
    return Println(StringUtils::ToString(inum));
}

ECode PrintWriter::Println(
    /* [in] */ Int64 lnum)
{
    return Println(StringUtils::ToString(lnum));
}

ECode PrintWriter::Println(
    /* [in] */ IInterface* obj)
{
    return Println(Object::ToString(obj));
}

ECode PrintWriter::Println(
    /* [in] */ const String& str)
{
    AutoLock lock(mLock);

    FAIL_RETURN(Print(str))
    FAIL_RETURN(Println())

    return NOERROR;
}

ECode PrintWriter::Println(
    /* [in] */ Boolean result)
{
    return Println(StringUtils::BooleanToString(result));
}

void PrintWriter::SetError()
{
    AutoLock lock(mLock);
    mIoError = TRUE;
}

ECode PrintWriter::Write(
    /* [in] */ ArrayOf<Char32>* buffer)
{
    return Write(buffer, 0, buffer->GetLength());
}

ECode PrintWriter::Write(
    /* [in] */ ArrayOf<Char32>* buffer,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 count)
{
    return DoWrite(offset, count, buffer);
}

ECode PrintWriter::Write(
    /* [in] */ Int32 oneChar32)
{
    AutoPtr<ArrayOf<Char32> > tmp = ArrayOf<Char32>::Alloc(1);
    (*tmp)[0] = (Char32)oneChar32;
    return DoWrite(0, 1, tmp);
}

ECode PrintWriter::DoWrite(
    /* [in] */ Int32 offset,
    /* [in] */ Int32 count,
    /* [in] */ ArrayOf<Char32>* buf)
{
    AutoLock lock(mLock);

    if (mOut != NULL) {
#ifdef DEBUG
        String info(buf);
        Slogger::D("PrintWriter", " >> %s", info.string());
#endif
        if (FAILED(mOut->Write(buf, offset, count))) {
            SetError();
        }
    }
    else {
        SetError();
    }

    return NOERROR;
}

ECode PrintWriter::Write(
    /* [in] */ const String& str)
{
    AutoPtr<ArrayOf<Char32> > tmp = str.GetChars();
    return Write(tmp);
}

ECode PrintWriter::Write(
    /* [in] */ const String& str,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 count)
{
    //write(str.substring(offset, offset + count).toCharArray());
    return Write(str.Substring(offset, offset + count));
}

ECode PrintWriter::AppendChar(
    /* [in] */ Char32 c)
{
    return Write((Int32)c);
}

ECode PrintWriter::Append(
    /* [in] */ ICharSequence* csq)
{
    Int32 length;

    if (csq == NULL) {
        AutoPtr<ICharSequence> _csq;
        CString::New(String("NULL"), (ICharSequence**)&_csq);
        FAIL_RETURN(_csq->GetLength(&length))
        return Append(_csq, 0, length);
    }

    FAIL_RETURN(csq->GetLength(&length))
    return Append(csq, 0, length);
}

ECode PrintWriter::Append(
    /* [in] */ ICharSequence* csq,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    String str("NULL");
    if (NULL != csq) {
        csq->ToString(&str);
    }

    return Write(str, start, end - start);
}

} // namespace IO
} // namespace Elastos
