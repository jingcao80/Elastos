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

#ifndef  __ELASTOS_DROID_LAUNCHER2_ICONCACHE_H__
#define  __ELASTOS_DROID_LAUNCHER2_ICONCACHE_H__

#include "_Elastos.Droid.Launcher2.h"
#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.CoreLibrary.Core.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::ILauncherActivityInfo;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Core::Object;
using Elastos::Core::ICharSequence;
using Elastos::Utility::IHashMap;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

/**
 * Cache of application icons.  Icons can be made from any thread.
 */
class IconCache
    : public Object
    , public IIconCache
{
private:
    class CacheEntry
        : public Object
    {
    public:
        AutoPtr<IBitmap> mIcon;
        String mTitle;
        AutoPtr<ICharSequence> mContentDescription;
    };

    class CacheKey
        : public Object
    {
    public:
        CacheKey(
            /* [in] */ IComponentName* componentName,
            /* [in] */ IUserHandle* user);

        //@Override
        CARAPI GetHashCode(
            /* [out] */ Int32 *hashCode);

        //@Override
        CARAPI Equals(
            /* [in] */ IInterface *obj,
            /* [out] */ Boolean *equal);

    public:
        AutoPtr<IComponentName> mComponentName;
        AutoPtr<IUserHandle> mUser;
    };

public:
    CAR_INTERFACE_DECL();

    IconCache();

    CARAPI constructor(
        /* [in] */ ILauncherApplication* context);

    CARAPI_(AutoPtr<IDrawable>) GetFullResDefaultActivityIcon();

    CARAPI_(AutoPtr<IDrawable>) GetFullResIcon(
        /* [in] */ IResources* resources,
        /* [in] */ Int32 iconId,
        /* [in] */ IUserHandle* user);

    CARAPI_(AutoPtr<IDrawable>) GetFullResIcon(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 iconId,
        /* [in] */ IUserHandle* user);

    CARAPI_(AutoPtr<IDrawable>) GetFullResIcon(
        /* [in] */ IResolveInfo* info,
        /* [in] */ IUserHandle* user);

    CARAPI_(AutoPtr<IDrawable>) GetFullResIcon(
        /* [in] */ IActivityInfo* info,
        /* [in] */ IUserHandle* user);

    /**
     * Remove any records for the supplied ComponentName.
     */
    CARAPI_(void) Remove(
        /* [in] */ IComponentName* componentName);

    /**
     * Empty out the cache.
     */
    CARAPI_(void) Flush();

    /**
     * Fill in "application" with the icon and label for "info."
     */
    CARAPI_(void) GetTitleAndIcon(
        /* [in] */ IApplicationInfo* application,
        /* [in] */ ILauncherActivityInfo* info,
        /* [in] */ IHashMap* labelCache);

    CARAPI_(AutoPtr<IBitmap>) GetIcon(
        /* [in] */ IIntent* intent,
        /* [in] */ IUserHandle* user);

    CARAPI_(AutoPtr<IBitmap>) GetIcon(
        /* [in] */ IComponentName* component,
        /* [in] */ ILauncherActivityInfo* info,
        /* [in] */ IHashMap* labelCache);

    CARAPI_(Boolean) IsDefaultIcon(
        /* [in] */ IBitmap* icon);

private:
    CARAPI_(AutoPtr<IBitmap>) MakeDefaultIcon();

    CARAPI_(AutoPtr<CacheEntry>) CacheLocked(
        /* [in] */ IComponentName* componentName,
        /* [in] */ ILauncherActivityInfo* info,
        /* [in] */ IHashMap* labelCache,
        /* [in] */ IUserHandle* user);


private:
    //@SuppressWarnings("unused")
    static const String TAG;

    static const Int32 INITIAL_ICON_CACHE_CAPACITY;

    AutoPtr<IBitmap> mDefaultIcon;
    AutoPtr<ILauncherApplication> mContext;
    AutoPtr<IPackageManager> mPackageManager;
    AutoPtr<IHashMap> mCache;
    Int32 mIconDpi;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_ICONCACHE_H__