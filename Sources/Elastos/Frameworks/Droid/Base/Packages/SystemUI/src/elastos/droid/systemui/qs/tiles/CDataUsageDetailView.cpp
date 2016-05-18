
#include "elastos/droid/systemui/qs/tiles/CDataUsageDetailView.h"
#include "elastos/droid/systemui/FontSizeUtils.h"
#include "../../R.h"
#include <elastos/droid/R.h>
#include <elastos/core/Math.h>

using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Text::CDecimalFormat;
using Elastos::Text::INumberFormat;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Qs {
namespace Tiles {

const Double CDataUsageDetailView::KB = 1024;
const Double CDataUsageDetailView::MB = 1024 * KB;
const Double CDataUsageDetailView::GB = 1024 * MB;
CAR_OBJECT_IMPL(CDataUsageDetailView)
CAR_INTERFACE_IMPL(CDataUsageDetailView, LinearLayout, IDataUsageDetailView)
CDataUsageDetailView::CDataUsageDetailView()
{
    CDecimalFormat::New(String("#.##"), (IDecimalFormat**)&FORMAT);
}

ECode CDataUsageDetailView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return LinearLayout::constructor(context, attrs);
}

void CDataUsageDetailView::OnConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    LinearLayout::OnConfigurationChanged(newConfig);
    FontSizeUtils::UpdateFontSize(this, Elastos::Droid::R::id::title, R::dimen::qs_data_usage_text_size);
    FontSizeUtils::UpdateFontSize(this, R::id::usage_text, R::dimen::qs_data_usage_usage_text_size);
    FontSizeUtils::UpdateFontSize(this, R::id::usage_carrier_text, R::dimen::qs_data_usage_text_size);
    FontSizeUtils::UpdateFontSize(this, R::id::usage_info_top_text, R::dimen::qs_data_usage_text_size);
    FontSizeUtils::UpdateFontSize(this, R::id::usage_period_text, R::dimen::qs_data_usage_text_size);
    FontSizeUtils::UpdateFontSize(this, R::id::usage_info_bottom_text, R::dimen::qs_data_usage_text_size);
}

ECode CDataUsageDetailView::Bind(
    /* [in] */ INetworkControllerDataUsageInfo* info)
{
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    Int32 titleId = 0;
    Int64 bytes = 0;
    Int32 usageColor = R::color::system_accent_color;
    String top;
    String bottom;
    Int64 limitLevel = 0, usageLevel = 0, warningLevel = 0;
    info->GetLimitLevel(&limitLevel);
    info->GetUsageLevel(&usageLevel);
    info->GetWarningLevel(&warningLevel);
    AutoPtr<ArrayOf<IInterface*> > objs = ArrayOf<IInterface*>::Alloc(1);
    AutoPtr<ICharSequence> obj;
    if (usageLevel < warningLevel || limitLevel <= 0) {
        // under warning, or no limit
        titleId = R::string::quick_settings_cellular_detail_data_usage;
        bytes = usageLevel;
        CString::New(FormatBytes(warningLevel), (ICharSequence**)&obj);
        objs->Set(0, obj);
        res->GetString(R::string::quick_settings_cellular_detail_data_warning, objs, &top);
    }
    else if (usageLevel <= limitLevel) {
        // over warning, under limit
        titleId = R::string::quick_settings_cellular_detail_remaining_data;
        bytes = limitLevel - usageLevel;

        CString::New(FormatBytes(usageLevel), (ICharSequence**)&obj);
        objs->Set(0, obj);

        res->GetString(R::string::quick_settings_cellular_detail_data_used, objs, &top);

        obj = NULL;
        CString::New(FormatBytes(limitLevel), (ICharSequence**)&obj);
        objs->Set(0, obj);
        res->GetString(R::string::quick_settings_cellular_detail_data_limit, objs, &bottom);
    }
    else {
        // over limit
        titleId = R::string::quick_settings_cellular_detail_over_limit;
        bytes = usageLevel - limitLevel;

        CString::New(FormatBytes(usageLevel), (ICharSequence**)&obj);
        objs->Set(0, obj);
        res->GetString(R::string::quick_settings_cellular_detail_data_used, objs, &top);

        obj = NULL;
        CString::New(FormatBytes(limitLevel), (ICharSequence**)&obj);
        objs->Set(0, obj);
        res->GetString(R::string::quick_settings_cellular_detail_data_limit, objs, &bottom);
        usageColor = R::color::system_warning_color;
    }

    AutoPtr<IView> view;
    FindViewById(Elastos::Droid::R::id::title, (IView**)&view);
    AutoPtr<ITextView> title = ITextView::Probe(view);
    title->SetText(titleId);

    view = NULL;
    FindViewById(R::id::usage_text, (IView**)&view);
    AutoPtr<ITextView> usage = ITextView::Probe(view);

    obj = NULL;
    CString::New(FormatBytes(bytes), (ICharSequence**)&obj);
    usage->SetText(obj);

    Int32 c = 0;
    res->GetColor(usageColor, &c);
    usage->SetTextColor(c);

    view = NULL;
    FindViewById(R::id::usage_graph, (IView**)&view);
    AutoPtr<IDataUsageGraph> graph = IDataUsageGraph::Probe(view);
    graph->SetLevels(limitLevel, warningLevel, usageLevel);
    view = NULL;
    FindViewById(R::id::usage_carrier_text, (IView**)&view);
    AutoPtr<ITextView> carrier = ITextView::Probe(view);

    String v;
    info->GetCarrier(&v);
    obj = NULL;
    CString::New(v, (ICharSequence**)&obj);
    carrier->SetText(obj);
    view = NULL;
    FindViewById(R::id::usage_period_text, (IView**)&view);
    AutoPtr<ITextView> period = ITextView::Probe(view);
    info->GetPeriod(&v);
    obj = NULL;
    CString::New(v, (ICharSequence**)&obj);
    period->SetText(obj);
    view = NULL;
    FindViewById(R::id::usage_info_top_text, (IView**)&view);
    AutoPtr<ITextView> infoTop = ITextView::Probe(view);
    IView::Probe(infoTop)->SetVisibility(top != NULL ? IView::VISIBLE : IView::GONE);

    obj = NULL;
    CString::New(top, (ICharSequence**)&obj);
    infoTop->SetText(obj);
    FindViewById(R::id::usage_info_bottom_text, (IView**)&view);
    AutoPtr<ITextView> infoBottom = ITextView::Probe(view);
    IView::Probe(infoBottom)->SetVisibility(bottom != NULL ? IView::VISIBLE : IView::GONE);
    obj = NULL;
    CString::New(bottom, (ICharSequence**)&obj);
    infoBottom->SetText(obj);
    return NOERROR;
}

String CDataUsageDetailView::FormatBytes(
    /* [in] */ Int64 bytes)
{
    const Int64 b = Elastos::Core::Math::Abs(bytes);
    Double val;
    String suffix;
    if (b > 100 * MB) {
        val = b / GB;
        suffix = "GB";
    }
    else if (b > 100 * KB) {
        val = b / MB;
        suffix = "MB";
    }
    else {
        val = b / KB;
        suffix = "KB";
    }
    String v;
    return (INumberFormat::Probe(FORMAT)->Format(val * (bytes < 0 ? -1 : 1), &v), v) + " " + suffix;
}

} // namespace Tiles
} // namespace Qs
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
