
#ifndef __ELASTOS_DROID_APP_CNOTIFICATIONGROUP_H__
#define __ELASTOS_DROID_APP_CNOTIFICATIONGROUP_H__

#include "Elastos.CoreLibrary.Utility.h"
#include "_Elastos_Droid_App_CNotificationGroup.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Core::Object;
using Elastos::Core::IStringBuilder;
using Elastos::Utility::ISet;
using Elastos::Utility::IUUID;
using Org::Xmlpull::V1::IXmlPullParser;

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CNotificationGroup)
    , public Object
    , public INotificationGroup
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CNotificationGroup();

    CARAPI constructor(
        /* [in] */ const String& name);

    CARAPI constructor(
        /* [in] */ const String& name,
        /* [in] */ Int32 nameResId,
        /* [in] */ IUUID* uuid);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI GetName(
        /* [out] */ String* name);

    CARAPI SetName(
        /* [in] */ const String& name);

    CARAPI GetUuid(
        /* [out] */ IUUID** uuid);

    CARAPI AddPackage(
        /* [in] */ const String& pkg);

    CARAPI GetPackages(
        /* [out, callee] */ ArrayOf<String>** packages);

    CARAPI RemovePackage(
        /* [in] */ const String& pkg);

    CARAPI HasPackage(
        /* [in] */ const String& pkg,
        /* [out] */ Boolean* result);

    CARAPI IsDirty(
        /* [out] */ Boolean* isDirty);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI GetXmlString(
        /* [in] */ IStringBuilder* builder,
        /* [in] */ IContext* context);

    static CARAPI FromXml(
        /* [in] */ IXmlPullParser* xpp,
        /* [in] */ IContext* context,
        /* [out] */ INotificationGroup** group);

private:
    static const String TAG;

    String mName;
    Int32 mNameResId;

    AutoPtr<IUUID> mUuid;

    AutoPtr<ISet> mPackages;

    Boolean mDirty;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_CNOTIFICATIONGROUP_H__
