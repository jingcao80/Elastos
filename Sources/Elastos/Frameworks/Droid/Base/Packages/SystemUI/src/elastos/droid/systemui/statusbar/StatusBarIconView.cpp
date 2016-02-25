
#include "elastos/droid/systemui/statusbar/StatusBarIconView.h"
#include "elastos/droid/R.h"
#include "../R.h"
#include "Elastos.CoreLibrary.Text.h"
#include "Elastos.Droid.Os.h"
#include <elastos/droid/text/TextUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::PaintAlign_CENTER;
using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Internal::StatusBar::CStatusBarIcon;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::View::Accessibility::IAccessibilityRecord;
using Elastos::Droid::Widget::ImageViewScaleType_CENTER;
using Elastos::Core::ICloneable;
using Elastos::Core::StringBuilder;
using Elastos::Text::INumberFormat;
using Elastos::Text::INumberFormatHelper;
using Elastos::Text::CNumberFormatHelper;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {

const String StatusBarIconView::TAG("StatusBarIconView");
const Boolean StatusBarIconView::DBG = FALSE;

CAR_INTERFACE_IMPL(StatusBarIconView, AnimatedImageView, IStatusBarIconView);
StatusBarIconView::StatusBarIconView()
    : mNumberX(0)
    , mNumberY(0)
{}

ECode StatusBarIconView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    AnimatedImageView::constructor(context, attrs);
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);

    Int32 outerBounds, imageBounds;
    res->GetDimensionPixelSize(R::dimen::status_bar_icon_size, &outerBounds);
    res->GetDimensionPixelSize(R::dimen::status_bar_icon_drawing_size, &imageBounds);
    Float scale = (Float)imageBounds / (Float)outerBounds;
    SetScaleX(scale);
    SetScaleY(scale);
    return NOERROR;
}

ECode StatusBarIconView::SetNotification(
    /* [in] */ INotification* notification)
{
    mNotification = notification;
    SetContentDescription(notification);
    return NOERROR;
}

ECode StatusBarIconView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ const String& slot,
    /* [in] */ INotification* notification)
{
    AnimatedImageView::constructor(context);
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    Int32 color;
    res->GetColor(R::drawable::notification_number_text_color, &color);

    mSlot = slot;
    CPaint::New((IPaint**)&mNumberPain);
    mNumberPain->SetTextAlign(PaintAlign_CENTER);
    mNumberPain->SetColor(color);
    mNumberPain->SetAntiAlias(TRUE);
    SetNotification(notification);

    // We do not resize and scale system icons (on the right), only notification icons (on the
    // left).
    if (notification != NULL) {
        Int32 outerBounds, imageBounds;
        res->GetDimensionPixelSize(R::dimen::status_bar_icon_size, &outerBounds);
        res->GetDimensionPixelSize(R::dimen::status_bar_icon_drawing_size, &imageBounds);
        Float scale = (Float)imageBounds / (Float)outerBounds;
        SetScaleX(scale);
        SetScaleY(scale);
    }

    SetScaleType(ImageViewScaleType_CENTER);
    return NOERROR;
}

Boolean StatusBarIconView::StrEQ(
    /* [in] */ const String& a,
    /* [in] */ const String& b)
{
    if (a == b) {
        return TRUE;
    }
    if (a.IsNull() && !b.IsNull()) {
        return FALSE;
    }
    if (!a.IsNull() && b.IsNull()) {
        return FALSE;
    }
    return a.Equals(b);
}

