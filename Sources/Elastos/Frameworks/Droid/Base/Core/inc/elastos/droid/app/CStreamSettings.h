
#ifndef __ELASTOS_DROID_APP_CSTREAMSETTINGS_H__
#define __ELASTOS_DROID_APP_CSTREAMSETTINGS_H__

#include "_Elastos_Droid_App_CStreamSettings.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Core::Object;
using Elastos::Core::IStringBuilder;
using Org::Xmlpull::V1::IXmlPullParser;

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CStreamSettings)
    , public Object
    , public IStreamSettings
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CStreamSettings();

    CARAPI constructor(
        /* [in] */ Int32 streamId);

    CARAPI constructor(
        /* [in] */ Int32 streamId,
        /* [in] */ Int32 value,
        /* [in] */ Boolean override);

    CARAPI GetStreamId(
        /* [out] */ Int32* streamId);

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

    static CARAPI FromXml(
        /* [in] */ IXmlPullParser* xpp,
        /* [in] */ IContext* context,
        /* [out] */ IStreamSettings** obj);

    CARAPI GetXmlString(
        /* [in] */ IStringBuilder* builder,
        /* [in] */ IContext* context);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

private:
    Int32 mStreamId;
    Int32 mValue;
    Boolean mOverride;
    Boolean mDirty;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_CSTREAMSETTINGS_H__
