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

#ifndef __ELASTOS_DROID_MEDIA_CAMRINPUTSTREAM_H__
#define __ELASTOS_DROID_MEDIA_CAMRINPUTSTREAM_H__

#include "_Elastos_Droid_Media_CAmrInputStream.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/io/InputStream.h>

using Elastos::IO::IInputStream;
using Elastos::IO::InputStream;

namespace Elastos {
namespace Droid {
namespace Media {

/**
 * AmrInputStream
 * @hide
 */
CarClass(CAmrInputStream)
    , public InputStream
    , public IAmrInputStream
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CAmrInputStream();

    virtual ~CAmrInputStream();

    /**
     * Create a new AmrInputStream, which converts 16 bit PCM to AMR
     * @param inputStream InputStream containing 16 bit PCM.
     */
    CARAPI constructor(
        /* [in] */ IInputStream* inputStream);

    CARAPI Read(
        /* [out] */ Int32* value);

    CARAPI Read(
        /* [out] */ ArrayOf<Byte>* buffer,
        /* [out] */ Int32* number);

    CARAPI Read(
        /* [out] */ ArrayOf<Byte>* buffer,
        /* [in] */ Int32 byteOffset,
        /* [in] */ Int32 byteCount,
        /* [out] */ Int32* number);

    CARAPI Close();

private:
    //
    // AudioRecord JNI interface
    //
    static CARAPI GsmAmrEncoderNew(
        /* [out] */ Int64* gae);

    static CARAPI GsmAmrEncoderInitialize(
        /* [in] */ Int64 gae);

    static CARAPI GsmAmrEncoderEncode(
        /* [in] */ Int64 gae,
        /* [in] */ ArrayOf<Byte>* pcm,
        /* [in] */ Int32 pcmOffset,
        /* [in] */ ArrayOf<Byte>* amr,
        /* [in] */ Int32 amrOffset,
        /* [out] */ Int32* number);

    static CARAPI_(void) GsmAmrEncoderCleanup(
        /* [in] */ Int64 gae);

    static CARAPI_(void) GsmAmrEncoderDelete(
        /* [in] */ Int64 gae);

private:
    static String TAG;

    // pcm input stream
    AutoPtr<IInputStream> mInputStream;

    // native handle
    Int64 mGae;

    // result amr stream
    AutoPtr<ArrayOf<Byte> > mBuf;
    Int32 mBufIn;
    Int32 mBufOut;

    // helper for bytewise read()
    AutoPtr<ArrayOf<Byte> > mOneByte;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CAMRINPUTSTREAM_H__
