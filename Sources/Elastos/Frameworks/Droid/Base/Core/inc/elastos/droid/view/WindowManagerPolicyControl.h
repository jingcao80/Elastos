#ifndef __ELASTOS_DROID_VIEW_WINDOWMANAGERPOLICYCONTROL_H__
#define __ELASTOS_DROID_VIEW_WINDOWMANAGERPOLICYCONTROL_H__

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Utility.h"
#include <Elastos.CoreLibrary.IO.h>
#include <elastos/core/Object.h>
#include <elastos/core/StringBuilder.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Utility::IArraySet;
using Elastos::IO::IPrintWriter;
using Elastos::Core::StringBuilder;
using Elastos::Core::Object;


namespace Elastos {
namespace Droid {
namespace View {

class WindowManagerPolicyControl
{
public:
    class ImmersiveDefaultStyles {
    public:
        static const Int32 IMMERSIVE_FULL = 0;
        static const Int32 IMMERSIVE_STATUS = 1;
        static const Int32 IMMERSIVE_NAVIGATION = 2;
    };

private:
    class Filter
        : public Object
    {
        friend class WindowManagerPolicyControl;
        public:
            CARAPI_(Boolean) Matches(
                /* [in] */ IWindowManagerLayoutParams* attrs);

            CARAPI_(Boolean) Matches(
                /* [in] */ const String& packageName);

            CARAPI_(Boolean) IsEnabledForAll();

            CARAPI_(Boolean) OnBlacklist(
                /* [in] */ const String& packageName);

            CARAPI_(Boolean) OnWhitelist(
                /* [in] */ const String& packageName);

            CARAPI ToString(
                /* [out] */ String* str);

            static CARAPI_(AutoPtr<Filter>) Parse(
                /* [in] */ const String&  value);

            virtual CARAPI Dump(
                /* [in] */ IPrintWriter* pw);

        private:
            Filter(
                /* [in] */ IArraySet* whitelist,
                /* [in] */ IArraySet* blacklist);

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
    static CARAPI_(Int32) GetSystemUiVisibility(
        /* [in] */ IWindowState* win,
        /* [in] */ IWindowManagerLayoutParams* attrs);

    static CARAPI_(Int32) GetWindowFlags(
        /* [in] */ IWindowState* win,
        /* [in] */ IWindowManagerLayoutParams* attrs);

    static CARAPI_(Int32) GetPrivateWindowFlags(
        /* [in] */ IWindowState* win,
        /* [in] */ IWindowManagerLayoutParams* attrs);

    static CARAPI_(Boolean) ImmersiveStatusFilterMatches(
        /* [in] */ const String& packageName);

    static CARAPI_(Boolean) ImmersiveNavigationFilterMatches(
        /* [in] */ const String& packageName);

    static CARAPI_(Int32) AdjustClearableFlags(
        /* [in] */ IWindowState* win,
        /* [in] */ Int32 clearableFlags);

    static CARAPI_(Boolean) DisableImmersiveConfirmation(
        /* [in] */ const String& pkg);

    static CARAPI_(void) ReloadFromSetting(
        /* [in] */ IContext* context);

    static CARAPI_(void) ReloadFromSetting(
        /* [in] */ IContext* context,
        /* [in] */ const String& key);

    static CARAPI_(void) ReloadStyleFromSetting(
        /* [in] */ IContext* context,
        /* [in] */ const String& key);

    static CARAPI_(void) SaveToSettings(
        /* [in] */ IContext* context);

    static CARAPI_(void) SaveToSettings(
        /* [in] */ IContext* context,
        /* [in] */ const String& key);

    static CARAPI_(void) SaveStyleToSettings(
        /* [in] */ IContext* context,
        /* [in] */ Int32 value);

    static CARAPI_(void) AddToStatusWhiteList(
        /* [in] */ const String& packageName);

    static CARAPI_(void) AddToNavigationWhiteList(
        /* [in] */ const String& packageName);

    static CARAPI_(void) RemoveFromWhiteLists(
        /* [in] */ const String& packageName);

    static CARAPI_(AutoPtr<IArraySet>) GetWhiteLists();

    static CARAPI_(Boolean) IsImmersiveFiltersActive();

private:
    static CARAPI_(void) WriteFilter(
        /* [in] */ const String& name,
        /* [in] */ Filter* filter,
        /* [in] */ StringBuilder& stringBuilder);

    static CARAPI_(void) WritePackages(
        /* [in] */ IArraySet* set,
        /* [in] */ StringBuilder& stringBuilder,
        /* [in] */ Boolean isBlackList);

    static CARAPI_(void) SetFilters(
        /* [in] */ const String& value);

private:
    static const String _TAG;
    static Boolean DEBUG;

    static const String NAME_IMMERSIVE_FULL;
    static const String NAME_IMMERSIVE_STATUS;
    static const String NAME_IMMERSIVE_NAVIGATION;
    static const String NAME_IMMERSIVE_PRECONFIRMATIONS;

    static Int32 sDefaultImmersiveStyle;
    static String sSettingValue;
    static AutoPtr<Filter> sImmersivePreconfirmationsFilter;
    static AutoPtr<Filter> sImmersiveStatusFilter;
    static AutoPtr<Filter> sImmersiveNavigationFilter;
};

} // namespace View
} // namespace Droid
} // namespace Elastos
#endif