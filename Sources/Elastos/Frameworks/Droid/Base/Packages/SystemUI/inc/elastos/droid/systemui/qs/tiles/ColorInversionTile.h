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

#ifndef __ELASTOS_DROID_SYSTEMUI_QS_TILES_COLORINVERSIONTILE_H__
#define __ELASTOS_DROID_SYSTEMUI_QS_TILES_COLORINVERSIONTILE_H__

#include "elastos/droid/systemui/qs/QSTile.h"
#include "elastos/droid/systemui/qs/SecureSetting.h"
#include "elastos/droid/systemui/qs/UsageTracker.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Qs {
namespace Tiles {

/** Quick settings tile: Invert colors **/
class ColorInversionTile
    : public QSTile
    , public IColorInversionTile
{
private:
    class ColorSettings: public SecureSetting
    {
    public:
        CARAPI constructor(
            /* [in] */ IContext* context,
            /* [in] */ IHandler* handler,
            /* [in] */ const String& settingName,
            /* [in] */ ColorInversionTile* host);

    protected:
        // @Override
        CARAPI HandleValueChanged(
            /* [in] */ Int32 value);

    private:
        ColorInversionTile* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    ColorInversionTile(
        /* [in] */ IQSTileHost* host);

    // @Override
    CARAPI SetListening(
        /* [in] */ Boolean listening);

    CARAPI_(String) GetSimpleName();

protected:
    // @Override
    CARAPI_(void) HandleDestroy();

    // @Override
    CARAPI_(AutoPtr<State>) NewTileState();

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
    AutoPtr<ColorSettings> mSetting;
    AutoPtr<UsageTracker> mUsageTracker;

    Boolean mListening;
};

} // namespace Tiles
} // namespace Qs
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_QS_TILES_COLORINVERSIONTILE_H__
