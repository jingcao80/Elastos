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

#ifndef __ELASTOS_UTILITY_ZIP_CCHECKEDOUTPUTSTREAM_H__
#define __ELASTOS_UTILITY_ZIP_CCHECKEDOUTPUTSTREAM_H__

#include "_Elastos_Utility_Zip_CCheckedOutputStream.h"
#include "FilterOutputStream.h"

using Elastos::IO::IOutputStream;
using Elastos::IO::FilterOutputStream;

namespace Elastos {
namespace Utility {
namespace Zip {

CarClass(CCheckedOutputStream)
    , public FilterOutputStream
    , public ICheckedOutputStream
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    /**
     * Constructs a new {@code CheckedOutputStream} on {@code OutputStream}
     * {@code os}. The checksum is calculated using the algorithm implemented
     * by {@code csum}.
     *
     * @param os
     *            the output stream to calculate checksum for.
     * @param cs
     *            an entity implementing the checksum algorithm.
     */
    CARAPI constructor(
        /* [in] */ IOutputStream* os,
        /* [in] */ IChecksum* cs);

    /**
     * Returns the checksum calculated on the stream read so far.
     *
     * @return the updated checksum.
     */
    CARAPI GetChecksum(
        /* [out] */ IChecksum** checksum);

    /**
     * Writes the specified byte to the underlying stream. The checksum is
     * updated with {@code val}.
     *
     * @param val
     *            the data value to written to the output stream.
     * @throws IOException
     *             if an IO error has occurred.
     */
    //@Override
    CARAPI Write(
        /* [in] */ Int32 val);

    /**
     * Writes n bytes of data from {@code buf} starting at offset {@code off} to
     * the underlying stream. The checksum is updated with the bytes written.
     *
     * @param buf
     *            data written to the output stream.
     * @param off
     *            the offset to start reading the data from {@code buf} written
     *            to the output stream.
     * @param nbytes
     *            number of bytes to write to the output stream.
     * @throws IOException
     *             if an IO error has occurred.
     */
    //@Override
    CARAPI Write(
        /* [in] */ ArrayOf<Byte>* buf,
        /* [in] */ Int32 off,
        /* [in] */ Int32 nbytes);

private:
    AutoPtr<IChecksum> mCheck;
};


} // namespace Zip
} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_ZIP_CCHECKEDOUTPUTSTREAM_H__
