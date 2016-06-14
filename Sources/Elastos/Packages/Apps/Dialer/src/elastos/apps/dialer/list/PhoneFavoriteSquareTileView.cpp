
#include "PhoneFavoriteSquareTileView.h"

namespace Elastos{
namespace Apps{
namespace Dialer {
namespace List {


//=================================================================
// PhoneFavoriteSquareTileView::SecondaryButtonOnClickListener
//=================================================================
CAR_INTERFACE_IMPL(PhoneFavoriteSquareTileView::SecondaryButtonOnClickListener, Object, IViewOnClickListener)

PhoneFavoriteSquareTileView::SecondaryButtonOnClickListener::SecondaryButtonOnClickListener(
    /* [in] */ PhoneFavoriteSquareTileView* host)
    : mHost(host)
{}

ECode PhoneFavoriteSquareTileView::SecondaryButtonOnClickListener::OnClick(
    /* [in] */ IView* v)
{
    mHost->LaunchQuickContact();
    return NOERROR;
}

//=================================================================
// PhoneFavoriteSquareTileView
//=================================================================
CAR_INTERFACE_IMPL(PhoneFavoriteSquareTileView, PhoneFavoriteTileView, IPhoneFavoriteSquareTileView)

static const String PhoneFavoriteSquareTileView::TAG("PhoneFavoriteSquareTileView");

ECode PhoneFavoriteSquareTileView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    PhoneFavoriteTileView::constructor(context, attrs);

    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    resources->GetFraction(R::dimen::contact_tile_height_to_width_ratio,
            1, 1, &mHeightToWidthRatio);

    return NOERROR;
}

ECode PhoneFavoriteSquareTileView::OnFinishInflate()
{
    PhoneFavoriteTileView::OnFinishInflate();
    AutoPtr<IView> nameView;
    FindViewById(R::id::contact_tile_name, (IView**)&nameView);
    ITextView::Probe(nameView)->SetElegantTextHeight(FALSE);
    AutoPtr<IView> phoneTypeView;
    FindViewById(R::id::contact_tile_phone_type, (IView**)&phoneTypeView);
    ITextView::Probe(phoneTypeView)->SetElegantTextHeight(false);

    AutoPtr<IView> button;
    FindViewById(R::id::contact_tile_secondary_button, (IView**)&button);
    mSecondaryButton = ImageButton::Probe(button);

    return NOERROR;
}

ECode PhoneFavoriteSquareTileView::GetApproximateImageSize(
    /* [out] */ Int32* size)
{
    VALUE_NOT_NULL(size);

    // The picture is the full size of the tile (minus some padding, but we can be generous)
    return GetWidth(size);
}

void PhoneFavoriteSquareTileView::LaunchQuickContact()
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IUri> uri;
    GetLookupUri((IUri**)&uri);

    AutoPtr<IContactsContractQuickContact> contact;
    CContactsContractQuickContact::AquireSingleton(
            (IContactsContractQuickContact**)&contact);
    contact->ShowQuickContact(context, this,
                uri, IContactsContractQuickContact::MODE_LARGE, NULL);
}

// TODO:
// ECode PhoneFavoriteSquareTileView::LoadFromContact(
//     /* [in] */ IContactEntry* entry)
// {
//     PhoneFavoriteTileView::LoadFromContact(entry);
//     if (entry != NULL) {
//         mSecondaryButton->SetOnClickListener(new SecondaryButtonOnClickListener(this));
//     }
//     mContactEntry = entry;

//     return NOERROR;
// }

ECode PhoneFavoriteSquareTileView::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    Int32 width = View::MeasureSpec::GetSize(widthMeasureSpec);
    Int32 height = (Int32) (mHeightToWidthRatio * width);
    Int32 count;
    GetChildCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);
        child->Measure(
                View::MeasureSpec::MakeMeasureSpec(width, View::MeasureSpec::EXACTLY),
                View::MeasureSpec::MakeMeasureSpec(height, View::MeasureSpec::EXACTLY));
    }
    SetMeasuredDimension(width, height);

    return NOERROR;
}

// TODO:
// ECode PhoneFavoriteSquareTileView::GetContactEntry(
//     /* [out] */ IContactEntry** entry)
// {
//     VALUE_NOT_NULL(entry);
//     *entry = mContactEntry;
//     REFCOUNT_ADD(*entry);
//     return NOERROR;
// }

} // List
} // Dialer
} // Apps
} // Elastos
