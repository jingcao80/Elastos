
#ifndef __ELASTOS_DROID_CONTENT_PM_CUSERINFO_H__
#define __ELASTOS_DROID_CONTENT_PM_CUSERINFO_H__

#include "_Elastos_Droid_Content_Pm_CUserInfo.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IUserHandle;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

/**
 * Per-user information.
 * @hide
 */
CarClass(CUserInfo)
    , public Object
    , public IUserInfo
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CUserInfo();

    virtual ~CUserInfo();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IUserInfo* orig);

    CARAPI constructor(
        /* [in] */ Int32 id,
        /* [in] */ const String& name,
        /* [in] */ Int32 flags);

    CARAPI constructor(
        /* [in] */ Int32 id,
        /* [in] */ const String& name,
        /* [in] */ const String& iconPath,
        /* [in] */ Int32 flags);

    CARAPI IsPrimary(
        /* [out] */ Boolean* isPrimary);

    CARAPI IsAdmin(
        /* [out] */ Boolean* isAdmin);

    CARAPI IsGuest(
        /* [out] */ Boolean* isGuest);

    CARAPI IsRestricted(
        /* [out] */ Boolean* result);

    CARAPI IsManagedProfile(
        /* [out] */ Boolean* result);

    CARAPI IsEnabled(
        /* [out] */ Boolean* result);

    CARAPI SupportsSwitchTo(
        /* [out] */ Boolean* result);

    CARAPI GetUserHandle(
        /* [out] */ IUserHandle** userHandle);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI GetId(
        /* [out] */ Int32* id);

    CARAPI SetId(
        /* [in] */ Int32 id);

    CARAPI GetSerialNumber(
        /* [out] */ Int32* serialNumber);

    CARAPI SetSerialNumber(
        /* [in] */ Int32 serialNumber);

    CARAPI GetName(
        /* [out] */ String* name);

    CARAPI SetName(
        /* [in] */ const String& name);

    CARAPI GetIconPath(
        /* [out] */ String* iconPath);

    CARAPI SetIconPath(
        /* [in] */ const String& iconPath);

    CARAPI GetFlags(
        /* [out] */ Int32* flags);

    CARAPI SetFlags(
        /* [in] */ Int32 flags);

    CARAPI GetCreationTime(
        /* [out] */ Int64* creationTime);

    CARAPI SetCreationTime(
        /* [in] */ Int64 creationTime);

    CARAPI GetLastLoggedInTime(
        /* [out] */ Int64* lastLoggedInTime);

    CARAPI SetLastLoggedInTime(
        /* [in] */ Int64 lastLoggedInTime);

    CARAPI GetPartial(
        /* [out] */ Boolean* partial);

    CARAPI SetPartial(
        /* [in] */ Boolean partial);

public:
    Int32 mId;
    Int32 mSerialNumber;
    String mName;
    String mIconPath;
    Int32 mFlags;
    Int64 mCreationTime;
    Int64 mLastLoggedInTime;
    Int32 mProfileGroupId;

    /** User is only partially created. */
    Boolean mPartial;
    Boolean mGuestToRemove;
};

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_CONTENT_PM_CUSERINFO_H__
