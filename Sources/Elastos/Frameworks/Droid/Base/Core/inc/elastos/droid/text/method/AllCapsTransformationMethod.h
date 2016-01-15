#ifndef __ELASTOS_DROID_TEXT_METHOD_ALLCAPSTRANSFORMATIONMETHOD_H__
#define __ELASTOS_DROID_TEXT_METHOD_ALLCAPSTRANSFORMATIONMETHOD_H__

#include "Elastos.Droid.Text.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Utility::ILocale;
using Elastos::Core::ICharSequence;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::View::IView;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

/**
 * Transforms source text into an ALL CAPS string, locale-aware.
 *
 * @hide
 */
class AllCapsTransformationMethod
    : public Object
    , public IAllCapsTransformationMethod
{
public:
    AllCapsTransformationMethod();

    virtual ~AllCapsTransformationMethod();

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context);

    //@Override
    CARAPI GetTransformation(
        /* [in] */ ICharSequence* source,
        /* [in] */ IView* view,
        /* [out] */ ICharSequence** ret);

    //@Override
    CARAPI OnFocusChanged(
        /* [in] */ IView* view,
        /* [in] */ ICharSequence* sourceText,
        /* [in] */ Boolean focused,
        /* [in] */ Int32 direction,
        /* [in] */ IRect* previouslyFocusedRect);

    //@Override
    CARAPI SetLengthChangesAllowed(
        /* [in] */ Boolean allowLengthChanges);


private:
    static const String TAG;

    Boolean mEnabled;
    AutoPtr<ILocale> mLocale;
};


} // namespace Method
} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_METHOD_ALLCAPSTRANSFORMATIONMETHOD_H__
