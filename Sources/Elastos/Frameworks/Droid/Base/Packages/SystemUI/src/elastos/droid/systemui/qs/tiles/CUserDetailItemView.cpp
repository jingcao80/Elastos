
#include "elastos/droid/systemui/qs/tiles/CUserDetailItemView.h"
#include "elastos/droid/systemui/FontSizeUtils.h"
#include "../../R.h"
#include <elastos/droid/R.h>
#include <elastos/droid/internal/utility/ArrayUtils.h>
#include <elastos/droid/view/LayoutInflater.h>

using Elastos::Droid::Graphics::CTypefaceHelper;
using Elastos::Droid::Graphics::ITypefaceHelper;
using Elastos::Droid::Internal::Utility::ArrayUtils;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::LayoutInflater;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Qs {
namespace Tiles {

CAR_OBJECT_IMPL(CUserDetailItemView)
CAR_INTERFACE_IMPL(CUserDetailItemView, LinearLayout, IUserDetailItemView)
ECode CUserDetailItemView::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode CUserDetailItemView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode CUserDetailItemView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return constructor(context, attrs, defStyleAttr, 0);
}

ECode CUserDetailItemView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    LinearLayout::constructor(context, attrs, defStyleAttr, defStyleRes);

    AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
        const_cast<Int32 *>(R::styleable::UserDetailItemView),
        ArraySize(R::styleable::UserDetailItemView));
    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(
            attrs, attrIds, defStyleAttr, defStyleRes, (ITypedArray**)&a);
    Int32 N = 0;
    a->GetIndexCount(&N);
    AutoPtr<ITypefaceHelper> helper;
    CTypefaceHelper::AcquireSingleton((ITypefaceHelper**)&helper);
    for (Int32 i = 0; i < N; i++) {
        Int32 attr = 0;
        a->GetIndex(i, &attr);
        switch (attr) {
            case R::styleable::UserDetailItemView_regularFontFamily: {
                String v;
                a->GetString(attr, &v);
                helper->Create(v, 0 /* style */, (ITypeface**)&mRegularTypeface);
                break;
            }
            case R::styleable::UserDetailItemView_activatedFontFamily: {
                String v;
                a->GetString(attr, &v);
                helper->Create(v, 0 /* style */, (ITypeface**)&mActivatedTypeface);
                break;
            }
        }
    }
    a->Recycle();
    return NOERROR;
}

AutoPtr<IUserDetailItemView> CUserDetailItemView::ConvertOrInflate(
    /* [in] */ IContext* context,
    /* [in] */ IView* _convertView,
    /* [in] */ IViewGroup* root)
{
    AutoPtr<IView> convertView = _convertView;
    if (IUserDetailItemView::Probe(convertView) == NULL) {
        AutoPtr<ILayoutInflater> inflater;
        LayoutInflater::From(context, (ILayoutInflater**)&inflater);
        inflater->Inflate(R::layout::qs_user_detail_item, root, FALSE, (IView**)&convertView);
    }
    return IUserDetailItemView::Probe(convertView);
}

ECode CUserDetailItemView::Bind(
    /* [in] */ const String& name,
    /* [in] */ IBitmap* picture)
{
    AutoPtr<ICharSequence> t;
    CString::New(name, (ICharSequence**)&t);
    mName->SetText(t);
    mAvatar->SetBitmap(picture);
    return NOERROR;
}

ECode CUserDetailItemView::Bind(
    /* [in] */ const String& name,
    /* [in] */ IDrawable* picture)
{
    AutoPtr<ICharSequence> t;
    CString::New(name, (ICharSequence**)&t);
    mName->SetText(t);
    mAvatar->SetDrawable(picture);
    return NOERROR;
}

ECode CUserDetailItemView::OnFinishInflate()
{
    AutoPtr<IView> view;
    FindViewById(R::id::user_picture, (IView**)&view);
    mAvatar = IUserAvatarView::Probe(view);

    view = NULL;
    FindViewById(R::id::user_name, (IView**)&view);
    mName = ITextView::Probe(view);
    if (mRegularTypeface == NULL) {
        mName->GetTypeface((ITypeface**)&mRegularTypeface);
    }
    if (mActivatedTypeface == NULL) {
        mName->GetTypeface((ITypeface**)&mActivatedTypeface);
    }
    UpdateTypeface();
    return NOERROR;
}

void CUserDetailItemView::OnConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    LinearLayout::OnConfigurationChanged(newConfig);
    FontSizeUtils::UpdateFontSize(mName, R::dimen::qs_detail_item_secondary_text_size);
}

ECode CUserDetailItemView::DrawableStateChanged()
{
    LinearLayout::DrawableStateChanged();
    UpdateTypeface();
    return NOERROR;
}

void CUserDetailItemView::UpdateTypeface()
{
    AutoPtr<ArrayOf<Int32> > values;
    GetDrawableState((ArrayOf<Int32>**)&values);
    Boolean activated = ArrayUtils::Contains(values.Get(), Elastos::Droid::R::attr::state_activated);
    mName->SetTypeface(activated ? mActivatedTypeface : mRegularTypeface);
}

ECode CUserDetailItemView::HasOverlappingRendering(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = FALSE;
    return NOERROR;
}

} // namespace Tiles
} // namespace Qs
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
