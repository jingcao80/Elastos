#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_UTILITY_CBLACKLISTUTILS_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_UTILITY_CBLACKLISTUTILS_H__

#include "_Elastos_Droid_Internal_Telephony_Utility_CBlacklistUtils.h"
#include "elastos/core/Singleton.h"

using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Utility {

CarClass(CBlacklistUtils)
    , public Singleton
    , public IBlacklistUtils
{
public:
    CAR_INTERFACE_DECL();

    CAR_SINGLETON_DECL();

    CARAPI AddOrUpdate(
        /* [in] */ IContext* context,
        /* [in] */ const String& number,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 valid,
        /* [out] */ Boolean* result);

    CARAPI IsListed(
        /* [in] */ IContext* context,
        /* [in] */ const String& number,
        /* [in] */ Int32 mode,
        /* [out] */ Int32* result);

    CARAPI IsBlacklistEnabled(
        /* [in] */ IContext* context,
        /* [out] */ Boolean* result);

    CARAPI IsBlacklistNotifyEnabled(
        /* [in] */ IContext* context,
        /* [out] */ Boolean* result);

    CARAPI IsBlacklistPrivateNumberEnabled(
        /* [in] */ IContext* context,
        /* [in] */ Int32 mode,
        /* [out] */ Boolean* result);

    CARAPI IsBlacklistUnknownNumberEnabled(
        /* [in] */ IContext* context,
        /* [in] */ Int32 mode,
        /* [out] */ Boolean* result);

    CARAPI IsBlacklistRegexEnabled(
        /* [in] */ IContext* context,
        /* [out] */ Boolean* result);
};

} // namespace Utility
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_UTILITY_CBLACKLISTUTILS_H__
