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

#ifndef __ELASTOS_SECURITY_MESSAGEDIGEST_H__
#define __ELASTOS_SECURITY_MESSAGEDIGEST_H__

#include "Elastos.CoreLibrary.Security.h"
#include "MessageDigestSpi.h"

using Elastos::Core::ICloneable;
using Elastos::IO::IByteBuffer;
using Org::Apache::Harmony::Security::Fortress::IEngine;

namespace Elastos {
namespace Security {

class MessageDigest
    : public MessageDigestSpi
    , public IMessageDigest
{
public:
    CAR_INTERFACE_DECL()

    static CARAPI GetInstance(
        /* [in] */ const String& algorithm,
        /* [out] */ IMessageDigest** instance);

    static CARAPI GetInstance(
        /* [in] */ const String& algorithm,
        /* [in] */ const String& provider,
        /* [out] */ IMessageDigest** instance);

    static CARAPI GetInstance(
        /* [in] */ const String& algorithm,
        /* [in] */ IProvider* provider,
        /* [out] */ IMessageDigest** instance);

    CARAPI Reset();

    CARAPI Update(
        /* [in] */ Byte arg0);

    CARAPI Update(
        /* [in] */ ArrayOf<Byte>* input,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 len);

    CARAPI Update(
        /* [in] */ ArrayOf<Byte>* input);

    CARAPI Digest(
        /* [out, callee] */ ArrayOf<Byte>** hashValue);

    CARAPI Digest(
        /* [in] */ ArrayOf<Byte>* buf,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 len,
        /* [out] */ Int32* number);

    CARAPI Digest(
        /* [in] */ ArrayOf<Byte>* input,
        /* [out, callee] */ ArrayOf<Byte>** hashValue);

    CARAPI ToString(
        /* [out] */ String* str);

    static CARAPI IsEqual(
        /* [in] */ ArrayOf<Byte>* digesta,
        /* [in] */ ArrayOf<Byte>* digestb,
        /* [out] */ Boolean* result);

    CARAPI GetAlgorithm(
        /* [out] */ String* algorithm);

    CARAPI GetProvider(
        /* [out, callee] */ IProvider** provider);

    CARAPI GetDigestLength(
        /* [out] */ Int32* length);

    CARAPI Update(
        /* [in] */ IByteBuffer* input);

    CARAPI SetProvider(
        /* [in] */ IProvider* provider);

    CARAPI SetAlgorithm(
        /* [in] */ const String& algorithm);

protected:
    /**
     * Constructs a new instance of {@code MessageDigest} with the name of
     * the algorithm to use.
     *
     * @param algorithm
     *            the name of algorithm to use
     */
    MessageDigest(
        /* [in] */ const String& algorithm);

private:
    static CARAPI_(AutoPtr<IEngine>) Init_ENGINE();

protected:
    // Used to access common engine functionality
    static const AutoPtr<IEngine> ENGINE;

    // The provider
    AutoPtr<IProvider> mProvider;

    // The algorithm.
    String mAlgorithm;
};

} // namespace Security
} // namespace Elastos

#endif // __ELASTOS_SECURITY_MESSAGEDIGEST_H__
