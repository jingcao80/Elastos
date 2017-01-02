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

#ifndef __ELASTOS_DROID_CONTENT_PM_COMPONENTINFO_H__
#define __ELASTOS_DROID_CONTENT_PM_COMPONENTINFO_H__

#include "elastos/droid/ext/frameworkext.h"
#include "PackageItemInfo.h"

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

/**
 * Base class containing information common to all application components
 * ({@link ActivityInfo}, {@link ServiceInfo}).  This class is not intended
 * to be used by itself; it is simply here to share common definitions
 * between all application components.  As such, it does not itself
 * implement Parcelable, but does provide convenience methods to assist
 * in the implementation of Parcelable in subclasses.
 */
class ComponentInfo
    : public PackageItemInfo
    , public IComponentInfo
{
public:
    CAR_INTERFACE_DECL()

    ComponentInfo();

    virtual ~ComponentInfo();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IComponentInfo* orig);

    // @override
    CARAPI LoadLabel(
        /* [in] */ IPackageManager* pm,
        /* [out] */ ICharSequence** label);

    /**
     * Return whether this component and its enclosing application are enabled.
     */
    virtual CARAPI IsEnabled(
        /* [out] */ Boolean* isEnable);

    /**
     * Return the icon resource identifier to use for this component.  If
     * the component defines an icon, that is used; else, the application
     * icon is used.
     *
     * @return The icon associated with this component.
     */
    CARAPI GetIconResource(
        /* [out] */ Int32* icon);

    /**
     * Return the logo resource identifier to use for this component.  If
     * the component defines a logo, that is used; else, the application
     * logo is used.
     *
     * @return The logo associated with this component.
     */
    CARAPI GetLogoResource(
        /* [out] */ Int32* icon);

    /**
     * Return the banner resource identifier to use for this component. If the
     * component defines a banner, that is used; else, the application banner is
     * used.
     *
     * @return The banner associated with this component.
     */
    CARAPI GetBannerResource(
        /* [out] */ Int32* icon);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* src);

    CARAPI GetApplicationInfo(
        /* [out] */ IApplicationInfo** info);

    CARAPI SetApplicationInfo(
        /* [in] */ IApplicationInfo* info);

    CARAPI GetProcessName(
        /* [out] */ String* processName);

    CARAPI SetProcessName(
        /* [in] */ const String& processName);

    CARAPI GetDescriptionRes(
        /* [out] */ Int32* desRes);

    CARAPI SetDescriptionRes(
        /* [in] */ Int32 desRes);

    CARAPI GetEnabled(
        /* [out] */ Boolean* enabled);

    CARAPI SetEnabled(
        /* [in] */ Boolean enabled);

    CARAPI GetExported(
        /* [out] */ Boolean* exported);

    CARAPI SetExported(
        /* [in] */ Boolean exported);

protected:
    CARAPI DumpFront(
        /* [in] */ IPrinter* pw,
        /* [in] */ const String& prefix);

    CARAPI DumpBack(
        /* [in] */ IPrinter* pw,
        /* [in] */ const String& prefix);

    /**
     * @hide
     */
    // @override
    CARAPI LoadDefaultIcon(
        /* [in] */ IPackageManager* pm,
        /* [out] */ IDrawable** icon);

    /**
     * @hide
     */
    // @override
    CARAPI LoadDefaultBanner(
        /* [in] */ IPackageManager* pm,
        /* [out] */ IDrawable** icon);

    /**
     * @hide
     */
    // @override
    CARAPI LoadDefaultLogo(
        /* [in] */ IPackageManager* pm,
        /* [out] */ IDrawable** icon);

    /**
     * @hide
     */
    // @override
    CARAPI_(AutoPtr<IApplicationInfo>) GetApplicationInfo();

public:
    /**
     * Global information about the application/package this component is a
     * part of.
     */
    AutoPtr<IApplicationInfo> mApplicationInfo;

    /**
     * The name of the process this component should run in.
     * From the "android:process" attribute or, if not set, the same
     * as <var>applicationInfo.processName</var>.
     */
    String mProcessName;

    /**
     * A string resource identifier (in the package's resources) containing
     * a user-readable description of the component.  From the "description"
     * attribute or, if not set, 0.
     */
    Int32 mDescriptionRes;

    /**
     * Indicates whether or not this component may be instantiated.  Note that this value can be
     * overriden by the one in its parent {@link ApplicationInfo}.
     */
    Boolean mEnabled;

    /**
     * Set to true if this component is available for use by other applications.
     * Comes from {@link android.R.attr#exported android:exported} of the
     * &lt;activity&gt;, &lt;receiver&gt;, &lt;service&gt;, or
     * &lt;provider&gt; tag.
     */
    Boolean mExported;
};

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_CONTENT_PM_COMPONENTINFO_H__
