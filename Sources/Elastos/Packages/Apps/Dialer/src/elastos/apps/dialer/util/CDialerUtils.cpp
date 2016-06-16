
#include "util/CDialerUtils.h"
#include "util/DialerUtils.h"

namespace Elastos {
namespace Apps {
namespace Dialer {
namespace Util {

CAR_INTERFACE_IMPL(CDialerUtils, Singleton, IDialerUtils);

CAR_SINGLETON_IMPL(CDialerUtils);

ECode CDialerUtils::StartActivityWithErrorToast(
    /* [in] */ IContext* context,
    /* [in] */ Intent* intent)
{
    DialerUtils::StartActivityWithErrorToast(context, intent);
    return NOERROR;
}

ECode CDialerUtils::StartActivityWithErrorToast(
    /* [in] */ IContext* context,
    /* [in] */ Intent* intent,
    /* [in] */ Int32 msgId)
{
    DialerUtils::StartActivityWithErrorToast(context, intent, msgId);
    return NOERROR;
}

ECode CDialerUtils::GetSmsComponent(
    /* [in] */ IContext* context,
    /* [out] */ IComponentName** component)
{
    VALUE_NOT_NULL(component);
    AutoPtr<IComponentName> result = DialerUtils::GetSmsComponent(context);
    *component = result;
    REFCOUNT_ADD(*component);
    return NOERROR;
}

ECode CDialerUtils::ConfigureEmptyListView(
    /* [in] */ IView* emptyListView,
    /* [in] */ Int32 imageResId,
    /* [in] */ Int32 strResId,
    /* [in] */ IResources* res)
{
    DialerUtils::ConfigureEmptyListView(emptyListView,
            imageResId, strResId, res);
    return NOERROR;
}

//TODO:
// ECode CDialerUtils::CloseQuietly(
//     /* [in] */ IAutoCloseable* closeable)
// {
//     return DialerUtils::CloseQuietly(closeable);
// }

ECode CDialerUtils::Join(
    /* [in] */ IResources* resources,
    /* [in] */ IIterable* list,
    /* [out] */ ICharSequence** result)
{
    VALUE_NOT_NULL(result);
    AutoPtr<ICharSequence> cs = DialerUtils::Join(resources, list);
    *result = cs;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CDialerUtils::IsRtl(
    /* [out] */ Boolean* result)
{
    VALUE_NOT_NULL(result);
    result = DialerUtils::IsRtl();
    return NOERROR;
}

ECode CDialerUtils::ShowInputMethod(
    /* [in] */ IView* view)
{
    DialerUtils::ShowInputMethod(view);
    return NOERROR;
}

ECode CDialerUtils::HideInputMethod(
    /* [in] */ IView* view)
{
    DialerUtils::HideInputMethod(view);
    return NOERROR;
}

} // Util
} // Dialer
} // Apps
} // Elastos
