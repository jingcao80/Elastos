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

#ifndef __ELASTOS_DROID_SETTINGS_INPUTMETHOD_CUSERDICTIONARYADDWORDFRAGMENT_H__
#define __ELASTOS_DROID_SETTINGS_INPUTMETHOD_CUSERDICTIONARYADDWORDFRAGMENT_H__

#include "_Elastos_Droid_Settings_Inputmethod_CUserDictionaryAddWordFragment.h"
#include "elastos/droid/settings/inputmethod/UserDictionaryAddWordContents.h"
#include "elastos/droid/app/Fragment.h"

using Elastos::Droid::Settings::Inputmethod::UserDictionaryAddWordContents;

using Elastos::Droid::App::Fragment;
using Elastos::Droid::Internal::App::ILocaleSelectionListener;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IMenu;
using Elastos::Droid::View::IMenuInflater;
using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::IAdapterViewOnItemSelectedListener;
using Elastos::Droid::Widget::IArrayAdapter;
using Elastos::Utility::IArrayList;
using Elastos::Utility::ILocale;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Inputmethod {

/**
 * Fragment to add a word/shortcut to the user dictionary.
 *
 * As opposed to the UserDictionaryActivity, this is only invoked within Settings
 * from the UserDictionarySettings.
 */
CarClass(CUserDictionaryAddWordFragment)
    , public Fragment
    , public IUserDictionaryAddWordFragment
    , public IAdapterViewOnItemSelectedListener
    , public ILocaleSelectionListener
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CUserDictionaryAddWordFragment();

    ~CUserDictionaryAddWordFragment();

    CARAPI constructor();

    //@Override
    CARAPI OnActivityCreated(
        /* [in] */ IBundle* savedInstanceState);

    //@Override
    CARAPI OnCreateView(
        /* [in] */ ILayoutInflater* inflater,
        /* [in] */ IViewGroup* container,
        /* [in] */ IBundle* savedState,
        /* [out] */ IView** result);

    //@Override
    CARAPI OnCreateOptionsMenu(
        /* [in] */ IMenu* menu,
        /* [in] */ IMenuInflater* inflater);

    /**
     * Callback for the framework when a menu option is pressed.
     *
     * This class only supports the delete menu item.
     * @param MenuItem the item that was pressed
     * @return FALSE to allow normal menu processing to proceed, TRUE to consume it here
     */
    //@Override
    CARAPI OnOptionsItemSelected(
        /* [in] */ IMenuItem* item,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnResume();

    //@Override
    CARAPI OnPause();

    //@Override
    CARAPI OnItemSelected(
        /* [in] */ IAdapterView* parent,
        /* [in] */ IView* view,
        /* [in] */ Int32 pos,
        /* [in] */ Int64 id);

    //@Override
    CARAPI OnNothingSelected(
        /* [in] */ IAdapterView* parent);

    // Called by the locale picker
    //@Override
    CARAPI OnLocaleSelected(
        /* [in] */ ILocale* locale);

private:
    CARAPI_(void) UpdateSpinner();

private:
    static const Int32 OPTIONS_MENU_DELETE;

    AutoPtr<UserDictionaryAddWordContents> mContents;
    AutoPtr<IView> mRootView;
    Boolean mIsDeleting;
};

} // namespace Inputmethod
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_INPUTMETHOD_CUSERDICTIONARYADDWORDFRAGMENT_H__