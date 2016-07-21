
#include "elastos/droid/contacts/common/util/ContactListViewUtils.h"
#include "elastos/droid/contacts/common/util/SchedulingUtils.h"
#include <elastos/utility/logging/Logger.h>
#include "Elastos.Droid.Content.h"
#include "R.h"

using Elastos::Droid::View::IViewGroup;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace Util {

//=================================================================
// ContactListViewUtils::ApplyCardPaddingToViewRunnable
//=================================================================

ECode ContactListViewUtils::ApplyCardPaddingToViewRunnable::Run()
{
    // Use the rootLayout.getWidth() instead of listView.getWidth() since
    // we sometimes hide the listView until we finish loading data. This would
    // result in incorrect padding.
    Int32 width;
    mRootLayout->GetWidth(&width);
    ContactListViewUtils::AddPaddingToView(mListView, width,  mSpaceWeight, mWeight);
    return NOERROR;
}


//=================================================================
// ContactListViewUtils
//=================================================================

const Double ContactListViewUtils::TEXT_LEFT_PADDING_TO_CARD_PADDING_RATIO;

void ContactListViewUtils::AddPaddingToView(
    /* [in] */ IListView* listView,
    /* [in] */ Int32 parentWidth,
    /* [in] */ Int32 listSpaceWeight,
    /* [in] */ Int32 listViewWeight)
{
    if (listSpaceWeight > 0 && listViewWeight > 0) {
        Double paddingPercent = (Double)listSpaceWeight / (Double)
                (listSpaceWeight * 2 + listViewWeight);
        AutoPtr<IView> v = IView::Probe(listView);
        Int32 top, bottom;
        v->GetPaddingTop(&top);
        v->GetPaddingBottom(&bottom);
        v->SetPadding(
                (Int32)(parentWidth * paddingPercent * TEXT_LEFT_PADDING_TO_CARD_PADDING_RATIO),
                top,
                (Int32)(parentWidth * paddingPercent * TEXT_LEFT_PADDING_TO_CARD_PADDING_RATIO),
                bottom);
        // The EdgeEffect and ScrollBar need to span to the edge of the ListView's padding.
        IViewGroup::Probe(listView)->SetClipToPadding(FALSE);
        v->SetScrollBarStyle(IView::SCROLLBARS_OUTSIDE_OVERLAY);
    }
}

ECode ContactListViewUtils::ApplyCardPaddingToView(
    /* [in] */ IResources* resources,
    /* [in] */ IListView* listView,
    /* [in] */ IView* rootLayout)
{
    // Set a padding on the list view so it appears in the center of the card
    // in the layout if required.
    Int32 listSpaceWeight;
    resources->GetInteger(
            Elastos::Droid::Dialer::R::integer::contact_list_space_layout_weight, &listSpaceWeight);
    Int32 listViewWeight;
    resources->GetInteger(
            Elastos::Droid::Dialer::R::integer::contact_list_card_layout_weight, &listViewWeight);
    if (listSpaceWeight > 0 && listViewWeight > 0) {
        rootLayout->SetBackgroundResource(0);
        // Set the card view visible
        AutoPtr<IView> mCardView;
        rootLayout->FindViewById(Elastos::Droid::Dialer::R::id::list_card, (IView**)&mCardView);
        if (mCardView == NULL) {
            Logger::E("ContactListViewUtils", "Your content must have a list card view who can be turned visible whenever it is necessary.");
            return E_RUNTIME_EXCEPTION;
        }
        mCardView->SetVisibility(IView::VISIBLE);
        // Add extra padding to the list view to make them appear in the center of the card.
        // In order to avoid jumping, we skip drawing the next frame of the ListView.
        AutoPtr<IRunnable> runnable = (IRunnable*)new ApplyCardPaddingToViewRunnable(
                listView, rootLayout, listSpaceWeight, listViewWeight);
        SchedulingUtils::DoOnPreDraw(IView::Probe(listView), /* drawNextFrame = */ FALSE, runnable);
    }
    return NOERROR;
}

} // namespace Util
} // namespace Common
} // namespace Contacts
} // namespace Droid
} // namespace Elastos
