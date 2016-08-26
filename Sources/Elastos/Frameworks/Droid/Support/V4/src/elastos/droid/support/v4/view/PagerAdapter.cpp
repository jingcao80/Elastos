
#include "elastos/droid/support/v4/view/PagerAdapter.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Database::CDataSetObservable;
using Elastos::Droid::Database::IObservable;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Support {
namespace V4 {
namespace View {

CAR_INTERFACE_IMPL(PagerAdapter, Object, IPagerAdapter);

PagerAdapter::PagerAdapter()
{
    CDataSetObservable::New((IDataSetObservable**)&mObservable);
}

ECode PagerAdapter::StartUpdate(
    /* [in] */ IViewGroup* container)
{
    return StartUpdate(IView::Probe(container));
}

ECode PagerAdapter::InstantiateItem(
    /* [in] */ IViewGroup* container,
    /* [in] */ Int32 position,
    /* [out] */ IInterface** newPage)
{
    VALIDATE_NOT_NULL(newPage);
    return InstantiateItem(IView::Probe(container), position, newPage);
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
    /* [out] */ IInterface** newPage)
{
    Logger::E("PagerAdapter", "Required method instantiateItem was not overridden");
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode PagerAdapter::DestroyItem(
    /* [in] */ IView* container,
    /* [in] */ Int32 position,
    /* [in] */ IInterface* object)
{
    Logger::E("PagerAdapter", "Required method destroyItem was not overridden");
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

ECode PagerAdapter::SaveState(
    /* [out] */ IParcelable** state)
{
    VALIDATE_NOT_NULL(state);
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
    /* [out] */ Int32* position)
{
    VALIDATE_NOT_NULL(position);
    *position = POSITION_UNCHANGED;
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
    VALIDATE_NOT_NULL(title);
    *title = NULL;
    return NOERROR;
}

ECode PagerAdapter::GetPageWidth(
    /* [in] */ Int32 position,
    /* [out] */ Float* width)
{
    VALIDATE_NOT_NULL(width);
    *width = 1;
    return NOERROR;
}

} // namespace View
} // namespace V4
} // namespace Support
} // namespace Droid
} // namespace Elastos
