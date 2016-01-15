

#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/DisplayAdjustments.h"
#include "elastos/droid/content/res/CCompatibilityInfoHelper.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Content::Res::ICompatibilityInfoHelper;
using Elastos::Droid::Content::Res::CCompatibilityInfoHelper;

namespace Elastos {
namespace Droid {
namespace View {

AutoPtr<IDisplayAdjustments> DisplayAdjustments::DEFAULT_DISPLAY_ADJUSTMENTS = new DisplayAdjustments();

CAR_INTERFACE_IMPL(DisplayAdjustments, Object, IDisplayAdjustments)

DisplayAdjustments::DisplayAdjustments()
{}

DisplayAdjustments::DisplayAdjustments(
    /* [in] */ IBinder* token)
{
    mActivityToken = token;
}

DisplayAdjustments::DisplayAdjustments(
    /* [in] */ IDisplayAdjustments* daj)
{
    AutoPtr<ICompatibilityInfo> info;
    AutoPtr<IBinder> token;
    daj->GetCompatibilityInfo((ICompatibilityInfo**)&info);
    daj->GetActivityToken((IBinder**)&token);
    new(this)DisplayAdjustments(info, token);
}

DisplayAdjustments::DisplayAdjustments(
    /* [in] */ ICompatibilityInfo* compatInfo,
    /* [in] */ IBinder* token)
{
    SetCompatibilityInfo(compatInfo);
    mActivityToken = token;
}

ECode DisplayAdjustments::SetCompatibilityInfo(
    /* [in] */ ICompatibilityInfo* compatInfo)
{
    if (this == DEFAULT_DISPLAY_ADJUSTMENTS) {
        SLOGGERE("DisplayAdjustments", "SetCompatibilityInfo: Cannot modify DEFAULT_DISPLAY_ADJUSTMENTS")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Boolean isScalingRequired, supportsScreen;
    compatInfo->IsScalingRequired(&isScalingRequired);
    compatInfo->SupportsScreen(&supportsScreen);
    if (compatInfo != NULL && (isScalingRequired
            || !supportsScreen)) {
        mCompatInfo = compatInfo;
    } else {
        AutoPtr<ICompatibilityInfoHelper> helper;
        CCompatibilityInfoHelper::AcquireSingleton((ICompatibilityInfoHelper**)&helper);
        AutoPtr<ICompatibilityInfo> defInfo;
        helper->GetDefault((ICompatibilityInfo**)&defInfo);
        mCompatInfo = defInfo;
    }
    return NOERROR;
}

ECode DisplayAdjustments::GetCompatibilityInfo(
    /* [out] */ ICompatibilityInfo** info)
{
    VALIDATE_NOT_NULL(info)
    *info = mCompatInfo;
    REFCOUNT_ADD(*info)
    return NOERROR;
}

ECode DisplayAdjustments::SetActivityToken(
    /* [in] */ IBinder* token)
{
    if (this == DEFAULT_DISPLAY_ADJUSTMENTS) {
        SLOGGERE("DisplayAdjustments", "SetCompatibilityInfo: Cannot modify DEFAULT_DISPLAY_ADJUSTMENTS")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mActivityToken = token;
    return NOERROR;
}

ECode DisplayAdjustments::GetActivityToken(
    /* [out] */ IBinder** token)
{
    VALIDATE_NOT_NULL(token)
    *token = mActivityToken;
    REFCOUNT_ADD(*token)
    return NOERROR;
}

ECode DisplayAdjustments::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    Int32 hash = 17;
    Int32 infoHash;
    IObject::Probe(mCompatInfo)->GetHashCode(&infoHash);
    hash = hash * 31 + infoHash;
    if (DEVELOPMENT_RESOURCES_DEPEND_ON_ACTIVITY_TOKEN) {
        Int32 tokenHash;
        IObject::Probe(mActivityToken)->GetHashCode(&tokenHash);
        hash = hash * 31 + (mActivityToken == NULL ? 0 : tokenHash);
    }
    return hash;
}

ECode DisplayAdjustments::Equals(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* equals)
{
    if (IDisplayAdjustments::Probe(object) == NULL) {
        return FALSE;
    }
    AutoPtr<IDisplayAdjustments> dajTmp = IDisplayAdjustments::Probe(object);
    AutoPtr<DisplayAdjustments> daj = (DisplayAdjustments*)dajTmp.Get();
    Boolean infoEquals, tokenEquals;
    IObject::Probe(mCompatInfo)->Equals(daj->mCompatInfo, &infoEquals);
    IObject::Probe(mActivityToken)->Equals(daj->mActivityToken, &tokenEquals);
    return infoEquals && tokenEquals;
}

ECode DisplayAdjustments::GetDEFAULT_DISPLAY_ADJUSTMENTS(
    /* [out] */ IDisplayAdjustments** daj)
{
    VALIDATE_NOT_NULL(daj)
    *daj = DEFAULT_DISPLAY_ADJUSTMENTS;
    REFCOUNT_ADD(*daj)
    return NOERROR;
}

} // namespace View
} // namespace Droid
} // namespace Elastos
