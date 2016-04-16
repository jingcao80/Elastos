
#include "elastos/droid/launcher2/HandleView.h"
#include "Elastos.Droid.Service.h"
#include "Elastos.CoreLibrary.Core.h"
#include <elastos/core/CoreUtils.h>
#include "R.h"

using Elastos::Core::CoreUtils;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

const Int32 HandleView::ORIENTATION_HORIZONTAL = 1;

HandleView::HandleView()
    : mOrientation(ORIENTATION_HORIZONTAL)
{
}

ECode HandleView::constructor(
    /* [in] */ IContext* context)
{
    return ImageView::constructor(context);
}

ECode HandleView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode HandleView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    ImageView::constructor(context, attrs, defStyle);

    AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
            const_cast<Int32 *>(Elastos::Droid::Launcher2::R::styleable::HandleView),
            ArraySize(Elastos::Droid::Launcher2::R::styleable::HandleView));
    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(attrs, attrIds, defStyle, 0, (ITypedArray**)&a);
    a->GetInt32(Elastos::Droid::Launcher2::R::styleable::HandleView_direction,
            ORIENTATION_HORIZONTAL, &mOrientation);
    a->Recycle();

    String str;
    context->GetString(Elastos::Droid::Launcher2::R::string::all_apps_button_label, &str);
    AutoPtr<ICharSequence> cchar = CoreUtils::Convert(str);
    return SetContentDescription(cchar);
}

ECode HandleView::FocusSearch(
    /* [in] */ Int32 direction,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);

    AutoPtr<IView> newFocus;
    ImageView::FocusSearch(direction, (IView**)&newFocus);
    Boolean res;
    assert(0 && "need class Launcher");
    //mLauncher->IsAllAppsVisible(&res);
    if (newFocus == NULL && !res) {
        AutoPtr<IWorkspace> workspace;
        assert(0 && "need class Workspace");
        // mLauncher->GetWorkspace((IWorkspace**)&workspace);
        // workspace->DispatchUnhandledMove(NULL, direction);
        // if (mOrientation == ORIENTATION_HORIZONTAL && direction == FOCUS_DOWN) {
        //     *view = IView::Probe(this);
        // }
        // else {
        //     *view = IView::Probe(workspace);
        // }
        REFCOUNT_ADD(*view);
        return NOERROR;
    }
    *view = newFocus;
    REFCOUNT_ADD(*view);
    return NOERROR;
}

ECode HandleView::OnTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 action;
    ev->GetAction(&action);
    Boolean res;
    assert(0 && "need class Launcher");
    //mLauncher->IsAllAppsVisible(&res)
    if (action == IMotionEvent::ACTION_DOWN && res) {
        *result = FALSE;
        return NOERROR;
    }
    return ImageView::OnTouchEvent(ev, result);
}

ECode HandleView::SetLauncher(
    /* [in] */ ILauncher* launcher)
{
    mLauncher = launcher;
    return NOERROR;
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos