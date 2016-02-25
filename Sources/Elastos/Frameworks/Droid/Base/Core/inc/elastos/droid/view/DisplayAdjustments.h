#ifndef __ELASTOS_DROID_VIEW_DISPLAYADJUSTMENTS_H__
#define __ELASTOS_DROID_VIEW_DISPLAYADJUSTMENTS_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Core::ICharSequence;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Content::Res::ICompatibilityInfo;

namespace Elastos {
namespace Droid {
namespace View {

class DisplayAdjustments
    : public Object
    , public IDisplayAdjustments
{
public:
    CAR_INTERFACE_DECL()

    DisplayAdjustments();

    DisplayAdjustments(
        /* [in] */ IBinder* token);

    DisplayAdjustments(
        /* [in] */ IDisplayAdjustments* daj);

    DisplayAdjustments(
        /* [in] */ ICompatibilityInfo* compatInfo,
        /* [in] */ IBinder* token);

    CARAPI SetCompatibilityInfo(
        /* [in] */ ICompatibilityInfo* compatInfo);

    CARAPI GetCompatibilityInfo(
        /* [out] */ ICompatibilityInfo** info);

    CARAPI SetActivityToken(
        /* [in] */ IBinder* token);

    CARAPI GetActivityToken(
        /* [out] */ IBinder** token);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    CARAPI Equals(
        /* [in] */ IInterface* object,
        /* [out] */ Boolean* equals);

private:
    CARAPI Init(
        /* [in] */ ICompatibilityInfo* compatInfo,
        /* [in] */ IBinder* token);
public:
    static CARAPI GetDEFAULT_DISPLAY_ADJUSTMENTS(
        /* [out] */ IDisplayAdjustments** da);

    static AutoPtr<IDisplayAdjustments> DEFAULT_DISPLAY_ADJUSTMENTS;

private:
    AutoPtr<ICompatibilityInfo> mCompatInfo;
    AutoPtr<IBinder> mActivityToken;
};

} // namespace View
} // namespace Droid
} // namespace Elastos

#endif