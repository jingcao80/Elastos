
#include "elastos/droid/launcher2/CFocusOnlyTabWidget.h"
#include "Elastos.Droid.Service.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CAR_INTERFACE_IMPL(CFocusOnlyTabWidget, TabWidget, IFocusOnlyTabWidget);

CAR_OBJECT_IMPL(CFocusOnlyTabWidget);

ECode CFocusOnlyTabWidget::constructor()
{
    return NOERROR;
}

ECode CFocusOnlyTabWidget::constructor(
    /* [in] */ IContext* context)
{
    return TabWidget::constructor(context);
}

ECode CFocusOnlyTabWidget::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return TabWidget::constructor(context, attrs);
}

ECode CFocusOnlyTabWidget::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    return TabWidget::constructor(context, attrs, defStyle);
}

ECode CFocusOnlyTabWidget::GetSelectedTab(
    /* [out] */ IView** tab)
{
    VALIDATE_NOT_NULL(tab);

    Int32 count;
    GetTabCount(&count);
    for (Int32 i = 0; i < count; ++i) {
        AutoPtr<IView> v;
        GetChildTabViewAt(i, (IView**)&v);
        Boolean res;
        v->IsSelected(&res);
        if (res) {
            *tab = v;
            REFCOUNT_ADD(*tab);
            return NOERROR;
        }
    }
    *tab = NULL;
    return NOERROR;
}

ECode CFocusOnlyTabWidget::GetChildTabIndex(
    /* [in] */ IView* v,
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index);

    Int32 tabCount;
    GetTabCount(&tabCount);
    for (Int32 i = 0; i < tabCount; ++i) {
        AutoPtr<IView> tmp;
        GetChildTabViewAt(i, (IView**)&tmp);
        if (TO_IINTERFACE(tmp) == TO_IINTERFACE(v)) {
            *index = i;
            return NOERROR;
        }
    }
    *index = -1;
    return NOERROR;
}

ECode CFocusOnlyTabWidget::SetCurrentTabToFocusedTab()
{
    AutoPtr<IView> tab;
    Int32 index = -1;
    Int32 count;
    GetTabCount(&count);
    for (Int32 i = 0; i < count; ++i) {
        AutoPtr<IView> v;
        GetChildTabViewAt(i, (IView**)&v);
        Boolean res;
        v->HasFocus(&res);
        if (res) {
            tab = v;
            index = i;
            break;
        }
    }
    if (index > -1) {
        TabWidget::SetCurrentTab(index);
        TabWidget::OnFocusChange(tab, TRUE);
    }
    return NOERROR;
}

ECode CFocusOnlyTabWidget::SuperOnFocusChange(
    /* [in] */ IView* v,
    /* [in] */ Boolean hasFocus)
{
    return TabWidget::OnFocusChange(v, hasFocus);
}

ECode CFocusOnlyTabWidget::OnFocusChange(
    /* [in] */ IView* v,
    /* [in] */ Boolean hasFocus)
{
    if (TO_IINTERFACE(v) == TO_IINTERFACE(this) && hasFocus) {
        Int32 count;
        GetTabCount(&count);
        if (count > 0) {
            AutoPtr<IView> view;
            GetSelectedTab((IView**)&view);
            Boolean res;
            view->RequestFocus(&res);
            return NOERROR;
        }
    }
    return NOERROR;
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos