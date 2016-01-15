
#ifndef __ELASTOS_DROID_VIEW_INPUTMETHOD_CEXTRACTEDTEXTREQUEST_H__
#define __ELASTOS_DROID_VIEW_INPUTMETHOD_CEXTRACTEDTEXTREQUEST_H__

#include "_Elastos_Droid_View_InputMethod_CExtractedTextRequest.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace View {
namespace InputMethod {

CarClass(CExtractedTextRequest)
    , public Object
    , public IExtractedTextRequest
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CExtractedTextRequest();

    CARAPI constructor();

    CARAPI GetToken(
        /* [out] */ Int32* token);

    CARAPI SetToken(
        /* [in] */ Int32 token);

    CARAPI GetFlags(
        /* [out] */ Int32* flags);

    CARAPI SetFlags(
        /* [in] */ Int32 flags);

    CARAPI GetHintMaxLines(
        /* [out] */ Int32* hintMaxLines);

    CARAPI SetHintMaxLines(
        /* [in] */ Int32 hintMaxLines);

    CARAPI GetHintMaxChars(
        /* [out] */ Int32* hintMaxChars);

    CARAPI SetHintMaxChars(
        /* [in] */ Int32 hintMaxChars);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel *source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel *dest);

public:
    /**
     * Arbitrary integer that can be supplied in the request, which will be
     * delivered back when reporting updates.
     */
    Int32 mToken;

    /**
     * Additional request flags, having the same possible values as the
     * flags parameter of {@link InputConnection#getTextBeforeCursor
     * InputConnection.getTextBeforeCursor()}.
     */
    Int32 mFlags;

    /**
     * Hint for the maximum number of lines to return.
     */
    Int32 mHintMaxLines;

    /**
     * Hint for the maximum number of characters to return.
     */
    Int32 mHintMaxChars;
};

} // namespace InputMethod
} // namespace View
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_VIEW_INPUTMETHOD_CEXTRACTEDTEXTREQUEST_H__
