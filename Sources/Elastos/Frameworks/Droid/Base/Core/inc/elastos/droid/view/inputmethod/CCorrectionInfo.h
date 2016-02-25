
#ifndef __ELASTOS_DROID_VIEW_INPUTMETHOD_CCORRECTIONINFO_H__
#define __ELASTOS_DROID_VIEW_INPUTMETHOD_CCORRECTIONINFO_H__

#include "_Elastos_Droid_View_InputMethod_CCorrectionInfo.h"
#include <elastos/core/Object.h>

using Elastos::Core::ICharSequence;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace View {
namespace InputMethod {

CarClass(CCorrectionInfo)
    , public Object
    , public ICorrectionInfo
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CCorrectionInfo();

    CARAPI constructor();

    /**
     * @param offset The offset in the edited text where the old and new text start.
     * @param oldText The old text that has been replaced.
     * @param newText The replacement text.
     */
    CARAPI constructor(
        /* [in] */ Int32 offset,
        /* [in] */ ICharSequence* oldText,
        /* [in] */ ICharSequence* newText);

    /**
     * Return the offset position of this correction in the text. Both the {@link #getOldText()} and
     * {@link #getNewText()} start at this offset.
     */
    CARAPI GetOffset(
        /* [out] */ Int32* offset);

    /**
     * Return the text that has actually been typed by the user, and which has been corrected.
     */
    CARAPI GetOldText(
        /* [out] */ ICharSequence** text);

    /**
     * Return the new text that corrects what was typed by the user.
     */
    CARAPI GetNewText(
        /* [out] */ ICharSequence** text);

    // @Override
    CARAPI ToString(
        /* [out] */ String* result);

    /**
     * Used to package this object into a {@link Parcel}.
     *
     * @param dest The {@link Parcel} to be written.
     * @param flags The flags used for parceling.
     */
    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

private:
    Int32 mOffset;
    AutoPtr<ICharSequence> mOldText;
    AutoPtr<ICharSequence> mNewText;
};

} // namespace InputMethod
} // namespace View
} // namespace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_VIEW_INPUTMETHOD_CCORRECTIONINFO_H__
