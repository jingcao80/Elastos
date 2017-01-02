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

#ifndef __ELASTOS_DROID_SETTINGS_DEVICEINFO_CMEMORY_H__
#define __ELASTOS_DROID_SETTINGS_DEVICEINFO_CMEMORY_H__

#include "Elastos.Droid.Hardware.h"
#include "_Elastos_Droid_Settings_Deviceinfo_CMemory.h"
#include "elastos/droid/settings/SettingsPreferenceFragment.h"
#include "elastos/droid/settings/search/BaseSearchIndexProvider.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/os/storage/StorageEventListener.h"

using Elastos::Droid::Settings::Search::BaseSearchIndexProvider;
using Elastos::Droid::Settings::Search::IIndexable;
using Elastos::Droid::Settings::Search::IIndexableSearchIndexProvider;

using Elastos::Droid::App::IDialog;
using Elastos::Droid::App::DialogFragment;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IIPackageDataObserver;
using Elastos::Droid::Hardware::Usb::IUsbManager;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Os::Storage::IIMountService;
using Elastos::Droid::Os::Storage::IStorageManager;
using Elastos::Droid::Os::Storage::StorageEventListener;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Droid::Preference::IPreferenceCategory;
using Elastos::Droid::View::IMenu;
using Elastos::Droid::View::IMenuInflater;
using Elastos::Droid::View::IMenuItem;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Deviceinfo {

/**
 * Panel showing storage usage on disk for known {@link StorageVolume} returned
 * by {@link StorageManager}. Calculates and displays usage of data types.
 */
CarClass(CMemory)
    , public SettingsPreferenceFragment
    , public IIndexable
{
public:
    class MyBaseSearchIndexProvider
        : public BaseSearchIndexProvider
    {
    public:
        TO_STRING_IMPL("CMemory::MyBaseSearchIndexProvider")

        MyBaseSearchIndexProvider();

        ~MyBaseSearchIndexProvider();

        //@Override
        CARAPI GetRawDataToIndex(
            /* [in] */ IContext* context,
            /* [in] */ Boolean enabled,
            /* [out] */ IList** list); //List<SearchIndexableRaw>
    };

    /**
     * Dialog to request user confirmation before clearing all cache data.
     */
    class ConfirmClearCacheFragment
        : public DialogFragment
    {
    public:
        TO_STRING_IMPL("CMemory::ConfirmClearCacheFragment")

        ConfirmClearCacheFragment();

        CARAPI constructor();

        static CARAPI_(void) Show(
            /* [in] */ CMemory* parent);

        //@Override
        CARAPI OnCreateDialog(
            /* [in] */ IBundle* savedInstanceState,
            /* [out] */ IDialog** result);
    };

    class ConfirmClearCacheFragmentDialogInterfaceOnClickListener
        : public Object
        , public IDialogInterfaceOnClickListener
    {
    public:
        TO_STRING_IMPL("CMemory::ConfirmClearCacheFragmentDialogInterfaceOnClickListener")

        CAR_INTERFACE_DECL()

        ConfirmClearCacheFragmentDialogInterfaceOnClickListener(
            /* [in] */ ConfirmClearCacheFragment* host);

        //@Override
        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which);

    private:
        ConfirmClearCacheFragment* mHost;
    };

    class MyStorageEventListener
        : public StorageEventListener
    {
    public:
        TO_STRING_IMPL("CMemory::MyStorageEventListener")

        MyStorageEventListener(
            /* [in] */ CMemory* host);

        //@Override
        CARAPI OnStorageStateChanged(
            /* [in] */ const String& path,
            /* [in] */ const String& oldState,
            /* [in] */ const String& newState);

    private:
        CMemory* mHost;
    };

    class MyBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("CMemory::MyBroadcastReceiver");

        MyBroadcastReceiver(
            /* [in] */ CMemory* host);

        //@Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        CMemory* mHost;
    };

    class ClearCacheObserver
        : public Object
        , public IIPackageDataObserver
        , public IBinder
    {
    public:
        TO_STRING_IMPL("CMemory::ClearCacheObserver")

        CAR_INTERFACE_DECL()

        ClearCacheObserver();

        CARAPI constructor(
            /* [in] */ ISettingsPreferenceFragment* target,
            /* [in] */ Int32 remaining);

        ~ClearCacheObserver();

        //@Override
        CARAPI OnRemoveCompleted(
            /* [in] */ const String& packageName,
            /* [in] */ Boolean succeeded);

    private:
        CMemory* mTarget;
        Int32 mRemaining;
    };

    class OnCreateDialogOnClickListener
        : public Object
        , public IDialogInterfaceOnClickListener
    {
    public:
        TO_STRING_IMPL("CMemory::OnCreateDialogOnClickListener")

        CAR_INTERFACE_DECL()

        OnCreateDialogOnClickListener(
            /* [in] */ CMemory* host);

        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which);

    private:
        CMemory* mHost;
    };

