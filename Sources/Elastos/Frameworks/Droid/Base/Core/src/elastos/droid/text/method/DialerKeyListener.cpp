
#include "Elastos.Droid.View.h"
#include "elastos/droid/text/method/DialerKeyListener.h"
#include "elastos/droid/text/method/CDialerKeyListener.h"
#include "elastos/droid/text/method/MetaKeyKeyListener.h"
#include "elastos/droid/view/KeyCharacterMap.h"


using Elastos::Droid::View::IKeyData;
using Elastos::Droid::View::KeyCharacterMap;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

AutoPtr<IDialerKeyListener> DialerKeyListener::sInstance;

static AutoPtr<ArrayOf<Char32> > InitCHARACTERS()
{
    Char32 ch[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '#', '*', '+', '-', '(', ')', ',', '/', 'N', '.', ' ', ';' };
    AutoPtr<ArrayOf<Char32> > array = ArrayOf<Char32>::Alloc(ArraySize(ch));
    array->Copy(ch, ArraySize(ch));
    return array;
}

const AutoPtr<ArrayOf<Char32> > DialerKeyListener::CHARACTERS = InitCHARACTERS();

DialerKeyListener::DialerKeyListener()
{}

DialerKeyListener::~DialerKeyListener()
{}

ECode DialerKeyListener::constructor()
{
    return NOERROR;
}

CAR_INTERFACE_IMPL(DialerKeyListener, Object, IDialerKeyListener)

AutoPtr< ArrayOf<Char32> > DialerKeyListener::GetAcceptedChars()
{
    return CHARACTERS;
}

ECode DialerKeyListener::GetCHARACTERS(
    /* [out, callee] */ ArrayOf<Char32>** ret)
{
    VALIDATE_NOT_NULL(ret)
    *ret = CHARACTERS;
    REFCOUNT_ADD(*ret)
    return NOERROR;
}

ECode DialerKeyListener::OnKeyUp(
    /* [in] */ IView* view,
    /* [in] */ IEditable* content,
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* ret)
{
    return MetaKeyKeyListener::OnKeyUp(view, content, keyCode, event, ret);
}

ECode DialerKeyListener::ClearMetaKeyState(
    /* [in] */ IView* view,
    /* [in] */ IEditable* content,
    /* [in] */ Int32 states)
{
    return MetaKeyKeyListener::ClearMetaKeyState(view, content, states);
}

ECode DialerKeyListener::GetInputType(
    /* [out] */ Int32* ret)
{
    *ret = IInputType::TYPE_CLASS_PHONE;
    return NOERROR;
}

Int32 DialerKeyListener::Lookup(
    /* [in] */ IKeyEvent* event,
    /* [in] */ ISpannable* content)
{
    Int32 meta;
    GetMetaState(ICharSequence::Probe(content), event, &meta);
    Char32 num;
    event->GetNumber(&num);
    Int32 number = (Int32)num;

    /*
     * Prefer number if no meta key is active, or if it produces something
     * valid and the meta lookup does not.
     */
    if ((meta & (IMetaKeyKeyListener::META_ALT_ON | IMetaKeyKeyListener::META_SHIFT_ON)) == 0) {
        if (number != 0) {
            return number;
        }
    }

    Int32 match = NumberKeyListener::Lookup(event, content);

    if (match != 0) {
        return match;
    } else {
        /*
         * If a meta key is active but the lookup with the meta key
         * did not produce anything, try some other meta keys, because
         * the user might have pressed SHIFT when they meant ALT,
         * or vice versa.
         */

        if (meta != 0) {
            //Java:    KeyData kd = new KeyData();
            AutoPtr<KeyCharacterMap::KeyData> kd;
            kd = new KeyCharacterMap::KeyData();
            AutoPtr< ArrayOf<Char32> > accepted = GetAcceptedChars();
            Boolean bGetKeyData;

            if ((event->GetKeyData(((IKeyData*)kd), &bGetKeyData), bGetKeyData)) {
                for (Int32 i = 1; i < (kd->mMeta)->GetLength(); i++) {
                    if (NumberKeyListener::Ok(accepted.Get(), (*(kd->mMeta))[i])) {
                        return (*(kd->mMeta))[i];
                    }
                }
            }
        }

        /*
         * Otherwise, use the number associated with the key, since
         * whatever they wanted to do with the meta key does not
         * seem to be valid here.
         */

        return number;
    }
}

ECode DialerKeyListener::GetInstance(
    /* [out] */ IDialerKeyListener** ret)
{
    VALIDATE_NOT_NULL(ret);
    if (sInstance == NULL) {
        CDialerKeyListener::New((IDialerKeyListener**)&sInstance);
    }

    *ret = sInstance.Get();
    REFCOUNT_ADD(*ret);
    return NOERROR;
}

} // namespace Method
} // namespace Text
} // namepsace Droid
} // namespace Elastos
