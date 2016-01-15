
#ifndef __ELASTOS_DROID_VIEW_INPUTMETHOD_CCOMPLETIONINFO_H__
#define __ELASTOS_DROID_VIEW_INPUTMETHOD_CCOMPLETIONINFO_H__

#include "_Elastos_Droid_View_InputMethod_CCompletionInfo.h"
#include <elastos/core/Object.h>

using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace View {
namespace InputMethod {

CarClass(CCompletionInfo)
    , public Object
    , public ICompletionInfo
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CCompletionInfo();

    CARAPI constructor();

    /**
     * Create a simple completion with just text, no label.
     */
    CARAPI constructor(
        /* [in] */ Int64 id,
        /* [in] */ Int32 index,
        /* [in] */ ICharSequence* text);

    /**
     * Create a full completion with both text and label.
     */
    CARAPI constructor(
        /* [in] */ Int64 id,
        /* [in] */ Int32 index,
        /* [in] */ ICharSequence* text,
        /* [in] */ ICharSequence* label);


    /**
     * Return the abstract identifier for this completion, typically
     * corresponding to the id associated with it in the original adapter.
     */
    CARAPI GetId(
        /* [out] */ Int64* id);

    /**
     * Return the original position of this completion, typically
     * corresponding to its position in the original adapter.
     */
    CARAPI GetPosition(
        /* [out] */ Int32* pos);

    /**
     * Return the actual text associated with this completion.  This is the
     * real text that will be inserted into the editor if the user selects it.
     */
    CARAPI GetText(
        /* [out] */ ICharSequence** text);

    /**
     * Return the user-visible label for the completion, or null if the plain
     * text should be shown.  If non-null, this will be what the user sees as
     * the completion option instead of the actual text.
     */
    CARAPI GetLabel(
        /* [out] */ ICharSequence** label);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel *source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel *dest);

private:
    Int64 mId;
    Int32 mPosition;
    AutoPtr<ICharSequence> mText;
    AutoPtr<ICharSequence> mLabel;
};

} // namespace InputMethod
} // namespace View
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_VIEW_INPUTMETHOD_CCOMPLETIONINFO_H__
