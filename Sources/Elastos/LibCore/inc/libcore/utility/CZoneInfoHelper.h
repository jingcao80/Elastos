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

#ifndef __LIBCORE_UTILITY_CZONEINFOHELPER_H__
#define __LIBCORE_UTILITY_CZONEINFOHELPER_H__

#include "Singleton.h"
#include "_Libcore_Utility_CZoneInfoHelper.h"

using Elastos::Core::Singleton;
using Libcore::IO::IBufferIterator;

namespace Libcore {
namespace Utility {

CarClass(CZoneInfoHelper)
    , public Singleton
    , public IZoneInfoHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI MakeTimeZone(
        /* [in] */ const String& id,
        /* [in] */ IBufferIterator* it,
        /* [out] */ IZoneInfo** zoneInfo);
};

} // namespace Utility
} // namespace Libcore
#endif