ECode StatusBarIconView::Set(
    /* [in] */ IStatusBarIcon* icon,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    assert(icon != NULL);
    Boolean iconEquals = FALSE;
    Boolean levelEquals = FALSE;
    Boolean visibilityEquals = FALSE;
    Boolean numberEquals = FALSE;

    String opkg;
    icon->GetIconPackage(&opkg);
    Int32 oid, olevel, on;
    icon->GetIconId(&oid);
    icon->GetIconLevel(&olevel);
    icon->GetNumber(&on);
    Boolean ov;
    icon->GetVisible(&ov);
    AutoPtr<IUserHandle> ouser;
    icon->GetUser((IUserHandle**)&ouser);
    AutoPtr<ICharSequence> oc;
    icon->GetContentDescription((ICharSequence**)&oc);

    AutoPtr<IInterface> obj;
    ICloneable::Probe(icon)->Clone((IInterface**)&obj);
    mIcon = IStatusBarIcon::Probe(obj);
    String pkg;
    mIcon->GetIconPackage(&pkg);
    iconEquals = StrEQ(pkg, opkg);
    if (iconEquals) {
        Int32 id;
        mIcon->GetIconId(&id);
        iconEquals = (id == oid);
    }

    if (iconEquals) {
        Int32 level;
        mIcon->GetIconLevel(&level);
        levelEquals = (level == olevel);
    }

    Boolean v;
    mIcon->GetVisible(&v);
    visibilityEquals = (v == ov);

    Int32 n;
    mIcon->GetNumber(&n);
    numberEquals = (n == on);

    mIcon = NULL;
    CStatusBarIcon::New(opkg, ouser, oid, olevel, on, oc, (IStatusBarIcon**)&mIcon);
    mIcon->SetVisible(ov);

    SetContentDescription(oc);
    if (!iconEquals) {
        if (!UpdateDrawable(FALSE /* no clear */)) {
            *result = FALSE;
            return NOERROR;
        }
    }
    if (!levelEquals) {
        SetImageLevel(olevel);
    }

    if (!numberEquals) {
        AutoPtr<IContext> c;
        GetContext((IContext**)&c);
        AutoPtr<IResources> res;
        c->GetResources((IResources**)&res);

        Boolean showNumber = FALSE;
        if (on > 0 && (res->GetBoolean(
            R::bool_::config_statusBarShowNumber, &showNumber), showNumber)) {
            if (mNumberBackground == NULL) {
                res->GetDrawable(Elastos::Droid::R::drawable::ic_notification_overlay,
                    (IDrawable**)&mNumberBackground);
            }
            PlaceNumber();
        }
        else {
            mNumberBackground = NULL;
            mNumberText = NULL;
        }
        Invalidate();
    }
    if (!visibilityEquals) {
        SetVisibility(ov ? IView::VISIBLE : IView::GONE);
    }
    *result = TRUE;
    return NOERROR;
}

ECode StatusBarIconView::UpdateDrawable()
{
    UpdateDrawable(TRUE /* with clear */);
    return NOERROR;
}

Boolean StatusBarIconView::UpdateDrawable(
    /* [in] */ Boolean withClear)
{
    AutoPtr<IDrawable> drawable = GetIcon(mIcon);
    if (drawable == NULL) {
        Slogger::W(TAG, "No icon for slot %s", mSlot.string());
        return FALSE;
    }
    if (withClear) {
        SetImageDrawable(NULL);
    }
    SetImageDrawable(drawable);
    return TRUE;
}

AutoPtr<IDrawable> StatusBarIconView::GetIcon(
    /* [in] */ IStatusBarIcon* icon)
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IDrawable> d = GetIcon(context, icon);
    return d;
}

AutoPtr<IDrawable> StatusBarIconView::GetIcon(
    /* [in] */ IContext* context,
    /* [in] */ IStatusBarIcon* icon)
{
    assert(icon != NULL);
    AutoPtr<IResources> r;

    String pkg;
    icon->GetIconPackage(&pkg);
    if (!pkg.IsNull()) {
        // try {
        AutoPtr<IUserHandle> user;
        icon->GetUser((IUserHandle**)&user);
        Int32 userId;
        user->GetIdentifier(&userId);

        if (userId == IUserHandle::USER_ALL) {
            userId = IUserHandle::USER_OWNER;
        }

        AutoPtr<IPackageManager> pm;
        context->GetPackageManager((IPackageManager**)&pm);
        ECode ec = pm->GetResourcesForApplicationAsUser(pkg, userId, (IResources**)&r);
        if (ec == (ECode)E_NAME_NOT_FOUND_EXCEPTION) {
            String p;
            icon->GetIconPackage(&p);
            Slogger::E(TAG, "Icon package not found: %s", p.string());
            return NULL;
        }
    }
    else {
        context->GetResources((IResources**)&r);
    }

    // TODO: delete
    if (r == NULL) {
        assert(0 && "TODO");
        Slogger::E("xihao", "StatusBarIconView::GetIcon r == NULL");
        return NULL;
    }

    Int32 iconId;
    icon->GetIconId(&iconId);
    if (iconId == 0) {
        return NULL;
    }

    AutoPtr<IDrawable> d;
    // try {
    if (r != NULL) {
        r->GetDrawable(iconId, (IDrawable**)&d);
    }
    // } catch (RuntimeException e) {
    //     Slog.w(TAG, "Icon not found in "
    //           + (icon.iconPackage != NULL ? icon.iconId : "<system>")
    //           + ": " + Integer.toHexString(icon.iconId));
    // }

    return d;
}

