#ifndef __ELASTOS_DROID_SETTINGS_NOTIFICATION_SETTINGPREF_H__
#define __ELASTOS_DROID_SETTINGS_NOTIFICATION_SETTINGPREF_H__

#include "Elastos.Droid.Net.h"
#include "elastos/droid/settings/SettingsPreferenceFragment.h"

using Elastos::Droid::Settings::SettingsPreferenceFragment;

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::ITwoStatePreference;
using Elastos::Droid::Preference::IPreferenceOnPreferenceChangeListener;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Notification {

/** Helper to manage a two-state or dropdown preference bound to a global or system setting. */
class SettingPref
    : public Object
    , public ISettingPref
{
private:
    class PreferenceOnPreferenceChangeListener
        : public Object
        , public IPreferenceOnPreferenceChangeListener
    {
    public:
        TO_STRING_IMPL("SettingPref::PreferenceOnPreferenceChangeListener")

        CAR_INTERFACE_DECL()

        PreferenceOnPreferenceChangeListener(
            /* [in] */ SettingPref* host,
            /* [in] */ IContext* context);

        //@Override
        CARAPI OnPreferenceChange(
            /* [in] */ IPreference* preference,
            /* [in] */ IInterface* newValue,
            /* [out] */ Boolean* result);

    private:
        SettingPref* mHost;
        AutoPtr<IContext> mContext;
    };

    class DropDownPreferenceCallback
        : public Object
        , public IDropDownPreferenceCallback
    {
    public:
        TO_STRING_IMPL("SettingPref::DropDownPreferenceCallback")

        CAR_INTERFACE_DECL()

        DropDownPreferenceCallback(
            /* [in] */ SettingPref* host,
            /* [in] */ IContext* context);

        //@Override
        CARAPI OnItemSelected(
            /* [in] */ Int32 pos,
            /* [in] */ IInterface* value,
            /* [out] */ Boolean* result);

    private:
        SettingPref* mHost;
        AutoPtr<IContext> mContext;
    };

public:
    TO_STRING_IMPL("SettingPref")

    CAR_INTERFACE_DECL()

    SettingPref(
        /* [in] */ Int32 type,
        /* [in] */ const String& key,
        /* [in] */ const String& setting,
        /* [in] */ Int32 def,
        /* [in] */ ArrayOf<Int32>* values);

    virtual CARAPI_(Boolean) IsApplicable(
        /* [in] */ IContext* context);

    virtual CARAPI_(AutoPtr<IPreference>) Init(
        /* [in] */ SettingsPreferenceFragment* settings);

    virtual CARAPI_(AutoPtr<IUri>) GetUri();

    virtual CARAPI_(String) GetKey();

    virtual CARAPI Update(
        /* [in] */ IContext* context);

protected:
    friend class CZenModeSettings;
    virtual CARAPI GetCaption(
        /* [in] */ IResources* res,
        /* [in] */ Int32 value,
        /* [out] */ String* result);

    virtual CARAPI SetSetting(
        /* [in] */ IContext* context,
        /* [in] */ Int32 value,
        /* [out] */ Boolean* result);

    static CARAPI PutInt32(
        /* [in] */ Int32 type,
        /* [in] */ IContentResolver* cr,
        /* [in] */ const String& setting,
        /* [in] */ Int32 value,
        /* [out] */ Boolean* result);

    static CARAPI GetInt32(
        /* [in] */ Int32 type,
        /* [in] */ IContentResolver* cr,
        /* [in] */ const String& setting,
        /* [in] */ Int32 def,
        /* [out] */ Int32* result);

private:
    static GetUriFor(
        /* [in] */ Int32 type,
        /* [in] */ const String& setting,
        /* [out] */ IUri** result);

public:
    static const Int32 TYPE_GLOBAL = 1;
    static const Int32 TYPE_SYSTEM = 2;

protected:
    Int32 mType;
    String mSetting;
    Int32 mDefault;

    AutoPtr<ITwoStatePreference> mTwoState;
    AutoPtr<IDropDownPreference> mDropDown;

private:
    String mKey;
    AutoPtr< ArrayOf<Int32> > mValues;
    AutoPtr<IUri> mUri;
};

} // namespace Notification
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_NOTIFICATION_SETTINGPREF_H__