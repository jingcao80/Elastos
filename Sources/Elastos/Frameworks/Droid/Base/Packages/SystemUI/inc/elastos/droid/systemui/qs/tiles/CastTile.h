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

#ifndef __ELASTOS_DROID_SYSTEMUI_QS_TILES_CASTTILE_H__
#define __ELASTOS_DROID_SYSTEMUI_QS_TILES_CASTTILE_H__

#include "elastos/droid/systemui/qs/QSTile.h"
#include "elastos/droid/systemui/statusbar/policy/KeyguardMonitor.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.Droid.View.h"

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::SystemUI::StatusBar::Policy::ICastControllerCallback;
using Elastos::Droid::SystemUI::StatusBar::Policy::IKeyguardMonitorCallback;
using Elastos::Droid::SystemUI::StatusBar::Policy::ICastController;
using Elastos::Droid::SystemUI::StatusBar::Policy::ICastControllerCastDevice;
using Elastos::Droid::SystemUI::StatusBar::Policy::IKeyguardMonitor;
using Elastos::Droid::SystemUI::StatusBar::Policy::KeyguardMonitor;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IViewOnAttachStateChangeListener;
using Elastos::Core::IBoolean;
using Elastos::Utility::ILinkedHashMap;
using Elastos::Utility::ISet;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Qs {
namespace Tiles {

/** Quick settings tile: Cast **/
class CastTile: public QSTile
{
private:
    class Callback
        : public Object
        , public ICastControllerCallback
        , public IKeyguardMonitorCallback
    {
    public:
        CAR_INTERFACE_DECL()

        Callback(
            /* [in] */ CastTile* host);

        // @Override
        CARAPI OnCastDevicesChanged();

        // @Override
        CARAPI OnKeyguardChanged();

    private:
        CastTile* mHost;
    };

    class CastDetailAdapter
        : public Object
        , public IQSTileDetailAdapter
        , public IQSDetailItemsCallback
    {
    private:
        class QSDetailItemsCallback
            : public Object
            , public IQSDetailItemsCallback
        {
        public:
            CAR_INTERFACE_DECL()

            QSDetailItemsCallback(
                /* [in] */ CastDetailAdapter* host);

            // @Override
            CARAPI OnDetailItemClick(
                /* [in] */ IQSDetailItemsItem* item);

            // @Override
            CARAPI OnDetailItemDisconnect(
                /* [in] */ IQSDetailItemsItem* item);

        private:
            CastDetailAdapter* mHost;
        };

        class MyListener
            : public Object
            , public IViewOnAttachStateChangeListener
        {
        public:
            CAR_INTERFACE_DECL()

            MyListener(
                /* [in] */ CastDetailAdapter* host);

            CARAPI OnViewAttachedToWindow(
                /* [in] */ IView* v);

            CARAPI OnViewDetachedFromWindow(
                /* [in] */ IView* v);

        private:
            CastDetailAdapter* mAdapter;
        };

    public:
        CAR_INTERFACE_DECL()

        CastDetailAdapter(
            /* [in] */ CastTile* host);

        // @Override
        CARAPI GetTitle(
            /* [out] */ Int32* result);

        // @Override
        CARAPI GetToggleState(
            /* [out] */ IBoolean** result);

        // @Override
        CARAPI GetSettingsIntent(
            /* [out] */ IIntent** result);

        // @Override
        CARAPI SetToggleState(
            /* [in] */ IBoolean* state);

        // @Override
        CARAPI CreateDetailView(
            /* [in] */ IContext* context,
            /* [in] */ IView* convertView,
            /* [in] */ IViewGroup* parent,
            /* [out] */ IView** result);

        CARAPI_(void) UpdateItems(
            /* [in] */ ISet/*<ICastControllerCastDevice>*/* devices);

        // @Override
        CARAPI OnDetailItemClick(
            /* [in] */ IQSDetailItemsItem* item);

        // @Override
        CARAPI OnDetailItemDisconnect(
            /* [in] */ IQSDetailItemsItem* item);

    private:
        AutoPtr<ILinkedHashMap> mVisibleOrder;  /*<String, ICastControllerCastDevice*/

        AutoPtr<IQSDetailItems> mItems;
        CastTile* mHost;
    };

public:
    CastTile(
        /* [in] */ IQSTileHost* host);

    // @Override
    CARAPI GetDetailAdapter(
        /* [out] */ IQSTileDetailAdapter** result);

    // @Override
    CARAPI SetListening(
        /* [in] */ Boolean listening);

    CARAPI_(String) GetSimpleName();

protected:
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
    CARAPI_(String) GetDeviceName(
        /* [in] */ ICastControllerCastDevice* device);

    static CARAPI_(AutoPtr<IIntent>) InitStatic();

private:
    static AutoPtr<IIntent> CAST_SETTINGS;

    AutoPtr<ICastController> mController;
    AutoPtr<CastDetailAdapter> mDetailAdapter;
    AutoPtr<IKeyguardMonitor> mKeyguard;
    AutoPtr<Callback> mCallback;
};

} // namespace Tiles
} // namespace Qs
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_QS_TILES_CASTTILE_H__
