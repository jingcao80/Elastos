
#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/app/NotificationStyle.h"
#include "elastos/droid/app/CNotificationBuilder.h"
#include "elastos/droid/R.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::View::IView;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::R;
using Elastos::Core::CoreUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace App {

CAR_INTERFACE_IMPL(NotificationStyle, Object, INotificationStyle)

NotificationStyle::NotificationStyle()
    : mSummaryTextSet(FALSE)
{
}

NotificationStyle::~NotificationStyle()
{
}

ECode NotificationStyle::SetBuilder(
    /* [in] */ INotificationBuilder *builder)
{
    if (mBuilder.Get() != builder) {
        mBuilder = builder;
        if (mBuilder != NULL) {
            mBuilder->SetStyle(this);
        }
    }
    return NOERROR;
}

void NotificationStyle::InternalSetBigContentTitle(
    /* [in] */ ICharSequence *title)
{
    mBigContentTitle = title;
}

void NotificationStyle::InternalSetSummaryText(
    /* [in] */ ICharSequence *cs)
{
    mSummaryText = cs;
    mSummaryTextSet = TRUE;
}

ECode NotificationStyle::CheckBuilder()
{
    if (mBuilder == NULL) {
        Logger::I("NotificationStyle", "Style requires a valid Builder object");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

AutoPtr<IRemoteViews> NotificationStyle::GetStandardView(
    /* [in] */ Int32 layoutId)
{
    if (FAILED(CheckBuilder()))
        return NULL;

    // Nasty.
    CNotificationBuilder* builder = (CNotificationBuilder*)mBuilder.Get();
    AutoPtr<ICharSequence> oldBuilderContentTitle = builder->mContentTitle;

    if (mBigContentTitle != NULL) {
        builder->SetContentTitle(mBigContentTitle);
    }

    AutoPtr<IRemoteViews> contentView;
    builder->ApplyStandardTemplateWithActions(layoutId, (IRemoteViews**)&contentView);

    builder->SetContentTitle(oldBuilderContentTitle);

    Boolean visiable = TRUE;
    if (mBigContentTitle != NULL) {
        String str;
        mBigContentTitle->ToString(&str);
        visiable = str.Equals("");
    }

    contentView->SetViewVisibility(R::id::line1,
        visiable ? IView::VISIBLE : IView::GONE);

    // The last line defaults to the subtext, but can be replaced by mSummaryText
    AutoPtr<ICharSequence> overflowText;
    if (mSummaryTextSet){
        overflowText = mSummaryText;
    }
    else {
        builder->GetSubText((ICharSequence**)&overflowText);
    }

    if (overflowText != NULL) {
        contentView->SetTextViewText(R::id::text, builder->ProcessLegacyText(overflowText));
        contentView->SetViewVisibility(R::id::overflow_divider, IView::VISIBLE);
        contentView->SetViewVisibility(R::id::line3, IView::VISIBLE);
    }
    else {
        // Clear text in case we use the line to show the profile badge.
        contentView->SetTextViewText(R::id::text, CoreUtils::Convert(""));
        contentView->SetViewVisibility(R::id::overflow_divider, IView::GONE);
        contentView->SetViewVisibility(R::id::line3, IView::GONE);
    }

    return contentView;
}

ECode NotificationStyle::ApplyTopPadding(
    /* [in] */ IRemoteViews* contentView)
{
    CNotificationBuilder* builder = (CNotificationBuilder*)mBuilder.Get();
    AutoPtr<IResources> res;
    builder->mContext->GetResources((IResources**)&res);
    AutoPtr<IConfiguration> cfg;
    res->GetConfiguration((IConfiguration**)&cfg);
    Float fontScale;
    cfg->GetFontScale(&fontScale);
    Int32 topPadding = CNotificationBuilder::CalculateTopPadding(
        builder->mContext,
        builder->mHasThreeLines, fontScale);
    return contentView->SetViewPadding(R::id::line1, 0, topPadding, 0, 0);
}

ECode NotificationStyle::AddExtras(
    /* [in] */ IBundle* extras)
{
    if (mSummaryTextSet) {
        extras->PutCharSequence(INotification::EXTRA_SUMMARY_TEXT, mSummaryText);
    }
    if (mBigContentTitle != NULL) {
        extras->PutCharSequence(INotification::EXTRA_TITLE_BIG, mBigContentTitle);
    }
    extras->PutString(INotification::EXTRA_TEMPLATE, String("CNotificationStyle"));
    return NOERROR;
}

ECode NotificationStyle::RestoreFromExtras(
    /* [in] */ IBundle* extras)
{
    Boolean bval;
    extras->ContainsKey(INotification::EXTRA_SUMMARY_TEXT, &bval);
    if (bval) {
        mSummaryText = NULL;
        extras->GetCharSequence(INotification::EXTRA_SUMMARY_TEXT, (ICharSequence**)&mSummaryText);
        mSummaryTextSet = TRUE;
    }
    extras->ContainsKey(INotification::EXTRA_TITLE_BIG, &bval);
    if (bval) {
        mBigContentTitle = NULL;
        extras->GetCharSequence(INotification::EXTRA_TITLE_BIG, (ICharSequence**)&mBigContentTitle);
    }
    return NOERROR;
}

ECode NotificationStyle::BuildStyled(
    /* [in] */ INotification* wip)
{
    PopulateTickerView(wip);
    PopulateContentView(wip);
    PopulateBigContentView(wip);
    PopulateHeadsUpContentView(wip);
    return NOERROR;
}

ECode NotificationStyle::PopulateTickerView(
    /* [in] */ INotification* wip)
{
    return NOERROR;
}

ECode NotificationStyle::PopulateContentView(
    /* [in] */ INotification* wip)
{
    return NOERROR;
}

ECode NotificationStyle::PopulateBigContentView(
    /* [in] */ INotification* wip)
{
    return NOERROR;
}

ECode NotificationStyle::PopulateHeadsUpContentView(
    /* [in] */ INotification* wip)
{
    return NOERROR;
}

ECode NotificationStyle::Build(
    /* [out] */ INotification** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    FAIL_RETURN(CheckBuilder())
    return mBuilder->Build(result);
}

Boolean NotificationStyle::HasProgress()
{
    return TRUE;
}

} // namespace App
} // namespace Droid
} // namespace Elastos
