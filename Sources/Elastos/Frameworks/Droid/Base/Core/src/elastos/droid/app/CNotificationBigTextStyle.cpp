
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/app/CNotificationBigTextStyle.h"
#include "elastos/droid/app/CNotification.h"
#include "elastos/droid/app/CNotificationBuilder.h"
#include "elastos/droid/R.h"

using Elastos::Droid::R;
using Elastos::Droid::View::IView;

namespace Elastos {
namespace Droid {
namespace App {

const Int32 CNotificationBigTextStyle::MAX_LINES = 13;
const Int32 CNotificationBigTextStyle::LINES_CONSUMED_BY_ACTIONS = 3;
const Int32 CNotificationBigTextStyle::LINES_CONSUMED_BY_SUMMARY = 2;

CAR_INTERFACE_IMPL(CNotificationBigTextStyle, NotificationStyle, INotificationBigTextStyle)

CAR_OBJECT_IMPL(CNotificationBigTextStyle)

CNotificationBigTextStyle::CNotificationBigTextStyle()
{
}

CNotificationBigTextStyle::~CNotificationBigTextStyle()
{
}

ECode CNotificationBigTextStyle::constructor()
{
    return NOERROR;
}

ECode CNotificationBigTextStyle::constructor(
    /* [in] */ INotificationBuilder* builder)
{
    return NotificationStyle::SetBuilder(builder);
}

ECode CNotificationBigTextStyle::SetBigContentTitle(
    /* [in] */ ICharSequence* title)
{
    InternalSetBigContentTitle(CNotification::SafeCharSequence(title));
    return NOERROR;
}

ECode CNotificationBigTextStyle::SetSummaryText(
    /* [in] */ ICharSequence* cs)
{
    InternalSetSummaryText(CNotification::SafeCharSequence(cs));
    return NOERROR;
}

ECode CNotificationBigTextStyle::BigText(
    /* [in] */ ICharSequence* cs)
{
    mBigText = CNotification::SafeCharSequence(cs);
    return NOERROR;
}

ECode CNotificationBigTextStyle::AddExtras(
    /* [in] */ IBundle* extras)
{
    NotificationStyle::AddExtras(extras);

    extras->PutCharSequence(INotification::EXTRA_BIG_TEXT, mBigText);
    return NOERROR;
}

ECode CNotificationBigTextStyle::RestoreFromExtras(
    /* [in] */ IBundle* extras)
{
    NotificationStyle::RestoreFromExtras(extras);

    mBigText = NULL;
    extras->GetCharSequence(INotification::EXTRA_BIG_TEXT, (ICharSequence**)&mBigText);
    return NOERROR;
}

AutoPtr<IRemoteViews> CNotificationBigTextStyle::MakeBigContentView()
{
    assert(mBuilder != NULL);

    CNotificationBuilder* builder = (CNotificationBuilder*)mBuilder.Get();
    // Nasty
    AutoPtr<ICharSequence> oldBuilderContentText = builder->mContentText;
    builder->mContentText = NULL;

    AutoPtr<IRemoteViews> contentView = GetStandardView(builder->GetBigTextLayoutResource());

    builder->mContentText = oldBuilderContentText;

    contentView->SetTextViewText(R::id::big_text, builder->ProcessLegacyText(mBigText));
    contentView->SetViewVisibility(R::id::big_text, IView::VISIBLE);
    contentView->SetInt32(R::id::big_text, String("setMaxLines"), CalculateMaxLines());
    contentView->SetViewVisibility(R::id::text2, IView::GONE);

    ApplyTopPadding(contentView);

    builder->ShrinkLine3Text(contentView);

    builder->AddProfileBadge(contentView, R::id::profile_badge_large_template);

    return contentView;
}

Int32 CNotificationBigTextStyle::CalculateMaxLines()
{
    CNotificationBuilder* builder = (CNotificationBuilder*)mBuilder.Get();
    Int32 lineCount = MAX_LINES;
    Boolean hasActions = builder->mActions.GetSize() > 0;
    Boolean hasSummary = (mSummaryTextSet ? mSummaryText : builder->mSubText) != NULL;
    if (hasActions) {
        lineCount -= LINES_CONSUMED_BY_ACTIONS;
    }
    if (hasSummary) {
        lineCount -= LINES_CONSUMED_BY_SUMMARY;
    }

    // If we have less top padding at the top, we can fit less lines.
    if (!builder->mHasThreeLines) {
        lineCount--;
    }
    return lineCount;
}

ECode CNotificationBigTextStyle::PopulateBigContentView(
    /* [in] */ INotification* wip)
{
    CNotificationBuilder* builder = (CNotificationBuilder*)mBuilder.Get();
    builder->SetBuilderBigContentView(wip, MakeBigContentView());
    return NOERROR;
}

} // namespace App
} // namespace Droid
} // namespace Elastos
