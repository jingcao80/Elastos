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

#ifndef __ELASTOS_IO_CHARSET_ELASTOS_IO_CCHARSETHELPER_H__
#define __ELASTOS_IO_CHARSET_ELASTOS_IO_CCHARSETHELPER_H__

#include "_Elastos_IO_Charset_CCharsetHelper.h"
#include "Singleton.h"

using Elastos::Core::Singleton;
using Elastos::Utility::ISortedMap;

namespace Elastos {
namespace IO {
namespace Charset {

CarClass(CCharsetHelper)
    , public Singleton
    , public ICharsetHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI AvailableCharsets(
        /* [out] */ ISortedMap** outsm);

    CARAPI ForName(
        /* [in] */ const String& charsetName,
        /* [out] */ ICharset** charset);

    CARAPI IsSupported(
        /* [in] */ const String& charsetName,
        /* [out] */ Boolean* result);

    CARAPI DefaultCharset(
        /* [out] */ICharset** charset);
};

} // namespace Charset
} // namespace IO
} // namespace Elastos

#endif //__ELASTOS_IO_CHARSET_ELASTOS_IO_CCHARSETHELPER_H__
