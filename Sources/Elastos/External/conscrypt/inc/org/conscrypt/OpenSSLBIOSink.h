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

#ifndef __ORG_CONSCRYPT_OPENSSLBIOSINK_H__
#define __ORG_CONSCRYPT_OPENSSLBIOSINK_H__

#include <elastos/coredef.h>
#include <Elastos.CoreLibrary.IO.h>
#include <elastos/core/Object.h>
#include "_Org.Conscrypt.h"

using Elastos::IO::IByteArrayOutputStream;

namespace Org {
namespace Conscrypt {

class OpenSSLBIOSink
    : public IOpenSSLBIOSink
    , public Object
{
public:
    CAR_INTERFACE_DECL()

    OpenSSLBIOSink();

    ~OpenSSLBIOSink();

    CARAPI constructor(
        /* [in] */ IByteArrayOutputStream* buffer);

    CARAPI Available(
        /* [out] */ Int32* result);

    CARAPI Reset();

    CARAPI Skip(
        /* [in] */  Int64 byteCount,
        /* [out] */ Int64* result);

    CARAPI GetContext(
        /* [out] */ Int64* result);

    CARAPI ToByteArray(
        /* [out, callee] */ ArrayOf<Byte>** result);

    CARAPI Position(
        /* [out] */ Int32* result);

    static CARAPI Create(
        /* [out] */ IOpenSSLBIOSink** result);

private:
    Int64 mCtx;
    AutoPtr<IByteArrayOutputStream> mBuffer;
    Int32 mPosition;
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_OPENSSLBIOSINK_H__
