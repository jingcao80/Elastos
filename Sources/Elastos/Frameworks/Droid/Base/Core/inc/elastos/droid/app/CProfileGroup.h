
#ifndef __ELASTOS_DROID_APP_CPROFILEGROUP_H__
#define __ELASTOS_DROID_APP_CPROFILEGROUP_H__

#include "Elastos.CoreLibrary.Utility.h"
#include "_Elastos.Droid.Media.h"
#include "_Elastos.Droid.Net.h"
#include "_Elastos_Droid_App_CProfileGroup.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Media::IRingtoneManager;
using Elastos::Droid::Net::IUri;
using Elastos::Core::Object;
using Elastos::Core::IStringBuilder;
using Elastos::Utility::IUUID;
using Elastos::Utility::IUUIDHelper;
using Org::Xmlpull::V1::IXmlPullParser;

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CProfileGroup)
    , public Object
    , public IProfileGroup
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CProfileGroup();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IUUID* uuid,
        /* [in] */ Boolean defaultGroup);

    CARAPI constructor(
        /* [in] */ const String& name,
        /* [in] */ IUUID* uuid,
        /* [in] */ Boolean defaultGroup);

    /** @hide */
    CARAPI Matches(
        /* [in] */ INotificationGroup* group,
        /* [in] */ Boolean defaultGroup,
        /* [out] */ Boolean* result);

    CARAPI GetUuid(
        /* [out] */ IUUID** uuid);

    CARAPI IsDefaultGroup(
        /* [out] */ Boolean* result);

    /** @hide */
    CARAPI IsDirty(
        /* [out] */ Boolean* dirty);

    /** @hide */
    CARAPI SetSoundOverride(
        /* [in] */ IUri* sound);

    CARAPI GetSoundOverride(
        /* [out] */ IUri** sound);

    /** @hide */
    CARAPI SetRingerOverride(
        /* [in] */ IUri* ringer);

    CARAPI GetRingerOverride(
        /* [out] */ IUri** ringer);

    /** @hide */
    CARAPI SetSoundMode(
        /* [in] */ ProfileGroupMode soundMode);

    CARAPI GetSoundMode(
        /* [out] */ ProfileGroupMode* soundMode);

    /** @hide */
    CARAPI SetRingerMode(
        /* [in] */ ProfileGroupMode ringerMode);

    CARAPI GetRingerMode(
        /* [out] */ ProfileGroupMode* ringerMode);

    /** @hide */
    CARAPI SetVibrateMode(
        /* [in] */ ProfileGroupMode vibrateMode);

    CARAPI GetVibrateMode(
        /* [out] */ ProfileGroupMode* vibrateMode);

    /** @hide */
    CARAPI SetLightsMode(
        /* [in] */ ProfileGroupMode lightsMode);

    CARAPI GetLightsMode(
        /* [out] */ ProfileGroupMode* lightsMode);

    /** @hide */
    CARAPI ApplyOverridesToNotification(
        /* [in] */ INotification* notification);

    CARAPI_(void) ValidateOverrideUris(
        /* [in] */ IContext* context);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    /** @hide */
    CARAPI GetXmlString(
        /* [in] */ IStringBuilder* builder,
        /* [in] */ IContext* context);

    static CARAPI FromXml(
        /* [in] */ IXmlPullParser* xpp,
        /* [in] */ IContext* context,
        /* [out] */ IProfileGroup** group);

private:
    CARAPI_(Boolean) ValidateOverrideUri(
        /* [in] */ IContext* context,
        /* [in] */ IUri* uri);

private:
    static const String TAG;

    String mName;
    Int32 mNameResId;

    AutoPtr<IUUID> mUuid;

    AutoPtr<IUri> mSoundOverride;
    AutoPtr<IUri> mRingerOverride;

    ProfileGroupMode mSoundMode;
    ProfileGroupMode mRingerMode;
    ProfileGroupMode mVibrateMode;
    ProfileGroupMode mLightsMode;

    Boolean mDefaultGroup;
    Boolean mDirty;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_CPROFILEGROUP_H__
