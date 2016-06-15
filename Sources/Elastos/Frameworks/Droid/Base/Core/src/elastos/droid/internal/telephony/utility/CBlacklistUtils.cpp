#include "elastos/droid/internal/telephony/utility/CBlacklistUtils.h"
#include "elastos/droid/internal/telephony/utility/BlacklistUtils.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Utility {

CAR_SINGLETON_IMPL(CBlacklistUtils)
CAR_INTERFACE_IMPL(CBlacklistUtils, Singleton, IBlacklistUtils)

ECode CBlacklistUtils::AddOrUpdate(
    /* [in] */ IContext* context,
    /* [in] */ const String& number,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 valid,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = BlacklistUtils::AddOrUpdate(context, number, flags, valid);
    return NOERROR;
}

ECode CBlacklistUtils::IsListed(
    /* [in] */ IContext* context,
    /* [in] */ const String& number,
    /* [in] */ Int32 mode,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = BlacklistUtils::IsListed(context, number, mode);
    return NOERROR;
}

ECode CBlacklistUtils::IsBlacklistEnabled(
    /* [in] */ IContext* context,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = BlacklistUtils::IsBlacklistEnabled(context);
    return NOERROR;
}

ECode CBlacklistUtils::IsBlacklistNotifyEnabled(
    /* [in] */ IContext* context,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = BlacklistUtils::IsBlacklistNotifyEnabled(context);
    return NOERROR;
}

ECode CBlacklistUtils::IsBlacklistPrivateNumberEnabled(
    /* [in] */ IContext* context,
    /* [in] */ Int32 mode,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = BlacklistUtils::IsBlacklistPrivateNumberEnabled(context, mode);
    return NOERROR;
}

ECode CBlacklistUtils::IsBlacklistUnknownNumberEnabled(
    /* [in] */ IContext* context,
    /* [in] */ Int32 mode,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = BlacklistUtils::IsBlacklistUnknownNumberEnabled(context, mode);
    return NOERROR;
}

ECode CBlacklistUtils::IsBlacklistRegexEnabled(
    /* [in] */ IContext* context,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = BlacklistUtils::IsBlacklistRegexEnabled(context);
    return NOERROR;
}


} // namespace Utility
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
