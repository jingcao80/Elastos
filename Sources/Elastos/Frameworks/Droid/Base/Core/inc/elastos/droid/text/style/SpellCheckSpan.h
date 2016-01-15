#ifndef __ELASTOS_DROID_TEXT_STYLE_SpellCheckSpan_H__
#define __ELASTOS_DROID_TEXT_STYLE_SpellCheckSpan_H__

#include "Elastos.Droid.Text.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Text {
namespace Style {

/**
 * A SpellCheckSpan is an internal data structure created by the TextView's SpellChecker to
 * annotate portions of the text that are about to or currently being spell checked. They are
 * automatically removed once the spell check is completed.
 *
 * @hide
 */
class SpellCheckSpan
    : public Object
    , public ISpellCheckSpan
    , public IParcelableSpan
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    SpellCheckSpan();

    virtual ~SpellCheckSpan();

    CARAPI constructor();

    CARAPI SetSpellCheckInProgress(
        /* [in] */ Boolean inProgress);

    CARAPI IsSpellCheckInProgress(
        /* [out] */ Boolean* result);

    //@Override

    //@Override
    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    //@Override
    CARAPI GetSpanTypeId(
        /* [out] */ Int32* id);

private:
    Boolean mSpellCheckInProgress;
};

} // namespace Style
} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_STYLE_SpellCheckSpan_H__
