#include "elastos/droid/app/CNotificationAction.h"
#include "elastos/droid/app/CPendingIntent.h"
// #include "elastos/droid/app/CRemoteInput.h"
#include "elastos/droid/os/CBundle.h"
#include "elastos/droid/os/CParcel.h"
#include "elastos/droid/text/TextUtils.h"

using Elastos::Droid::Os::CBundle;
using Elastos::Droid::App::CPendingIntent;
using Elastos::Droid::Text::TextUtils;
using Elastos::Core::EIID_ICloneable;

namespace Elastos {
namespace Droid {
namespace App {

CAR_INTERFACE_IMPL_3(CNotificationAction, Object, INotificationAction, IParcelable, ICloneable)

CAR_OBJECT_IMPL(CNotificationAction)

CNotificationAction::CNotificationAction()
    : mIcon(-1)
{
}

CNotificationAction::~CNotificationAction()
{
}

ECode CNotificationAction::constructor()
{
    return NOERROR;
}

ECode CNotificationAction::constructor(
    /* [in] */ Int32 icon,
    /* [in] */ ICharSequence* title,
    /* [in] */ IPendingIntent* intent)
{
    AutoPtr<IBundle> bundle;
    CBundle::New((IBundle**)&bundle);
    return constructor(icon, title, intent, bundle, NULL);
}

ECode CNotificationAction::constructor(
    /* [in] */ Int32 icon,
    /* [in] */ ICharSequence* title,
    /* [in] */ IPendingIntent* intent,
    /* [in] */ IBundle* extras,
    /* [in] */ ArrayOf<IRemoteInput*>* remoteInputs)
{
    mIcon = icon;
    mTitle = title;
    mActionIntent = intent;
    if (extras != NULL) {
        mExtras = extras;
    }
    else {
        CBundle::New((IBundle**)&mExtras);
    }
    mRemoteInputs = remoteInputs;
    return NOERROR;
}

ECode CNotificationAction::WriteToParcel(
    /* [in] */ IParcel* out)
{
    out->WriteInt32(mIcon);
    TextUtils::WriteToParcel(mTitle, out);
    if (mActionIntent != NULL) {
        out->WriteInt32(1);
        IParcelable* parcleable = IParcelable::Probe(mActionIntent.Get());
        assert(parcleable);
        parcleable->WriteToParcel(out);
    } else {
        out->WriteInt32(0);
    }

    Elastos::Droid::Os::CParcel::WriteBundle(out, mExtras);

    if (mRemoteInputs != NULL) {
        Int32 N = mRemoteInputs->GetLength();
        out->WriteInt32(N);
        for (Int32 i = 0; i < N; ++i) {
            IRemoteInput* ri = (*mRemoteInputs)[i];
            if (ri != NULL) {
                out->WriteInt32(1);
                IParcelable::Probe(ri)->WriteToParcel(out);
            }
            else {
                out->WriteInt32(0);
            }
        }
    }
    else {
        out->WriteInt32(-1);
    }
    return NOERROR;
}

ECode CNotificationAction::ReadFromParcel(
    /* [in] */ IParcel* in)
{
    in->ReadInt32(&mIcon);
    mTitle = NULL;
    FAIL_RETURN(TextUtils::CHAR_SEQUENCE_CREATOR::CreateFromParcel(in, (ICharSequence**)&mTitle));

    mActionIntent = NULL;
    Int32 value;
    in->ReadInt32(&value);
    if (value == 1) {
        CPendingIntent::New((IPendingIntent**)&mActionIntent);
        IParcelable* parcleable = IParcelable::Probe(mActionIntent.Get());
        assert(parcleable);
        parcleable->ReadFromParcel(in);
    }

    mExtras = Elastos::Droid::Os::CParcel::ReadBundle(in);

    mRemoteInputs = NULL;
    in->ReadInt32(&value);
    if (value >= 0) {
        mRemoteInputs = ArrayOf<IRemoteInput*>::Alloc(value);
        for (Int32 i = 0; i < value; ++i) {
            in->ReadInt32(&value);
            if (value == 1) {
                AutoPtr<IRemoteInput> ri;
                // CRemoteInput::New((IRemoteInput**)&ri);
                IParcelable::Probe(ri)->ReadFromParcel(in);
                mRemoteInputs->Set(i, ri);
            }
        }
    }

    return NOERROR;
}

ECode CNotificationAction::GetExtras(
    /* [out] */ IBundle** extras)
{
    VALIDATE_NOT_NULL(extras)
    *extras = mExtras;
    REFCOUNT_ADD(*extras)
    return NOERROR;
}

ECode CNotificationAction::GetRemoteInputs(
    /* [out, callee] */ ArrayOf<IRemoteInput*>** inputs)
{
    VALIDATE_NOT_NULL(inputs)
    *inputs = mRemoteInputs;
    REFCOUNT_ADD(*inputs)
    return NOERROR;
}

ECode CNotificationAction::Clone(
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj)

    AutoPtr<IBundle> bundle;
    CBundle::New(mExtras, (IBundle**)&bundle);
    AutoPtr<ArrayOf<IRemoteInput*> > ris;
    GetRemoteInputs((ArrayOf<IRemoteInput*>**)&ris);
    AutoPtr<INotificationAction> na;
    CNotificationAction::New(
        mIcon, mTitle, mActionIntent,
        bundle, ris, (INotificationAction**)&na);
    *obj = na.Get();
    REFCOUNT_ADD(*obj)
    return NOERROR;
}

ECode CNotificationAction::GetIcon(
    /* [out] */ Int32* icon)
{
    VALIDATE_NOT_NULL(icon);
    *icon = mIcon;
    return NOERROR;
}

ECode CNotificationAction::SetIcon(
    /* [in] */ Int32 icon)
{
    mIcon = icon;
    return NOERROR;
}

ECode CNotificationAction::GetTitle(
    /* [out] */ ICharSequence** title)
{
    VALIDATE_NOT_NULL(title);
    *title = mTitle;
    REFCOUNT_ADD(*title);
    return NOERROR;
}

ECode CNotificationAction::SetTitle(
    /* [in] */ ICharSequence* title)
{
    mTitle = title;
    return NOERROR;
}

ECode CNotificationAction::GetActionIntent(
    /* [out] */ IPendingIntent** actionIntent)
{
    VALIDATE_NOT_NULL(actionIntent);
    *actionIntent = mActionIntent;
    REFCOUNT_ADD(*actionIntent);
    return NOERROR;
}

ECode CNotificationAction::SetActionIntent(
    /* [in] */ IPendingIntent* actionIntent)
{
    mActionIntent = actionIntent;
    return NOERROR;
}

}
}
}
