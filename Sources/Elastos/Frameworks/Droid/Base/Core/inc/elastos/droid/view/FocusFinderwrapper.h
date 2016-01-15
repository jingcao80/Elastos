#ifndef __ELASTOS_DROID_INTERNAL_VIEW_FOCUSFINDERWRAPPER_H__
#define __ELASTOS_DROID_INTERNAL_VIEW_FOCUSFINDERWRAPPER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/view/FocusFinder.h"

using Elastos::Droid::View::FocusFinder;

namespace Elastos {
namespace Droid {
namespace View {

#ifndef FIND_PROBE
#define FIND_PROBE(host) ((FocusFinder*)IFocusFinder::Probe(host))
#endif

class FocusFinderWrapper
    : public Object
{
public:
    FocusFinderWrapper();

    CARAPI constructor(
        /* [in] */ IFocusFinder* finder);

    CARAPI_(Boolean) IsBetterCandidate(
        /* [in] */ Int32 direction,
        /* [in] */ IRect* source,
        /* [in] */ IRect* rect1,
        /* [in] */ IRect* rect2);

    CARAPI_(Boolean) BeamBeats(
        /* [in] */ Int32 direction,
        /* [in] */ IRect* source,
        /* [in] */ IRect* rect1,
        /* [in] */ IRect* rect2);

    CARAPI_(Boolean) IsCandidate(
        /* [in] */ IRect* srcRect,
        /* [in] */ IRect* destRect,
        /* [in] */ Int32 direction);

    CARAPI_(Boolean) BeamsOverlap(
        /* [in] */ Int32 direction,
        /* [in] */ IRect* rect1,
        /* [in] */ IRect* rect2);

    static CARAPI_(Int32) MajorAxisDistance(
        /* [in] */ Int32 direction,
        /* [in] */ IRect* source,
        /* [in] */ IRect* dest);

    static CARAPI_(Int32) MajorAxisDistanceToFarEdge(
        /* [in] */ Int32 direction,
        /* [in] */ IRect* source,
        /* [in] */ IRect* dest);

private:
    AutoPtr<IFocusFinder> mFocusFinder;
};

} // namespace View
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_VIEW_FOCUSFINDERWRAPPER_H__