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

AutoPtr<Learner> GestureLibrary::GetLearner()
{
    return ((CGestureStore*)mStore.Get())->GetLearner();
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
