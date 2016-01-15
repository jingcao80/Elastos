
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/app/CNotificationInboxStyle.h"
#include "elastos/droid/app/CNotificationBuilder.h"
#include "elastos/droid/app/CNotification.h"
#include "elastos/droid/R.h"

using Elastos::Droid::R;
using Elastos::Droid::View::IView;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Utility::ITypedValue;
using Elastos::Utility::CArrayList;

namespace Elastos {
namespace Droid {
namespace App {

CAR_INTERFACE_IMPL(CNotificationInboxStyle, NotificationStyle, INotificationInboxStyle)

CAR_OBJECT_IMPL(CNotificationInboxStyle)

CNotificationInboxStyle::CNotificationInboxStyle()
{
    CArrayList::New(5, (IArrayList**)&mTexts);
}

CNotificationInboxStyle::~CNotificationInboxStyle()
{
}

ECode CNotificationInboxStyle::constructor()
{
    return NOERROR;
}

ECode CNotificationInboxStyle::constructor(
    /* [in] */ INotificationBuilder* builder)
{
    return NotificationStyle::SetBuilder(builder);
}

ECode CNotificationInboxStyle::SetBigContentTitle(
    /* [in] */ ICharSequence* title)
{
    InternalSetBigContentTitle(CNotification::SafeCharSequence(title));
    return NOERROR;
}

ECode CNotificationInboxStyle::SetSummaryText(
    /* [in] */ ICharSequence* cs)
{
    InternalSetSummaryText(CNotification::SafeCharSequence(cs));
    return NOERROR;
}

ECode CNotificationInboxStyle::AddLine(
    /* [in] */ ICharSequence* cs)
{
    AutoPtr<ICharSequence> obj = CNotification::SafeCharSequence(cs);
    mTexts->Add(obj);
    return NOERROR;
}

ECode CNotificationInboxStyle::AddExtras(
    /* [in] */ IBundle* extras)
{
    NotificationStyle::AddExtras(extras);

    extras->PutCharSequenceArrayList(INotification::EXTRA_TEXT_LINES, mTexts);
    return NOERROR;
}

ECode CNotificationInboxStyle::RestoreFromExtras(
    /* [in] */ IBundle* extras)
{
    NotificationStyle::RestoreFromExtras(extras);

    mTexts->Clear();
    Boolean contains;
    extras->ContainsKey(INotification::EXTRA_TEXT_LINES, &contains);
    if (contains) {
        AutoPtr<IArrayList> list;
        extras->GetCharSequenceArrayList(INotification::EXTRA_TEXT_LINES, (IArrayList**)&list);
        mTexts = list;
    }
    return NOERROR;
}

AutoPtr<IRemoteViews> CNotificationInboxStyle::MakeBigContentView()
{
    // Remove the content text so line3 disappears unless you have a summary

    CNotificationBuilder* builder = (CNotificationBuilder*)mBuilder.Get();
    // Nasty
    AutoPtr<ICharSequence> oldBuilderContentText = builder->mContentText;
    builder->mContentText = NULL;

    AutoPtr<IRemoteViews> contentView = GetStandardView(builder->GetInboxLayoutResource());

    builder->mContentText = oldBuilderContentText;

    contentView->SetViewVisibility(R::id::text2, IView::GONE);

    const Int32 rowIdsLength = 7;
    Int32 rowIds[rowIdsLength] = {
        R::id::inbox_text0,
        R::id::inbox_text1,
        R::id::inbox_text2,
        R::id::inbox_text3,
        R::id::inbox_text4,
        R::id::inbox_text5,
        R::id::inbox_text6
    };

    // Make sure all rows are gone in case we reuse a view.
    for (Int32 i = 0; i < rowIdsLength; ++i) {
        contentView->SetViewVisibility(rowIds[i], IView::GONE);
    }

    AutoPtr<IResources> res;
    builder->mContext->GetResources((IResources**)&res);
    AutoPtr<IConfiguration> config;
    res->GetConfiguration((IConfiguration**)&config);
    Float fontScale;
    config->GetFontScale(&fontScale);
    Boolean largeText = fontScale > 1.0f;
    Int32 subTextSize;
    res->GetDimensionPixelSize(R::dimen::notification_subtext_size, &subTextSize);
    Int32 i = 0;
    Int32 size;
    mTexts->GetSize(&size);
    while (i < size && i < rowIdsLength) {
        AutoPtr<IInterface> obj;
        mTexts->Get(i, (IInterface**)&obj);
        String str = Object::ToString(obj);
        if (str != NULL && !str.Equals("")) {
            contentView->SetViewVisibility(rowIds[i], IView::VISIBLE);
            contentView->SetTextViewText(rowIds[i],
                builder->ProcessLegacyText(ICharSequence::Probe(obj)));
            if (largeText) {
                contentView->SetTextViewTextSize(rowIds[i],
                    ITypedValue::COMPLEX_UNIT_PX, subTextSize);
            }
        }
        i++;
    }

    contentView->SetViewVisibility(R::id::inbox_end_pad,
        size > 0 ? IView::VISIBLE : IView::GONE);

    contentView->SetViewVisibility(R::id::inbox_more,
        size > rowIdsLength ? IView::VISIBLE : IView::GONE);

    ApplyTopPadding(contentView);

    builder->ShrinkLine3Text(contentView);

    builder->AddProfileBadge(contentView, R::id::profile_badge_large_template);

    return contentView;
}

ECode CNotificationInboxStyle::PopulateBigContentView(
    /* [in] */ INotification* wip)
{
    CNotificationBuilder* builder = (CNotificationBuilder*)mBuilder.Get();
    builder->SetBuilderBigContentView(wip, MakeBigContentView());
    return NOERROR;
}

} // namespace App
} // namespace Droid
} // namespace Elastos
