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

#ifndef  __ELASTOS_DROID_LAUNCHER2_INSATLLSHORTCUTRECEIVER_H__
#define  __ELASTOS_DROID_LAUNCHER2_INSATLLSHORTCUTRECEIVER_H__

#include "_Elastos.Droid.Launcher2.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.View.h"
#include "Elastos.CoreLibrary.Core.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/core/Object.h>
#include <elastos/core/Thread.h>

using Elastos::Droid::Content::ISharedPreferencesEditor;
using Elastos::Droid::Content::ISharedPreferences;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IIntentShortcutIconResource;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Core::Object;
using Elastos::Core::Thread;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

class InstallShortcutReceiver
    : public BroadcastReceiver
    , public IInstallShortcutReceiver
{
private:
    class PendingInstallShortcutInfo
        : public Object
    {
    public:
        PendingInstallShortcutInfo(
            /* [in] */ IIntent* rawData,
            /* [in] */ const String& shortcutName,
            /* [in] */ IIntent* shortcutIntent);

    public:
        AutoPtr<IIntent> mData;
        AutoPtr<IIntent> mLaunchIntent;
        String mName;
        AutoPtr<IBitmap> mIcon;
        AutoPtr<IIntentShortcutIconResource> mIconResource;
    };

    class MyThread
        : public Thread
    {
    public:
        MyThread(
            /* [in] */ const String& name,
            /* [in] */ ISharedPreferences* sharedPrefs,
            /* [in] */ Int32 screen,
            /* [in] */ IIntent* intent);

        CARAPI Run();

    private:
        AutoPtr<ISharedPreferences> mSharedPrefs;
        Int32 mScreen;
        AutoPtr<IIntent> mIntent;
    };

public:
    TO_STRING_IMPL("InstallShortcutReceiver")

    CAR_INTERFACE_DECL();

    InstallShortcutReceiver();

    CARAPI constructor();

    //@Override
    CARAPI OnReceive(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent);

    static CARAPI EnableInstallQueue();

    static CARAPI DisableAndFlushInstallQueue(
        /* [in] */ IContext* context);

    static CARAPI FlushInstallQueue(
        /* [in] */ IContext* context);


private:
    static CARAPI AddToStringSet(
        /* [in] */ ISharedPreferences* sharedPrefs,
        /* [in] */ ISharedPreferencesEditor* editor,
        /* [in] */ const String& key,
        /* [in] */ const String& value);

    static CARAPI AddToInstallQueue(
        /* [in] */ ISharedPreferences* sharedPrefs,
        /* [in] */ PendingInstallShortcutInfo* info);

    static CARAPI_(AutoPtr<IArrayList>) GetAndClearInstallQueue(
        /* [in] */ ISharedPreferences* sharedPrefs);

    static CARAPI_(void) ProcessInstallShortcut(
        /* [in] */ IContext* context,
        /* [in] */ PendingInstallShortcutInfo* pendingInfo);

    static CARAPI_(Boolean) InstallShortcut(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* data,
        /* [in] */ IArrayList* items,
        /* [in] */ const String& name,
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 screen,
        /* [in] */ Boolean shortcutExists,
        /* [in] */ ISharedPreferences* sharedPrefs,
        /* [in] */ ArrayOf<Int32>* result);

    static CARAPI_(Boolean) FindEmptyCell(
        /* [in] */ IContext* context,
        /* [in] */ IArrayList* items,
        /* [in] */ ArrayOf<Int32>* xy,
        /* [in] */ Int32 screen);

private:
    static const Int32 INSTALL_SHORTCUT_SUCCESSFUL;
    static const Int32 INSTALL_SHORTCUT_IS_DUPLICATE;
    static const Int32 INSTALL_SHORTCUT_NO_SPACE;

    static Object sLock;

    // Determines whether to defer installing shortcuts immediately until
    // processAllPendingInstalls() is called.
    static Boolean mUseInstallQueue;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_INSATLLSHORTCUTRECEIVER_H__