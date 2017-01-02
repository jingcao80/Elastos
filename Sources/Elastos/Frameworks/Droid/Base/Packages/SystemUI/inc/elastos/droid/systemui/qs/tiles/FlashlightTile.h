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

#ifndef __ELASTOS_DROID_SYSTEMUI_QS_TILES_FLASHLIGHTTILE_H__
#define __ELASTOS_DROID_SYSTEMUI_QS_TILES_FLASHLIGHTTILE_H__

#include "elastos/droid/systemui/qs/QSTile.h"
#include <elastos/core/Runnable.h>

using Elastos::Core::Runnable;
using Elastos::Droid::SystemUI::StatusBar::Policy::IFlashlightController;
using Elastos::Droid::SystemUI::StatusBar::Policy::IFlashlightListener;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Qs {
namespace Tiles {

/** Quick settings tile: Control flashlight **/
class FlashlightTile
    : public QSTile
{
private:
    class InnerListener
        : public Object
        , public IFlashlightListener
    {
    public:
        CAR_INTERFACE_DECL()

        InnerListener(
            /* [in] */ FlashlightTile* host);

        // @Override
        CARAPI OnFlashlightOff();

        // @Override
        CARAPI OnFlashlightError();

        // @Override
        CARAPI OnFlashlightAvailabilityChanged(
            /* [in] */ Boolean available);

    private:
        FlashlightTile* mHost;
    };

    class RecentlyOnTimeout: public Runnable
    {
    public:
        RecentlyOnTimeout(
            /* [in] */ FlashlightTile* host);

        // @Override
        CARAPI Run();

    private:
        FlashlightTile* mHost;
    };

public:
    FlashlightTile(
        /* [in] */ IQSTileHost* host);

    // @Override
    CARAPI SetListening(
        /* [in] */ Boolean listening);

    // @Override
    CARAPI OnFlashlightOff();

    // @Override
    CARAPI OnFlashlightError();

    // @Override
    CARAPI OnFlashlightAvailabilityChanged(
        /* [in] */ Boolean available);

    CARAPI_(String) GetSimpleName();

protected:
    // @Override
    CARAPI_(void) HandleDestroy();

    // @Override
    CARAPI_(AutoPtr<QSTile::State>) NewTileState();

    // @Override
    CARAPI_(void) HandleUserSwitch(
        /* [in] */ Int32 newUserId);

    // @Override
    CARAPI_(void) HandleClick();

    // @Override
    CARAPI_(void) HandleUpdateState(
        /* [in] */ State* state,
        /* [in] */ IInterface* arg);

    // @Override
    CARAPI_(String) ComposeChangeAnnouncement();

private:
    /** Grace period for which we consider the flashlight
     * still available because it was recently on. */
    static const Int64 RECENTLY_ON_DURATION_MILLIS;

    AutoPtr<IFlashlightController> mFlashlightController;
    Int64 mWasLastOn;
    AutoPtr<IRunnable> mRecentlyOnTimeout;
    AutoPtr<InnerListener> mListener;
};

} // namespace Tiles
} // namespace Qs
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_QS_TILES_FLASHLIGHTTILE_H__
