
#include "LatinKeyboard.h"
#include "R.h"

using Elastos::Core::CStringWrapper;
using Elastos::Droid::View::InputMethod::IEditorInfo;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace SoftKeyboard {


LatinKeyboard::LatinKey::LatinKey(
    /* [in] */ IResources* res,
    /* [in] */ Row* parent,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ IXmlResourceParser* parser)
    : Keyboard::Key(res, parent, x, y, parser)
{}

ECode LatinKeyboard::LatinKey::IsInside(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [out] */ Boolean* isInside)
{
    assert(isInside != NULL);

    return Key::IsInside(x, (*mCodes)[0] == IKeyboard::KEYCODE_CANCEL ? y - 10 : y, isInside);
}


LatinKeyboard::LatinKeyboard(
    /* [in] */ IContext* ctx,
    /* [in] */ Int32 xmlLayoutResId)
    : Keyboard(ctx, xmlLayoutResId)
{}

LatinKeyboard::LatinKeyboard(
    /* [in] */ IContext* ctx,
    /* [in] */ Int32 layoutTemplateResId,
    /* [in] */ ICharSequence* characters,
    /* [in] */ Int32 columns,
    /* [in] */ Int32 horizontalPadding)
    : Keyboard(ctx, layoutTemplateResId, characters, columns, horizontalPadding)
{}

AutoPtr<Keyboard::Key> LatinKeyboard::CreateKeyFromXml(
    /* [in] */ IResources* res,
    /* [in] */ Row* parent,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ IXmlResourceParser* parser)
{
    AutoPtr<Key> key = new LatinKey(res, parent, x, y, parser);
    if ((*key->mCodes)[0] == 10) {
        mEnterKey = key;
    }
    return key;
}

ECode LatinKeyboard::SetImeOptions(
    /* [in] */ IResources* res,
    /* [in] */ Int32 options)
{
    assert(res != NULL);
    if (mEnterKey == NULL) {
        return NOERROR;
    }

    switch (options & (IEditorInfo::IME_MASK_ACTION | IEditorInfo::IME_FLAG_NO_ENTER_ACTION)) {
        case IEditorInfo::IME_ACTION_GO: {
            mEnterKey->SetIconPreview(NULL);
            mEnterKey->SetIcon(NULL);

            AutoPtr<ICharSequence> label;
            res->GetText(R::string::label_go_key, (ICharSequence**)&label);
            mEnterKey->SetLabel(label);
            break;
        }
        case IEditorInfo::IME_ACTION_NEXT: {
            mEnterKey->SetIconPreview(NULL);
            mEnterKey->SetIcon(NULL);

            AutoPtr<ICharSequence> label;
            res->GetText(R::string::label_next_key, (ICharSequence**)&label);
            mEnterKey->SetLabel(label);
            break;
        }
        case IEditorInfo::IME_ACTION_SEARCH: {
            AutoPtr<IDrawable> icon;
            res->GetDrawable(R::drawable::sym_keyboard_search, (IDrawable**)&icon);

            mEnterKey->SetIcon(icon);
            mEnterKey->SetLabel(NULL);
            break;
        }
        case IEditorInfo::IME_ACTION_SEND: {
            mEnterKey->SetIconPreview(NULL);
            mEnterKey->SetIcon(NULL);

            AutoPtr<ICharSequence> label;
            res->GetText(R::string::label_send_key, (ICharSequence**)&label);
            mEnterKey->SetLabel(label);
            break;
        }
        default: {
            AutoPtr<IDrawable> icon;
            res->GetDrawable(R::drawable::sym_keyboard_return, (IDrawable**)&icon);
            assert(icon != NULL);

            mEnterKey->SetIcon(icon);
            mEnterKey->SetLabel(NULL);
            break;
        }
    }

    return NOERROR;
}

} // namespace SoftKeyboard
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
