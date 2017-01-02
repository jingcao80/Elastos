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

#ifndef __ELASTOS_NET_CURLENCODER_H__
#define __ELASTOS_NET_CURLENCODER_H__

#include "_Elastos_Net_CURLEncoder.h"
#include "libcore/net/UriCodec.h"
#include "elastos/core/Singleton.h"

using Elastos::Core::Singleton;
using Libcore::Net::UriCodec;

namespace Elastos {
namespace Net {

CarClass(CURLEncoder)
    , public Singleton
    , public IURLEncoder
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI Encode(
        /* [in] */ const String& s,
        /* [out] */ String* result);

    CARAPI Encode(
        /* [in] */ const String& s,
        /* [in] */ const String& charsetName,
        /* [out] */ String* result);
private:
    class MyUriCodec
        : public UriCodec
    {
        Boolean IsRetained(
            /* [in] */ Char32 c)
        {
            String str(" .-*_");
            return str.IndexOf(c) != -1;
        }
    };

    static MyUriCodec ENCODER;
};

} // namespace Net
} // namespace Elastos

#endif //__ELASTOS_NET_URLENCODER_H__
