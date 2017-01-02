//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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
