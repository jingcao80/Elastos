
#ifndef __ELASTOS_DROID_CONTENT_RES_CTHEMEMANAGER_H__
#define __ELASTOS_DROID_CONTENT_RES_CTHEMEMANAGER_H__

#include "_Elastos_Droid_Content_Res_CThemeManager.h"
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Os.h"
#include "elastos/droid/content/res/CThemeChangeListener.h"
#include "elastos/droid/content/res/CThemeProcessingListener.h"
#define HASH_FOR_CONTENT
#include "elastos/droid/ext/frameworkhash.h"
#include <elastos/core/Object.h>
#include <elastos/utility/etl/HashSet.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IHandler;
using Elastos::Utility::IList;
using Elastos::Utility::IMap;
using Elastos::Utility::Etl::HashSet;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Res {

CarClass(CThemeManager)
    , public Object
    , public IThemeManager
{
public:
    CThemeManager();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IIThemeService* service);

    CARAPI AddClient(
        /* [in] */ IThemeChangeListener* listener);

    CARAPI RemoveClient(
        /* [in] */ IThemeChangeListener* listener);

    CARAPI OnClientPaused(
        /* [in] */ IThemeChangeListener* listener);

    CARAPI OnClientResumed(
        /* [in] */ IThemeChangeListener* listener);

    CARAPI OnClientDestroyed(
        /* [in] */ IThemeChangeListener* listener);

    /**
     * Register a ThemeProcessingListener to be notified when a theme is done being processed.
     * @param listener ThemeChangeListener to register
     */
    CARAPI RegisterProcessingListener(
        /* [in] */ IThemeProcessingListener* listener);

    /**
     * Unregister a ThemeChangeListener.
     * @param listener ThemeChangeListener to unregister
     */
    CARAPI UnregisterProcessingListener(
        /* [in] */ IThemeProcessingListener* listener);

    /**
     * Convenience method. Applies the entire theme.
     */
    CARAPI RequestThemeChange(
        /* [in] */ const String& pkgName);

    CARAPI RequestThemeChange(
        /* [in] */ const String& pkgName,
        /* [in] */ IList* components);

    CARAPI RequestThemeChange(
        /* [in] */ const String& pkgName,
        /* [in] */ IList* components,
        /* [in] */ Boolean removePerAppThemes);

    CARAPI RequestThemeChange(
        /* [in] */ IMap* componentMap);

    CARAPI RequestThemeChange(
        /* [in] */ IMap* componentMap,
        /* [in] */ Boolean removePerAppThemes);

    CARAPI RequestThemeChange(
        /* [in] */ IThemeChangeRequest* request,
        /* [in] */ Boolean removePerAppThemes);

    CARAPI ApplyDefaultTheme();

    CARAPI IsThemeApplying(
        /* [out] */ Boolean* result);

    CARAPI IsThemeBeingProcessed(
        /* [in] */ const String& themePkgName,
        /* [out] */ Boolean* result);

    CARAPI GetProgress(
        /* [out] */ Int32* progress);

    CARAPI ProcessThemeResources(
        /* [in] */ const String& themePkgName,
        /* [out] */ Boolean* result);

private:
    static const String TAG;
    IContext* mContext;
    AutoPtr<IIThemeService> mService;
    AutoPtr<IHandler> mHandler;

    HashSet<AutoPtr<IThemeChangeListener> > mChangeListeners;
    Object mChangeListenersLock;

    HashSet<AutoPtr<IThemeProcessingListener> > mProcessingListeners;
    Object mProcessingListenersLock;

    AutoPtr<IIThemeChangeListener> mThemeChangeListener;
    AutoPtr<IIThemeProcessingListener> mThemeProcessingListener;

    friend class CThemeChangeListener;
    friend class CThemeProcessingListener;
};

} // namespace Res
} // namespace Content
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_CONTENT_RES_CTHEMEMANAGER_H__
