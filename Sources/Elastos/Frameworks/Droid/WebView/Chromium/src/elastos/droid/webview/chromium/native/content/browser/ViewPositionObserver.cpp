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

#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/webkit/webview/chromium/native/content/browser/ViewPositionObserver.h"

using Elastos::Droid::View::EIID_IOnPreDrawListener;
using Elastos::Droid::View::IViewTreeObserver;
using Elastos::Utility::EIID_IIterable;
using Elastos::Utility::IIterable;
using Elastos::Utility::IIterator;
using Elastos::Utility::CArrayList;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {

//=====================================================================
//     ViewPositionObserver::InnerViewTreeObserverOnPreDrawListener
//=====================================================================

CAR_INTERFACE_IMPL(ViewPositionObserver::InnerViewTreeObserverOnPreDrawListener, Object, IOnPreDrawListener);

ViewPositionObserver::InnerViewTreeObserverOnPreDrawListener::InnerViewTreeObserverOnPreDrawListener(
    /* [in] */ ViewPositionObserver* owner)
    : mOwner(owner)
{
}

ECode ViewPositionObserver::InnerViewTreeObserverOnPreDrawListener::OnPreDraw(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    mOwner->UpdatePosition();
    *result = TRUE;
    return NOERROR;
}

//=====================================================================
//                         ViewPositionObserver
//=====================================================================
ViewPositionObserver::ViewPositionObserver(
    /* [in] */ IView* view)
    : mView(view)
{
    CArrayList::New((IArrayList**)&mListeners);
    UpdatePosition();
    mPreDrawListener = new InnerViewTreeObserverOnPreDrawListener(this);
}

Int32 ViewPositionObserver::GetPositionX()
{
    // The stored position may be out-of-date. Get the real current position.
    UpdatePosition();
    return (*mPosition)[0];
}

Int32 ViewPositionObserver::GetPositionY()
{
    // The stored position may be out-of-date. Get the real current position.
    UpdatePosition();
    return (*mPosition)[1];
}

ECode ViewPositionObserver::AddListener(
    /* [in] */ Listener* listener)
{
    Boolean bContains;
    mListeners->Contains((IObject*)listener, &bContains);
    if (bContains) return NOERROR;

    Boolean bIsEmpty;
    mListeners->IsEmpty(&bIsEmpty);
    if (bIsEmpty) {
        AutoPtr<IViewTreeObserver> observer;
        mView->GetViewTreeObserver((IViewTreeObserver**)&observer);
        observer->AddOnPreDrawListener(mPreDrawListener);
        UpdatePosition();
    }

    Boolean result;
    return mListeners->Add((IObject*)listener, &result);
}

ECode ViewPositionObserver::RemoveListener(
    /* [in] */ Listener* listener)
{
    Boolean bContains;
    mListeners->Contains((IObject*)listener, &bContains);
    if (!bContains) return NOERROR;

    Boolean modified;
    mListeners->Remove((IObject*)listener, &modified);

    Boolean bIsEmpty;
    mListeners->IsEmpty(&bIsEmpty);
    if (bIsEmpty) {
        AutoPtr<IViewTreeObserver> observer;
        mView->GetViewTreeObserver((IViewTreeObserver**)&observer);
        observer->RemoveOnPreDrawListener(mPreDrawListener);
    }

    return NOERROR;
}

AutoPtr< ArrayOf<Int32> > ViewPositionObserver::MiddleInitMposition()
{
    return ArrayOf<Int32>::Alloc(2);
}

ECode ViewPositionObserver::NotifyListeners()
{
    AutoPtr<IIterable> iterable = IIterable::Probe(mListeners);
    AutoPtr<IIterator> iterator;
    iterable->GetIterator((IIterator**)&iterator);
    Boolean bNext = FALSE;
    for (iterator->HasNext(&bNext); bNext; iterator->HasNext(&bNext)) {
        AutoPtr<IInterface> listenerObj;
        iterator->GetNext((IInterface**)&listenerObj);
        Listener* listener = (Listener*)(IObject::Probe(listenerObj));
        listener->OnPositionChanged((*mPosition)[0], (*mPosition)[1]);
    }

    return NOERROR;
}

ECode ViewPositionObserver::UpdatePosition()
{
    Int32 previousPositionX = (*mPosition)[0];
    Int32 previousPositionY = (*mPosition)[1];
    mView->GetLocationInWindow(mPosition);
    if ((*mPosition)[0] != previousPositionX || (*mPosition)[1] != previousPositionY) {
        NotifyListeners();
    }

    return NOERROR;
}

} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
