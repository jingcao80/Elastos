#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Content.h"
#include "elastos/droid/widget/SimpleCursorAdapter.h"
#include "elastos/droid/net/CUriHelper.h"
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::StringUtils;
using Elastos::Core::CString;
using Elastos::Utility::Logging::Logger;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Widget {

CAR_INTERFACE_IMPL(SimpleCursorAdapter, ResourceCursorAdapter, ISimpleCursorAdapter);

SimpleCursorAdapter::SimpleCursorAdapter()
    : mStringConversionColumn(-1)
{
}

ECode SimpleCursorAdapter::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Int32 layout,
    /* [in] */ ICursor* c,
    /* [in] */ ArrayOf<String>* from,
    /* [in] */ ArrayOf<Int32>* to)
{
    FAIL_RETURN(ResourceCursorAdapter::constructor(context, layout, c));
    return InitImpl(c, from, to);
}

ECode SimpleCursorAdapter::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Int32 layout,
    /* [in] */ ICursor* c,
    /* [in] */ ArrayOf<String>* from,
    /* [in] */ ArrayOf<Int32>* to,
    /* [in] */ Int32 flags)
{
    FAIL_RETURN(ResourceCursorAdapter::constructor(context, layout, c, flags));
    return InitImpl(c, from, to);
}

ECode SimpleCursorAdapter::InitImpl(
    /* [in] */ ICursor* c,
    /* [in] */ ArrayOf<String>* from,
    /* [in] */ ArrayOf<Int32>* to)
{
    VALIDATE_NOT_NULL(from);
    VALIDATE_NOT_NULL(to);

    mTo = to;
    mOriginalFrom = from;
    return FindColumns(c, from);
}

ECode SimpleCursorAdapter::BindView(
    /* [in] */ IView* view,
    /* [in] */ IContext* context,
    /* [in] */ ICursor* cursor)
{
    AutoPtr<ISimpleCursorAdapterViewBinder> binder = mViewBinder;
    Int32 count = mTo->GetLength();
    AutoPtr<ArrayOf<Int32> > from = mFrom;
    AutoPtr<ArrayOf<Int32> > to = mTo;
    ITextView* tv;
    IImageView* iv;
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> v;
        view->FindViewById((*to)[i], (IView**)&v);
        if (v != NULL) {
            Boolean bound = FALSE;
            if (binder != NULL) {
                binder->SetViewValue(v, cursor, (*from)[i], &bound);
            }

            if (!bound) {
                String text;
                cursor->GetString((*from)[i], &text);
                if (text.IsNull()) {
                    text = String("");
                }

                if (tv = ITextView::Probe(v), tv) {
                    SetViewText(tv, text);
                }
                else if (iv = IImageView::Probe(v), iv) {
                    SetViewImage(iv, text);
                }
                else {
                    Logger::E("SimpleCursorAdapter",
                        "%s is not a view that can be bounds by this SimpleCursorAdapter.", TO_CSTR(v));
                    return E_ILLEGAL_STATE_EXCEPTION;
                }
            }
        }
    }
    return NOERROR;
}

ECode SimpleCursorAdapter::GetViewBinder(
    /* [out] */ ISimpleCursorAdapterViewBinder** viewBinder)
{
    VALIDATE_NOT_NULL(viewBinder);
    *viewBinder = mViewBinder;
    REFCOUNT_ADD(*viewBinder);
    return NOERROR;
}

ECode SimpleCursorAdapter::SetViewBinder(
    /* [in] */ ISimpleCursorAdapterViewBinder* viewBinder)
{
    mViewBinder = viewBinder;
    return NOERROR;
}

