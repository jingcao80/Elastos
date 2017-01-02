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

#ifndef __ELASTOS_UTILITY_JAR_ELASTOS_UTILITY_CJARINPUTSTREAM_H__
#define __ELASTOS_UTILITY_JAR_ELASTOS_UTILITY_CJARINPUTSTREAM_H__

#include "_Elastos_Utility_Jar_CJarInputStream.h"
#include "ZipInputStream.h"
#include "JarVerifier.h"

using Elastos::Utility::Zip::ZipInputStream;
using Elastos::Utility::Zip::IZipEntry;
using Elastos::IO::IOutputStream;
using Elastos::IO::IOutputStream;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Utility {
namespace Jar {

CarClass(CJarInputStream)
    , public ZipInputStream
    , public IJarInputStream
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CJarInputStream();

    CARAPI constructor(
        /* [in] */ IInputStream* stream,
        /* [in] */ Boolean verify);

    CARAPI constructor(
        /* [in] */ IInputStream* stream);

    /**
     * Returns the next {@code ZipEntry} contained in this stream or {@code
     * null} if no more entries are present.
     *
     * @return the next extracted ZIP entry.
     * @throws IOException
     *             if an error occurs while reading the entry.
     */
    CARAPI GetNextEntry(
        /* [out] */ IZipEntry** entry);

    CARAPI GetManifest(
        /* [out] */ IManifest** manifest);

    CARAPI GetNextJarEntry(
        /* [out] */ IJarEntry** jarEntry);

    /**
     * Reads up to {@code byteCount} bytes of decompressed data and stores it in
     * {@code buffer} starting at {@code byteOffset}. Returns the number of uncompressed bytes read.
     *
     * @throws IOException
     *             if an IOException occurs.
     */
    CARAPI Read(
        /* [in] */ ArrayOf<Byte>* buffer,
        /* [in] */ Int32 byteOffset,
        /* [in] */ Int32 byteCount,
        /* [out] */ Int32* read);

private:
    AutoPtr<IManifest> mManifest;

    AutoPtr<IJarEntry> mCurrentJarEntry;

    AutoPtr<IJarEntry> mPendingJarEntry;

    AutoPtr<JarVerifier> mVerifier;

    AutoPtr<IOutputStream> mVerStream;

    Boolean mIsMeta;
    Boolean mVerified;
};

} // namespace Jar
} // namespace Utility
} // namespace Elastos

#endif // __ELASTOS_UTILITY_JAR_ELASTOS_UTILITY_CJARINPUTSTREAM_H__
