
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/telecom/StatusHints.h"

namespace Elastos {
namespace Droid {
namespace Telecom {

//===============================================================
// StatusHints::
//===============================================================
CAR_INTERFACE_IMPL_2(StatusHints, Object, IStatusHints, IParcelable)

ECode StatusHints::constructor()
{
    return NOERROR;
}

ECode StatusHints::constructor(
    /* [in] */ IComponentName* packageName,
    /* [in] */ ICharSequence* label,
    /* [in] */ Int32 iconResId,
    /* [in] */ IBundle* extras)
{
    mPackageName = packageName;
    mLabel = label;
    mIconResId = iconResId;
    mExtras = extras;
    return NOERROR;
}

ECode StatusHints::GetPackageName(
    /* [out] */ IComponentName** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mPackageName;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode StatusHints::GetLabel(
    /* [out] */ ICharSequence** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mLabel;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode StatusHints::GetIconResId(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mIconResId;
    return NOERROR;
}

ECode StatusHints::GetIcon(
    /* [in] */ IContext* context,
    /* [out] */ IDrawable** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IDrawable> dr = GetIcon(context, mIconResId);
    *result = dr;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode StatusHints::GetExtras(
    /* [out] */ IBundle** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mExtras;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode StatusHints::WriteToParcel(
    /* [in] */ IParcel* out)
{
    IParcelable::Probe(mPackageName)->WriteToParcel(out);
    IParcelable::Probe(mLabel)->WriteToParcel(out);
    out->WriteInt32(mIconResId);
    IParcelable::Probe(mExtras)->WriteToParcel(out);
    return NOERROR;
}

ECode StatusHints::ReadFromParcel(
    /* [in] */ IParcel* in)
{
    IParcelable::Probe(mPackageName)->ReadFromParcel(in);
    IParcelable::Probe(mLabel)->ReadFromParcel(in);
    in->ReadInt32(&mIconResId);
    IParcelable::Probe(mExtras)->ReadFromParcel(in);
    return NOERROR;
}

AutoPtr<IDrawable> StatusHints::GetIcon(
    /* [in] */ IContext* context,
    /* [in] */ Int32 resId)
{
    AutoPtr<IContext> packageContext;
    // try {
    String name;
    mPackageName->GetPackageName(&name);
    context->CreatePackageContext(name, 0, (IContext**)&packageContext);
    // } catch (PackageManager.NameNotFoundException e) {
    //     Log.e(this, e, "Cannot find package %s", mPackageName.getPackageName());
    //     return null;
    // }
    // try {
    AutoPtr<IDrawable> res;
    packageContext->GetDrawable(resId, (IDrawable**)&res);
    return res;
    // } catch (MissingResourceException e) {
    //     Log.e(this, e, "Cannot find icon %d in package %s",
    //             resId, mPackageName.getPackageName());
    //     return null;
    // }
}

ECode StatusHints::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (other != NULL && IStatusHints::Probe(other) != NULL) {
        AutoPtr<IStatusHints> otherHints = IStatusHints::Probe(other);
        AutoPtr<IComponentName> otherName, name;
        otherHints->GetPackageName((IComponentName**)&otherName);
        GetPackageName((IComponentName**)&name);

        AutoPtr<ICharSequence> otherLabel, label;
        otherHints->GetLabel((ICharSequence**)&otherLabel);
        GetLabel((ICharSequence**)&label);

        Int32 otheId = 0, id = 0;
        otherHints->GetIconResId(&otheId);
        GetIconResId(&id);

        AutoPtr<IBundle> otherExt, ext;
        otherHints->GetExtras((IBundle**)&otherExt);
        GetExtras((IBundle**)&ext);

        *result = Object::Equals(otherName, name) &&
                Object::Equals(otherLabel, label) &&
                (otheId == id) &&
                Object::Equals(otherExt, ext);
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode StatusHints::GetHashCode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;
    Int32 hc = 0;
    IObject::Probe(mPackageName)->GetHashCode(&hc);
    *result += hc;
    hc = 0;
    IObject::Probe(mLabel)->GetHashCode(&hc);
    *result += hc;
    *result += mIconResId;
    hc = 0;
    IObject::Probe(mExtras)->GetHashCode(&hc);
    *result += hc;
    return NOERROR;
}

} // namespace Telecom
} // namespace Droid
} // namespace Elastos