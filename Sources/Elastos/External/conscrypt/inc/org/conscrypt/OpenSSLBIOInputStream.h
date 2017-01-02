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

#ifndef __ORG_CONSCRYPT_OPENSSLBIOINPUTSTREAM_H__
#define __ORG_CONSCRYPT_OPENSSLBIOINPUTSTREAM_H__

#include <elastos/coredef.h>
#include <Elastos.CoreLibrary.IO.h>
#include <elastos/io/FilterInputStream.h>
#include "_Org.Conscrypt.h"

using Elastos::IO::FilterInputStream;
using Elastos::IO::IInputStream;

namespace Org {
namespace Conscrypt {

/**
 * Provides an interface to OpenSSL's BIO system directly from a Java
 * InputStream. It allows an OpenSSL API to read directly from something more
 * flexible interface than a byte array.
 */
class OpenSSLBIOInputStream
    : public IOpenSSLBIOInputStream
    , public FilterInputStream
{
public:
    CAR_INTERFACE_DECL()

    OpenSSLBIOInputStream();

    CARAPI constructor(
        /* [in] */ IInputStream* bytes);

    CARAPI GetBioContext(
        /* [out] */  Int64* result);

    CARAPI ReleaseResources();

    /**
     * Similar to a {@code readLine} method, but matches what OpenSSL expects
     * from a {@code BIO_gets} method.
     */
    CARAPI Gets(
        /* [in] */  ArrayOf<Byte>* buffer,
        /* [out] */  Int32* result);

private:
    Int64 mCtx;
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_OPENSSLBIOINPUTSTREAM_H__
