
#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_INPUT_DATETIMECHOOSERANDROID_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_INPUT_DATETIMECHOOSERANDROID_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/input/InputDialogContainer.h"

using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {

class ContentViewCore;

namespace Input {

/**
 * Plumbing for the different date/time dialog adapters.
 */
//@JNINamespace("content")
class DateTimeChooserElastos
    : public Object
{
private:
    class InnerInputActionDelegate
        : public InputDialogContainer::InputActionDelegate
    {
    public:
        InnerInputActionDelegate(
            /* [in] */ DateTimeChooserElastos* owner);

        //@Override
        CARAPI_(void) ReplaceDateTime(
            /* [in] */ Double value);

        //@Override
        CARAPI_(void) CancelDateTimeDialog();

    private:
        DateTimeChooserElastos* mOwner;
    };

public:
    static CARAPI_(void*) ElaDateTimeChooserElastosCallback_Init();

private:
    static CARAPI_(AutoPtr<IInterface>) CreateDateTimeChooser(
        /* [in] */ IInterface* contentViewCore,
        /* [in] */ Handle64 nativeDateTimeChooserElastos,
        /* [in] */ Int32 dialogType,
        /* [in] */ Double dialogValue,
        /* [in] */ Double min,
        /* [in] */ Double max,
        /* [in] */ Double step,
        /* [in] */ ArrayOf<IInterface*>* suggestions);

    DateTimeChooserElastos(
        /* [in] */ IContext* context,
        /* [in] */ Handle64 nativeDateTimeChooserElastos);

    CARAPI_(void) ShowDialog(
        /* [in] */ Int32 dialogType,
        /* [in] */ Double dialogValue,
        /* [in] */ Double min,
        /* [in] */ Double max,
        /* [in] */ Double step,
        /* [in] */ ArrayOf<IInterface*>* suggestions);

    //@CalledByNative
    static CARAPI_(AutoPtr<DateTimeChooserElastos>) CreateDateTimeChooser(
        /* [in] */ ContentViewCore* contentViewCore,
        /* [in] */ Handle64 nativeDateTimeChooserElastos,
        /* [in] */ Int32 dialogType,
        /* [in] */ Double dialogValue,
        /* [in] */ Double min,
        /* [in] */ Double max,
        /* [in] */ Double step,
        /* [in] */ ArrayOf<IInterface*>* suggestions);

    //@CalledByNative return DateTimeSuggestion
    static CARAPI_(AutoPtr<ArrayOf<IInterface*> >) CreateSuggestionsArray(
        /* [in] */ Int32 size);

    /**
     * @param array DateTimeSuggestion array that should get a new suggestion set.
     * @param index Index in the array where to place a new suggestion.
     * @param value Value of the suggestion.
     * @param localizedValue Localized value of the suggestion.
     * @param label Label of the suggestion.
     */
    //@CalledByNative
    static CARAPI_(void) SetDateTimeSuggestionAt(
        /* [in] */ ArrayOf<IInterface*>* array,
        /* [in] */ Int32 index,
        /* [in] */ Double value,
        /* [in] */ const String& localizedValue,
        /* [in] */ const String& label);

    //@CalledByNative
    static CARAPI_(void) InitializeDateInputTypes(
        /* [in] */ Int32 textInputTypeDate,
        /* [in] */ Int32 textInputTypeDateTime,
        /* [in] */ Int32 textInputTypeDateTimeLocal,
        /* [in] */ Int32 textInputTypeMonth,
        /* [in] */ Int32 textInputTypeTime,
        /* [in] */ Int32 textInputTypeWeek);

    CARAPI_(void) NativeReplaceDateTime(
        /* [in] */ Handle64 nativeDateTimeChooserElastos,
        /* [in] */ Double dialogValue);

    CARAPI_(void) NativeCancelDialog(
        /* [in] */ Handle64 nativeDateTimeChooserElastos);

private:
    const Handle64 mNativeDateTimeChooserElastos;
    AutoPtr<InputDialogContainer> mInputDialogContainer;
};

} // namespace Input
} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_INPUT_DATETIMECHOOSERANDROID_H__
