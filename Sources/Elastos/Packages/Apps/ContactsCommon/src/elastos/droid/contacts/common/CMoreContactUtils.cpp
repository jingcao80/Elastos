
#include "CMoreContactUtils.h"
#include "MoreContactUtils.h"

namespace Elastos{
namespace Apps{
namespace Contacts {
namespace Common {

CAR_INTERFACE_IMPL(CMoreContactUtils, Singleton, IMoreContactUtils)

CAR_SINGLETON_IMPL(CMoreContactUtils)

ECode CMoreContactUtils::ShouldCollapse(
    /* [in] */ ICharSequence* mimetype1,
    /* [in] */ ICharSequence* data1,
    /* [in] */ ICharSequence* mimetype2,
    /* [in] */ ICharSequence* data2,
    /* [out] */ Boolean* result)
{
    VALUE_NOT_NULL(result);

    *result = MoreContactUtils::ShouldCollapse(
            mimetype1, data1, mimetype2, data2);
    return NOERROR;
}

ECode CMoreContactUtils::GetTargetRectFromView(
    /* [in] */ IView* view,
    /* [out] */ IRect** rect)
{
    VALUE_NOT_NULL(rect);

    AutoPtr<IRect> result = MoreContactUtils::GetTargetRectFromView(view);
    *rect = result;
    REFCOUNT_ADD(*rect);
    return NOERROR;
}

ECode CMoreContactUtils::CreateHeaderView(
    /* [in] */ IContext* context,
    /* [in] */ Int32 textResourceId,
    /* [out] */ ITextView** textView)
{
    VALUE_NOT_NULL(textView);

    AutoPtr<ITextView> result = MoreContactUtils::CreateHeaderView(
                context, textResourceId);
    *textView = result;
    REFCOUNT_ADD(*textView);
    return NOERROR;
}

ECode CMoreContactUtils::SetHeaderViewBottomPadding(
    /* [in] */ IContext* context,
    /* [in] */ ITextView* textView,
    /* [in] */ Boolean isFirstRow)
{
    MoreContactUtils::SetHeaderViewBottomPadding(context, textView, isFirstRow);
    return NOERROR;
}

ECode CMoreContactUtils::GetInvitableIntent(
    /* [in] */ IAccountType* accountType,
    /* [in] */ IUri* lookupUri,
    /* [out] */ IIntent** intent)
{
    VALUE_NOT_NULL(intent);

    AutoPtr<IIntent> result = MoreContactUtils::GetInvitableIntent(
                accountType, lookupUri);
    *intent = result;
    REFCOUNT_ADD(*intent);
    return NOERROR;
}

} // Common
} // Contacts
} // Apps
} // Elastos
