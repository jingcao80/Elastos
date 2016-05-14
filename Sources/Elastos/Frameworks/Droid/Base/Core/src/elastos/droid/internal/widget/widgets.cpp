#include "elastos/droid/internal/widget/CAbsActionBarView.h"
#include "elastos/droid/internal/widget/CActionBarOverlayLayoutLayoutParams.h"
#include "elastos/droid/internal/widget/CAbsListViewAutoScroller.h"
#include "elastos/droid/internal/widget/CActionBarContainer.h"
#include "elastos/droid/internal/widget/CActionBarContextView.h"
#include "elastos/droid/internal/widget/CActionBarOverlayLayout.h"
#include "elastos/droid/internal/widget/CActionBarView.h"
#include "elastos/droid/internal/widget/CActionBarViewSavedState.h"
#include "elastos/droid/internal/widget/CBackgroundFallback.h"
#include "elastos/droid/internal/widget/CDialogTitle.h"
#include "elastos/droid/internal/widget/CDrawableHolder.h"
#include "elastos/droid/internal/widget/CEditableInputConnection.h"
#include "elastos/droid/internal/widget/CFaceUnlockView.h"
#include "elastos/droid/internal/widget/CLinearLayoutWithDefaultTouchRecepient.h"
#include "elastos/droid/internal/widget/CLockPatternView.h"
#include "elastos/droid/internal/widget/CLockPatternViewCell.h"
#include "elastos/droid/internal/widget/CLockPatternViewSavedState.h"
#include "elastos/droid/internal/widget/CLockPatternUtils.h"
#include "elastos/droid/internal/widget/CPasswordEntryKeyboard.h"
#include "elastos/droid/internal/widget/CPasswordEntryKeyboardHelper.h"
#include "elastos/droid/internal/widget/CPasswordEntryKeyboardView.h"
#include "elastos/droid/internal/widget/CPointerLocationView.h"
#include "elastos/droid/internal/widget/CPreferenceImageView.h"
#include "elastos/droid/internal/widget/CResolverDrawerLayout.h"
#include "elastos/droid/internal/widget/CResolverDrawerLayoutParams.h"
#include "elastos/droid/internal/widget/CResolverDrawerLayoutSavedState.h"
#include "elastos/droid/internal/widget/CRotarySelector.h"
#include "elastos/droid/internal/widget/CScrollingTabContainerTabView.h"
#include "elastos/droid/internal/widget/CScrollingTabContainerView.h"
#include "elastos/droid/internal/widget/CSlidingTab.h"
#include "elastos/droid/internal/widget/CSizeAdaptiveLayout.h"
#include "elastos/droid/internal/widget/CSizeAdaptiveLayoutLayoutParams.h"
#include "elastos/droid/internal/widget/CSmileys.h"
#include "elastos/droid/internal/widget/CSubtitleView.h"
#include "elastos/droid/internal/widget/CSwipeDismissLayout.h"
#include "elastos/droid/internal/widget/CTextProgressBar.h"
#include "elastos/droid/internal/widget/CToolbarWidgetWrapper.h"
#include "elastos/droid/internal/widget/CWaveView.h"
#include "elastos/droid/internal/widget/CWeightedLinearLayout.h"
#include "elastos/droid/internal/widget/CActionBarViewHomeView.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

CAR_OBJECT_IMPL(CAbsActionBarView)
CAR_OBJECT_IMPL(CActionBarOverlayLayoutLayoutParams)
CAR_OBJECT_IMPL(CAbsListViewAutoScroller)
CAR_OBJECT_IMPL(CActionBarContainer)
CAR_OBJECT_IMPL(CActionBarContextView)
CAR_OBJECT_IMPL(CActionBarOverlayLayout)
CAR_OBJECT_IMPL(CActionBarView)
CAR_OBJECT_IMPL(CActionBarViewSavedState)
CAR_OBJECT_IMPL(CBackgroundFallback)
CAR_OBJECT_IMPL(CDialogTitle)
CAR_OBJECT_IMPL(CDrawableHolder)
CAR_OBJECT_IMPL(CEditableInputConnection)
CAR_OBJECT_IMPL(CFaceUnlockView)
CAR_OBJECT_IMPL(CLinearLayoutWithDefaultTouchRecepient)
CAR_OBJECT_IMPL(CLockPatternView)
CAR_OBJECT_IMPL(CLockPatternViewCell)
CAR_OBJECT_IMPL(CLockPatternViewSavedState)
CAR_OBJECT_IMPL(CLockPatternUtils)
CAR_OBJECT_IMPL(CPasswordEntryKeyboard)
CAR_OBJECT_IMPL(CPasswordEntryKeyboardHelper)
CAR_OBJECT_IMPL(CPasswordEntryKeyboardView)
CAR_OBJECT_IMPL(CPointerLocationView)
CAR_OBJECT_IMPL(CPreferenceImageView)
CAR_OBJECT_IMPL(CResolverDrawerLayout)
CAR_OBJECT_IMPL(CResolverDrawerLayoutParams)
CAR_OBJECT_IMPL(CResolverDrawerLayoutSavedState)
CAR_OBJECT_IMPL(CRotarySelector)
CAR_OBJECT_IMPL(CScrollingTabContainerTabView)
CAR_OBJECT_IMPL(CScrollingTabContainerView)
CAR_OBJECT_IMPL(CSlidingTab)
CAR_OBJECT_IMPL(CSizeAdaptiveLayout)
CAR_OBJECT_IMPL(CSizeAdaptiveLayoutLayoutParams)
CAR_OBJECT_IMPL(CSmileys)
CAR_OBJECT_IMPL(CSubtitleView)
CAR_OBJECT_IMPL(CSwipeDismissLayout)
CAR_OBJECT_IMPL(CTextProgressBar)
CAR_OBJECT_IMPL(CToolbarWidgetWrapper)
CAR_OBJECT_IMPL(CWaveView)
CAR_OBJECT_IMPL(CWeightedLinearLayout)
CAR_OBJECT_IMPL(CActionBarViewHomeView);

}// namespace Widget
}// namespace Internal
}// namespace Droid
}// namespace Elastos
