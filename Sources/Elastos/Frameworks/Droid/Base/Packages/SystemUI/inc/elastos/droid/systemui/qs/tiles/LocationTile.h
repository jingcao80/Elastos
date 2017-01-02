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

#ifndef __ELASTOS_DROID_SYSTEMUI_QS_TILES_LOCATIONTILE_H__
#define __ELASTOS_DROID_SYSTEMUI_QS_TILES_LOCATIONTILE_H__

#include "elastos/droid/systemui/qs/QSTile.h"

using Elastos::Droid::SystemUI::StatusBar::Policy::IKeyguardMonitor;
using Elastos::Droid::SystemUI::StatusBar::Policy::IKeyguardMonitorCallback;
using Elastos::Droid::SystemUI::StatusBar::Policy::ILocationController;
using Elastos::Droid::SystemUI::StatusBar::Policy::ILocationSettingsChangeCallback;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Qs {
namespace Tiles {

/** Quick settings tile: Location **/
class LocationTile: public QSTile
{
private:
    class Callback
        : public Object
        , public ILocationSettingsChangeCallback
        , public IKeyguardMonitorCallback
    {
    public:
        CAR_INTERFACE_DECL()

        Callback(
            /* [in] */ LocationTile* host);

        // @Override
        CARAPI OnLocationSettingsChanged(
            /* [in] */ Boolean enabled);

        // @Override
        CARAPI OnKeyguardChanged();

    private:
        LocationTile* mHost;
    };

public:
    LocationTile(
        /* [in] */ IQSTileHost* host);

    // @Override
    CARAPI SetListening(
        /* [in] */ Boolean listening);

    CARAPI_(String) GetSimpleName();

protected:
    // @Override
    CARAPI_(AutoPtr<State>) NewTileState();

    // @Override
    CARAPI_(void) HandleClick();

    // @Override
    CARAPI_(void) HandleUpdateState(
        /* [in] */ State* state,
        /* [in] */ IInterface* arg);

    // @Override
    CARAPI_(String) ComposeChangeAnnouncement();

private:
    AutoPtr<ILocationController> mController;
    AutoPtr<IKeyguardMonitor> mKeyguard;
    AutoPtr<Callback> mCallback;
};

} // namespace Tiles
} // namespace Qs
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_QS_TILES_LOCATIONTILE_H__
