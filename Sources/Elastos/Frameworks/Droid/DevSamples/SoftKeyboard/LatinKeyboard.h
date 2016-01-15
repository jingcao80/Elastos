
#ifndef __LATINKEYBOARD_H__
#define __LATINKEYBOARD_H__

#include <inputmethodservice/Keyboard.h>

using Elastos::Droid::InputMethodService::Keyboard;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace SoftKeyboard {

class LatinKeyboard : public Keyboard
{
public:
    class LatinKey : public Keyboard::Key
    {
    public:
        LatinKey(
            /* [in] */ IResources* res,
            /* [in] */ Row* parent,
            /* [in] */ Int32 x,
            /* [in] */ Int32 y,
            /* [in] */ IXmlResourceParser* parser);

        CARAPI IsInside(
            /* [in] */ Int32 x,
            /* [in] */ Int32 y,
            /* [out] */ Boolean* isInside);
    };

public:
    LatinKeyboard(
        /* [in] */ IContext* ctx,
        /* [in] */ Int32 xmlLayoutResId);

    LatinKeyboard(
        /* [in] */ IContext* ctx,
        /* [in] */ Int32 layoutTemplateResId,
        /* [in] */ ICharSequence* characters,
        /* [in] */ Int32 columns,
        /* [in] */ Int32 horizontalPadding);

    CARAPI SetImeOptions(
        /* [in] */ IResources* res,
        /* [in] */ Int32 options);

protected:
    CARAPI_(AutoPtr<Keyboard::Key>) CreateKeyFromXml(
        /* [in] */ IResources* res,
        /* [in] */ Row* parent,
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ IXmlResourceParser* parser);

private:
    AutoPtr<Key>  mEnterKey;
};

} // namespace SoftKeyboard
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos

#endif // __LATINKEYBOARD_H__
