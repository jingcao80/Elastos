#ifndef __ELASTOS_DROID_PROFILE_H__
#define __ELASTOS_DROID_PROFILE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"
#include <Elastos.CoreLibrary.h>
#include <Elastos.Droid.App.h>
#include <Elastos.CoreLibrary.External.h>
#include <Elastos.CoreLibrary.Utility.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::App::IAirplaneModeSettings;
using Elastos::Droid::App::IBrightnessSettings;
using Elastos::Droid::App::IProfileGroup;
using Elastos::Droid::App::IRingModeSettings;
using Elastos::Droid::App::IStreamSettings;
using Elastos::Core::IStringBuilder;
using Elastos::Core::IComparable;
using Elastos::Utility::IArrayList;
using Elastos::Utility::ICollection;
using Elastos::Utility::IHashMap;
using Elastos::Utility::IList;
using Elastos::Utility::IUUID;

using Org::Xmlpull::V1::IXmlPullParser;

namespace Elastos {
namespace Droid {
namespace App {

class Profile
    : public Object
    , public IProfile
    , public IParcelable
    , public IComparable
{
public:
    class ProfileTrigger
        : public Object
        , public IProfileTrigger
        , public IParcelable
    {
    public:
        CAR_INTERFACE_DECL()

        ProfileTrigger();

        virtual ~ProfileTrigger();

        CARAPI constructor(
            /* [in] */ Int32 type,
            /* [in] */ const String& id,
            /* [in] */ Int32 state,
            /* [in] */ const String& name);

        CARAPI GetType(
            /* [out] */ Int32* type);

        CARAPI GetName(
            /* [out] */ String* name);

        CARAPI GetId(
            /* [out] */ String* id);

        CARAPI GetState(
            /* [out] */ Int32* state);

        CARAPI GetXmlString(
            /* [in] */ IStringBuilder* builder,
            /* [in] */ IContext* context);

        CARAPI ReadFromParcel(
            /* [in] */ IParcel* source);

        CARAPI WriteToParcel(
            /* [in] */ IParcel* dest);

        CARAPI ToString(
            /* [out] */ String* str);

    public:
        static CARAPI_(AutoPtr<IProfileTrigger>) FromXml(
            /* [in] */ IXmlPullParser* xpp,
            /* [in] */ IContext* context);

    private:
        static CARAPI_(String) GetIdType(
            /* [in] */ Int32 type);

    private:
        friend class Profile;
        Int32 mType;
        String mId;
        String mName;
        Int32 mState;
    };

    CAR_INTERFACE_DECL()

    Profile();

    virtual ~Profile();

    CARAPI constructor(
        /* [in] */ const String& name);

    CARAPI constructor(
        /* [in] */ const String& name,
        /* [in] */ Int32 nameResId,
        /* [in] */ IUUID* uuid);

    CARAPI GetTrigger(
        /* [in] */ Int32 type,
        /* [in] */ const String& id,
        /* [out] */ Int32* triggerState);

    CARAPI GetTriggersFromType(
        /* [in] */ Int32 type,
        /* [out] */ IArrayList** triggers);

    CARAPI SetTrigger(
        /* [in] */ Int32 type,
        /* [in] */ const String& id,
        /* [in] */ Int32 state,
        /* [in] */ const String& name);

    CARAPI AddProfileGroup(
        /* [in] */ IProfileGroup* value);

    CARAPI RemoveProfileGroup(
        /* [in] */ IUUID* uuid);

    CARAPI GetProfileGroups(
        /* [out, callee] */ ArrayOf<IProfileGroup*>** groups);

    CARAPI GetProfileGroup(
        /* [in] */ IUUID* uuid,
        /* [out] */ IProfileGroup** group);

    CARAPI GetDefaultGroup(
        /* [out] */ IProfileGroup** group);

    CARAPI GetName(
        /* [out] */ String* name);

    CARAPI SetName(
        /* [in] */ const String& name);

    CARAPI GetProfileType(
        /* [out] */ Int32* type);

    CARAPI SetProfileType(
        /* [in] */ Int32 type);

    CARAPI GetUuid(
        /* [out] */ IUUID** uuid);

    CARAPI GetSecondaryUuids(
        /* [out, callee] */ ArrayOf<IUUID*>** uuids);

    CARAPI SetSecondaryUuids(
        /* [in] */ IList* uuids);

    CARAPI AddSecondaryUuid(
        /* [in] */ IUUID* uuid);

    CARAPI GetStatusBarIndicator(
        /* [out] */ Boolean* result);

    CARAPI SetStatusBarIndicator(
        /* [in] */ Boolean newStatusBarIndicator);

    CARAPI IsConditionalType(
        /* [out] */ Boolean* result);

    CARAPI SetConditionalType();

    CARAPI GetRingMode(
        /* [out] */ IRingModeSettings** descriptor);

    CARAPI SetRingMode(
        /* [in] */ IRingModeSettings* descriptor);

    CARAPI GetScreenLockMode(
        /* [out] */ Int32* screenLockMode);

    CARAPI SetScreenLockMode(
        /* [in] */ Int32 screenLockMode);

    CARAPI GetExpandedDesktopMode(
        /* [out] */ Int32* expandedDesktopMode);

    CARAPI SetExpandedDesktopMode(
        /* [in] */ Int32 expandedDesktopMode);

    CARAPI GetAirplaneMode(
        /* [out] */ IAirplaneModeSettings** descriptor);

    CARAPI SetAirplaneMode(
        /* [in] */ IAirplaneModeSettings* descriptor);

    CARAPI GetBrightness(
        /* [out] */ IBrightnessSettings** descriptor);

    CARAPI SetBrightness(
        /* [in] */ IBrightnessSettings* descriptor);

    CARAPI IsDirty(
        /* [out] */ Boolean* result);

    CARAPI GetXmlString(
        /* [in] */ IStringBuilder* builder,
        /* [in] */ IContext* context);

    CARAPI ValidateRingtones(
        /* [in] */ IContext* context);

    CARAPI DoSelect(
        /* [in] */ IContext* context);

    CARAPI GetSettingsForStream(
        /* [in] */ Int32 streamId,
        /* [out] */ IStreamSettings** descriptor);

    CARAPI SetStreamSettings(
        /* [in] */ IStreamSettings* descriptor);

    CARAPI GetStreamSettings(
        /* [out] */ ICollection** descriptors);

    CARAPI GetSettingsForConnection(
        /* [in] */ Int32 connectionId,
        /* [out] */ IConnectionSettings** descriptor);

    CARAPI SetConnectionSettings(
        /* [in] */ IConnectionSettings* descriptor);

    CARAPI GetConnectionSettings(
        /* [out] */ ICollection** descriptors);

    CARAPI CompareTo(
        /* [in] */ IInterface* obj,
        /* [out] */ Int32* result);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ToString(
        /* [out] */ String* str);

public:
    static CARAPI_(AutoPtr<IProfile>) FromXml(
        /* [in] */ IXmlPullParser* xpp,
        /* [in] */ IContext* context);

private:
    static CARAPI_(AutoPtr<IList>) ReadSecondaryUuidsFromXml(
        /* [in] */ IXmlPullParser* xpp,
        /* [in] */ IContext* context);

    static CARAPI_(ECode) ReadTriggersFromXml(
        /* [in] */ IXmlPullParser* xpp,
        /* [in] */ IContext* context,
        /* [in] */ IProfile* profile);

private:
    static const String TAG; // = "Profile";

    static const Int32 CONDITIONAL_TYPE;

    static const Int32 TOGGLE_TYPE;

    String mName;

    Int32 mNameResId;

    AutoPtr<IUUID> mUuid;

    AutoPtr<IArrayList> mSecondaryUuids;

    AutoPtr<IHashMap> mProfileGroups;

    AutoPtr<IProfileGroup> mDefaultGroup;

    Boolean mStatusBarIndicator;

    Boolean mDirty;

    Int32 mProfileType;

    AutoPtr<IHashMap> mStreams;

    AutoPtr<IHashMap> mTriggers;

    AutoPtr<IHashMap> mConnections;

    AutoPtr<IRingModeSettings> mRingMode;

    AutoPtr<IAirplaneModeSettings> mAirplaneMode;

    AutoPtr<IBrightnessSettings> mBrightness;

    Int32 mScreenLockMode;

    Int32 mExpandedDesktopMode;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_PROFILE_H__
