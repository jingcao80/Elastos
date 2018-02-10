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

#include "elastos/droid/webkit/webview/chromium/native/content/browser/input/DateTimeChooserElastos.h"
#include "elastos/droid/webkit/webview/chromium/native/content/api/DateTimeChooserElastos_dec.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/input/DateTimeSuggestion.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/ContentViewCore.h"

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {
namespace Input {

//==================================================================
//        DateTimeChooserElastos::InnerInputActionDelegate
//==================================================================

DateTimeChooserElastos::InnerInputActionDelegate::InnerInputActionDelegate(
    /* [in] */ DateTimeChooserElastos* owner)
    : mOwner(owner)
{
}

//@Override
void DateTimeChooserElastos::InnerInputActionDelegate::ReplaceDateTime(
    /* [in] */ Double value)
{
    mOwner->NativeReplaceDateTime(mOwner->mNativeDateTimeChooserElastos, value);
}

//@Override
void DateTimeChooserElastos::InnerInputActionDelegate::CancelDateTimeDialog()
{
    mOwner->NativeCancelDialog(mOwner->mNativeDateTimeChooserElastos);
}

//==================================================================
//                     DateTimeChooserElastos
//==================================================================

DateTimeChooserElastos::DateTimeChooserElastos(
    /* [in] */ IContext* context,
    /* [in] */ HANDLE nativeDateTimeChooserElastos)
    : mNativeDateTimeChooserElastos(nativeDateTimeChooserElastos)
{
    AutoPtr<InputDialogContainer::InputActionDelegate> delegate = new InnerInputActionDelegate(this);
    mInputDialogContainer = new InputDialogContainer(context, delegate);
}

void DateTimeChooserElastos::ShowDialog(
    /* [in] */ Int32 dialogType,
    /* [in] */ Double dialogValue,
    /* [in] */ Double min,
    /* [in] */ Double max,
    /* [in] */ Double step,
    /* [in] */ ArrayOf<IInterface*>* suggestions)
{
    mInputDialogContainer->ShowDialog(dialogType, dialogValue, min, max, step, suggestions);
}

//@CalledByNative
AutoPtr<DateTimeChooserElastos> DateTimeChooserElastos::CreateDateTimeChooser(
    /* [in] */ ContentViewCore* contentViewCore,
    /* [in] */ HANDLE nativeDateTimeChooserElastos,
    /* [in] */ Int32 dialogType,
    /* [in] */ Double dialogValue,
    /* [in] */ Double min,
    /* [in] */ Double max,
    /* [in] */ Double step,
    /* [in] */ ArrayOf<IInterface*>* suggestions)
{
    AutoPtr<DateTimeChooserElastos> chooser =
            new DateTimeChooserElastos(
                    contentViewCore->GetContext(),
                    nativeDateTimeChooserElastos);
    chooser->ShowDialog(dialogType, dialogValue, min, max, step, suggestions);

    return chooser;
}

//@CalledByNative
AutoPtr<ArrayOf<IInterface*> > DateTimeChooserElastos::CreateSuggestionsArray(
    /* [in] */ Int32 size)
{
    AutoPtr<ArrayOf<IInterface*> > array = ArrayOf<IInterface*>::Alloc(size);
    return array;
}

/**
 * @param array DateTimeSuggestion array that should get a new suggestion set.
 * @param index Index in the array where to place a new suggestion.
 * @param value Value of the suggestion.
 * @param localizedValue Localized value of the suggestion.
 * @param label Label of the suggestion.
 */
//@CalledByNative
void DateTimeChooserElastos::SetDateTimeSuggestionAt(
    /* [in] */ ArrayOf<IInterface*>* array,
    /* [in] */ Int32 index,
    /* [in] */ Double value,
    /* [in] */ const String& localizedValue,
    /* [in] */ const String& label)
{
    //(*array)[index] = new DateTimeSuggestion(value, localizedValue, label);
    array->Set(index, TO_IINTERFACE(new DateTimeSuggestion(value, localizedValue, label)));
}

//@CalledByNative
void DateTimeChooserElastos::InitializeDateInputTypes(
    /* [in] */ Int32 textInputTypeDate,
    /* [in] */ Int32 textInputTypeDateTime,
    /* [in] */ Int32 textInputTypeDateTimeLocal,
    /* [in] */ Int32 textInputTypeMonth,
    /* [in] */ Int32 textInputTypeTime,
    /* [in] */ Int32 textInputTypeWeek)
{
    InputDialogContainer::InitializeInputTypes(
            textInputTypeDate,
            textInputTypeDateTime, textInputTypeDateTimeLocal,
            textInputTypeMonth, textInputTypeTime, textInputTypeWeek);
}

void DateTimeChooserElastos::NativeReplaceDateTime(
    /* [in] */ HANDLE nativeDateTimeChooserElastos,
    /* [in] */ Double dialogValue)
{
    Elastos_DateTimeChooserAndroid_nativeReplaceDateTime(TO_IINTERFACE(this), (HANDLE)nativeDateTimeChooserElastos, dialogValue);
}

void DateTimeChooserElastos::NativeCancelDialog(
    /* [in] */ HANDLE nativeDateTimeChooserElastos)
{
    Elastos_DateTimeChooserAndroid_nativeCancelDialog(TO_IINTERFACE(this), (HANDLE)nativeDateTimeChooserElastos);
}

AutoPtr<IInterface> DateTimeChooserElastos::CreateDateTimeChooser(
    /* [in] */ IInterface* contentViewCore,
    /* [in] */ HANDLE nativeDateTimeChooserElastos,
    /* [in] */ Int32 dialogType,
    /* [in] */ Double dialogValue,
    /* [in] */ Double min,
    /* [in] */ Double max,
    /* [in] */ Double step,
    /* [in] */ ArrayOf<IInterface*>* suggestions)
{
    AutoPtr<ContentViewCore> cvc = (ContentViewCore*)(IObject::Probe(contentViewCore));
    return TO_IINTERFACE(CreateDateTimeChooser(cvc, nativeDateTimeChooserElastos,
                dialogType, dialogValue, min, max, step, suggestions));
}

} // namespace Input
} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
