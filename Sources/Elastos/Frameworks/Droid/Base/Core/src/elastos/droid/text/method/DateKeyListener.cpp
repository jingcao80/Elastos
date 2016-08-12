#include "elastos/droid/text/method/DateKeyListener.h"
#include "elastos/droid/text/method/CDateKeyListener.h"

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

static AutoPtr<ArrayOf<Char32> > InitCHARACTERS()
{
    Char32 ch[] = {
            '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
            '/', '-', '.'};
    AutoPtr<ArrayOf<Char32> > chars = ArrayOf<Char32>::Alloc(ArraySize(ch));
    chars->Copy(ch, ArraySize(ch));
    return chars;
}

const AutoPtr<ArrayOf<Char32> > DateKeyListener::CHARACTERS = InitCHARACTERS();


AutoPtr<IDateKeyListener> DateKeyListener::sInstance;

CAR_INTERFACE_IMPL(DateKeyListener, NumberKeyListener, IDateKeyListener)

DateKeyListener::DateKeyListener()
{}

DateKeyListener::~DateKeyListener()
{}

ECode DateKeyListener::constructor()
{
    return NOERROR;
}

AutoPtr<IDateKeyListener> DateKeyListener::GetInstance()
{
    if (sInstance == NULL) {
        CDateKeyListener::New((IDateKeyListener**)&sInstance);
    }
    return sInstance;
}

ECode DateKeyListener::GetInputType(
    /* [out] */ Int32* ret)
{
    VALIDATE_NOT_NULL(ret);
    *ret = IInputType::TYPE_CLASS_DATETIME | IInputType::TYPE_DATETIME_VARIATION_DATE;
    return NOERROR;
}

AutoPtr< ArrayOf<Char32> > DateKeyListener::GetAcceptedChars()
{
    return CHARACTERS;
}

ECode DateKeyListener::GetCHARACTERS(
    /* [out] */ ArrayOf<Char32>** array)
{
    VALIDATE_NOT_NULL(array)
    *array = CHARACTERS;
    REFCOUNT_ADD(*array);
    return NOERROR;
}

//override
ECode DateKeyListener::OnKeyUp(
    /* [in] */ IView* view,
    /* [in] */ IEditable* content,
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* ret)
{
    return MetaKeyKeyListener::OnKeyUp(view, content, keyCode, event, ret);
}

CARAPI DateKeyListener::ClearMetaKeyState(
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