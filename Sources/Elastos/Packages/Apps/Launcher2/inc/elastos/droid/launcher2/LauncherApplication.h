#ifndef  __ELASTOS_DROI_LAUNCHER2_LAUNCHERAPPLICATION_H__
#define  __ELASTOS_DROI_LAUNCHER2_LAUNCHERAPPLICATION_H__

#include "_Elastos.Droid.Launcher2.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/launcher2/IconCache.h"
#include "elastos/droid/launcher2/LauncherModel.h"
#include "elastos/droid/app/Application.h"
#include "elastos/droid/database/ContentObserver.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Database.h"
#include "Elastos.Droid.Os.h"

using Elastos::Droid::App::Application;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Database::IContentObserver;
using Elastos::Droid::Os::IHandler;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

class LauncherApplication
    : public Application
    , public ILauncherApplication
{
public:
    class MyContentObserver
        : public ContentObserver
    {
    public:
        MyContentObserver();

        CARAPI constructor();

        CARAPI constructor(
            /* [in] */ IHandler* handler,
            /* [in] */ ILauncherApplication* host);

        //@Override
        CARAPI OnChange(
            /* [in] */ Boolean selfChange);

    private:
        AutoPtr<LauncherApplication> mHost;
    };

public:
    CAR_INTERFACE_DECL();

    LauncherApplication();

    CARAPI constructor();

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
        /* [out] */ ILauncherModel** mode);

    CARAPI GetIconCache(
        /* [out] */ IIconCache** cache);

    CARAPI GetModel(
        /* [out] */ ILauncherModel** mode);

    CARAPI GetWidgetPreviewCacheDb(
        /* [out] */ IWidgetPreviewLoaderCacheDb** db);

    CARAPI SetLauncherProvider(
        /* [in] */ ILauncherProvider* provider);

    CARAPI GetLauncherProvider(
        /* [out] */ ILauncherProvider** provider);

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
    AutoPtr<ILauncherModel> mModel;
    AutoPtr<IconCache> mIconCache;
    AutoPtr<IWidgetPreviewLoaderCacheDb> mWidgetPreviewCacheDb;
    static Boolean sIsScreenLarge;
    static Float sScreenDensity;
    static Int32 sLongPressTimeout;
    static const String sSharedPreferencesKey;
    AutoPtr<IWeakReference> mLauncherProvider;

    /**
     * Receives notifications whenever the user favorites have changed.
     */
    AutoPtr<IContentObserver> mFavoritesObserver;

    AutoPtr<ILauncherApps> mLauncherApps;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROI_LAUNCHER2_LAUNCHERAPPLICATION_H__