ECode SimpleCursorAdapter::SetViewImage(
    /* [in] */ IImageView* v,
    /* [in] */ const String& value)
{
    VALIDATE_NOT_NULL(v);

    Int32 intVal = 0;
    ECode ec = StringUtils::Parse(value, &intVal);
    if (SUCCEEDED(ec)) {
        return v->SetImageResource(intVal);
    }
    else {
        // NumberFormatException
        AutoPtr<IUriHelper> helper;
        CUriHelper::AcquireSingleton((IUriHelper**)&helper);
        AutoPtr<IUri> uri;
        helper->Parse(value, (IUri**)&uri);
        return v->SetImageURI(uri);
    }
    return NOERROR;
}

ECode SimpleCursorAdapter::SetViewText(
    /* [in] */ ITextView* v,
    /* [in] */ const String& text)
{
    VALIDATE_NOT_NULL(v);
    AutoPtr<ICharSequence> seq;
    CString::New(text, (ICharSequence**)&seq);
    return v->SetText(seq);
}

ECode SimpleCursorAdapter::GetStringConversionColumn(
    /* [in] */ Int32* column)
{
    VALIDATE_NOT_NULL(column);
    *column = mStringConversionColumn;
    return NOERROR;
}

ECode SimpleCursorAdapter::SetStringConversionColumn(
    /* [in] */ Int32 stringConversionColumn)
{
    mStringConversionColumn = stringConversionColumn;
    return NOERROR;
}

ECode SimpleCursorAdapter::GetCursorToStringConverter(
    /* [out] */ ICursorToStringConverter** converter)
{
    VALIDATE_NOT_NULL(converter);
    *converter = mCursorToStringConverter;
    REFCOUNT_ADD(*converter);
    return NOERROR;
}

ECode SimpleCursorAdapter::SetCursorToStringConverter(
    /* [in] */ ICursorToStringConverter* cursorToStringConverter)
{
    mCursorToStringConverter = cursorToStringConverter;
    return NOERROR;
}

ECode SimpleCursorAdapter::ConvertToString(
    /* [in] */ ICursor* cursor,
    /* [out] */ ICharSequence** seq)
{
    VALIDATE_NOT_NULL(seq)
    if (mCursorToStringConverter != NULL) {
        return mCursorToStringConverter->ConvertToString(cursor, seq);
    }
    else if (mStringConversionColumn > -1) {
        String str;
        cursor->GetString(mStringConversionColumn, &str);
        return CString::New(str, seq);
    }

    return ResourceCursorAdapter::ConvertToString(cursor, seq);
}

ECode SimpleCursorAdapter::FindColumns(
    /* [in] */ ICursor* c,
    /* [in] */ ArrayOf<String>* from)
{
    if (c != NULL) {
        Int32 count = from->GetLength();
        if (mFrom == NULL || mFrom->GetLength() != count) {
            mFrom = NULL;
            mFrom = ArrayOf<Int32>::Alloc(count);
        }

        for (Int32 i = 0; i < count; i++) {
            c->GetColumnIndexOrThrow((*from)[i], &(*mFrom)[i]);
        }
    }
    else {
        mFrom = NULL;
    }
    return NOERROR;
}

ECode SimpleCursorAdapter::SwapCursor(
    /* [in] */ ICursor* c,
    /* [out] */ ICursor** cursor)
{
    VALIDATE_NOT_NULL(cursor);
    // super.swapCursor() will notify observers before we have
    // a valid mapping, make sure we have a mapping before this
    // happens
    FindColumns(c, mOriginalFrom);
    return ResourceCursorAdapter::SwapCursor(c, cursor);
}

ECode SimpleCursorAdapter::ChangeCursorAndColumns(
    /* [in] */ ICursor* c,
    /* [in] */ ArrayOf<String>* from,
    /* [in] */ ArrayOf<Int32>* to)
{
    mOriginalFrom = NULL;
    mOriginalFrom = from;
    mTo = NULL;
    mTo = to;

    // super.changeCursor() will notify observers before we have
    // a valid mapping, make sure we have a mapping before this
    // happens
    FindColumns(c, mOriginalFrom);
    return ResourceCursorAdapter::ChangeCursor(c);
}

} // namespace Elastos
} // namespace Droid
} // namespace Widget
