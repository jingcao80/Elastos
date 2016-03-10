
#ifndef __ELASTOS_DROID_APP_CRINGMODESETTINGS_H__
#define __ELASTOS_DROID_APP_CRINGMODESETTINGS_H__

#include "_Elastos_Droid_App_CRingModeSettings.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Core::Object;
using Elastos::Core::IStringBuilder;
using Org::Xmlpull::V1::IXmlPullParser;

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CRingModeSettings)
    , public Object
    , public IRingModeSettings
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CRingModeSettings();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ const String& value,
        /* [in] */ Boolean override);

    CARAPI GetValue(
        /* [out] */ String* value);

    CARAPI SetValue(
        /* [in] */ const String& value);

    CARAPI SetOverride(
        /* [in] */ Boolean override);

    CARAPI IsOverride(
        /* [out] */ Boolean* override);

    /** @hide */
    CARAPI IsDirty(
        /* [out] */ Boolean* dirty);

    CARAPI ProcessOverride(
        /* [in] */ IContext* context);

    static CARAPI FromXml(
        /* [in] */ IXmlPullParser* xpp,
        /* [in] */ IContext* context,
        /* [out] */ IRingModeSettings** obj);

    /** @hide */
    CARAPI GetXmlString(
        /* [in] */ IStringBuilder* builder,
        /* [in] */ IContext* context);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

private:
    static const String RING_MODE_NORMAL;
    static const String RING_MODE_VIBRATE;
    static const String RING_MODE_MUTE;

    String mValue;
    Boolean mOverride;
    Boolean mDirty;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_CRINGMODESETTINGS_H__
