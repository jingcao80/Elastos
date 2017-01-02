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

#ifndef __LIBCORE_IO_CSOCKETTAGGERHELPER_H__
#define __LIBCORE_IO_CSOCKETTAGGERHELPER_H__

#include "elastos/core/Singleton.h"
#include "_Libcore_IO_CSocketTaggerHelper.h"

using Elastos::Core::Singleton;

namespace Libcore {
namespace IO {

CarClass(CSocketTaggerHelper)
    , public Singleton
    , public ISocketTaggerHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    /**
     * Sets this process' socket tagger to {@code tagger}.
     */
    CARAPI Set(
        /* [in] */ ISocketTagger* tagger);

    /**
     * Returns this process socket tagger.
     */
    CARAPI Get(
        /* [out] */ ISocketTagger** tagger);
};

} // namespace IO
} // namespace Libcore

#endif // __LIBCORE_IO_CSOCKETTAGGERHELPER_H__
