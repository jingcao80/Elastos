#ifndef __ELASTOS_DROID_INTERNAL_POLICY_IMPL_POLICYCONTROL_H__
#define __ELASTOS_DROID_INTERNAL_POLICY_IMPL_POLICYCONTROL_H__

#include <Elastos.CoreLibrary.IO.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Utility::IArraySet;
using Elastos::Droid::View::IWindowManagerLayoutParams;
using Elastos::Droid::View::IWindowState;
using Elastos::IO::IPrintWriter;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Policy {
namespace Impl {

/**
  * Runtime adjustments applied to the global window policy.
  *
  * This includes forcing immersive mode behavior for one or both system bars (based on a package
  * list) and permanently disabling immersive mode confirmations for specific packages.
  *
  * Control by setting {@link Settings.Global.POLICY_CONTROL} to one or more name-value pairs.
  * e.g.
  *   to force immersive mode everywhere:
  *     "immersive.full=*"
  *   to force transient status for all apps except a specific package:
  *     "immersive.status=apps,-com.package"
  *   to disable the immersive mode confirmations for specific packages:
  *     "immersive.preconfirms=com.package.one,com.package.two"
  *
  * Separate multiple name-value pairs with ':'
  *   e.g. "immersive.status=apps:immersive.preconfirms=*"
  */
class PolicyControl
{
private:
    class Filter
        : public Object
    {
    public:
        virtual CARAPI Matches(
            /* [in] */ IWindowManagerLayoutParams* attrs,
            /* [out] */ Boolean* result);

        virtual CARAPI Matches(
            /* [in] */ const String& packageName,
            /* [out] */ Boolean* result);

        virtual CARAPI Dump(
            /* [in] */ IPrintWriter* pw);

        // @Override
        CARAPI_(String) ToString();

        // value = comma-delimited list of tokens, where token = (package name|apps|*)
        // e.g. "com.package1", or "apps, com.android.keyguard" or "*"
        static CARAPI_(AutoPtr<Filter>) Parse(
            /* [in] */ const String& value);

    private:
        Filter(
            /* [in] */ IArraySet* whitelist,
            /* [in] */ IArraySet* blacklist);

        CARAPI_(Boolean) OnBlacklist(
            /* [in] */ const String& packageName);

        CARAPI_(Boolean) OnWhitelist(
            /* [in] */ const String& packageName);

        CARAPI_(void) Dump(
            /* [in] */ const String& name,
            /* [in] */ IArraySet* set,//String
            /* [in] */ IPrintWriter* pw);

    private:
        static const String ALL;
        static const String APPS;
        AutoPtr<IArraySet> mWhitelist;
        AutoPtr<IArraySet> mBlacklist;
    };

public:

    static CARAPI GetSystemUiVisibility(
        /* [in] */ IWindowState* win,
        /* [in] */ IWindowManagerLayoutParams* attrs,
        /* [out] */ Int32* res);

    static CARAPI GetWindowFlags(
        /* [in] */ IWindowState* win,
        /* [in] */ IWindowManagerLayoutParams* attrs,
        /* [out] */ Int32* res);

    static CARAPI AdjustClearableFlags(
        /* [in] */ IWindowState* win,
        /* [in] */ Int32 clearableFlags,
        /* [out] */ Int32* res);

    static CARAPI DisableImmersiveConfirmation(
        /* [in] */ const String& pkg,
        /* [out] */ Boolean* res);

    static CARAPI ReloadFromSetting(
        /* [in] */ IContext* context);

    static CARAPI Dump(
        /* [in] */ const String& prefix,
        /* [in] */ IPrintWriter* pw);

private:
    static CARAPI_(void) Dump(
        /* [in] */ const String& name,
        /* [in] */ Filter* filter,
        /* [in] */ const String& prefix,
        /* [in] */ IPrintWriter* pw);

    static CARAPI_(void) SetFilters(
        /* [in] */ const String& value);

private:
    static String TAG;
    static Boolean DEBUG;
    static const String NAME_IMMERSIVE_FULL;
    static const String NAME_IMMERSIVE_STATUS;
    static const String NAME_IMMERSIVE_NAVIGATION;
    static const String NAME_IMMERSIVE_PRECONFIRMATIONS;
    static String sSettingValue;
    static AutoPtr<Filter> sImmersivePreconfirmationsFilter;
    static AutoPtr<Filter> sImmersiveStatusFilter;
    static AutoPtr<Filter> sImmersiveNavigationFilter;
};

} // namespace Impl
} // namespace Policy
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_POLICY_IMPL_POLICYCONTROL_H__

