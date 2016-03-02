
#ifndef __ELASTOS_DROID_APP_CAIRPLANEMODESETTINGS_H__
#define __ELASTOS_DROID_APP_CAIRPLANEMODESETTINGS_H__

#include "_Elastos_Droid_App_CAirplaneModeSettings.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Core::Object;
using Elastos::Core::IStringBuilder;
using Org::Xmlpull::V1::IXmlPullParser;

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CAirplaneModeSettings)
    , public Object
    , public IAirplaneModeSettings
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 value,
        /* [in] */ Boolean override);

    CARAPI GetValue(
        /* [out] */ Int32* value);

    CARAPI SetValue(
        /* [in] */ Int32 value);

    CARAPI SetOverride(
        /* [in] */ Boolean override);

    CARAPI IsOverride(
        /* [out] */ Boolean* override);

    CARAPI IsDirty(
        /* [out] */ Boolean* dirty);

    CARAPI ProcessOverride(
        /* [in] */ IContext* context);

    static CARAPI FromXml(
        /* [in] */ IXmlPullParser* xpp,
        /* [in] */ IContext* context,
        /* [out] */ IAirplaneModeSettings** obj);

    CARAPI GetXmlString(
        /* [in] */ IStringBuilder* builder,
        /* [in] */ IContext* context);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

private:
    Int32 mValue;
    Boolean mOverride;
    Boolean mDirty;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_CAIRPLANEMODESETTINGS_H__
