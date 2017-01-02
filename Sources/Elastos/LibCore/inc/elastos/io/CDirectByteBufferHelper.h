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

#ifndef __ELASTOS_IO_CDIRECTBYTEBUFFERHELPER_H__
#define __ELASTOS_IO_CDIRECTBYTEBUFFERHELPER_H__

#include "_Elastos_IO_CDirectByteBufferHelper.h"
#include "elastos/core/Singleton.h"
#include "Elastos.CoreLibrary.IO.h"

using Elastos::Core::Singleton;

namespace Elastos {
namespace IO {

CarClass(CDirectByteBufferHelper)
    , public Singleton
    , public IDirectByteBufferHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetNewInstance(
        /* [in] */ Int64 address,
        /* [in] */ Int32 capacity,
        /* [out] */ IDirectByteBuffer** instance);
};

}
}

#endif