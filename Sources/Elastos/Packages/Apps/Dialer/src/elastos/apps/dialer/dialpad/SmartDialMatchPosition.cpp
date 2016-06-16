
#include "dialpad/SmartDialMatchPosition.h"

namespace Elastos {
namespace Apps {
namespace Dialer {
namespace Dialpad {

const String SmartDialMatchPosition::TAG("SmartDialMatchPosition");

CAR_INTERFACE_IMPL(SmartDialMatchPosition, Object, ISmartDialMatchPosition);

ECode SmartDialMatchPosition::constructor(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    mStart = start;
    mEnd = end;
}

void SmartDialMatchPosition::Advance(
    /* [in] */ Int32 toAdvance)
{
    mStart += toAdvance;
    mEnd += toAdvance;
}

void SmartDialMatchPosition::AdvanceMatchPositions(
    /* [in] */ IArrayList* inList,
    /* [in] */ Int32 toAdvance)
{
    Int32 size;
    inList->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> item;
        inList->Get(i, (IInterface**)&item);
        ((SmartDialMatchPosition*)item)->Advance(toAdvance);
    }
}

void SmartDialMatchPosition::Print(
    /* [in] */ IArrayList* list)
{
    Int32 size;
    inList->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> item;
        list->Get(i, (IInterface**)&item);
        SmartDialMatchPosition* m = (SmartDialMatchPosition*)item;
        Logger::D(TAG, "[%d,%d]", m->mStart , m->mEnd);
    }
}

} // Dialpad
} // Dialer
} // Apps
} // Elastos
