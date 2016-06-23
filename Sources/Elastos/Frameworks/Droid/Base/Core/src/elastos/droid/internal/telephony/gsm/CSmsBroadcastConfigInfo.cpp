#include "elastos/droid/internal/telephony/gsm/CSmsBroadcastConfigInfo.h"
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Gsm {

CAR_INTERFACE_IMPL(CSmsBroadcastConfigInfo, Object, ISmsBroadcastConfigInfo)

CAR_OBJECT_IMPL(CSmsBroadcastConfigInfo)

CSmsBroadcastConfigInfo::CSmsBroadcastConfigInfo()
    : mFromServiceId(0)
    , mToServiceId(0)
    , mFromCodeScheme(0)
    , mToCodeScheme(0)
    , mSelected(FALSE)
{
}

ECode CSmsBroadcastConfigInfo::constructor()
{
    return NOERROR;
}

ECode CSmsBroadcastConfigInfo::constructor(
    /* [in] */ Int32 fromId,
    /* [in] */ Int32 toId,
    /* [in] */ Int32 fromScheme,
    /* [in] */ Int32 toScheme,
    /* [in] */ Boolean selected)
{
    mFromServiceId = fromId;
    mToServiceId = toId;
    mFromCodeScheme = fromScheme;
    mToCodeScheme = toScheme;
    mSelected = selected;
    return NOERROR;
}

ECode CSmsBroadcastConfigInfo::SetFromServiceId(
    /* [in] */ Int32 fromServiceId)
{
    mFromServiceId = fromServiceId;
    return NOERROR;
}

ECode CSmsBroadcastConfigInfo::GetFromServiceId(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mFromServiceId;
    return NOERROR;
}

ECode CSmsBroadcastConfigInfo::SetToServiceId(
    /* [in] */ Int32 toServiceId)
{
    mToServiceId = toServiceId;
    return NOERROR;
}

ECode CSmsBroadcastConfigInfo::GetToServiceId(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mToServiceId;
    return NOERROR;
}

ECode CSmsBroadcastConfigInfo::SetFromCodeScheme(
    /* [in] */ Int32 fromCodeScheme)
{
    mFromCodeScheme = fromCodeScheme;
    return NOERROR;
}

ECode CSmsBroadcastConfigInfo::GetFromCodeScheme(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mFromCodeScheme;
    return NOERROR;
}

ECode CSmsBroadcastConfigInfo::SetToCodeScheme(
    /* [in] */ Int32 toCodeScheme)
{
    mToCodeScheme = toCodeScheme;
    return NOERROR;
}

ECode CSmsBroadcastConfigInfo::GetToCodeScheme(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mToCodeScheme;
    return NOERROR;
}

ECode CSmsBroadcastConfigInfo::SetSelected(
    /* [in] */ Boolean selected)
{
    mSelected = selected;
    return NOERROR;
}

ECode CSmsBroadcastConfigInfo::IsSelected(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mSelected;
    return NOERROR;
}

ECode CSmsBroadcastConfigInfo::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = String("SmsBroadcastConfigInfo: Id [")
            + StringUtils::ToString(mFromServiceId) + ','
            + StringUtils::ToString(mToServiceId) + "] Code ["
            + StringUtils::ToString(mFromCodeScheme) + ','
            + StringUtils::ToString(mToCodeScheme) + "] " +
            (mSelected ? "ENABLED" : "DISABLED");
    return NOERROR;
}

} // namespace Gsm
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos