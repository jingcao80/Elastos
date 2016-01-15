
#ifndef __ELASTOS_DROID_CONTENT_PM_PERMISSIONGROUPINFO_H__
#define __ELASTOS_DROID_CONTENT_PM_PERMISSIONGROUPINFO_H__

#include "PackageItemInfo.h"

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

/**
 * Information you can retrieve about a particular security permission
 * group known to the system.  This corresponds to information collected from the
 * AndroidManifest.xml's &lt;permission-group&gt; tags.
 */
class PermissionGroupInfo
    : public PackageItemInfo
    , public IPermissionGroupInfo
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    PermissionGroupInfo();

    virtual ~PermissionGroupInfo();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IPermissionGroupInfo* orig);

    /**
     * Retrieve the textual description of this permission.  This
     * will call back on the given PackageManager to load the description from
     * the application.
     *
     * @param pm A PackageManager from which the label can be loaded; usually
     * the PackageManager from which you originally retrieved this item.
     *
     * @return Returns a CharSequence containing the permission's description.
     * If there is no description, null is returned.
     */
    CARAPI LoadDescription(
        /* [in] */ IPackageManager* pm,
        /* [out] */ ICharSequence** description);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI LoadLabel(
        /* [in] */ IPackageManager* pm,
        /* [out] */ ICharSequence** label);

    CARAPI LoadIcon(
        /* [in] */ IPackageManager* pm,
        /* [out] */ IDrawable** icon);

    CARAPI LoadLogo(
        /* [in] */ IPackageManager* pm,
        /* [out] */ IDrawable** logo);

    CARAPI LoadXmlMetaData(
        /* [in] */ IPackageManager* pm,
        /* [in] */ const String& name,
        /* [out] */ IXmlResourceParser** resource);

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

    CARAPI GetLogo(
        /* [out] */ Int32* logo);

    CARAPI SetLogo(
        /* [in] */ Int32 logo);

    CARAPI GetMetaData(
        /* [out] */ IBundle** metaData);

    CARAPI SetMetaData(
        /* [in] */ IBundle* metaData);

    CARAPI GetDescriptionRes(
        /* [out] */ Int32* descriptionRes);

    CARAPI SetDescriptionRes(
        /* [in] */ Int32 descriptionRes);

    CARAPI GetNonLocalizedDescription(
        /* [out] */ ICharSequence** nonLocalizedDescription);

    CARAPI SetNonLocalizedDescription(
        /* [in] */ ICharSequence* nonLocalizedDescription);

    CARAPI GetFlags(
        /* [out] */ Int32* flags);

    CARAPI SetFlags(
        /* [in] */ Int32 flags);

    CARAPI GetPriority(
        /* [out] */ Int32* priority);

    CARAPI SetPriority(
        /* [in] */ Int32 priority);

public:
    /**
     * A string resource identifier (in the package's resources) of this
     * permission's description.  From the "description" attribute or,
     * if not set, 0.
     */
    Int32 mDescriptionRes;

    /**
     * The description string provided in the AndroidManifest file, if any.  You
     * probably don't want to use this, since it will be null if the description
     * is in a resource.  You probably want
     * {@link PermissionInfo#loadDescription} instead.
     */
    AutoPtr<ICharSequence> mNonLocalizedDescription;

    /**
     * Additional flags about this group as given by
     * {@link android.R.attr#permissionGroupFlags}.
     */
    Int32 mFlags;

    /**
     * Prioritization of this group, for visually sorting with other groups.
     */
    Int32 mPriority;

};

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_CONTENT_PM_PERMISSIONGROUPINFO_H__
