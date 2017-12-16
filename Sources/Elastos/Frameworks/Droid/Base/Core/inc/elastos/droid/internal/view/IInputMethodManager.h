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

#ifndef __ELASTOS_DROID_INTERNAL_VIEW_IINPUTMETHODMANAGER_H__
#define __ELASTOS_DROID_INTERNAL_VIEW_IINPUTMETHODMANAGER_H__

#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Text.h"
#include "Elastos.Droid.View.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/core/Object.h>
#include <binder/Binder.h>

using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IResultReceiver;
using Elastos::Droid::Text::Style::ISuggestionSpan;
using Elastos::Droid::View::InputMethod::IEditorInfo;
using Elastos::Droid::View::InputMethod::IInputMethodSubtype;
using Elastos::Core::Object;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace View {

class IInputMethodManagerProxy
    : public Object
    , public IIInputMethodManager
{
public:
    IInputMethodManagerProxy(
        /* [in] */ android::sp<android::IBinder>& remote);

    CAR_INTERFACE_DECL();

    CARAPI GetInputMethodList(
        /* [out] */ IList** infos);

    CARAPI GetEnabledInputMethodList(
        /* [out] */ IList** infos);

    CARAPI GetEnabledInputMethodSubtypeList(
        /* [in] */ const String& imiId,
        /* [in] */ Boolean allowsImplicitlySelectedSubtypes,
        /* [out] */ IList** infos);

    CARAPI GetLastInputMethodSubtype(
        /* [out] */ IInputMethodSubtype** subType);

    CARAPI GetShortcutInputMethodsAndSubtypes(
        /* [out] */ IList** subtypes);

    CARAPI AddClient(
        /* [in] */ IInputMethodClient* client,
        /* [in] */ IIInputContext* inputContext,
        /* [in] */ Int32 uid,
        /* [in] */ Int32 pid);

    CARAPI RemoveClient(
        /* [in] */ IInputMethodClient* client);

    CARAPI FinishInput(
        /* [in] */ IInputMethodClient* client);

    CARAPI ShowSoftInput(
        /* [in] */ IInputMethodClient* client,
        /* [in] */ Int32 flags,
        /* [in] */ IResultReceiver* resultReceiver,
        /* [out] */ Boolean* res);

    CARAPI HideSoftInput(
        /* [in] */ IInputMethodClient* client,
        /* [in] */ Int32 flags,
        /* [in] */ IResultReceiver* resultReceiver,
        /* [out] */ Boolean* res);

    CARAPI StartInputOrWindowGainedFocus(
        /* [in] */ Int32 startInputReason,
        /* [in] */ IInputMethodClient* client,
        /* [in] */ IBinder* windowToken,
        /* [in] */ Int32 controlFlags,
        /* [in] */ Int32 softInputMode,
        /* [in] */ Int32 windowFlags,
        /* [in] */ IEditorInfo* attribute,
        /* [in] */ IIInputContext* inputContext,
        /* [out] */ IInputBindResult** result);

    CARAPI ShowInputMethodPickerFromClient(
        /* [in] */ IInputMethodClient* client);

    CARAPI ShowInputMethodAndSubtypeEnablerFromClient(
        /* [in] */ IInputMethodClient* client,
        /* [in] */ const String& topId);

    CARAPI SetInputMethod(
        /* [in] */ IBinder* token,
        /* [in] */ const String& id);

    CARAPI SetInputMethodAndSubtype(
        /* [in] */ IBinder* token,
        /* [in] */ const String& id,
        /* [in] */ IInputMethodSubtype* subtype);

    CARAPI HideMySoftInput(
        /* [in] */ IBinder* token,
        /* [in] */ Int32 flags);

    CARAPI ShowMySoftInput(
        /* [in] */ IBinder* token,
        /* [in] */ Int32 flags);

    CARAPI UpdateStatusIcon(
        /* [in] */ IBinder* token,
        /* [in] */ const String& capsuleName,
        /* [in] */ Int32 iconId);

    CARAPI SetImeWindowStatus(
        /* [in] */ IBinder* token,
        /* [in] */ Int32 vis,
        /* [in] */ Int32 backDisposition);

    CARAPI RegisterSuggestionSpansForNotification(
        /* [in] */ ArrayOf<ISuggestionSpan*>* spans);

    CARAPI NotifySuggestionPicked(
        /* [in] */ ISuggestionSpan* span,
        /* [in] */ const String& originalString,
        /* [in] */ Int32 index,
        /* [out] */ Boolean* picked);

    CARAPI GetCurrentInputMethodSubtype(
        /* [out] */ IInputMethodSubtype** subType);

    CARAPI SetCurrentInputMethodSubtype(
        /* [in] */ IInputMethodSubtype* subtype,
        /* [out] */ Boolean* result);

    CARAPI SwitchToLastInputMethod(
        /* [in] */ IBinder* token,
        /* [out] */ Boolean* result);

    CARAPI SwitchToNextInputMethod(
        /* [in] */ IBinder* token,
        /* [in] */ Boolean onlyCurrentIme,
        /* [out] */ Boolean* switched);

    CARAPI ShouldOfferSwitchingToNextInputMethod(
        /* [in] */ IBinder* token,
        /* [out] */ Boolean* result);

    CARAPI SetInputMethodEnabled(
        /* [in] */ const String& id,
        /* [in] */ Boolean enabled,
        /* [out] */ Boolean* res);

    CARAPI SetAdditionalInputMethodSubtypes(
        /* [in] */ const String& id,
        /* [in] */ ArrayOf<IInputMethodSubtype*>* subtypes);

    CARAPI GetInputMethodWindowVisibleHeight(
        /* [out] */ Int32* height);

    CARAPI NotifyUserAction(
        /* [in] */ Int32 sequenceNumber);

private:
    static const String DESCRIPTOR;
    static const Int32 TRANSACTION_finishInput;
    static const Int32 TRANSACTION_startInputOrWindowGainedFocus;

    android::sp<android::IBinder> mRemote;
};

} // namespace View
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_VIEW_IINPUTMETHODMANAGER_H__
