
#include "elastos/droid/support/v4/view/PagerAdapter.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Database::CDataSetObservable;
using Elastos::Droid::Database::IObservable;
using Elastos::Droid::Support::V4::View::EIID_IPagerAdapter;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Support {
namespace V4 {
namespace View {

PagerAdapter::PagerAdapter()
{
    CDataSetObservable::New((IDataSetObservable**)&mObservable);
}

CAR_INTERFACE_IMPL(PagerAdapter, Object, IPagerAdapter)

ECode PagerAdapter::StartUpdate(
    /* [in] */ IViewGroup* container)
{
    return StartUpdate(IView::Probe(container));
}

ECode PagerAdapter::InstantiateItem(
    /* [in] */ IViewGroup* container,
    /* [in] */ Int32 position,
    /* [out] */ IInterface** item)
{
    VALIDATE_NOT_NULL(item)
    return InstantiateItem(IView::Probe(container), position, item);
}

ECode PagerAdapter::DestroyItem(
    /* [in] */ IViewGroup* container,
    /* [in] */ Int32 position,
    /* [in] */ IInterface* object)
{
    return DestroyItem(IView::Probe(container), position, object);
}

ECode PagerAdapter::SetPrimaryItem(
    /* [in] */ IViewGroup* container,
    /* [in] */ Int32 position,
    /* [in] */ IInterface* object)
{
    return SetPrimaryItem(IView::Probe(container), position, object);
}

ECode PagerAdapter::FinishUpdate(
    /* [in] */ IViewGroup* container)
{
    return FinishUpdate(IView::Probe(container));
}

ECode PagerAdapter::StartUpdate(
    /* [in] */ IView* container)
{
    return NOERROR;
}

ECode PagerAdapter::InstantiateItem(
    /* [in] */ IView* container,
    /* [in] */ Int32 position,
    /* [out] */ IInterface** item)
{
    VALIDATE_NOT_NULL(item)
    Slogger::E("PagerAdapter", "Required method instantiateItem was not overridden");
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode PagerAdapter::DestroyItem(
    /* [in] */ IView* container,
    /* [in] */ Int32 position,
    /* [in] */ IInterface* object)
{
    Slogger::E("PagerAdapter", "Required method destroyItem was not overridden");
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode PagerAdapter::SetPrimaryItem(
    /* [in] */ IView* container,
    /* [in] */ Int32 position,
    /* [in] */ IInterface* object)
{
    return NOERROR;
}

ECode PagerAdapter::FinishUpdate(
    /* [in] */ IView* container)
{
    return NOERROR;
}

ECode PagerAdapter::IsViewFromObject(
    /* [in] */ IView* view,
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    return NOERROR;
}

ECode PagerAdapter::SaveState(
    /* [out] */ IParcelable** state)
{
    VALIDATE_NOT_NULL(state)
    *state = NULL;
    return NOERROR;
}

ECode PagerAdapter::RestoreState(
    /* [in] */ IParcelable* state,
    /* [in] */ IClassLoader* loader)
{
    return NOERROR;
}

ECode PagerAdapter::GetItemPosition(
    /* [in] */ IInterface* object,
    /* [out] */ Int32* pos)
{
    VALIDATE_NOT_NULL(pos)
    *pos = POSITION_UNCHANGED;
    return NOERROR;
}

ECode PagerAdapter::NotifyDataSetChanged()
{
    return mObservable->NotifyChanged();
}

ECode PagerAdapter::RegisterDataSetObserver(
    /* [in] */ IDataSetObserver* observer)
{
    return IObservable::Probe(mObservable)->RegisterObserver(observer);
}

ECode PagerAdapter::UnregisterDataSetObserver(
    /* [in] */ IDataSetObserver* observer)
{
    return IObservable::Probe(mObservable)->UnregisterObserver(observer);
}

ECode PagerAdapter::GetPageTitle(
    /* [in] */ Int32 position,
    /* [out] */ ICharSequence** title)
{
    VALIDATE_NOT_NULL(title)
    *title = NULL;
    return NOERROR;
}

ECode PagerAdapter::GetPageWidth(
    /* [in] */ Int32 position,
    /* [out] */ Float* width)
{
    VALIDATE_NOT_NULL(width)
    *width = 1.0;
    return NOERROR;
}

} // namespace View
} // namespace V4
} // namespace Support
} // namespace Droid
} // namespace Elastos
