
#ifndef __ELASTOS_DROID_CONTENT_PM_PACKAGEITEMINFO_H__
#define __ELASTOS_DROID_CONTENT_PM_PACKAGEITEMINFO_H__

#include "Elastos.Droid.Content.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::Res::IXmlResourceParser;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Utility::IPrinter;

using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

/**
 * Base class containing information common to all package items held by
 * the package manager.  This provides a very common basic set of attributes:
 * a label, icon, and meta-data.  This class is not intended
 * to be used by itself; it is simply here to share common definitions
 * between all items returned by the package manager.  As such, it does not
 * itself implement Parcelable, but does provide convenience methods to assist
 * in the implementation of Parcelable in subclasses.
 */
class PackageItemInfo
    : public Object
    , public IPackageItemInfo
{
public:
    CAR_INTERFACE_DECL()

    PackageItemInfo();

    virtual ~PackageItemInfo();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IPackageItemInfo* orig);

    /**
     * Retrieve the current textual label associated with this item.  This
     * will call back on the given PackageManager to load the label from
     * the application.
     *
     * @param pm A PackageManager from which the label can be loaded; usually
     * the PackageManager from which you originally retrieved this item.
     *
     * @return Returns a CharSequence containing the item's label.  If the
     * item does not have a label, its name is returned.
     */
    virtual CARAPI LoadLabel(
        /* [in] */ IPackageManager* pm,
        /* [out] */ ICharSequence** label);

    /**
     * Retrieve the current graphical icon associated with this item.  This
     * will call back on the given PackageManager to load the icon from
     * the application.
     *
     * @param pm A PackageManager from which the icon can be loaded; usually
     * the PackageManager from which you originally retrieved this item.
     *
     * @return Returns a Drawable containing the item's icon.  If the
     * item does not have an icon, the item's default icon is returned
     * such as the default activity icon.
     */
    virtual CARAPI LoadIcon(
        /* [in] */ IPackageManager* pm,
        /* [out] */ IDrawable** icon);

    /**
     * Retrieve the current graphical banner associated with this item.  This
     * will call back on the given PackageManager to load the banner from
     * the application.
     *
     * @param pm A PackageManager from which the banner can be loaded; usually
     * the PackageManager from which you originally retrieved this item.
     *
     * @return Returns a Drawable containing the item's banner.  If the item
     * does not have a banner, this method will return null.
     */
    virtual CARAPI LoadBanner(
        /* [in] */ IPackageManager* pm,
        /* [out] */ IDrawable** icon);

    /**
     * Retrieve the current graphical logo associated with this item. This
     * will call back on the given PackageManager to load the logo from
     * the application.
     *
     * @param pm A PackageManager from which the logo can be loaded; usually
     * the PackageManager from which you originally retrieved this item.
     *
     * @return Returns a Drawable containing the item's logo. If the item
     * does not have a logo, this method will return null.
     */
    virtual CARAPI LoadLogo(
        /* [in] */ IPackageManager* pm,
        /* [out] */ IDrawable** logo);

    /**
     * Load an XML resource attached to the meta-data of this item.  This will
     * retrieved the name meta-data entry, and if defined call back on the
     * given PackageManager to load its XML file from the application.
     *
     * @param pm A PackageManager from which the XML can be loaded; usually
     * the PackageManager from which you originally retrieved this item.
     * @param name Name of the meta-date you would like to load.
     *
     * @return Returns an XmlPullParser you can use to parse the XML file
     * assigned as the given meta-data.  If the meta-data name is not defined
     * or the XML resource could not be found, null is returned.
     */
    virtual CARAPI LoadXmlMetaData(
        /* [in] */ IPackageManager* pm,
        /* [in] */ const String& name,
        /* [out] */ IXmlResourceParser** resource);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* src);

    CARAPI GetName(
        /* [out] */ String* name);

    CARAPI SetName(
        /* [in] */ const String& name);

    CARAPI GetPackageName(
        /* [out] */ String* name);

    CARAPI SetPackageName(
        /* [in] */ const String& name);

    CARAPI GetLabelRes(
        /* [out] */ Int32* labelRes);

    CARAPI SetLabelRes(
        /* [in] */ Int32 labelRes);

    CARAPI GetNonLocalizedLabel(
        /* [out] */ ICharSequence** label);

    CARAPI SetNonLocalizedLabel(
        /* [in] */ ICharSequence* label);

    CARAPI GetIcon(
        /* [out] */ Int32* icon);

    CARAPI SetIcon(
        /* [in] */ Int32 icon);

    CARAPI GetThemedIcon(
        /* [out] */ Int32* themeIcon);

    CARAPI SetThemedIcon(
        /* [in] */ Int32 themeIcon);

    CARAPI GetBanner(
        /* [out] */ Int32* banner);

    CARAPI SetBanner(
        /* [in] */ Int32 banner);

    CARAPI GetLogo(
        /* [out] */ Int32* logo);

    CARAPI SetLogo(
        /* [in] */ Int32 logo);

    CARAPI GetMetaData(
        /* [out] */ IBundle** metaData);

    CARAPI SetMetaData(
        /* [in] */ IBundle* metaData);

    CARAPI GetShowUserIcon(
        /* [out] */ Int32* icon);

    CARAPI SetShowUserIcon(
        /* [in] */ Int32 icon);

    /**
     * Retrieve the default graphical icon associated with this item.
     *
     * @param pm A PackageManager from which the icon can be loaded; usually
     * the PackageManager from which you originally retrieved this item.
     *
     * @return Returns a Drawable containing the item's default icon
     * such as the default activity icon.
     *
     * @hide
     */
    virtual CARAPI LoadDefaultIcon(
        /* [in] */ IPackageManager* pm,
        /* [out] */ IDrawable** icon);

    CARAPI ToString(
        /* [out] */ String* str);
