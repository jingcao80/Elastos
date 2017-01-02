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

#ifndef __ELASTOS_DROID_CONTENT_PM_RESOLVEINFO_H__
#define __ELASTOS_DROID_CONTENT_PM_RESOLVEINFO_H__

#include "Elastos.Droid.Content.h"
#include <elastos/core/Object.h>

using Elastos::Core::ICharSequence;

using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Utility::IPrinter;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

/**
 * Information that is returned from resolving an intent
 * against an IntentFilter. This partially corresponds to
 * information collected from the AndroidManifest.xml's
 * &lt;intent&gt; tags.
 */
class ResolveInfo
    : public Object
    , public IResolveInfo
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    ResolveInfo();

    virtual ~ResolveInfo();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IResolveInfo* orig);

    CARAPI constructor(
        /* [in] */ IParcel* source);

    /**
     * Retrieve the current textual label associated with this resolution.  This
     * will call back on the given PackageManager to load the label from
     * the application.
     *
     * @param pm A PackageManager from which the label can be loaded; usually
     * the PackageManager from which you originally retrieved this item.
     *
     * @return Returns a CharSequence containing the resolutions's label.  If the
     * item does not have a label, its name is returned.
     */
    CARAPI LoadLabel(
        /* [in] */ IPackageManager* pm,
        /* [out] */ ICharSequence** label);

    /**
     * Retrieve the current graphical icon associated with this resolution.  This
     * will call back on the given PackageManager to load the icon from
     * the application.
     *
     * @param pm A PackageManager from which the icon can be loaded; usually
     * the PackageManager from which you originally retrieved this item.
     *
     * @return Returns a Drawable containing the resolution's icon.  If the
     * item does not have an icon, the default activity icon is returned.
     */
    CARAPI LoadIcon(
        /* [in] */ IPackageManager* pm,
        /* [out] */ IDrawable** icon);

    /**
     * Return the icon resource identifier to use for this match.  If the
     * match defines an icon, that is used; else if the activity defines
     * an icon, that is used; else, the application icon is used.
     *
     * @return The icon associated with this match.
     */
    CARAPI GetIconResource(
        /* [out] */ Int32* iconRes);

    CARAPI Dump(
        /* [in] */ IPrinter* pw,
        /* [in] */ const String& prefix);

    CARAPI ToString(
        /* [out] */ String* star);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI GetActivityInfo(
        /* [out] */ IActivityInfo** activityInfo);

    CARAPI SetActivityInfo(
        /* [in] */ IActivityInfo* activityInfo);

    CARAPI GetServiceInfo(
        /* [out] */ IServiceInfo** serviceInfo);

    CARAPI SetServiceInfo(
        /* [in] */ IServiceInfo* serviceInfo);

    CARAPI GetProviderInfo(
        /* [out] */ IProviderInfo** providerInfo);

    CARAPI SetProviderInfo(
        /* [in] */ IProviderInfo* providerInfo);

    CARAPI GetFilter(
        /* [out] */IIntentFilter** filter);

    CARAPI SetFilter(
        /* [in] */ IIntentFilter* filter);

    CARAPI GetPriority(
        /* [out] */ Int32* priority);

    CARAPI SetPriority(
        /* [in] */ Int32 priority);

    CARAPI GetPreferredOrder(
        /* [out] */ Int32* preferredOrder);

    CARAPI SetPreferredOrder(
        /* [in] */ Int32 preferredOrder);

    CARAPI GetMatch(
        /* [out] */ Int32* match);

    CARAPI SetMatch(
        /* [in] */ Int32 match);

    CARAPI GetSpecificIndex(
        /* [out] */ Int32* index);

    CARAPI SetSpecificIndex(
        /* [in] */ Int32 index);

    CARAPI GetIsDefault(
        /* [out] */ Boolean* isDefault);

    CARAPI SetIsDefault(
        /* [in] */ Boolean isDefault);

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

    CARAPI GetResolvePackageName(
        /* [out] */ String* resolvePackageName);

    CARAPI SetResolvePackageName(
        /* [in] */ const String& resolvePackageName);

    CARAPI GetTargetComponentName(
        /* [out] */ IComponentName** targetComponentName);

    CARAPI SetTargetComponentName(
        /* [in] */ IComponentName* targetComponentName);

    CARAPI GetTargetUserId(
        /* [out] */ Int32* userId);

    CARAPI SetTargetUserId(
        /* [in] */ Int32 userId);

    CARAPI GetNoResourceId(
        /* [out] */ Boolean* id);

    CARAPI SetNoResourceId(
        /* [in] */ Boolean id);

    CARAPI GetSystem(
        /* [out] */ Boolean* sys);

    CARAPI SetSystem(
        /* [in] */ Boolean sys);

