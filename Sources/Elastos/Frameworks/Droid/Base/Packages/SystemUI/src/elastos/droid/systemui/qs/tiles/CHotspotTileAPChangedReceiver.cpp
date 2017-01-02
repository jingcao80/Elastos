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

#include "elastos/droid/systemui/qs/tiles/CHotspotTileAPChangedReceiver.h"
#include "elastos/droid/systemui/qs/tiles/HotspotTile.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Qs {
namespace Tiles {

CAR_OBJECT_IMPL(CHotspotTileAPChangedReceiver)
ECode CHotspotTileAPChangedReceiver::constructor(
    /* [in] */ IQSTile* host)
{
    mHost = (HotspotTile*)host;
    return NOERROR;
}

ECode CHotspotTileAPChangedReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    if (mUsageTracker == NULL) {
        mUsageTracker = new UsageTracker(context, mHost/*HotspotTile.class*/);
    }
    mUsageTracker->TrackUsage();
    return NOERROR;
}

} // namespace Tiles
} // namespace Qs
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
