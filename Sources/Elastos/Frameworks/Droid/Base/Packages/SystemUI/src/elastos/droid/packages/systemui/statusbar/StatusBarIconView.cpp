#include "elastos/droid/systemui/statusbar/StatusBarIconView.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Slogger.h>
#include "elastos/droid/R.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/graphics/CPaint.h"
#include "elastos/droid/statusbar/CStatusBarIcon.h"
#include "elastos/droid/systemui/SystemUIR.h"

using Elastos::Utility::Logging::Slogger;
using Elastos::Core::StringBuilder;
using Elastos::Text::INumberFormat;
using Elastos::Text::INumberFormatHelper;
using Elastos::Text::CNumberFormatHelper;
using Elastos::Droid::R;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Graphics::PaintAlign_CENTER;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Widget::ImageViewScaleType_CENTER;
using Elastos::Droid::StatusBar::CStatusBarIcon;
using Elastos::Droid::SystemUI::SystemUIR;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {

const String StatusBarIconView::TAG("StatusBarIconView");
const Boolean StatusBarIconView::DBG = FALSE;

StatusBarIconView::StatusBarIconView()
    : mNumberX(0)
    , mNumberY(0)
{}

StatusBarIconView::StatusBarIconView(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
    : AnimatedImageView(context, attrs)
    , mNumberX(0)
    , mNumberY(0)
{
    InitPrivate(context);
}

StatusBarIconView::StatusBarIconView(
    /* [in] */ IContext* context,
    /* [in] */ const String& slot,
    /* [in] */ INotification* notification)
    : AnimatedImageView(context)
    , mNumberX(0)
    , mNumberY(0)
{
    InitWithNotification(context, slot, notification);
}

ECode StatusBarIconView::Init(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    AnimatedImageView::Init(context, attrs);

    return InitPrivate(context);
}

ECode StatusBarIconView::Init(
    /* [in] */ IContext* context,
    /* [in] */ const String& slot,
    /* [in] */ INotification* notification)
{
    AnimatedImageView::Init(context);

    return InitWithNotification(context, slot, notification);
}

ECode StatusBarIconView::InitWithNotification(
    /* [in] */ IContext* context,
    /* [in] */ const String& slot,
    /* [in] */ INotification* notification)
{
    assert(mNumberPain == NULL);

    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    Int32 color;
    res->GetColor(SystemUIR::drawable::notification_number_text_color, &color);

    mSlot = slot;
    CPaint::New((IPaint**)&mNumberPain);
    mNumberPain->SetTextAlign(PaintAlign_CENTER);
    mNumberPain->SetColor(color);
    mNumberPain->SetAntiAlias(true);
    mNotification = notification;
    SetContentDescriptionByNotification(notification);

    // We do not resize and scale system icons (on the right), only notification icons (on the
    // left).
    if (notification != NULL) {
        InitPrivate(context);
    }

    SetScaleType(ImageViewScaleType_CENTER);
    return NOERROR;
}

ECode StatusBarIconView::InitPrivate(
    /* [in] */ IContext* context)
{
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);

    Int32 outerBounds, imageBounds;
    res->GetDimensionPixelSize(SystemUIR::dimen::status_bar_icon_size, &outerBounds);
    res->GetDimensionPixelSize(SystemUIR::dimen::status_bar_icon_drawing_size, &imageBounds);
    Float scale = (Float)imageBounds / (Float)outerBounds;
    SetScaleX(scale);
    SetScaleY(scale);
    Float alpha;
    res->GetFraction(SystemUIR::dimen::status_bar_icon_drawing_alpha, 1, 1, &alpha);
    SetAlpha(alpha);
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

Boolean StatusBarIconView::Set(
    /* [in] */ IStatusBarIcon* icon)
{
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

    if (mIcon != NULL) {
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
    }

    mIcon = NULL;
    CStatusBarIcon::New(opkg, ouser, oid, olevel, on, oc, (IStatusBarIcon**)&mIcon);
    mIcon->SetVisible(ov);

    SetContentDescription(oc);
    if (!iconEquals) {
        AutoPtr<IDrawable> drawable = GetIcon(icon);
        if (drawable == NULL) {
            Slogger::W(TAG, "No icon for slot %s", mSlot.string());
            return FALSE;
        }
        SetImageDrawable(drawable);
    }
    if (!levelEquals) {
        SetImageLevel(olevel);
    }

    if (!numberEquals) {
        AutoPtr<IResources> res;
        mContext->GetResources((IResources**)&res);

        Boolean showNumber = FALSE;
        if (on > 0) {
            res->GetBoolean(SystemUIR::bool_::config_statusBarShowNumber, &showNumber);
        }

        if (on > 0 && showNumber) {
            if (mNumberBackground == NULL) {
                res->GetDrawable(SystemUIR::drawable::ic_notification_overlay,
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
    return TRUE;
}

AutoPtr<IDrawable> StatusBarIconView::GetIcon(
    /* [in] */ IStatusBarIcon* icon)
{
    AutoPtr<IContext> context = GetContext();
    return GetIcon(context, icon);
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
            return NULL;
        }
        // } catch (PackageManager.NameNotFoundException ex) {
        //     Slog.e(TAG, "Icon package not found: " + icon.iconPackage);
        //     return NULL;
        // }
    }
    else {
        context->GetResources((IResources**)&r);
    }

    // TODO: delete
    if (r == NULL) {
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

AutoPtr<IStatusBarIcon> StatusBarIconView::GetStatusBarIcon()
{
    return mIcon;
}

ECode StatusBarIconView::OnInitializeAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    AnimatedImageView::OnInitializeAccessibilityEvent(event);
    if (mNotification != NULL) {
        event->SetParcelableData(IParcelable::Probe(mNotification.Get()));
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

    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    Int32 tooBig, number;
    res->GetInteger(R::integer::status_bar_notification_info_maxnum, &tooBig);
    mIcon->GetNumber(&number);
    if (number > tooBig) {
        res->GetString(R::string::status_bar_notification_info_overflow, &str);
    }
    else {
        AutoPtr<INumberFormatHelper> helper;
        CNumberFormatHelper::AcquireSingleton((INumberFormatHelper**)&helper);
        AutoPtr<INumberFormat> f;
        helper->GetIntegerInstance((INumberFormat**)&f);
        f->FormatInt64((Int64)number, &str);
    }
    mNumberText = str;

    Int32 w = GetWidth();
    Int32 h = GetHeight();
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

void StatusBarIconView::SetContentDescriptionByNotification(
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

String StatusBarIconView::ToString()
{
    StringBuilder sb("StatusBarIconView(slot=");
    sb += mSlot;
    sb += " icon=";
    sb += mIcon;
    sb += " notification=";
    if (mNotification != NULL) {
        String tmp;
        mNotification->ToString(&tmp);
        sb += tmp;
    }
    else {
        sb += "NULL";
    }
    sb += ")";
    return sb.ToString();;
}

}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