private:
    CARAPI GetComponentInfo(
        /* [out] */ IComponentInfo** info);

public:
    static const String TAG;// = "ResolveInfo";

    /**
     * The activity or broadcast receiver that corresponds to this resolution
     * match, if this resolution is for an activity or broadcast receiver.
     * Exactly one of {@link #activityInfo}, {@link #serviceInfo}, or
     * {@link #providerInfo} will be non-null.
     */
    AutoPtr<IActivityInfo> mActivityInfo;

    /**
     * The service that corresponds to this resolution match, if this resolution
     * is for a service. Exactly one of {@link #activityInfo},
     * {@link #serviceInfo}, or {@link #providerInfo} will be non-null.
     */
    AutoPtr<IServiceInfo> mServiceInfo;

    /**
     * The provider that corresponds to this resolution match, if this
     * resolution is for a provider. Exactly one of {@link #activityInfo},
     * {@link #serviceInfo}, or {@link #providerInfo} will be non-null.
     */
    AutoPtr<IProviderInfo> mProviderInfo;

    /**
     * The IntentFilter that was matched for this ResolveInfo.
     */
    AutoPtr<IIntentFilter> mFilter;

    /**
     * The declared priority of this match.  Comes from the "priority"
     * attribute or, if not set, defaults to 0.  Higher values are a higher
     * priority.
     */
    Int32 mPriority;

    /**
     * Order of result according to the user's preference.  If the user
     * has not set a preference for this result, the value is 0; higher
     * values are a higher priority.
     */
    Int32 mPreferredOrder;

    /**
     * The system's evaluation of how well the activity matches the
     * IntentFilter.  This is a match constant, a combination of
     * {@link IntentFilter#MATCH_CATEGORY_MASK IntentFilter.MATCH_CATEGORY_MASK}
     * and {@link IntentFilter#MATCH_ADJUSTMENT_MASK IntentFiler.MATCH_ADJUSTMENT_MASK}.
     */
    Int32 mMatch;

    /**
     * Only set when returned by
     * {@link PackageManager#queryIntentActivityOptions}, this tells you
     * which of the given specific intents this result came from.  0 is the
     * first in the list, < 0 means it came from the generic Intent query.
     */
    Int32 mSpecificIndex;

    /**
     * This filter has specified the Intent.CATEGORY_DEFAULT, meaning it
     * would like to be considered a default action that the user can
     * perform on this data.
     */
    Boolean mIsDefault;

    /**
     * A string resource identifier (in the package's resources) of this
     * match's label.  From the "label" attribute or, if not set, 0.
     */
    Int32 mLabelRes;

    /**
     * The actual string retrieve from <var>labelRes</var> or null if none
     * was provided.
     */
    AutoPtr<ICharSequence> mNonLocalizedLabel;

    /**
     * A drawable resource identifier (in the package's resources) of this
     * match's icon.  From the "icon" attribute or, if not set, 0.
     */
    Int32 mIcon;

    /**
     * Optional -- if non-null, the {@link #labelRes} and {@link #icon}
     * resources will be loaded from this package, rather than the one
     * containing the resolved component.
     */
    String mResolvePackageName;

    /**
     * Optional -- if non-null, the component name of the target of the original
     * intent. The ResolveInfo can change the component so this field stores
     * the origial target. This can be used for retargetting by the receiver.
     * @hide
     */
    AutoPtr<IComponentName> mTargetComponentName;

    /**
     * If not equal to UserHandle.USER_CURRENT, then the intent will be forwarded to this user.
     * @hide
     */
    Int32 mTargetUserId;

    /**
     * @hide
     */
    Boolean mNoResourceId;

    /**
     * @hide Target comes from system process?
     */
    Boolean mSystem;
};

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_CONTENT_PM_RESOLVEINFO_H__
