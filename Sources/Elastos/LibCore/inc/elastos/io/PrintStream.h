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

#ifndef __ELASTOS_IO_PRINTSTREAM_H__
#define __ELASTOS_IO_PRINTSTREAM_H__

#include "FilterOutputStream.h"

using Elastos::Core::ICharSequence;
using Elastos::Core::IAppendable;
using Elastos::Utility::ILocale;

namespace Elastos {
namespace IO {

class ECO_PUBLIC PrintStream
    : public FilterOutputStream
    , public IPrintStream
    , public IAppendable
{
public:
    CAR_INTERFACE_DECL()

    PrintStream();

    virtual ~PrintStream();

    CARAPI constructor(
        /* [in] */ IOutputStream* outs);

    CARAPI constructor(
        /* [in] */ IOutputStream* outs,
        /* [in] */ Boolean autoflush);

    CARAPI constructor(
        /* [in] */ IOutputStream* outs,
        /* [in] */ Boolean autoflush,
        /* [in] */ const String& charsetName);

    CARAPI constructor(
        /* [in] */ IFile* file);

    CARAPI constructor(
        /* [in] */ IFile* file,
        /* [in] */ const String& charsetName);

    CARAPI constructor(
        /* [in] */ const String& fileName);

    CARAPI constructor(
        /* [in] */ const String& fileName,
        /* [in] */ const String& charsetName);

    CARAPI CheckError(
        /* [out] */ Boolean* hasError);

    CARAPI Close();

    CARAPI Flush();

    CARAPI Format(
        /* [in] */ const String& format,
        /* [in] */ ArrayOf<IInterface*>* args,
        /* [out] */ IPrintStream** pw);

    CARAPI Format(
        /* [in] */ ILocale* l,
        /* [in] */ const String& format,
        /* [in] */ ArrayOf<IInterface*>* args,
        /* [out] */ IPrintStream** pw);

    CARAPI Printf(
        /* [in] */ const String& format,
        /* [in] */ ArrayOf<IInterface*>* args,
        /* [out] */ IPrintStream** pw);

    CARAPI Printf(
        /* [in] */ ILocale* l,
        /* [in] */ const String& format,
        /* [in] */ ArrayOf<IInterface*>* args,
        /* [out] */ IPrintStream** ps);

    CARAPI Print(
        /* [in] */ ArrayOf<Char32>* charArray);

    CARAPI PrintChar(
        /* [in] */ Char32 ch);

    CARAPI Print(
        /* [in] */ Double dnum);

    CARAPI Print(
        /* [in] */ Float fnum);

    CARAPI Print(
        /* [in] */ Int32 inum);

    CARAPI Print(
        /* [in] */ Int64 lnum);

    CARAPI Print(
        /* [in] */ IInterface* obj);

    CARAPI Print(
        /* [in] */ const String& str);

    CARAPI Print(
        /* [in] */ Boolean result);

    CARAPI Println();

    CARAPI Println(
        /* [in] */ ArrayOf<Char32>* charArray);

    CARAPI PrintCharln(
        /* [in] */ Char32 ch);

    CARAPI Println(
        /* [in] */ Double dnum);

    CARAPI Println(
        /* [in] */ Float fnum);

    CARAPI Println(
        /* [in] */ Int32 inum);

    CARAPI Println(
        /* [in] */ Int64 lnum);

    CARAPI Println(
        /* [in] */ IInterface* obj);

    CARAPI Println(
        /* [in] */ const String& str);

    CARAPI Println(
        /* [in] */ Boolean result);

    CARAPI Write(
        /* [in] */ Int32 oneChar32);

    CARAPI Write(
        /* [in] */ ArrayOf<Byte>* buffer,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 count);

    CARAPI AppendChar(
        /* [in] */ Char32 c);

    CARAPI Append(
        /* [in] */ ICharSequence* csq);

    CARAPI Append(
        /* [in] */ ICharSequence* csq,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end);

protected:
    CARAPI ClearError();

    CARAPI SetError();

private:
    ECO_LOCAL CARAPI Newline();

private:
    ECO_LOCAL static const String sLineSeparator;

    /**
     * indicates whether or not this PrintStream has incurred an error.
     */
    Boolean mIoError;

    /**
     * indicates whether or not this PrintStream should flush its contents after
     * printing a new line.
     */
    Boolean mAutoFlush;

    String mEncoding;
};

} // namespace IO
} // namespace Elastos

#endif //__ELASTOS_IO_PRINTSTREAM_H__
