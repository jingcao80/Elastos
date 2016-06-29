#ifndef __ELASTOS_APPS_DIALER_SETTINGS_DEFAULTSETTINGSACTIVITY_H__
#define __ELASTOS_APPS_DIALER_SETTINGS_DEFAULTSETTINGSACTIVITY_H__

#include "_Elastos.Apps.Dialer.h"
#include "elastos/droid/app/Activity.h"
#include "elastos/droid/widget/ArrayAdapter.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "Elastos.CoreLibrary.Utility.h"

using Elastos::Droid::App::Activity;
using Elastos::Droid::App::ISharedPreferences;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::ArrayAdapter;
using Elastos::Droid::Widget::IListAdapter;
using Elastos::Droid::Widget::ITextView;
using Elastos::Utility::IList;

namespace Elastos{
namespace Apps{
namespace Dialer {
namespace Settings {

class DialerSettingsActivity
    // TODO:
    /*: public AnalyticsPreferenceActivity*/
    : public Activity
    , public IDialerSettingsActivity
{
private:
    /**
     * This custom {@code ArrayAdapter} is mostly identical to the equivalent one in
     * {@code PreferenceActivity}, except with a local layout resource.
     */
    class HeaderAdapter
        : public ArrayAdapter
    {
    public:
        class HeaderViewHolder
            : public Object
        {
        public:
            AutoPtr<ITextView> mTitle;
            AutoPtr<ITextView> summary;
        };

    public:
        CARAPI constructor(
            /* [in] */ IContext* context,
            /* [in] */ IList* objects);

        // @Override
        CARAPI GetView(
            /* [in] */ Int32 position,
            /* [in] */ IView* convertView,
            /* [in] */ IViewGroup* parent,
            /* [out] */ IView** view);

    private:
        AutoPtr<ILayoutInflater> mInflater;
    };

public:
    CAR_INTERFACE_DECL()

    // @Override
    CARAPI OnBuildHeaders(
        /* [in] */ IList* target);

    // @Override
    CARAPI OnOptionsItemSelected(
        /* [in] */ IMenuItem* item,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI SetListAdapter(
        /* [in] */ IListAdapter* adapter);
protected:
    // @Override
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    // @Override
    CARAPI IsValidFragment(
        /* [in] */ const String& fragmentName,
        /* [out] */ Boolean* result);

private:
    /**
     * Whether a user handle associated with the current user is that of the primary owner. That is,
     * whether there is a user handle which has an id which matches the owner's handle.
     * @return Whether the current user is the primary user.
     */
    CARAPI_(Boolean) IsPrimaryUser();

protected:
    AutoPtr<ISharedPreferences> mPreferences;

private:
    AutoPtr<HeaderAdapter> mHeaderAdapter;

    static const Int32 OWNER_HANDLE_ID; // = 0;
};

} // Settings
} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_SETTINGS_DEFAULTSETTINGSACTIVITY_H__