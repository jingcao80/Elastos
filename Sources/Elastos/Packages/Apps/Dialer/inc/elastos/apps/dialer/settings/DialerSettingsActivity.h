#ifndef __ELASTOS_APPS_DIALER_SETTINGS_DEFAULTSETTINGSACTIVITY_H__
#define __ELASTOS_APPS_DIALER_SETTINGS_DEFAULTSETTINGSACTIVITY_H__

namespace Elastos{
namespace Apps{
namespace Dialer {
namespace Settings {

class DialerSettingsActivity
    : public AnalyticsPreferenceActivity
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
    AutoPtr<IHeaderAdapter> mHeaderAdapter;

    static const Int32 OWNER_HANDLE_ID; // = 0;
};

} // Settings
} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_SETTINGS_DEFAULTSETTINGSACTIVITY_H__