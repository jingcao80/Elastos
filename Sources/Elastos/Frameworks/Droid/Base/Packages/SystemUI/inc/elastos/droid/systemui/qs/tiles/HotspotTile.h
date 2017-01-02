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

#ifndef __ELASTOS_DROID_SYSTEMUI_QS_TILES_HOTSPOTTILE_H__
#define __ELASTOS_DROID_SYSTEMUI_QS_TILES_HOTSPOTTILE_H__

#include "elastos/droid/systemui/qs/QSTile.h"
#include "elastos/droid/systemui/qs/UsageTracker.h"

using Elastos::Droid::SystemUI::StatusBar::Policy::IHotspotController;
using Elastos::Droid::SystemUI::StatusBar::Policy::IHotspotControllerCallback;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Qs {
namespace Tiles {

/** Quick settings tile: Hotspot **/
class HotspotTile: public QSTile
{
private:
    class Callback
        : public Object
        , public IHotspotControllerCallback
    {
    public:
        CAR_INTERFACE_DECL()

        Callback(
            /* [in] */ HotspotTile* host);

        // @Override
        CARAPI OnHotspotChanged(
            /* [in] */ Boolean enabled);

    private:
        HotspotTile* mHost;
    };

public:
    HotspotTile(
        /* [in] */ IQSTileHost* host);

    // @Override
    CARAPI SetListening(
        /* [in] */ Boolean listening);

    CARAPI_(String) GetSimpleName();

protected:
    // @Override
    CARAPI_(void) HandleDestroy();

    // @Override
    CARAPI_(AutoPtr<QSTile::State>) NewTileState();

    // @Override
    CARAPI_(void) HandleClick();

    // @Override
    CARAPI_(void) HandleUpdateState(
        /* [in] */ State* state,
        /* [in] */ IInterface* arg);

    // @Override
    CARAPI_(String) ComposeChangeAnnouncement();

private:
    AutoPtr<IHotspotController> mController;
    AutoPtr<Callback> mCallback;
    AutoPtr<UsageTracker> mUsageTracker;
};

} // namespace Tiles
} // namespace Qs
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_QS_TILES_HOTSPOTTILE_H__
