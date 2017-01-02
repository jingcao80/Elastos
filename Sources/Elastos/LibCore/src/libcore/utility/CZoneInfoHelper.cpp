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

#include "CZoneInfoHelper.h"
#include "ZoneInfo.h"

namespace Libcore {
namespace Utility {

CAR_SINGLETON_IMPL(CZoneInfoHelper)

CAR_INTERFACE_IMPL(CZoneInfoHelper, Singleton, IZoneInfoHelper)

ECode CZoneInfoHelper::MakeTimeZone(
    /* [in] */ const String& id,
    /* [in] */ IBufferIterator* it,
    /* [out] */ IZoneInfo** zoneInfo)
{
    AutoPtr<IZoneInfo> zoneInfoObj = ZoneInfo::MakeTimeZone(id, it);
    *zoneInfo = zoneInfoObj;
    REFCOUNT_ADD(*zoneInfo)
    return NOERROR;
}


} // namespace Utility
} // namespace Libcore