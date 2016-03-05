
#ifndef __ELASTOS_DROID_APP_CCONNECTIONSETTINGS_H__
#define __ELASTOS_DROID_APP_CCONNECTIONSETTINGS_H__

#include "_Elastos_Droid_App_CConnectionSettings.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Core::Object;
using Elastos::Core::IStringBuilder;
using Org::Xmlpull::V1::IXmlPullParser;

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CConnectionSettings)
    , public Object
    , public IConnectionSettings
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CConnectionSettings();

    CARAPI constructor(
        /* [in] */ Int32 connectionId);

    CARAPI constructor(
        /* [in] */ Int32 connectionId,
        /* [in] */ Int32 value,
        /* [in] */ Boolean override);

    CARAPI GetConnectionId(
        /* [out] */ Int32* connectionId);

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
        /* [out] */ IConnectionSettings** obj);

    CARAPI GetXmlString(
        /* [in] */ IStringBuilder* builder,
        /* [in] */ IContext* context);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

private:
    Int32 mConnectionId;
    Int32 mValue;
    Boolean mOverride;
    Boolean mDirty;

    // retrieved from Phone.apk
    static const String ACTION_MODIFY_NETWORK_MODE;
    static const String EXTRA_NETWORK_MODE;

    static const Int32 CM_MODE_2G;
    static const Int32 CM_MODE_3G;
    static const Int32 CM_MODE_4G;
    static const Int32 CM_MODE_2G3G;
    static const Int32 CM_MODE_ALL;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_CCONNECTIONSETTINGS_H__
