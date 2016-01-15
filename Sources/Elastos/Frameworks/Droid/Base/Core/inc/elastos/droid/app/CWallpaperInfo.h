#ifndef __ELASTOS_DROID_APP_CWALLPAPERINFO_H__
#define __ELASTOS_DROID_APP_CWALLPAPERINFO_H__

#include "Elastos.Droid.Content.h"
#include "_Elastos_Droid_App_CWallpaperInfo.h"
#include <elastos/core/Object.h>

using Elastos::Core::ICharSequence;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Pm::IServiceInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Utility::IPrinter;

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CWallpaperInfo)
    , public Object
    , public IWallpaperInfo
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CWallpaperInfo();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IResolveInfo* service);

    CARAPI constructor();

    /**
     * Return the .apk package that implements this wallpaper.
     */
    CARAPI GetPackageName(
        /* [out] */ String* pkgName);

    /**
     * Return the class name of the service component that implements
     * this wallpaper.
     */
    CARAPI GetServiceName(
        /* [out] */ String* sName);

    /**
     * Return the raw information about the Service implementing this
     * wallpaper.  Do not modify the returned object.
     */
    CARAPI GetServiceInfo(
        /* [out] */ IServiceInfo** info);

    /**
     * Return the component of the service that implements this wallpaper.
     */
    CARAPI GetComponent(
        /* [out] */ IComponentName** component);

    /**
     * Load the user-displayed label for this wallpaper.
     *
     * @param pm Supply a PackageManager used to load the wallpaper's
     * resources.
     */
    CARAPI LoadLabel(
        /* [in] */ IPackageManager* pm,
        /* [out] */ ICharSequence** label);

    /**
     * Load the user-displayed icon for this wallpaper.
     *
     * @param pm Supply a PackageManager used to load the wallpaper's
     * resources.
     */
    CARAPI LoadIcon(
        /* [in] */ IPackageManager* pm,
        /* [out] */ IDrawable** icon);

    /**
     * Load the thumbnail image for this wallpaper.
     *
     * @param pm Supply a PackageManager used to load the wallpaper's
     * resources.
     */
    CARAPI LoadThumbnail(
        /* [in] */ IPackageManager* pm,
        /* [out] */ IDrawable** drawable);

    /**
     CARAPI * Return a string indicating the author(s) of this wallpaper.
     */
    CARAPI LoadAuthor(
        /* [in] */ IPackageManager* pm,
        /* [out] */ ICharSequence** author);

    /**
     * Return a brief summary of this wallpaper's behavior.
     */
    CARAPI LoadDescription(
        /* [in] */ IPackageManager* pm,
        /* [out] */ ICharSequence** summary);

    /**
     * Return the class name of an activity that provides a settings UI for
     * the wallpaper.  You can launch this activity be starting it with
     * an {@link android.content.Int32ent} whose action is MAIN and with an
     * explicit {@link android.content.ComponentName}
     * composed of {@link #getPackageName} and the class name returned here.
     *
     * <p>A null will be returned if there is no settings activity associated
     * with the wallpaper.
     */
    CARAPI GetSettingsActivity(
        /* [out] */ String* name);

    CARAPI Dump(
        /* [in] */ IPrinter* pw,
        /* [in] */ const String& prefix);

    CARAPI SetService(
        /* [in] */ IResolveInfo* service);

    CARAPI GetService(
        /* [out] */ IResolveInfo** service);

    CARAPI SetSettingsActivityName(
        /* [in] */ const String& name);

    CARAPI GetSettingsActivityName(
        /* [out] */ String* name);

    CARAPI SetThumbnailResource(
        /* [in] */ Int32 thumbnail);

    CARAPI GetThumbnailResource(
        /* [out] */ Int32* thumbnail);

    CARAPI SetAuthorResource(
        /* [in] */ Int32 author);

    CARAPI GetAuthorResource(
        /* [out] */ Int32* author);

    CARAPI SetDescriptionResource(
        /* [in] */ Int32 description);

    CARAPI GetDescriptionResource(
        /* [out] */ Int32* description);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

public:
    static const String TAG;

    /**
     * The Service that implements this wallpaper component.
     */
    AutoPtr<IResolveInfo> mService;

    /**
     * The wallpaper setting activity's name, to
     * launch the setting activity of this wallpaper.
     */
    String mSettingsActivityName;

    /**
     * Resource identifier for this wallpaper's thumbnail image.
     */
    Int32 mThumbnailResource;

    /**
     * Resource identifier for a string indicating the author of the wallpaper.
     */
    Int32 mAuthorResource;

    /**
     * Resource identifier for a string containing a short description of the wallpaper.
     */
    Int32 mDescriptionResource;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_APP_CWALLPAPERINFO_H__
