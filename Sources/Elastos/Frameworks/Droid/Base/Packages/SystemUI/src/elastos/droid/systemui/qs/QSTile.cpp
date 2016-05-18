
#include "elastos/droid/systemui/qs/QSTile.h"
#include "elastos/droid/systemui/qs/QSTileView.h"
#include "Elastos.Droid.Os.h"
#include <elastos/droid/os/Looper.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::Looper;
using Elastos::Droid::SystemUI::StatusBar::Policy::EIID_IListenable;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Qs {

CAR_INTERFACE_IMPL(QSTile::State, Object, IQSTileState)
QSTile::State::State()
    : mVisible(FALSE)
    , mIconId(0)
    , mAutoMirrorDrawable(TRUE)
{}

ECode QSTile::State::CopyTo(
    /* [in] */ IQSTileState* _other,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (_other == NULL) {
        // throw new IllegalArgumentException();
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    State* other = (State*)_other;
    if (!other->GetClass().Equals(GetClass())) {
        // throw new IllegalArgumentException();
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Boolean changed = other->mVisible != mVisible
            || other->mIconId != mIconId
            || !Objects::Equals(other->mIcon, mIcon)
            || !other->mLabel.Equals(mLabel)
            || !other->mContentDescription.Equals(mContentDescription)
            || other->mAutoMirrorDrawable != mAutoMirrorDrawable
            || !other->mDualLabelContentDescription.Equals(mDualLabelContentDescription);
    other->mVisible = mVisible;
    other->mIconId = mIconId;
    other->mIcon = mIcon;
    other->mLabel = mLabel;
    other->mContentDescription = mContentDescription;
    other->mDualLabelContentDescription = mDualLabelContentDescription;
    other->mAutoMirrorDrawable = mAutoMirrorDrawable;
    *result = changed;
    return NOERROR;
}

ECode QSTile::State::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    return ToStringBuilder()->ToString(str);
}

String QSTile::State::GetClass()
{
    return String("Elastos.Droid.SystemUI.Qs.QSTileView.State");
}

AutoPtr<StringBuilder> QSTile::State::ToStringBuilder()
{
    AutoPtr<StringBuilder> sb = new StringBuilder(GetClass()/*.getSimpleName()).append('['*/);
    sb->Append("[");
    sb->Append("mVisible=");
    sb->Append(mVisible);
    sb->Append(",mIconId=");
    sb->Append(mIconId);
    sb->Append(",mIcon=");
    sb->Append(mIcon);
    sb->Append(",mLabel=");
    sb->Append(mLabel);
    sb->Append(",mContentDescription=");
    sb->Append(mContentDescription);
    sb->Append(",mDualLabelContentDescription=");
    sb->Append(mDualLabelContentDescription);
    sb->Append(",mAutoMirrorDrawable=");
    sb->Append(mAutoMirrorDrawable);
    sb->Append("]");
    return sb;
}

QSTile::BooleanState::BooleanState()
    : mValue(FALSE)
{}

ECode QSTile::BooleanState::CopyTo(
    /* [in] */ IQSTileState* other,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<BooleanState> o = (BooleanState*) other;
    Boolean tmp = FALSE;
    Boolean changed = (State::CopyTo(other, &tmp), tmp) || o->mValue != mValue;
    o->mValue = mValue;
    *result = changed;
    return NOERROR;
}

String QSTile::BooleanState::GetClass()
{
    return String("Elastos.Droid.SystemUI.Qs.QSTileView.BooleanState");
}

AutoPtr<StringBuilder> QSTile::BooleanState::ToStringBuilder()
{
    AutoPtr<StringBuilder> rt = State::ToStringBuilder();
    Int32 len = 0;
    rt->Insert((rt->GetLength(&len), len) - 1, String(",value=") + StringUtils::ToString(mValue));
    return rt;
}

QSTile::SignalState::SignalState()
    : mEnabled(FALSE)
    , mConnected(FALSE)
    , mActivityIn(FALSE)
    , mActivityOut(FALSE)
    , mOverlayIconId(0)
    , mFilter(FALSE)
    , mIsOverlayIconWide(FALSE)
{}

ECode QSTile::SignalState::CopyTo(
    /* [in] */ IQSTileState* other,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<SignalState> o = (SignalState*) other;
    Boolean changed = o->mEnabled != mEnabled
            || o->mConnected != mConnected || o->mActivityIn != mActivityIn
            || o->mActivityOut != mActivityOut
            || o->mOverlayIconId != mOverlayIconId
            || o->mIsOverlayIconWide != mIsOverlayIconWide;
    o->mEnabled = mEnabled;
    o->mConnected = mConnected;
    o->mActivityIn = mActivityIn;
    o->mActivityOut = mActivityOut;
    o->mOverlayIconId = mOverlayIconId;
    o->mFilter = mFilter;
    o->mIsOverlayIconWide = mIsOverlayIconWide;
    Boolean tmp = FALSE;
    *result = (State::CopyTo(other, &tmp), tmp) || changed;
    return NOERROR;
}

String QSTile::SignalState::GetClass()
{
    return String("Elastos.Droid.SystemUI.Qs.QSTileView.SignalState");
}

AutoPtr<StringBuilder> QSTile::SignalState::ToStringBuilder()
{
    AutoPtr<StringBuilder> rt = State::ToStringBuilder();
    Int32 len = 0;
    rt->Insert((rt->GetLength(&len), len) - 1, String(",mEnabled=") + StringUtils::ToString(mEnabled));
    rt->Insert((rt->GetLength(&len), len) - 1, String(",mConnected=") + StringUtils::ToString(mConnected));
    rt->Insert((rt->GetLength(&len), len) - 1, String(",mActivityIn=") + StringUtils::ToString(mActivityIn));
    rt->Insert((rt->GetLength(&len), len) - 1, String(",mActivityOut=") + StringUtils::ToString(mActivityOut));
    rt->Insert((rt->GetLength(&len), len) - 1, String(",mOverlayIconId=") + StringUtils::ToString(mOverlayIconId));
    rt->Insert((rt->GetLength(&len), len) - 1, String(",mFilter=") + StringUtils::ToString(mFilter));
    rt->Insert((rt->GetLength(&len), len) - 1, String(",wideOverlayIcon=") + StringUtils::ToString(mIsOverlayIconWide));
    return rt;
}

const Int32 QSTile::H::SET_CALLBACK;
const Int32 QSTile::H::CLICK;
const Int32 QSTile::H::SECONDARY_CLICK;
const Int32 QSTile::H::REFRESH_STATE;
const Int32 QSTile::H::SHOW_DETAIL;
const Int32 QSTile::H::USER_SWITCH;
const Int32 QSTile::H::TOGGLE_STATE_CHANGED;
const Int32 QSTile::H::SCAN_STATE_CHANGED;
const Int32 QSTile::H::DESTROY;
QSTile::H::H(
    /* [in] */ QSTile* host,
    /* [in] */ ILooper* looper)
    : mQsTile(host)
{
    Handler::constructor(looper);
}

ECode QSTile::H::HandleMessage(
    /* [in] */ IMessage* msg)
{
    String name;
    ECode ec = NOERROR;
    do {
        Int32 what = 0, arg1 = 0;
        msg->GetWhat(&what);
        msg->GetArg1(&arg1);
        AutoPtr<IInterface> obj;
        msg->GetObj((IInterface**)&obj);
        if (what == SET_CALLBACK) {
            name = "handleSetCallback";
            mQsTile->HandleSetCallback(IQSTileCallback::Probe(obj));
        }
        else if (what == CLICK) {
            name = "handleClick";
            mQsTile->mAnnounceNextStateChange = TRUE;
            mQsTile->HandleClick();
        }
        else if (what == SECONDARY_CLICK) {
            name = "handleSecondaryClick";
            mQsTile->HandleSecondaryClick();
        }
        else if (what == REFRESH_STATE) {
            name = "handleRefreshState";
            mQsTile->HandleRefreshState(obj);
        }
        else if (what == SHOW_DETAIL) {
            name = "handleShowDetail";
            mQsTile->HandleShowDetail(arg1 != 0);
        }
        else if (what == USER_SWITCH) {
            name = "handleUserSwitch";
            mQsTile->HandleUserSwitch(arg1);
        }
        else if (what == TOGGLE_STATE_CHANGED) {
            name = "handleToggleStateChanged";
            mQsTile->HandleToggleStateChanged(arg1 != 0);
        }
        else if (what == SCAN_STATE_CHANGED) {
            name = "handleScanStateChanged";
            mQsTile->HandleScanStateChanged(arg1 != 0);
        }
        else if (what == DESTROY) {
            name = "handleDestroy";
            mQsTile->HandleDestroy();
        }
        else {
            // throw new IllegalArgumentException("Unknown msg: " + what);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    } while (0/*Throwable t*/);

    if (FAILED(ec)) {
        String error = String("Error in ") + name;
        Logger::W(mQsTile->TAG, error);
        mQsTile->mHost->Warn(error);
    }
    return NOERROR;
}

const Boolean QSTile::DEBUG = Logger::IsLoggable("QSTile", Logger::___DEBUG);
CAR_INTERFACE_IMPL_2(QSTile, Object, IQSTile, IListenable);
QSTile::QSTile()
{
}

ECode QSTile::constructor(
    /* [in] */ IQSTileHost* host)
{
    TAG = String("QSTile. + getClass().getSimpleName()");
    CHandler::New(Looper::GetMainLooper(), (IHandler**)&mUiHandler);
    mState = NewTileState();
    mTmpState = NewTileState();
    mHost = host;
    host->GetContext((IContext**)&mContext);

    AutoPtr<ILooper> looper;
    host->GetLooper((ILooper**)&looper);
    mHandler = new H(this, looper);
    return NOERROR;
}

ECode QSTile::SupportsDualTargets(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

ECode QSTile::GetHost(
    /* [out] */ IQSTileHost** host)
{
    VALIDATE_NOT_NULL(host);
    *host = mHost;
    REFCOUNT_ADD(*host);
    return NOERROR;
}

ECode QSTile::CreateTileView(
    /* [in] */ IContext* context,
    /* [out] */ IQSTileView** view)
{
    VALIDATE_NOT_NULL(view);
    AutoPtr<QSTileView> qv = new QSTileView();
    qv->constructor(context);
    *view = qv;
    REFCOUNT_ADD(*view);
    return NOERROR;
}

ECode QSTile::GetDetailAdapter(
    /* [out] */ IQSTileDetailAdapter** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL; // optional
    return NOERROR;
}

// safe to call from any thread
ECode QSTile::SetCallback(
    /* [in] */ IQSTileCallback* callback)
{
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(H::SET_CALLBACK, callback, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode QSTile::Click()
{
    Boolean tmp = FALSE;
    mHandler->SendEmptyMessage(H::CLICK, &tmp);
    return NOERROR;
}

ECode QSTile::SecondaryClick()
{
    Boolean tmp = FALSE;
    mHandler->SendEmptyMessage(H::SECONDARY_CLICK, &tmp);
    return NOERROR;
}

ECode QSTile::ShowDetail(
    /* [in] */ Boolean show)
{
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(H::SHOW_DETAIL, show ? 1 : 0, 0, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

void QSTile::RefreshState()
{
    RefreshState(NULL);
}

void QSTile::RefreshState(
    /* [in] */ IInterface* arg)
{
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(H::REFRESH_STATE, arg, (IMessage**)&msg);
    msg->SendToTarget();
}

ECode QSTile::UserSwitch(
    /* [in] */ Int32 newUserId)
{
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(H::USER_SWITCH, newUserId, 0, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode QSTile::FireToggleStateChanged(
    /* [in] */ Boolean state)
{
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(H::TOGGLE_STATE_CHANGED, state ? 1 : 0, 0, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode QSTile::FireScanStateChanged(
    /* [in] */ Boolean state)
{
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(H::SCAN_STATE_CHANGED, state ? 1 : 0, 0, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode QSTile::Destroy()
{
    Boolean tmp = FALSE;
    mHandler->SendEmptyMessage(H::DESTROY, &tmp);
    return NOERROR;
}

ECode QSTile::GetState(
    /* [out] */ IQSTileState** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mState;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

void QSTile::HandleSetCallback(
    /* [in] */ IQSTileCallback* callback)
{
    mCallback = callback;
    HandleRefreshState(NULL);
}

void QSTile::HandleSecondaryClick()
{
    // optional
}

void QSTile::HandleRefreshState(
    /* [in] */ IInterface* arg)
{
    HandleUpdateState(mTmpState, arg);
    Boolean changed = FALSE;
    mTmpState->CopyTo(mState, &changed);
    if (changed) {
        HandleStateChanged();
    }
}

void QSTile::HandleStateChanged()
{
    Boolean delayAnnouncement = ShouldAnnouncementBeDelayed();
    if (mCallback != NULL) {
        mCallback->OnStateChanged(mState);
        if (mAnnounceNextStateChange && !delayAnnouncement) {
            String announcement = ComposeChangeAnnouncement();
            if (announcement != NULL) {
                AutoPtr<ICharSequence> v;
                CString::New(announcement, (ICharSequence**)&v);
                mCallback->OnAnnouncementRequested(v);
            }
        }
    }
    mAnnounceNextStateChange = mAnnounceNextStateChange && delayAnnouncement;
}

Boolean QSTile::ShouldAnnouncementBeDelayed()
{
    return FALSE;
}

String QSTile::ComposeChangeAnnouncement()
{
    return String(NULL);
}

void QSTile::HandleShowDetail(
    /* [in] */ Boolean show)
{
    if (mCallback != NULL) {
        mCallback->OnShowDetail(show);
    }
}

void QSTile::HandleToggleStateChanged(
    /* [in] */ Boolean state)
{
    if (mCallback != NULL) {
        mCallback->OnToggleStateChanged(state);
    }
}

void QSTile::HandleScanStateChanged(
    /* [in] */ Boolean state)
{
    if (mCallback != NULL) {
        mCallback->OnScanStateChanged(state);
    }
}

void QSTile::HandleUserSwitch(
    /* [in] */ Int32 newUserId)
{
    HandleRefreshState(NULL);
}

void QSTile::HandleDestroy()
{
    SetListening(FALSE);
    mCallback = NULL;
}

} // namespace Qs
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
