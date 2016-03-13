#ifndef  __ELASTOS_DROID_LAUNCHER2_INSATLLSHORTCUTRECEIVER_H__
#define  __ELASTOS_DROID_LAUNCHER2_INSATLLSHORTCUTRECEIVER_H__

#include "elastos/droid/ext/frameworkext.h"

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
            /* [in] */ InstallShortcutReceiver* host,
            /* [in] */ ISharedPreferences* sharedPrefs,
            /* [in] */ Int32 screen,
            /* [in] */ IIntent* intent)

        CARAPI Run();

    private:
        InstallShortcutReceiver* mHost;
        ISharedPreferences* mSharedPrefs;
        Int32 mScreen;
        IIntent* mIntent;
    };

public:
    CAR_INTERFACE_DECL();

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
        /* [in] */ ISharedPreferences sharedPrefs);

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
        /* [in] */ Int32 screen)

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