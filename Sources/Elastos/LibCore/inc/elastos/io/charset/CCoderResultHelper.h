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

#ifndef __ELASTOS_IO_CHARSET_CCODERRESULTHELPER_H__
#define __ELASTOS_IO_CHARSET_CCODERRESULTHELPER_H__

#include "_Elastos_IO_Charset_CCoderResultHelper.h"
#include "Singleton.h"

using Elastos::Core::Singleton;

namespace Elastos {
namespace IO {
namespace Charset {

CarClass(CCoderResultHelper)
    , public Singleton
    , public ICoderResultHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetUNDERFLOW(
        /* [out] */ ICoderResult** result);

    CARAPI GetOVERFLOW(
        /* [out] */ ICoderResult** result);

    CARAPI MalformedForLength(
        /* [in] */ Int32 length,
        /* [out] */ ICoderResult** result);

    CARAPI UnmappableForLength(
        /* [in] */ Int32 length,
        /* [out] */ ICoderResult** result);
};

} // namespace Charset
} // namespace IO
} // namespace Elastos

#endif //__ELASTOS_IO_CHARSET_CCODERRESULTHELPER_H__
