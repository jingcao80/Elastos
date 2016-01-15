
#include "elastos/droid/content/pm/LabeledIntent.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

CAR_INTERFACE_IMPL(LabeledIntent, Intent, ILabeledIntent)

LabeledIntent::LabeledIntent()
    : mLabelRes(0)
    , mIcon(0)
{}

LabeledIntent::~LabeledIntent()
{}

ECode LabeledIntent::constructor()
{
    return NOERROR;
}

ECode LabeledIntent::constructor(
    /* [in] */ IIntent* origIntent,
    /* [in] */ const String& sourcePackage,
    /* [in] */ Int32 labelRes,
    /* [in] */ Int32 icon)
{
    FAIL_RETURN(Intent::constructor(origIntent))
    mSourcePackage = sourcePackage;
    mLabelRes = labelRes;
    mIcon = icon;

    return NOERROR;
}

ECode LabeledIntent::constructor(
    /* [in] */ IIntent* origIntent,
    /* [in] */ const String& sourcePackage,
    /* [in] */ ICharSequence* nonLocalizedLabel,
    /* [in] */ Int32 icon)
{
    FAIL_RETURN(Intent::constructor(origIntent))
    mSourcePackage = sourcePackage;
    mLabelRes = 0;
    mNonLocalizedLabel = nonLocalizedLabel;
    mIcon = icon;

    return NOERROR;
}

ECode LabeledIntent::constructor(
    /* [in] */ const String& sourcePackage,
    /* [in] */ Int32 labelRes,
    /* [in] */ Int32 icon)
{
    mSourcePackage = sourcePackage;
    mLabelRes = labelRes;
    mIcon = icon;

    return NOERROR;
}

ECode LabeledIntent::constructor(
    /* [in] */ const String& sourcePackage,
    /* [in] */ ICharSequence* nonLocalizedLabel,
    /* [in] */ Int32 icon)
{
    mSourcePackage = sourcePackage;
    mLabelRes = 0;
    mNonLocalizedLabel = nonLocalizedLabel;
    mIcon = icon;

    return NOERROR;
}

ECode LabeledIntent::GetSourcePackage(
    /* [out] */ String* sourcePackage)
{
    VALIDATE_NOT_NULL(sourcePackage);
    *sourcePackage = mSourcePackage;
    return NOERROR;
}

ECode LabeledIntent::GetLabelResource(
    /* [out] */ Int32* labelRes)
{
    VALIDATE_NOT_NULL(labelRes);
    *labelRes = mLabelRes;
    return NOERROR;
}

ECode LabeledIntent::GetNonLocalizedLabel(
    /* [out] */ ICharSequence** label)
{
    VALIDATE_NOT_NULL(label);
    *label = mNonLocalizedLabel;
    REFCOUNT_ADD(*label);
    return NOERROR;
}

ECode LabeledIntent::GetIconResource(
    /* [out] */ Int32* iconRes)
{
    VALIDATE_NOT_NULL(iconRes);
    *iconRes = mIcon;
    return NOERROR;
}

ECode LabeledIntent::LoadLabel(
    /* [in] */ IPackageManager* pm,
    /* [out] */ ICharSequence** label)
{
    VALIDATE_NOT_NULL(label);
    *label = NULL;

    if (mNonLocalizedLabel != NULL) {
        *label = mNonLocalizedLabel;
        REFCOUNT_ADD(*label);
        return NOERROR;
    }

    if (mLabelRes != 0 && !mSourcePackage.IsNull()) {
        FAIL_RETURN(pm->GetText(mSourcePackage, mLabelRes, NULL, label));
    }

    return NOERROR;
}

ECode LabeledIntent::LoadIcon(
    /* [in] */ IPackageManager* pm,
    /* [out] */ IDrawable** icon)
{
    VALIDATE_NOT_NULL(icon);
    *icon = NULL;

    if (mIcon != 0 && !mSourcePackage.IsNull()) {
        FAIL_RETURN(pm->GetDrawable(mSourcePackage, mIcon, NULL, icon));
    }

    return NOERROR;
}

ECode LabeledIntent::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    assert(source != NULL);

    Intent::ReadFromParcel(source);
    source->ReadString(&mSourcePackage);
    source->ReadInt32(&mLabelRes);
    AutoPtr<IInterface> obj;
    source->ReadInterfacePtr((Handle32*)&obj);
    mNonLocalizedLabel = obj != NULL ? ICharSequence::Probe(obj) : NULL;
    source->ReadInt32(&mIcon);

    return NOERROR;
}

ECode LabeledIntent::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    assert(dest != NULL);

    Intent::WriteToParcel(dest);
    dest->WriteString(mSourcePackage);
    dest->WriteInt32(mLabelRes);
    dest->WriteInterfacePtr(mNonLocalizedLabel);
    dest->WriteInt32(mIcon);

    return NOERROR;
}

}
}
}
}

