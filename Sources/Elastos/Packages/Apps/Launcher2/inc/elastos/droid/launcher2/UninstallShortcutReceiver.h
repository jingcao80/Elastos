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

#ifndef  __ELASTOS_DROID_LAUNCHER2_UNINSTALLSHORTCUTRECEIVER_H__
#define  __ELASTOS_DROID_LAUNCHER2_UNINSTALLSHORTCUTRECEIVER_H__

#include "_Elastos.Droid.Launcher2.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.CoreLibrary.Core.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/core/Object.h>
#include <elastos/core/Thread.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::ISharedPreferences;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Core::Object;
using Elastos::Core::Thread;
using Elastos::Utility::ISet;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

class UninstallShortcutReceiver
    : public BroadcastReceiver
    , public IUninstallShortcutReceiver
{
private:
    class PendingUninstallShortcutInfo
        : public Object
    {
    public:
        PendingUninstallShortcutInfo(
            /* [in] */ IIntent* rawData);
    public:
        AutoPtr<IIntent> mData;
    };

    class MyThread
        : public Thread
    {
    public:
        MyThread(
            /* [in] */ ISet* savedNewApps,
            /* [in] */ ISharedPreferences* sharedPrefs);

        CARAPI Run();

    private:
        AutoPtr<ISet> mSavedNewApps;
        AutoPtr<ISharedPreferences> mSharedPrefs;
    };

public:
    TO_STRING_IMPL("UninstallShortcutReceiver")

    CAR_INTERFACE_DECL();

    UninstallShortcutReceiver();

    CARAPI constructor();

    CARAPI OnReceive(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* data);

    static CARAPI EnableUninstallQueue();

    static CARAPI DisableAndFlushUninstallQueue(
        /* [in] */ IContext* context);

private:
    static CARAPI_(void) ProcessUninstallShortcut(
        /* [in] */ IContext* context,
        /* [in] */ PendingUninstallShortcutInfo* pendingInfo);

    static CARAPI_(void) RemoveShortcut(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* data,
        /* [in] */ ISharedPreferences* sharedPrefs);

private:
    static const String ACTION_UNINSTALL_SHORTCUT;

    // The set of shortcuts that are pending uninstall
    static AutoPtr<IArrayList> mUninstallQueue;

    // Determines whether to defer uninstalling shortcuts immediately until
    // disableAndFlushUninstallQueue() is called.
    static Boolean mUseUninstallQueue;

};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_UNINSTALLSHORTCUTRECEIVER_H__