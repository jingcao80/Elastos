#include "elastos/droid/text/method/DateTimeKeyListener.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

AutoPtr<IDateTimeKeyListener> DateTimeKeyListener::sInstance;

static AutoPtr<ArrayOf<Char32> > InitCHARACTERS()
{
    Char32 ch[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'm', 'p', ':', '/', '-', ' ' };
    AutoPtr<ArrayOf<Char32> > chars = ArrayOf<Char32>::Alloc(ArraySize(ch));
    chars->Copy(ch, ArraySize(ch));
    return chars;
}

const AutoPtr<ArrayOf<Char32> > DateTimeKeyListener::CHARACTERS = InitCHARACTERS();

CAR_INTERFACE_IMPL_6(DateTimeKeyListener, Object, IDateTimeKeyListener, INumberKeyListener, IBaseKeyListener, IMetaKeyKeyListener, IKeyListener, IInputFilter)

ECode DateTimeKeyListener::constructor()
{
    return NOERROR;
}

ECode DateTimeKeyListener::GetInputType(
    /* [out] */ Int32* ret)
{
    *ret = IInputType::TYPE_CLASS_DATETIME | IInputType::TYPE_DATETIME_VARIATION_NORMAL;
    return NOERROR;
}

AutoPtr< ArrayOf<Char32> > DateTimeKeyListener::GetAcceptedChars()
{
    return CHARACTERS;
}

ECode DateTimeKeyListener::GetCHARACTERS(
    /* [out] */ ArrayOf<Char32>** ret)
{
    VALIDATE_NOT_NULL(ret)
    *ret = CHARACTERS;
    REFCOUNT_ADD(*ret);
    return NOERROR;
}

ECode DateTimeKeyListener::GetInstance(
    /* [out] */ IDateTimeKeyListener** ret)
{
    VALIDATE_NOT_NULL(ret)
    if (sInstance == NULL) {
        sInstance = new DateTimeKeyListener();
    }

    *ret = sInstance;
    REFCOUNT_ADD(*ret);
    return NOERROR;
}

ECode DateTimeKeyListener::OnKeyUp(
    /* [in] */ IView* view,
    /* [in] */ IEditable* content,
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* ret)
{
    return MetaKeyKeyListener::OnKeyUp(view, content, keyCode, event, ret);
}

ECode DateTimeKeyListener::ClearMetaKeyState(
    /* [in] */ IView* view,
    /* [in] */ IEditable* content,
    /* [in] */ Int32 states)
{
    return MetaKeyKeyListener::ClearMetaKeyState(view, content, states);
}

} // namespace Method
} // namespace Text
} // namepsace Droid
} // namespace Elastos