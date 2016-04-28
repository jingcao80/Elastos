
#include "CCallUtil.h"
#include "CallUtil.h"

namespace Elastos{
namespace Apps{
namespace Contacts {
namespace Common {

CAR_INTERFACE_IMPL(CCallUtil, Singleton, ICallUtil)

CAR_SINGLETON_IMPL(CCallUtil)

ECode CCallUtil::GetCallIntent(
    /* [in] */ const String& number,
    /* [out] */ IIntent** intent)
{
    VALUE_NOT_NULL(intent);
    *intent = CallUtil::GetCallIntent(number);
    REFCOUNT_ADD(*intent);
    return NOERROR;
}

ECode CCallUtil::GetCallIntent(
    /* [in] */ IUri* uri,
    /* [out] */ IIntent** intent)
{
    VALUE_NOT_NULL(intent);
    *intent = CallUtil::GetCallIntent(uri);
    REFCOUNT_ADD(*intent);
    return NOERROR;
}

ECode CCallUtil::GetCallIntent(
    /* [in] */ const String& number,
    /* [in] */ const String& callOrigin,
    /* [out] */ IIntent** intent)
{
    VALUE_NOT_NULL(intent);
    *intent = CallUtil::GetCallIntent(number, callOrigin);
    REFCOUNT_ADD(*intent);
    return NOERROR;
}

ECode CCallUtil::GetCallIntent(
    /* [in] */ const String& number,
    /* [in] */ IPhoneAccountHandle* accountHandle,
    /* [out] */ IIntent** intent)
{
    VALUE_NOT_NULL(intent);
    *intent = CallUtil::GetCallIntent(number, accountHandle);
    REFCOUNT_ADD(*intent);
    return NOERROR;
}

ECode CCallUtil::GetCallIntent(
    /* [in] */ IUri* uri,
    /* [in] */ IPhoneAccountHandle* accountHandle,
    /* [out] */ IIntent** intent)
{
    VALUE_NOT_NULL(intent);
    *intent = CallUtil::GetCallIntent(uri, accountHandle);
    REFCOUNT_ADD(*intent);
    return NOERROR;
}

ECode CCallUtil::GetCallIntent(
    /* [in] */ const String& number,
    /* [in] */ const String& callOrigin,
    /* [in] */ IPhoneAccountHandle* accountHandle,
    /* [out] */ IIntent** intent)
{
    VALUE_NOT_NULL(intent);
    *intent = CallUtil::GetCallIntent(number, callOrigin, accountHandle);
    REFCOUNT_ADD(*intent);
    return NOERROR;
}

ECode CCallUtil::GetCallIntent(
    /* [in] */ IUri* uri,
    /* [in] */ const String& callOrigin,
    /* [in] */ IPhoneAccountHandle* accountHandle,
    /* [out] */ IIntent** intent)
{
    VALUE_NOT_NULL(intent);
    *intent = CallUtil::GetCallIntent(uri, callOrigin, accountHandle);
    REFCOUNT_ADD(*intent);
    return NOERROR;
}

ECode CCallUtil::GetVideoCallIntent(
    /* [in] */ const String& number,
    /* [in] */ const String& callOrigin,
    /* [out] */ IIntent** intent)
{
    VALUE_NOT_NULL(intent);
    *intent = CallUtil::GetVideoCallIntent(number, callOrigin);
    REFCOUNT_ADD(*intent);
    return NOERROR;
}

ECode CCallUtil::GetVideoCallIntent(
    /* [in] */ const String& number,
    /* [in] */ const String& callOrigin,
    /* [in] */ IPhoneAccountHandle* accountHandle,
    /* [out] */ IIntent** intent)
{
    VALUE_NOT_NULL(intent);
    *intent = CallUtil::GetVideoCallIntent(number, callOrigin, accountHandle);
    REFCOUNT_ADD(*intent);
    return NOERROR;
}

ECode CCallUtil::GetVideoCallIntent(
    /* [in] */ const String& number,
    /* [in] */ IPhoneAccountHandle* accountHandle,
    /* [out] */ IIntent** intent)
{
    VALUE_NOT_NULL(intent);
    *intent = CallUtil::GetVideoCallIntent(number, accountHandle);
    REFCOUNT_ADD(*intent);
    return NOERROR;
}

ECode CCallUtil::GetCallIntent(
    /* [in] */ IUri* uri,
    /* [in] */ const String& callOrigin,
    /* [in] */ IPhoneAccountHandle* accountHandle,
    /* [in] */ Int32 videoState,
    /* [out] */ IIntent** intent)
{
    VALUE_NOT_NULL(intent);
    *intent = CallUtil::GetCallIntent(uri, number, callOrigin, accountHandle, videoState);
    REFCOUNT_ADD(*intent);
    return NOERROR;
}

ECode CCallUtil::GetCallUri(
    /* [in] */ const String& number,
    /* [out] */ IUri* uri)
{
    VALUE_NOT_NULL(uri);
    *uri = CallUtil::GetCallUri(number);
    REFCOUNT_ADD(*uri);
    return NOERROR;
}

ECode CCallUtil::IsVideoEnabled(
    /* [in] */ IContext* context,
    /* [out] */ Boolean* result)
{
    VALUE_NOT_NULL(result);
    *result = CallUtil::IsVideoEnabled(context);
    return NOERROR;
}

} // Common
} // Contacts
} // Apps
} // Elastos
