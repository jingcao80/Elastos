

package com.android.settings.notification;

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::ITwoStatePreference;
using Elastos::Droid::Preference::Preference::IOnPreferenceChangeListener;
using Elastos::Droid::Provider::Settings::IGlobal;
using Elastos::Droid::Provider::Settings::ISystem;

using Elastos::Droid::Settings::ISettingsPreferenceFragment;

/** Helper to manage a two-state or dropdown preference bound to a global or system setting. */
public class SettingPref {
    public static const Int32 TYPE_GLOBAL = 1;
    public static const Int32 TYPE_SYSTEM = 2;

    protected final Int32 mType;
    private final String mKey;
    protected final String mSetting;
    protected final Int32 mDefault;
    private final Int32[] mValues;
    private final Uri mUri;

    protected TwoStatePreference mTwoState;
    protected DropDownPreference mDropDown;

    public SettingPref(Int32 type, String key, String setting, Int32 def, Int32... values) {
        mType = type;
        mKey = key;
        mSetting = setting;
        mDefault = def;
        mValues = values;
        mUri = GetUriFor(mType, mSetting);
    }

    public Boolean IsApplicable(Context context) {
        return TRUE;
    }

    protected String GetCaption(Resources res, Int32 value) {
        throw new UnsupportedOperationException();
    }

    public Preference Init(SettingsPreferenceFragment settings) {
        final Context context = settings->GetActivity();
        Preference p = settings->GetPreferenceScreen()->FindPreference(mKey);
        if (p != NULL && !IsApplicable(context)) {
            settings->GetPreferenceScreen()->RemovePreference(p);
            p = NULL;
        }
        if (p instanceof TwoStatePreference) {
            mTwoState = (TwoStatePreference) p;
        } else if (p instanceof DropDownPreference) {
            mDropDown = (DropDownPreference) p;
            for (Int32 value : mValues) {
                mDropDown->AddItem(GetCaption(context->GetResources(), value), value);
            }
        }
        Update(context);
        if (mTwoState != NULL) {
            p->SetOnPreferenceChangeListener(new OnPreferenceChangeListener() {
                //@Override
                public Boolean OnPreferenceChange(Preference preference, Object newValue) {
                    SetSetting(context, (Boolean) newValue ? 1 : 0);
                    return TRUE;
                }
            });
            return mTwoState;
        }
        if (mDropDown != NULL) {
            mDropDown->SetCallback(new DropDownPreference->Callback() {
                //@Override
                public Boolean OnItemSelected(Int32 pos, Object value) {
                    return SetSetting(context, (Integer) value);
                }
            });
            return mDropDown;
        }
        return NULL;
    }

    protected Boolean SetSetting(Context context, Int32 value) {
        return PutInt(mType, context->GetContentResolver(), mSetting, value);
    }

    public Uri GetUri() {
        return mUri;
    }

    public String GetKey() {
        return mKey;
    }

    CARAPI Update(Context context) {
        final Int32 val = GetInt(mType, context->GetContentResolver(), mSetting, mDefault);
        if (mTwoState != NULL) {
            mTwoState->SetChecked(val != 0);
        } else if (mDropDown != NULL) {
            mDropDown->SetSelectedValue(val);
        }
    }

    private static Uri GetUriFor(Int32 type, String setting) {
        Switch(type) {
            case TYPE_GLOBAL:
                return Global->GetUriFor(setting);
            case TYPE_SYSTEM:
                return System->GetUriFor(setting);
        }
        throw new IllegalArgumentException();
    }

    protected static Boolean PutInt(Int32 type, ContentResolver cr, String setting, Int32 value) {
        Switch(type) {
            case TYPE_GLOBAL:
                return Global->PutInt(cr, setting, value);
            case TYPE_SYSTEM:
                return System->PutInt(cr, setting, value);
        }
        throw new IllegalArgumentException();
    }

    protected static Int32 GetInt(Int32 type, ContentResolver cr, String setting, Int32 def) {
        Switch(type) {
            case TYPE_GLOBAL:
                return Global->GetInt(cr, setting, def);
            case TYPE_SYSTEM:
                return System->GetInt(cr, setting, def);
        }
        throw new IllegalArgumentException();
    }
}