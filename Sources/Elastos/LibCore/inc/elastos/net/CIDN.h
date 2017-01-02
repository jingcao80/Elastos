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

#ifndef __ELASTOS_NET_CIDN_H__
#define __ELASTOS_NET_CIDN_H__

#include "_Elastos_Net_CIDN.h"
#include "Singleton.h"

using Elastos::Core::Singleton;

namespace Elastos {
namespace Net {

CarClass(CIDN)
    , public Singleton
    , public IIDN
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI ToASCII(
        /* [in] */ const String& input,
        /* [in] */ Int32 flags,
        /* [out] */ String* result);

    CARAPI ToASCII(
        /* [in] */ const String& input,
        /* [out] */ String* result);

    CARAPI ToUnicode(
        /* [in] */ const String& input,
        /* [in] */ Int32 flags,
        /* [out] */ String* result);

    CARAPI ToUnicode(
        /* [in] */ const String& input,
        /* [out] */ String* result);

private:
    static CARAPI Convert(
        /* [in] */ const String& s,
        /* [in] */ Int32 flags,
        /* [in] */ Boolean toAscii,
        /* [out] */ String* result);

    static CARAPI NativeConvert(
        /* [in] */ const String& s,
        /* [in] */ Int32 flags,
        /* [in] */ Boolean toAscii,
        /* [out] */ String* result);
};

} // namespace Net
} // namespace Elastos

#endif //__ELASTOS_NET_CIDN_H__
