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

#ifndef __ELASTOS_IO_PRINTWRITR_H__
#define __ELASTOS_IO_PRINTWRITR_H__

#include "Writer.h"

using Elastos::Utility::ILocale;

namespace Elastos {
namespace IO {

class ECO_PUBLIC PrintWriter
    : public Writer
    , public IPrintWriter
{
public:
    CAR_INTERFACE_DECL()

    PrintWriter();

    virtual ~PrintWriter();

    CARAPI CheckError(
        /* [out] */ Boolean* hasError);

    CARAPI Close();

    CARAPI Flush();

    CARAPI Format(
        /* [in] */ const String& format,
        /* [in] */ ArrayOf<IInterface*>* args);

    CARAPI Format(
        /* [in] */ ILocale* l,
        /* [in] */ const String& format,
        /* [in] */ ArrayOf<IInterface*>* args);

    CARAPI Printf(
        /* [in] */ const String& format,
        /* [in] */ ArrayOf<IInterface*>* args);

    CARAPI Printf(
        /* [in] */ ILocale* l,
        /* [in] */ const String& format,
        /* [in] */ ArrayOf<IInterface*>* args);

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
        /* [in] */ ArrayOf<Char32>* buffer);

    CARAPI Write(
        /* [in] */ ArrayOf<Char32>* buffer,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 count);

    CARAPI Write(
        /* [in] */ Int32 oneChar32);

    CARAPI Write(
        /* [in] */ const String& str);

    CARAPI Write(
        /* [in] */ const String& str,
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

    CARAPI constructor(
        /* [in] */ IOutputStream* outs);

    CARAPI constructor(
        /* [in] */ IOutputStream* outs,
        /* [in] */ Boolean autoflush);

    CARAPI constructor(
        /* [in] */ IWriter* wr);

    CARAPI constructor(
        /* [in] */ IWriter* wr,
        /* [in] */ Boolean autoflush);

    CARAPI constructor(
        /* [in] */ IFile* file);

    CARAPI constructor(
        /* [in] */ IFile* file,
        /* [in] */ const String& csn);

    CARAPI constructor(
        /* [in] */ const String& fileName);

    CARAPI constructor(
        /* [in] */ const String& fileName,
        /* [in] */ const String& csn);

protected:
    CARAPI ClearError();

    CARAPI_(void) SetError();

private:
    ECO_LOCAL CARAPI DoWrite(
        /* [in] */ Int32 offset,
        /* [in] */ Int32 count,
        /* [in] */ ArrayOf<Char32>* buf);

protected:
    /**
     * The writer to print data to.
     */
    AutoPtr<IWriter> mOut;

private:
    /**
     * Indicates whether this PrintWriter is in an error state.
     */
    Boolean mIoError;

    /**
     * Indicates whether or not this PrintWriter should flush its contents after
     * printing a new line.
     */
    Boolean mAutoflush;

    const String mLineSeparator;;

};

} // namespace IO
} // namespace Elastos

#endif //__ELASTOS_IO_PRINTWRITR_H__
