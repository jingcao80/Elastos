
#ifndef __ELASTOS_DROID_VIEW_INPUTMETHOD_CEDITORINFO_H__
#define __ELASTOS_DROID_VIEW_INPUTMETHOD_CEDITORINFO_H__

#include "_Elastos_Droid_View_InputMethod_CEditorInfo.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Text::IInputType;

using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace View {
namespace InputMethod {

CarClass(CEditorInfo)
    , public Object
    , public IEditorInfo
    , public IInputType
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CEditorInfo();

    ~CEditorInfo();

    CARAPI constructor();

    CARAPI ReadFromParcel(
        /* [in] */ IParcel *source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel *dest);

    CARAPI GetInputType(
        /* [out] */ Int32* type);

    CARAPI SetInputType(
        /* [in] */ Int32 type);

    CARAPI GetImeOptions(
        /* [out] */ Int32* options);

    CARAPI SetImeOptions(
        /* [in] */ Int32 options);

    CARAPI GetPrivateImeOptions(
        /* [out] */ String* options);

    CARAPI SetPrivateImeOptions(
        /* [in] */ const String& options);

    CARAPI GetActionLabel(
        /* [out] */ ICharSequence** actionLabel);

    CARAPI SetActionLabel(
        /*[in]  */ ICharSequence* actionLabel);

    CARAPI GetActionId(
        /* [out] */ Int32* actionId);

    CARAPI SetActionId(
        /* [in] */ Int32 actionId);

    CARAPI GetInitialSelStart(
        /* [out] */ Int32* initialSelStart);

    CARAPI SetInitialSelStart(
        /* [in] */ Int32 initialSelStart);

    CARAPI GetInitialSelEnd(
        /* [out] */ Int32* initialSelEnd);

    CARAPI SetInitialSelEnd(
        /* [in] */ Int32 initialSelEnd);

    CARAPI GetInitialCapsMode(
        /* [out] */ Int32* initialCapsMode);

    CARAPI SetInitialCapsMode(
        /* [in] */ Int32 initialCapsMode);

    CARAPI GetHintText(
        /* [out] */ ICharSequence** hintText);

    CARAPI SetHintText(
        /* [in] */ ICharSequence* hintText);

    CARAPI GetLabel(
        /* [out] */ ICharSequence** label);

    CARAPI SetLabel(
        /* [in] */ ICharSequence* label);

    CARAPI GetPackageName(
        /* [out] */ String* packageName);

    CARAPI SetPackageName(
        /* [in] */ const String& packageName);

    CARAPI GetFieldId(
        /* [out] */ Int32* fieldId);

    CARAPI SetFieldId(
        /* [in] */ Int32 fieldId);

    CARAPI GetFieldName(
        /* [out] */ String* fieldName);

    CARAPI SetFieldName(
        /* [in] */ const String& fieldName);

    CARAPI GetExtras(
        /* [out] */ IBundle** extras);

    CARAPI SetExtras(
        /* [in] */ IBundle* extras);

    CARAPI MakeCompatible(
        /* [in] */ Int32 targetSdkVersion);

public:
    /**
     * The content type of the text box, whose bits are defined by
     * {@link InputType}.
     *
     * @see InputType
     * @see #TYPE_MASK_CLASS
     * @see #TYPE_MASK_VARIATION
     * @see #TYPE_MASK_FLAGS
     */
    Int32 mInputType;

    /**
     * Extended type information for the editor, to help the IME better
     * integrate with it.
     */
    Int32 mImeOptions;

    /**
     * A string supplying additional information options that are
     * private to a particular IME implementation.  The string must be
     * scoped to a package owned by the implementation, to ensure there are
     * no conflicts between implementations, but other than that you can put
     * whatever you want in it to communicate with the IME.  For example,
     * you could have a string that supplies an argument like
     * <code>"com.example.myapp.SpecialMode=3"</code>.  This field is can be
     * filled in from the {@link android.R.attr#privateImeOptions}
     * attribute of a TextView.
     */
    String mPrivateImeOptions;

    /**
     * In some cases an IME may be able to display an arbitrary label for
     * a command the user can perform, which you can specify here.  You can
     * not count on this being used.
     */
    AutoPtr<ICharSequence> mActionLabel;

    /**
     * If {@link #actionLabel} has been given, this is the id for that command
     * when the user presses its button that is delivered back with
     * {@link InputConnection#performEditorAction(int)
     * InputConnection.performEditorAction()}.
     */
    Int32 mActionId;

    /**
     * The text offset of the start of the selection at the time editing
     * began; -1 if not known.
     */
    Int32 mInitialSelStart;

    /**
     * The text offset of the end of the selection at the time editing
     * began; -1 if not known.
     */
    Int32 mInitialSelEnd;

    /**
     * The capitalization mode of the first character being edited in the
     * text.  Values may be any combination of
     * {@link TextUtils#CAP_MODE_CHARACTERS TextUtils.CAP_MODE_CHARACTERS},
     * {@link TextUtils#CAP_MODE_WORDS TextUtils.CAP_MODE_WORDS}, and
     * {@link TextUtils#CAP_MODE_SENTENCES TextUtils.CAP_MODE_SENTENCES}, though
     * you should generally just take a non-zero value to mean start out in
     * caps mode.
     */
    Int32 mInitialCapsMode;

    /**
     * The "hint" text of the text view, typically shown in-line when the
     * text is empty to tell the user what to enter.
     */
    AutoPtr<ICharSequence> mHintText;

    /**
     * A label to show to the user describing the text they are writing.
     */
    AutoPtr<ICharSequence> mLabel;

    /**
     * Name of the package that owns this editor.
     */
    String mPackageName;

    /**
     * Identifier for the editor's field.  This is optional, and may be
     * 0.  By default it is filled in with the result of
     * {@link android.view.View#getId() View.getId()} on the View that
     * is being edited.
     */
    Int32 mFieldId;

    /**
     * Additional name for the editor's field.  This can supply additional
     * name information for the field.  By default it is null.  The actual
     * contents have no meaning.
     */
    String mFieldName;

    /**
     * Any extra data to supply to the input method.  This is for extended
     * communication with specific input methods; the name fields in the
     * bundle should be scoped (such as "com.mydomain.im.SOME_FIELD") so
     * that they don't conflict with others.  This field is can be
     * filled in from the {@link android.R.attr#editorExtras}
     * attribute of a TextView.
     */
    AutoPtr<IBundle> mExtras;
};

} // namespace InputMethod
} // namespace View
} // namespace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_VIEW_INPUTMETHOD_CEDITORINFO_H__