protected:
    /**
     * Retrieve the default graphical banner associated with this item.
     *
     * @param pm A PackageManager from which the banner can be loaded; usually
     * the PackageManager from which you originally retrieved this item.
     *
     * @return Returns a Drawable containing the item's default banner
     * or null if no default logo is available.
     *
     * @hide
     */
    virtual CARAPI LoadDefaultBanner(
        /* [in] */ IPackageManager* pm,
        /* [out] */ IDrawable** icon);

    /**
     * Retrieve the default graphical logo associated with this item.
     *
     * @param pm A PackageManager from which the logo can be loaded; usually
     * the PackageManager from which you originally retrieved this item.
     *
     * @return Returns a Drawable containing the item's default logo
     * or null if no default logo is available.
     *
     * @hide
     */
    virtual CARAPI LoadDefaultLogo(
        /* [in] */ IPackageManager* pm,
        /* [out] */ IDrawable** logo);

    virtual CARAPI DumpFront(
        /* [in] */ IPrinter* pw,
        /* [in] */ const String& prefix);

    virtual CARAPI DumpBack(
        /* [in] */ IPrinter* pw,
        /* [in] */ const String& prefix);

    /**
     * Get the ApplicationInfo for the application to which this item belongs,
     * if available, otherwise returns null.
     *
     * @return Returns the ApplicationInfo of this item, or null if not known.
     *
     * @hide
     */
    virtual CARAPI_(AutoPtr<IApplicationInfo>) GetApplicationInfo();

public:
    /**
     * Public name of this item. From the "android:name" attribute.
     */
    String mName;

    /**
     * Name of the package that this item is in.
     */
    String mPackageName;

    /**
     * A string resource identifier (in the package's resources) of this
     * component's label.  From the "label" attribute or, if not set, 0.
     */
    Int32 mLabelRes;

    /**
     * The string provided in the AndroidManifest file, if any.  You
     * probably don't want to use this.  You probably want
     * {@link PackageManager#getApplicationLabel}
     */
    AutoPtr<ICharSequence> mNonLocalizedLabel;

    /**
     * A drawable resource identifier (in the package's resources) of this
     * component's icon.  From the "icon" attribute or, if not set, 0.
     */
    Int32 mIcon;

    /**
     * A drawable resource identifier in the icon pack's resources
     * If there isn't an icon pack or not set, then 0.
     * @hide
     */
    Int32 mThemedIcon;

    /**
     * A drawable resource identifier (in the package's resources) of this
     * component's banner.  From the "banner" attribute or, if not set, 0.
     */
    Int32 mBanner;

    /**
     * A drawable resource identifier (in the package's resources) of this
     * component's logo. Logos may be larger/wider than icons and are
     * displayed by certain UI elements in place of a name or name/icon
     * combination. From the "logo" attribute or, if not set, 0.
     */
    Int32 mLogo;

    /**
     * Additional meta-data associated with this component.  This field
     * will only be filled in if you set the
     * {@link PackageManager#GET_META_DATA} flag when requesting the info.
     */
    AutoPtr<IBundle> mMetaData;

    /**
     * If different of UserHandle.USER_NULL, The icon of this item will be the one of that user.
     * @hide
     */
    Int32 mShowUserIcon;
};

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_CONTENT_PM_PACKAGEITEMINFO_H__
