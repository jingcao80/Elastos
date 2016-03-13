#ifndef  __ELASTOS_DROID_LAUNCHER2_UNINSTALLSHORTCUTRECEIVER_H__
#define  __ELASTOS_DROID_LAUNCHER2_UNINSTALLSHORTCUTRECEIVER_H__

#include "elastos/droid/ext/frameworkext.h"

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
        ISet* mSavedNewApps;
        ISharedPreferences* mSharedPrefs;
    };

public:
    CAR_INTERFACE_DECL();

    CARAP OnReceive(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* data);

    static CARAP EnableUninstallQueue();

    static CARAP DisableAndFlushUninstallQueue(
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