ECode StatusBarIconView::GetStatusBarIcon(
    /* [out] */ IStatusBarIcon** icon)
{
    VALIDATE_NOT_NULL(icon);
    *icon = mIcon;
    REFCOUNT_ADD(*icon);
    return NOERROR;
}

ECode StatusBarIconView::OnInitializeAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    AnimatedImageView::OnInitializeAccessibilityEvent(event);
    if (mNotification != NULL) {
        AutoPtr<IParcelable> p = IParcelable::Probe(mNotification);
        IAccessibilityRecord::Probe(event)->SetParcelableData(p);
    }
    return NOERROR;
}

void StatusBarIconView::OnSizeChanged(
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ Int32 oldw,
    /* [in] */ Int32 oldh)
{
     AnimatedImageView::OnSizeChanged(w, h, oldw, oldh);
    if (mNumberBackground != NULL) {
        PlaceNumber();
    }
}

void StatusBarIconView::OnDraw(
    /* [in] */ ICanvas* canvas)
{
    AnimatedImageView::OnDraw(canvas);

    if (mNumberBackground != NULL) {
        mNumberBackground->Draw(canvas);
        canvas->DrawText(mNumberText, mNumberX, mNumberY, mNumberPain);
    }
}

void StatusBarIconView::PlaceNumber()
{
    String str;
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    Int32 tooBig, number;
    res->GetInteger(Elastos::Droid::R::integer::status_bar_notification_info_maxnum, &tooBig);
    mIcon->GetNumber(&number);
    if (number > tooBig) {
        res->GetString(Elastos::Droid::R::string::status_bar_notification_info_overflow, &str);
    }
    else {
        AutoPtr<INumberFormatHelper> helper;
        CNumberFormatHelper::AcquireSingleton((INumberFormatHelper**)&helper);
        AutoPtr<INumberFormat> f;
        helper->GetIntegerInstance((INumberFormat**)&f);
        f->Format((Int64)number, &str);
    }
    mNumberText = str;

    Int32 w = 0;
    GetWidth(&w);
    Int32 h = 0;
    GetHeight(&h);
    AutoPtr<IRect> rect;
    CRect::New((IRect**)&rect);
    mNumberPain->GetTextBounds(str, 0, str.GetLength(), rect);
    Int32 l, t, r, b;
    rect->Get(&l, &t, &r, &b);
    Int32 tw = r - l;
    Int32 th = b - t;
    Boolean result;
    mNumberBackground->GetPadding(rect, &result);
    rect->Get(&l, &t, &r, &b);
    Int32 dw = l + tw + r;
    Int32 minW;
    mNumberBackground->GetMinimumWidth(&minW);
    if (dw < minW) {
        dw = minW;
    }

    mNumberX = w - r - ((dw - r - l)/2);
    Int32 dh = t + th + b;
    if (dh < minW) {
        dh = minW;
    }
    mNumberY = h - b - ((dh - t - th - b)/2);
    mNumberBackground->SetBounds(w - dw, h - dh, w, h);
}

void StatusBarIconView::SetContentDescription(
    /* [in] */ INotification* notification)
{
    if (notification != NULL) {
        AutoPtr<ICharSequence> tickerText;
        notification->GetTickerText((ICharSequence**)&tickerText);
        if (!TextUtils::IsEmpty(tickerText)) {
            SetContentDescription(tickerText);
        }
    }
}

ECode StatusBarIconView::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    StringBuilder sb("StatusBarIconView(slot=");
    sb += mSlot;
    sb += " icon=";
    sb += mIcon;
    sb += " notification=";
    if (mNotification != NULL) {
        String tmp;
        IObject::Probe(mNotification)->ToString(&tmp);
        sb += tmp;
    }
    else {
        sb += "NULL";
    }
    sb += ")";
    return sb.ToString(str);
}

} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
