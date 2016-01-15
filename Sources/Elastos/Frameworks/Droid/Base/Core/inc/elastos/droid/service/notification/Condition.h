#ifndef __ELASTOS_DROID_SERVICE_NOTIFICATION_CONDITION_H__
#define __ELASTOS_DROID_SERVICE_NOTIFICATION_CONDITION_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Service.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Net::IUriBuilder;

namespace Elastos {
namespace Droid {
namespace Service {
namespace Notification {

class Condition
    : public Object
    , public ICondition
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    Condition();

    CARAPI constructor(
        /* [in] */ IUri* id,
        /* [in] */ const String& summary,
        /* [in] */ const String& line1,
        /* [in] */ const String& line2,
        /* [in] */ Int32 icon,
        /* [in] */ Int32 state,
        /* [in] */ Int32 flags);

    CARAPI constructor(
        /* [in] */ IParcelable* source);

    static CARAPI_(Boolean) IsValidState(
        /* [in] */ Int32 state);

public:
    // @Override
    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    // @Override
    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    // @Override
    CARAPI ToString(
        /* [out] */ String* str);

    static CARAPI StateToString(
        /* [in] */ Int32 state,
        /* [out] */ String* str);

    static CARAPI RelevanceToString(
        /* [in] */ Int32 flags,
        /* [out] */ String* str);

    // @Override
    CARAPI Equals(
        /* [in] */ IInterface* o,
        /* [out] */ Boolean* isEquals);

    // @Override
    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    CARAPI Copy(
        /* [out] */ ICondition** copy);

    static CARAPI NewId(
        /* [in] */ IContext* context,
        /* [out] */ IUriBuilder** id);

    static CARAPI IsValidId(
        /* [in] */ IUri* id,
        /* [in] */ const String& pkg,
        /* [out] */ Boolean* isValidId);

public:
    AutoPtr<IUri> mId;
    String mSummary;
    String mLine1;
    String mLine2;
    Int32 mIcon;
    Int32 mState;
    Int32 mFlags;

private:
    const static String TAG;
};

} // namespace Notification
} // namespace Service
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICE_NOTIFICATION_CONDITION_H__