public:
    TO_STRING_IMPL("CMemory")

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CMemory();

    ~CMemory();

    CARAPI constructor();

    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* icicle);

    //@Override
    CARAPI OnResume();

    //@Override
    CARAPI OnPause();

    //@Override
    CARAPI OnDestroy();

    //@Override
    CARAPI OnCreateOptionsMenu(
        /* [in] */ IMenu* menu,
        /* [in] */ IMenuInflater* inflater);

    //@Override
    CARAPI OnPrepareOptionsMenu(
        /* [in] */ IMenu* menu);

    //@Override
    CARAPI OnOptionsItemSelected(
        /* [in] */ IMenuItem* item,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnPreferenceTreeClick(
        /* [in] */ IPreferenceScreen* preferenceScreen,
        /* [in] */ IPreference* preference,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnCreateDialog(
        /* [in] */ Int32 id,
        /* [out] */ IDialog** result);

    static CARAPI_(AutoPtr<IIndexableSearchIndexProvider>) GetSEARCH_INDEX_DATA_PROVIDER();

private:
    CARAPI_(void) AddCategory(
        /* [in] */ IPreferenceCategory* category);

    CARAPI_(Boolean) IsMassStorageEnabled();

    CARAPI_(AutoPtr<IIMountService>) GetMountService(); //synchronized


    CARAPI_(void) DoUnmount();

    CARAPI_(void) ShowDialogInner(
        /* [in] */ Int32 id);

    CARAPI_(Boolean) HasAppsAccessingStorage();// throws RemoteException

    CARAPI_(void) Unmount();

    CARAPI_(void) Mount();

    CARAPI_(void) OnCacheCleared();

private:
    static const String TAG;

    static const String TAG_CONFIRM_CLEAR_CACHE;

    static const Int32 DLG_CONFIRM_UNMOUNT;
    static const Int32 DLG_ERROR_UNMOUNT;

    // The mountToggle Preference that has last been clicked.
    // Assumes no two successive unmount event on 2 different volumes are performed before the first
    // one's preference is disabled
    static AutoPtr<IPreference> sLastClickedMountToggle;
    static String sClickedMountPoint;

    /**
     * Enable indexing of searchable data
     */
    static AutoPtr<IIndexableSearchIndexProvider> SEARCH_INDEX_DATA_PROVIDER;

    // Access using GetMountService()
    AutoPtr<IIMountService> mMountService;
    AutoPtr<IStorageManager> mStorageManager;
    AutoPtr<IUsbManager> mUsbManager;

    AutoPtr<IArrayList> mCategories;//ArrayList<StorageVolumePreferenceCategory>  = Lists->NewArrayList();

    AutoPtr<MyStorageEventListener> mStorageListener;

    AutoPtr<MyBroadcastReceiver> mMediaScannerReceiver;
};

} // namespace Deviceinfo
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_DEVICEINFO_CMEMORY_H__