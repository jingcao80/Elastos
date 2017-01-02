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

#ifndef __ELASTOS_DROID_SETTINGSPROVIDER_CPROFILEPICKERACTIVITY_H__
#define __ELASTOS_DROID_SETTINGSPROVIDER_CPROFILEPICKERACTIVITY_H__

#include "elastos/droid/internal/app/AlertActivity.h"
#include "_Elastos_Droid_SettingsProvider_CProfilePickerActivity.h"
#include <Elastos.Droid.App.h>
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Internal.h>
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.Widget.h>
#include "Elastos.CoreLibrary.Utility.h"

using Elastos::Droid::App::IProfile;
using Elastos::Droid::App::IProfileManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::IResourcesTheme;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::Internal::App::AlertActivity;
using Elastos::Droid::Internal::App::IAlertController;
using Elastos::Droid::Internal::App::IAlertControllerAlertParamsOnPrepareListViewListener;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Widget::IArrayAdapter;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::Widget::ITextView;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;
using Elastos::Utility::IUUID;

namespace Elastos {
namespace Droid {
namespace SettingsProvider {

/**
 * The {@link ProfilePickerActivity} allows the user to choose one from all of the
 * available profiles.
 *
 * @see ProfileManager#ACTION_PROFILE_PICKER
 * @hide
 */
CarClass(CProfilePickerActivity)
    , public AlertActivity
    , public IDialogInterfaceOnClickListener
    , public IAlertControllerAlertParamsOnPrepareListViewListener

{
private:
    class MyDialogInterfaceOnClickListener
        : public Object
        , public IDialogInterfaceOnClickListener
    {
    public:
        TO_STRING_IMPL("CProfilePickerActivity::MyDialogInterfaceOnClickListener")

        CAR_INTERFACE_DECL()

        MyDialogInterfaceOnClickListener(
            /* [in] */ CProfilePickerActivity* host)
            : mHost(host)
        {}

        //@Override
        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which);

    private:
        CProfilePickerActivity* mHost;
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CProfilePickerActivity();

    CARAPI constructor();

    //@Override
    CARAPI OnSaveInstanceState(
        /* [in] */ IBundle* outState);

    //@Override
    CARAPI OnPrepareListView(
        /* [in] */ IListView* listView);

    /*
     * On click of Ok/Cancel buttons
     */
    CARAPI OnClick(
        /* [in] */ IDialogInterface* dialog,
        /* [in] */ Int32 which);

protected:
    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

private:
    CARAPI_(Int32) GetPositionForUUID(
        /* [in] */ IUUID* uuid);

    /**
     * Adds a static item to the top of the list. A static item is one that is not from the
     * ProfileManager.
     *
     * @param listView The ListView to add to.
     * @param textResId The resource ID of the text for the item.
     * @return The position of the inserted item.
     */
    CARAPI_(Int32) AddStaticItem(
        /* [in] */ IListView* listView,
        /* [in] */ Int32 textResId);

    CARAPI_(Int32) AddNoneItem(
        /* [in] */ IListView* listView);

private:
    static const String TAG;

    static const String SAVE_CLICKED_POS;

    AutoPtr<IProfileManager> mProfileManager;

    /** The position in the list of the 'None' item. */
    Int32 mNonePos;

    /** The position in the list of the last clicked item. */
    Int32 mClickedPos;

    /** Whether this list has the 'None' item. */
    Boolean mHasNoneItem;

    /** The UUID to place a checkmark next to. */
    AutoPtr<IUUID> mExistingUUID;

    AutoPtr<IList> mProfiles;

    AutoPtr<IDialogInterfaceOnClickListener> mProfileClickListener;
};

} // namespace SettingsProvider
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SETTINGSPROVIDER_CPROFILEPICKERACTIVITY_H__
