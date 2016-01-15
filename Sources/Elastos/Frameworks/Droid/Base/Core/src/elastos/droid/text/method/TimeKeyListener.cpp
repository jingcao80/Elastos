#include "elastos/droid/text/method/TimeKeyListener.h"
#include "elastos/droid/text/method/CTimeKeyListener.h"

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

static AutoPtr<ArrayOf<Char32> > InitCHARACTERS()
{
    Char32 ch[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'm', 'p', ':' };
    AutoPtr<ArrayOf<Char32> > array = ArrayOf<Char32>::Alloc(ArraySize(ch));
    array->Copy(ch, ArraySize(ch));
    return array;
}

AutoPtr<ITimeKeyListener> TimeKeyListener::sInstance;

const AutoPtr<ArrayOf<Char32> > TimeKeyListener::CHARACTERS = InitCHARACTERS();

TimeKeyListener::TimeKeyListener()
{}

TimeKeyListener::~TimeKeyListener()
{}

CAR_INTERFACE_IMPL(TimeKeyListener, Object, ITimeKeyListener);

ECode TimeKeyListener::constructor()
{
    return NOERROR;
}

ECode TimeKeyListener::GetInstance(
    /* [out] */ ITimeKeyListener** ret)
{
    VALIDATE_NOT_NULL(ret);
    if (sInstance == NULL) {
        CTimeKeyListener::New((ITimeKeyListener**)&sInstance);
    }

    *ret = sInstance;
    REFCOUNT_ADD(*ret);
    return NOERROR;
}

ECode TimeKeyListener::GetInputType(
    /* [out] */ Int32* ret)
{
    VALIDATE_NOT_NULL(ret);
    *ret = IInputType::TYPE_CLASS_DATETIME | IInputType::TYPE_DATETIME_VARIATION_TIME;
    return NOERROR;
}

AutoPtr< ArrayOf<Char32> > TimeKeyListener::GetAcceptedChars()
{
    return CHARACTERS;
}

ECode TimeKeyListener::GetCHARACTERS(
    /* [out] */ ArrayOf<Char32>** ret)
{
    VALIDATE_NOT_NULL(ret)
    *ret = CHARACTERS;
    REFCOUNT_ADD(*ret);
    return NOERROR;
}

//override
ECode TimeKeyListener::OnKeyUp(
    /* [in] */ IView* view,
    /* [in] */ IEditable* content,
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* ret)
{
    return MetaKeyKeyListener::OnKeyUp(view, content, keyCode, event, ret);
}

CARAPI TimeKeyListener::ClearMetaKeyState(
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