#ifndef  __ELASTOS_DROI_LAUNCHER2_LAUNCHERAPPLICATION_H__
#define  __ELASTOS_DROI_LAUNCHER2_LAUNCHERAPPLICATION_H__

#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

class LauncherApplication
    : public Application
    , public ILauncherApplication
{
private:
    class MyContentObserver
        : ContentObserver
    {
    public:
        MyContentObserver(
            /* [in] */ IHandler* handler,
            /* [in] */ LauncherApplication* host);

        //@Override
        CARAPI OnChange(
            /* [in] */ Boolean selfChange);

    private:
        LauncherApplication* mHost;
    };

public:
    CAR_INTERFACE_DECL();

    LauncherApplication();

    //@Override
    CARAPI OnCreate();

    CARAPI RecreateWidgetPreviewDb();

    /**
     * There's no guarantee that this function is ever called.
     */
    //@Override
    CARAPI OnTerminate();

    CARAPI SetLauncher(
        /* [in] */ ILauncher* launcher,
        /* [out] */ LauncherModel** mode);

    CARAPI GetIconCache(
        /* [out] */ IconCache** cache);

    CARAPI GetModel(
        /* [out] */ LauncherModel** mode);

    CARAPI GetWidgetPreviewCacheDb(
        /* [out] */ WidgetPreviewLoader::CacheDb** db);

    CARAPI SetLauncherProvider(
        /* [in] */ ILauncherProvider* provider);

    CARAPI GetLauncherProvider(
        /* [in] */ ILauncherProvider** provider);

    static CARAPI GetSharedPreferencesKey(
        /* [out] */ String* key);

    static CARAPI IsScreenLarge(
        /* [out] */ Boolean* result);

    static CARAPI IsScreenLandscape(
        /* [in] */ IContext* context,
        /* [out] */ Boolean* result);

    static CARAPI GetScreenDensity(
        /* [out] */ Float* density);

    static CARAPI GetLongPressTimeout(
        /* [out] */ Int32* timeout);

protected:
    static const String TAG;

private:
    AutoPtr<LauncherModel> mModel;
    AutoPtr<IconCache> mIconCache;
    AutoPtr<WidgetPreviewLoader::CacheDb> mWidgetPreviewCacheDb;
    static Boolean sIsScreenLarge;
    static Float sScreenDensity;
    static Int32 sLongPressTimeout;
    static const String sSharedPreferencesKey;
    AutoPtr<IWeakReference> mLauncherProvider;

    /**
     * Receives notifications whenever the user favorites have changed.
     */
    AutoPtr<IContentObserver> mFavoritesObserver;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROI_LAUNCHER2_LAUNCHERAPPLICATION_H__