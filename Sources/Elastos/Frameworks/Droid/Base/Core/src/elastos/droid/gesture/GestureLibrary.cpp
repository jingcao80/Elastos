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

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/gesture/GestureLibrary.h"
#include "elastos/droid/gesture/CGestureStore.h"

namespace Elastos {
namespace Droid {
namespace Gesture {

CAR_INTERFACE_IMPL(GestureLibrary, Object, IGestureLibrary);

GestureLibrary::GestureLibrary()
{
    CGestureStore::NewByFriend((CGestureStore**)&mStore);
}

GestureLibrary::~GestureLibrary()
{}

ECode GestureLibrary::IsReadOnly(
    /* [out] */ Boolean *isReadOnly)
{
    VALIDATE_NOT_NULL(isReadOnly);
    *isReadOnly = FALSE;
    return NOERROR;
}

ECode GestureLibrary::GetLearner(
    /* [out] */ ILearner** learner)
{
    VALIDATE_NOT_NULL(learner);
    AutoPtr<Learner> ln = ((CGestureStore*)mStore.Get())->GetLearner();
    *learner = ln;
    REFCOUNT_ADD(*learner);
    return NOERROR;
}

ECode GestureLibrary::SetOrientationStyle(
    /* [in] */ Int32 style)
{
    return mStore->SetOrientationStyle(style);
}

ECode GestureLibrary::GetOrientationStyle(
    /* [out] */ Int32 *style)
{
    VALIDATE_NOT_NULL(style);
    return mStore->GetOrientationStyle(style);
}

ECode GestureLibrary::SetSequenceType(
    /* [in] */ Int32 type)
{
    return mStore->SetSequenceType(type);
}

ECode GestureLibrary::GetSequenceType(
    /* [out] */ Int32 *type)
{
    VALIDATE_NOT_NULL(type);
    return mStore->GetSequenceType(type);
}

ECode GestureLibrary::GetGestureEntries(
    /* [out] */ IList **entries)
{
    return mStore->GetGestureEntries(entries);
}

ECode GestureLibrary::Recognize(
    /* [in] */ IGesture *gesture,
    /* [out] */ IArrayList** list)
{
    return mStore->Recognize(gesture, list);
}

ECode GestureLibrary::AddGesture(
    /* [in] */  const String& entryName,
    /* [in] */  IGesture *gesture)
{
    return mStore->AddGesture(entryName, gesture);
}

ECode GestureLibrary::RemoveGesture(
    /* [in] */ const String& entryName,
    /* [in] */ IGesture *gesture)
{
    return mStore->RemoveGesture(entryName, gesture);
}

ECode GestureLibrary::RemoveEntry(
    /* [in] */ const String& entryName)
{
    return mStore->RemoveEntry(entryName);
}

ECode GestureLibrary::GetGestures(
    /* [in] */ const String& entryName,
    /* [out] */ IArrayList** gestures)
{
    return mStore->GetGestures(entryName, gestures);
}

} // namespace Gesture
} // namespace Droid
} // namespace Elastos
