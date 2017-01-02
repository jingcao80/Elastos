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

#ifndef __LIBCORE_IO_CSTRICTLINEREADER_H__
#define __LIBCORE_IO_CSTRICTLINEREADER_H__

#include "_Libcore_IO_CStrictLineReader.h"
#include "Object.h"
#include "ByteArrayOutputStream.h"

using Elastos::IO::ICloseable;
using Elastos::IO::IInputStream;
using Elastos::IO::Charset::ICharset;
using Elastos::IO::ByteArrayOutputStream;

namespace Libcore {
namespace IO {

CarClass(CStrictLineReader)
    , public Object
    , public IStrictLineReader
    , public ICloseable
{
private:
    class MyOutputStream : public ByteArrayOutputStream
    {
    public:
        MyOutputStream(
            /* [in] */ Int32 size,
            /* [in] */ CStrictLineReader* host);

        CARAPI ToString(
            /* [out] */ String* str);
    private:
        CStrictLineReader* mHost;
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IInputStream* in);

    CARAPI constructor(
        /* [in] */ IInputStream* in,
        /* [in] */ Int32 capacity);

    CARAPI constructor(
        /* [in] */ IInputStream* in,
        /* [in] */ ICharset* Charset);

    CARAPI constructor(
        /* [in] */ IInputStream* in,
        /* [in] */ Int32 capacity,
        /* [in] */ ICharset* Charset);

    /**
     * Reads the next line. A line ends with {@code "\n"} or {@code "\r\n"},
     * this end of line marker is not included in the result.
     *
     * @return the next line from the input.
     * @throws IOException for underlying {@code InputStream} errors.
     * @throws EOFException for the end of source stream.
     */
    CARAPI ReadLine(
        /* [out] */ String* line);

    /**
     * Read an {@code int} from a line containing its decimal representation.
     *
     * @return the value of the {@code int} from the next line.
     * @throws IOException for underlying {@code InputStream} errors or conversion error.
     * @throws EOFException for the end of source stream.
     */
    CARAPI ReadInt32(
        /* [out] */ Int32* value);

    /**
     * Check whether there was an unterminated line at end of input after the line reader reported
     * end-of-input with EOFException. The value is meaningless in any other situation.
     *
     * @return true if there was an unterminated line at end of input.
     */
    CARAPI HasUnterminatedLine(
        /* [out] */ Boolean* value);

    CARAPI Close();

private:
    CARAPI FillBuf();

private:
    static const Byte CR;
    static const Byte LF;

    AutoPtr<IInputStream> mIn;
    AutoPtr<ICharset> mCharset;

    /*
     * Buffered data is stored in {@code buf}. As long as no exception occurs, 0 <= pos <= end
     * and the data in the range [pos, end) is buffered for reading. At end of input, if there is
     * an unterminated line, we set end == -1, otherwise end == pos. If the underlying
     * {@code InputStream} throws an {@code IOException}, end may remain as either pos or -1.
     */
    AutoPtr<ArrayOf<Byte> > mBuf;
    Int32 mPos;
    Int32 mEnd;
    Object mLockIn;
};

} // namespace IO
} // namespace Libcore
#endif
