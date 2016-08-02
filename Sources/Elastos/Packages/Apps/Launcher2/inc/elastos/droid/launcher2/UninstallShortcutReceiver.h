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