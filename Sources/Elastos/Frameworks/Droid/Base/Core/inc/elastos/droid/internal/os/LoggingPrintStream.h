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

#ifndef __ELASTOS_DROID_INTERNAL_OS_LOGGINGPRINTSTREAM_H__
#define __ELASTOS_DROID_INTERNAL_OS_LOGGINGPRINTSTREAM_H__

#include "Elastos.Droid.Core.h"
#include <Elastos.CoreLibrary.Core.h>
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/io/PrintStream.h>

using Elastos::Core::StringBuilder;
using Elastos::IO::IByteBuffer;
using Elastos::IO::ICharBuffer;
using Elastos::IO::PrintStream;
using Elastos::IO::Charset::ICharsetDecoder;
using Elastos::Utility::IFormatter;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Os {

class LoggingPrintStream
    : public PrintStream
{
private:
    class MyOutputStream : public OutputStream
    {
    public:
        CARAPI Write(
            /* [in] */ Int32 oneByte);
    };

public:
    CARAPI Flush();

    CARAPI Write(
        /* [in] */ Int32 oneByte);

    CARAPI Write(
        /* [in] */ ArrayOf<Byte>* buffer);

    CARAPI Write(
        /* [in] */ ArrayOf<Byte>* buffer,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 count);

    CARAPI CheckError(
        /* [out] */ Boolean* hasError);

    CARAPI Close();

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

    CARAPI AppendChar(
        /* [in] */ Char32 c);

    CARAPI Append(
        /* [in] */ ICharSequence* csq);

    CARAPI Append(
        /* [in] */ ICharSequence* csq,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end);

protected:
    LoggingPrintStream();

    virtual CARAPI_(void) Log(
        /* [in] */ const String& line) = 0;

    CARAPI SetError();

private:
    /**
     * Searches buffer for line breaks and logs a message for each one.
     *
     * @param completely true if the ending chars should be treated as a line
     *  even though they don't end in a line break
     */
    CARAPI_(void) Flush(
        /* [in] */ Boolean completely);

private:
    AutoPtr<StringBuilder> mBuilder;

    /**
     * A buffer that is initialized when raw bytes are first written to this
     * stream. It may contain the leading bytes of multi-byte characters.
     * Between writes this buffer is always ready to receive data; ie. the
     * position is at the first unassigned byte and the limit is the capacity.
     */
    AutoPtr<IByteBuffer> mEncodedBytes;

    /**
     * A buffer that is initialized when raw bytes are first written to this
     * stream. Between writes this buffer is always clear; ie. the position is
     * zero and the limit is the capacity.
     */
    AutoPtr<ICharBuffer> mDecodedChars;

    /**
     * Decodes bytes to characters using the system default charset. Initialized
     * when raw bytes are first written to this stream.
     */
    AutoPtr<ICharsetDecoder> mDecoder;

    AutoPtr<IFormatter> mFormatter;
};

} // namespace Os
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_OS_LOGGINGPRINTSTREAM_H__
