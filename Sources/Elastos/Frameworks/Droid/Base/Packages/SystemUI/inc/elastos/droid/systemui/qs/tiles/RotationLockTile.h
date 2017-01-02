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

#ifndef __ELASTOS_DROID_SYSTEMUI_QS_TILES_ROTATIONLOCKTILE_H__
#define __ELASTOS_DROID_SYSTEMUI_QS_TILES_ROTATIONLOCKTILE_H__

#include "elastos/droid/systemui/qs/QSTile.h"

using Elastos::Droid::SystemUI::StatusBar::Policy::IRotationLockController;
using Elastos::Droid::SystemUI::StatusBar::Policy::IRotationLockControllerCallback;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Qs {
namespace Tiles {

/** Quick settings tile: Rotation **/
class RotationLockTile: public QSTile
{
private:
    class Callback
        : public Object
        , public IRotationLockControllerCallback
    {
    public:
        CAR_INTERFACE_DECL()

        Callback(
            /* [in] */ RotationLockTile* host);

        // @Override
        CARAPI OnRotationLockStateChanged(
            /* [in] */ Boolean rotationLocked,
            /* [in] */ Boolean affordanceVisible);

    private:
        RotationLockTile* mHost;
    };

public:
    CARAPI constructor(
        /* [in] */ IQSTileHost* host);

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
    /**
     * Get the correct accessibility string based on the state
     *
     * @param idWhenPortrait The id which should be used when locked in portrait.
     * @param idWhenLandscape The id which should be used when locked in landscape.
     * @param idWhenOff The id which should be used when the rotation lock is off.
     * @return
     */
    CARAPI_(String) GetAccessibilityString(
        /* [in] */ Int32 idWhenPortrait,
        /* [in] */ Int32 idWhenLandscape,
        /* [in] */ Int32 idWhenOff);

private:
    AutoPtr<IRotationLockController> mController;
    AutoPtr<IRotationLockControllerCallback> mCallback;
};

} // namespace Tiles
} // namespace Qs
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_QS_TILES_ROTATIONLOCKTILE_H__
