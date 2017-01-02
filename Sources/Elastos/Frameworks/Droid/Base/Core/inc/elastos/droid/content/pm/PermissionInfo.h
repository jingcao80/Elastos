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

#ifndef __ELASTOS_DROID_CONTENT_PM_PERMISSIONINFO_H__
#define __ELASTOS_DROID_CONTENT_PM_PERMISSIONINFO_H__

#include "PackageItemInfo.h"

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

/**
 * Information you can retrieve about a particular security permission
 * known to the system.  This corresponds to information collected from the
 * AndroidManifest.xml's &lt;permission&gt; tags.
 */
class PermissionInfo
    : public PackageItemInfo
    , public IPermissionInfo
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    PermissionInfo();

    virtual ~PermissionInfo();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Elastos::Droid::Content::Pm::IPermissionInfo* orig);

    /** @hide */
    static CARAPI_(Int32) FixProtectionLevel(
        /* [in] */ Int32 level);

    /** @hide */
    static CARAPI_(String) ProtectionToString(
        /* [in] */ Int32 level);

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

    CARAPI GetProtectionLevel(
        /* [out] */ Int32* level);

    CARAPI SetProtectionLevel(
        /* [in] */ Int32 level);

    CARAPI GetGroup(
        /* [out] */ String* group);

    CARAPI SetGroup(
        /* [in] */ const String& group);

    CARAPI GetFlags(
        /* [out] */ Int32* flags);

    CARAPI SetFlags(
        /* [in] */ Int32 flags);

    CARAPI GetDescriptionRes(
        /* [out] */ Int32* desRes);

    CARAPI SetDescriptionRes(
        /* [in] */ Int32 desRes);

    CARAPI GetNonLocalizedDescription(
        /* [out] */ ICharSequence** des);

    CARAPI SetNonLocalizedDescription(
        /* [in] */ ICharSequence* des);

public:
    /**
     * The level of access this permission is protecting, as per
     * {@link android.R.attr#protectionLevel}.  Values may be
     * {@link #PROTECTION_NORMAL}, {@link #PROTECTION_DANGEROUS}, or
     * {@link #PROTECTION_SIGNATURE}.  May also include the additional
     * flags {@link #PROTECTION_FLAG_SYSTEM} or {@link #PROTECTION_FLAG_DEVELOPMENT}
     * (which only make sense in combination with the base
     * {@link #PROTECTION_SIGNATURE}.
     */
    Int32 mProtectionLevel;

    /**
     * The group this permission is a part of, as per
     * {@link android.R.attr#permissionGroup}.
     */
    String mGroup;

    /**
     * Additional flags about this permission as given by
     * {@link android.R.attr#permissionFlags}.
     */
    Int32 mFlags;

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
};

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_CONTENT_PM_PERMISSIONINFO_H__
