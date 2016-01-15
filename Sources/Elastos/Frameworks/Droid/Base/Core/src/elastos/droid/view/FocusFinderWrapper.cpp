
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/FocusFinderwrapper.h"

namespace Elastos {
namespace Droid {
namespace View {

FocusFinderWrapper::FocusFinderWrapper()
{
}

ECode FocusFinderWrapper::constructor(
    /* [in] */ IFocusFinder* finder)
{
    mFocusFinder = finder;
    return NOERROR;
}

Boolean FocusFinderWrapper::IsBetterCandidate(
    /* [in] */ Int32 direction,
    /* [in] */ IRect* source,
    /* [in] */ IRect* rect1,
    /* [in] */ IRect* rect2)
{
    return FIND_PROBE(mFocusFinder)->IsBetterCandidate(direction, source, rect1, rect2);
}

Boolean FocusFinderWrapper::BeamBeats(
    /* [in] */ Int32 direction,
    /* [in] */ IRect* source,
    /* [in] */ IRect* rect1,
    /* [in] */ IRect* rect2)
{
    return FIND_PROBE(mFocusFinder)->BeamBeats(direction, source, rect1, rect2);
}

Boolean FocusFinderWrapper::IsCandidate(
    /* [in] */ IRect* srcRect,
    /* [in] */ IRect* destRect,
    /* [in] */ Int32 direction)
{
    return FIND_PROBE(mFocusFinder)->IsCandidate(srcRect, destRect, direction);
}

Boolean FocusFinderWrapper::BeamsOverlap(
    /* [in] */ Int32 direction,
    /* [in] */ IRect* rect1,
    /* [in] */ IRect* rect2)
{
    return FIND_PROBE(mFocusFinder)->BeamsOverlap(direction, rect1, rect2);
}

Int32 FocusFinderWrapper::MajorAxisDistance(
    /* [in] */ Int32 direction,
    /* [in] */ IRect* source,
    /* [in] */ IRect* dest)
{
    return FocusFinder::MajorAxisDistance(direction, source, dest);
}

Int32 FocusFinderWrapper::MajorAxisDistanceToFarEdge(
    /* [in] */ Int32 direction,
    /* [in] */ IRect* source,
    /* [in] */ IRect* dest)
{
    return FocusFinder::MajorAxisDistanceToFarEdge(direction, source, dest);
}

} // namespace View
} // namespace Droid
} // namespace Elastos
