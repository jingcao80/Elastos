#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Widget.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Content.h"
#include "elastos/droid/widget/SimpleCursorTreeAdapter.h"
#include "elastos/droid/net/CUriHelper.h"
#include <elastos/core/StringUtils.h>
#include <elastos/core/Math.h>

using Elastos::Core::Math;
using Elastos::Core::StringUtils;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Widget {

CAR_INTERFACE_IMPL(SimpleCursorTreeAdapter, ResourceCursorTreeAdapter, ISimpleCursorTreeAdapter);
SimpleCursorTreeAdapter::SimpleCursorTreeAdapter()
{}

ECode SimpleCursorTreeAdapter::constructor(
    /* [in] */ IContext* context,
    /* [in] */ ICursor* cursor,
    /* [in] */ Int32 collapsedGroupLayout,
    /* [in] */ Int32 expandedGroupLayout,
    /* [in] */ ArrayOf<String>* groupFrom,
    /* [in] */ ArrayOf<Int32>* groupTo,
    /* [in] */ Int32 childLayout,
    /* [in] */ Int32 lastChildLayout,
    /* [in] */ ArrayOf<String>* childFrom,
    /* [in] */ ArrayOf<Int32>* childTo)
{
    ASSERT_SUCCEEDED(ResourceCursorTreeAdapter::constructor(context, cursor,
        collapsedGroupLayout, expandedGroupLayout, childLayout, lastChildLayout));
    ASSERT_SUCCEEDED(Init(groupFrom, groupTo, childFrom, childTo));
    return NOERROR;
}

ECode SimpleCursorTreeAdapter::constructor(
    /* [in] */ IContext* context,
    /* [in] */ ICursor* cursor,
    /* [in] */ Int32 collapsedGroupLayout,
    /* [in] */ Int32 expandedGroupLayout,
    /* [in] */ ArrayOf<String>* groupFrom,
    /* [in] */ ArrayOf<Int32>* groupTo,
    /* [in] */ Int32 childLayout,
    /* [in] */ ArrayOf<String>* childFrom,
    /* [in] */ ArrayOf<Int32>* childTo)
{
    ASSERT_SUCCEEDED(ResourceCursorTreeAdapter::constructor(context, cursor, collapsedGroupLayout, expandedGroupLayout, childLayout));
    ASSERT_SUCCEEDED(Init(groupFrom, groupTo, childFrom, childTo));
    return NOERROR;
}

ECode SimpleCursorTreeAdapter::constructor(
    /* [in] */ IContext* context,
    /* [in] */ ICursor* cursor,
    /* [in] */ Int32 groupLayout,
    /* [in] */ ArrayOf<String>* groupFrom,
    /* [in] */ ArrayOf<Int32>* groupTo,
    /* [in] */ Int32 childLayout,
    /* [in] */ ArrayOf<String>* childFrom,
    /* [in] */ ArrayOf<Int32>* childTo)
{
    ASSERT_SUCCEEDED(ResourceCursorTreeAdapter::constructor(context, cursor, groupLayout, childLayout));
    ASSERT_SUCCEEDED(Init(groupFrom, groupTo, childFrom, childTo));
    return NOERROR;
}

ECode SimpleCursorTreeAdapter::GetViewBinder(
    /* [out] */ ISimpleCursorTreeAdapterViewBinder** viewBinder)
{
    VALIDATE_NOT_NULL(viewBinder);
    *viewBinder = mViewBinder;
    REFCOUNT_ADD(*viewBinder);
    return NOERROR;
}

ECode SimpleCursorTreeAdapter::SetViewBinder(
    /* [in] */ ISimpleCursorTreeAdapterViewBinder* viewBinder)
{
    mViewBinder = viewBinder;
    return NOERROR;
}

ECode SimpleCursorTreeAdapter::SetViewText(
    /* [in] */ ITextView* v,
    /* [in] */ const String& text)
{
    AutoPtr<ICharSequence> cs;
    CString::New(text, (ICharSequence**)&cs);
    return v->SetText(cs);
}

ECode SimpleCursorTreeAdapter::BindChildView(
    /* [in] */ IView* view,
    /* [in] */ IContext* context,
    /* [in] */ ICursor* cursor,
    /* [in] */ Boolean isLastChild)
{
    if (mChildFrom == NULL) {
        mChildFrom = ArrayOf<Int32>::Alloc(mChildFromNames->GetLength());
        InitFromColumns(cursor, mChildFromNames, mChildFrom);
    }

    return BindView(view, context, cursor, mChildFrom, mChildTo);
}

ECode SimpleCursorTreeAdapter::BindGroupView(
    /* [in] */ IView* view,
    /* [in] */ IContext* context,
    /* [in] */ ICursor* cursor,
    /* [in] */ Boolean isExpanded)
{
    if (mGroupFrom == NULL) {
        mGroupFrom = ArrayOf<Int32>::Alloc(mGroupFromNames->GetLength());
        InitFromColumns(cursor, mGroupFromNames, mGroupFrom);
    }

    return BindView(view, context, cursor, mGroupFrom, mGroupTo);
}


ECode SimpleCursorTreeAdapter::SetViewImage(
    /* [in] */ IImageView* v,
    /* [in] */ const String& value)
{
    Int64 iValue = StringUtils::ParseInt64(value, 10, Elastos::Core::Math::INT64_MAX_VALUE);
    if(iValue == Elastos::Core::Math::INT64_MAX_VALUE)
    {
        AutoPtr<IUriHelper> helper;
        CUriHelper::AcquireSingleton((IUriHelper**)&helper);
        AutoPtr<IUri> uri;
        helper->Parse(value, (IUri**)&uri);
        return v->SetImageURI(uri);
    } else {
        Int32 trueValue = (Int32)iValue;
        v->SetImageResource(trueValue);
    }
    // try {
    //     v->SetImageResource(Integer.parseInt(value));
    // // } catch (NumberFormatException nfe) {
    //     v.setImageURI(Uri.parse(value));
    // }
    return NOERROR;
}

ECode SimpleCursorTreeAdapter::Init(
    /* [in] */ ArrayOf<String>* groupFromNames,
    /* [in] */ ArrayOf<Int32>* groupTo,
    /* [in] */ ArrayOf<String>* childFromNames,
    /* [in] */ ArrayOf<Int32>* childTo)
{
    mGroupFromNames = groupFromNames;
    mGroupTo = groupTo;

    mChildFromNames = childFromNames;
    mChildTo = childTo;
    return NOERROR;
}

ECode SimpleCursorTreeAdapter::BindView(
    /* [in] */ IView* view,
    /* [in] */ IContext* context,
    /* [in] */ ICursor* cursor,
    /* [in] */ ArrayOf<Int32>* from,
    /* [in] */ ArrayOf<Int32>* to)
{
    AutoPtr<ISimpleCursorTreeAdapterViewBinder> binder = mViewBinder;

    for (Int32 i = 0; i < to->GetLength(); i++) {
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
                if (text == NULL) {
                    text = String("");
                }
                if (ITextView::Probe(v)) {
                    AutoPtr<ITextView> tv = ITextView::Probe(v);
                    SetViewText(tv, text);
                } else if (IImageView::Probe(v)) {
                    AutoPtr<IImageView> iv = IImageView::Probe(v);
                    SetViewImage(iv, text);
                } else {
                    // throw new IllegalStateException("SimpleCursorTreeAdapter can bind values" +
                    //         " only to TextView and ImageView!");
                    return E_ILLEGAL_STATE_EXCEPTION;
                }
            }
        }
    }
    return NOERROR;
}

ECode SimpleCursorTreeAdapter::InitFromColumns(
    /* [in] */ ICursor* cursor,
    /* [in] */ ArrayOf<String>* fromColumnNames,
    /* [in] */ ArrayOf<Int32>* fromColumns)
{
    for (Int32 i = fromColumnNames->GetLength() - 1; i >= 0; i--) {
        Int32 rst;
        cursor->GetColumnIndexOrThrow((*fromColumnNames)[i], &rst);
        (*fromColumns)[i] = rst;
    }
    return NOERROR;
}

} // namespace Widget
} // namespace Droid
} // namespace Elastos
