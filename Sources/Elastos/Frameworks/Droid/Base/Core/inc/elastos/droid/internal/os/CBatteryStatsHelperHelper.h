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

#ifndef __ELASTOS_DROID_INTERNAL_OS_CBATTERYSTATSHELPERHELPER_H__
#define __ELASTOS_DROID_INTERNAL_OS_CBATTERYSTATSHELPERHELPER_H__

#include "_Elastos_Droid_Internal_Os_CBatteryStatsHelperHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IBatteryStats;
using Elastos::Core::Singleton;
using Elastos::IO::IFileInputStream;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Os {

CarClass(CBatteryStatsHelperHelper)
    , public Singleton
    , public IBatteryStatsHelperHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI CheckWifiOnly(
        /* [in] */ IContext* context,
        /* [out] */ Boolean* result);

    CARAPI StatsFromFile(
        /* [in] */ IContext* context,
        /* [in] */ const String& fname,
        /* [out] */ IBatteryStats** stats);

    CARAPI DropFile(
        /* [in] */ IContext* context,
        /* [in] */ const String& fname);

    CARAPI MakemAh(
        /* [in] */ Double power,
        /* [out] */ String* result);

    CARAPI ReadFully(
        /* [in] */ IFileInputStream* stream,
        /* [out, callee] */ ArrayOf<Byte>** bytes);

    CARAPI ReadFully(
        /* [in] */ IFileInputStream* stream,
        /* [in] */ Int32 avail,
        /* [out, callee] */ ArrayOf<Byte>** bytes);
};

} // namespace Os
} // namespace Droid
} // namespace Internal
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_OS_CBATTERYSTATSHELPERHELPER_H__
