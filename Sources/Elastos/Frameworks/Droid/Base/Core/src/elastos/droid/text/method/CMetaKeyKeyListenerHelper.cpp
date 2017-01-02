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

#include "elastos/droid/text/method/CMetaKeyKeyListenerHelper.h"
#include "elastos/droid/text/method/MetaKeyKeyListener.h"

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

CAR_SINGLETON_IMPL(CMetaKeyKeyListenerHelper)

CAR_INTERFACE_IMPL(CMetaKeyKeyListenerHelper, Singleton, IMetaKeyKeyListenerHelper)

ECode CMetaKeyKeyListenerHelper::ResetMetaState(
    /* [in] */ ISpannable* text)
{
    return MetaKeyKeyListener::ResetMetaState(text);
}

ECode CMetaKeyKeyListenerHelper::GetMetaState(
    /* [in] */ ICharSequence* text,
    /* [out] */ Int32* state)
{
    assert(state != NULL);
    return MetaKeyKeyListener::GetMetaState(text, state);
}

ECode CMetaKeyKeyListenerHelper::GetMetaState(
    /* [in] */ ICharSequence* text,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Int32* ret)
{
    VALIDATE_NOT_NULL(ret)
    return MetaKeyKeyListener::GetMetaState(text, event, ret);
}

ECode CMetaKeyKeyListenerHelper::GetMetaState(
    /* [in] */ ICharSequence* text,
    /* [in] */ Int32 meta,
    /* [out] */ Int32* state)
{
    assert(state != NULL);
    return MetaKeyKeyListener::GetMetaState(text, meta, state);
}

ECode CMetaKeyKeyListenerHelper::GetMetaState(
    /* [in] */ ICharSequence* text,
    /* [in] */ Int32 meta,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Int32* ret)
{
    VALIDATE_NOT_NULL(ret)
    return MetaKeyKeyListener::GetMetaState(text, meta, event, ret);
}

ECode CMetaKeyKeyListenerHelper::GetMetaState(
    /* [in] */ Int64 state,
    /* [out] */ Int32* ret)
{
    assert(ret != NULL);
    return MetaKeyKeyListener::GetMetaState(state, ret);
}

ECode CMetaKeyKeyListenerHelper::GetMetaState(
    /* [in] */ Int64 state,
    /* [in] */ Int32 meta,
    /* [out] */ Int32* ret)
{
    assert(ret != NULL);
    return MetaKeyKeyListener::GetMetaState(state, meta, ret);

}

ECode CMetaKeyKeyListenerHelper::AdjustMetaAfterKeypress(
    /* [in] */ ISpannable* content)
{
    return MetaKeyKeyListener::AdjustMetaAfterKeypress(content);
}

ECode CMetaKeyKeyListenerHelper::IsMetaTracker(
    /* [in] */ ICharSequence* text,
    /* [in] */ IInterface* what,
    /* [out] */ Boolean* state)
{
    return MetaKeyKeyListener::IsMetaTracker(text, what, state);
}

ECode CMetaKeyKeyListenerHelper::IsSelectingMetaTracker(
    /* [in] */ ICharSequence* text,
    /* [in] */ IInterface* what,
    /* [out] */ Boolean* state)
{
    return MetaKeyKeyListener::IsSelectingMetaTracker(text, what, state);
}

ECode CMetaKeyKeyListenerHelper::StartSelecting(
    /* [in] */ IView* view,
    /* [in] */ ISpannable* content)
{
    return MetaKeyKeyListener::StartSelecting(view, content);
}

ECode CMetaKeyKeyListenerHelper::StopSelecting(
    /* [in] */ IView* view,
    /* [in] */ ISpannable* content)
{
    return MetaKeyKeyListener::StartSelecting(view, content);
}

ECode CMetaKeyKeyListenerHelper::ClearMetaKeyState(
    /* [in] */ IEditable* content,
    /* [in] */ Int32 states)
{
    return MetaKeyKeyListener::ClearMetaKeyState(content, states);
}

ECode CMetaKeyKeyListenerHelper::ResetLockedMeta(
    /* [in] */ Int64 state,
    /* [out] */ Int64* meta)
{
    return MetaKeyKeyListener::ResetLockedMeta(state, meta);
}

ECode CMetaKeyKeyListenerHelper::AdjustMetaAfterKeypress(
    /* [in] */ Int64 state,
    /* [out] */ Int64* result)
{
    return MetaKeyKeyListener::AdjustMetaAfterKeypress(state, result);
}

ECode CMetaKeyKeyListenerHelper::HandleKeyDown(
    /* [in] */ Int64 state,
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Int64* result)
{
    return MetaKeyKeyListener::HandleKeyDown(state, keyCode, event, result);
}

ECode CMetaKeyKeyListenerHelper::HandleKeyUp(
    /* [in] */ Int64 state,
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Int64* result)
{
    return MetaKeyKeyListener::HandleKeyUp(state, keyCode, event, result);
}


} // namespace Method
} // namespace Text
} // namepsace Droid
} // namespace Elastos
