
#include "elastos/droid/ext/frameworkext.h"
#include <Elastos.CoreLibrary.IO.h>
#include "elastos/droid/content/CComponentName.h"

using Elastos::Core::StringBuilder;
using Elastos::IO::IWriter;
using Elastos::Core::EIID_ICloneable;
using Elastos::Core::EIID_IComparable;

namespace Elastos {
namespace Droid {
namespace Content {

CAR_INTERFACE_IMPL_4(CComponentName, Object, IComponentName, IParcelable, ICloneable, IComparable)

CAR_OBJECT_IMPL(CComponentName)

ECode CComponentName::constructor()
{
    return NOERROR;
}

ECode CComponentName::constructor(
    /* [in] */ const String& pkg,
    /* [in] */ const String& cls)
{
    if (pkg.IsNull()) {
        //throw new NullPointerException("package name is null");
        return E_NULL_POINTER_EXCEPTION;
    }
    if (cls.IsNull()) {
        //throw new NullPointerException("class name is null");
        return E_NULL_POINTER_EXCEPTION;
    }
    mPackage = pkg;
    mClass = cls;
    return NOERROR;
}

ECode CComponentName::constructor(
    /* [in] */ IContext* pkg,
    /* [in] */ const String& cls)
{
    if (cls.IsNull()) {
        //throw new NullPointerException("class name is null");
        return E_NULL_POINTER_EXCEPTION;
    }
    pkg->GetPackageName(&mPackage);
    mClass = cls;
    return NOERROR;
}

ECode CComponentName::constructor(
    /* [in] */ IContext* pkg,
    /* [in] */ IClassInfo* cls)
{
    pkg->GetPackageName(&mPackage);
    String name;
    cls->GetName(&name);

    //TODO?: In java: cls.getName will get the content(package name + '.' + class name), so...
    mClass = mPackage + String(".") + name;
    return NOERROR;
}

ECode CComponentName::Clone(
    /* [out] */ IInterface** newComponentName)
{
    VALIDATE_NOT_NULL(newComponentName)
    AutoPtr<IComponentName> cloneObj;
    CComponentName::New(mPackage, mClass, (IComponentName**)&cloneObj);
    *newComponentName = TO_IINTERFACE(cloneObj);
    REFCOUNT_ADD(*newComponentName)
    return NOERROR;
}

ECode CComponentName::GetPackageName(
    /* [out] */ String* packageName)
{
    VALIDATE_NOT_NULL(packageName)
    *packageName = mPackage;
    return NOERROR;
}

ECode CComponentName::GetClassName(
    /* [out] */ String* className)
{
    VALIDATE_NOT_NULL(className)
    *className = mClass;
    return NOERROR;
}

ECode CComponentName::GetShortClassName(
    /* [out] */ String* shortClassName)
{
    VALIDATE_NOT_NULL(shortClassName)
    if (mClass.StartWith(mPackage)) {
        Int32 PN = mPackage.GetLength();
        Int32 CN = mClass.GetLength();
        if (CN > PN && mClass.GetChar(PN) == '.') {
            *shortClassName = mClass.Substring(PN, CN);
            return NOERROR;
        }
    }
    *shortClassName = mClass;
    return NOERROR;
}

ECode CComponentName::AppendShortClassName(
    /* [in] */ IStringBuilder* sb,
    /* [in] */ const String& packageName,
    /* [in] */ const String& className)
{
    if (className.StartWith(packageName)) {
        Int32 PN = packageName.GetLength();
        Int32 CN = className.GetLength();
        if (CN > PN && className.GetChar(PN) == '.') {
            sb->Append(className.Substring(PN));
            return NOERROR;
        }
    }
    sb->Append(className);
    return NOERROR;
}

ECode CComponentName::PrintShortClassName(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ const String& packageName,
    /* [in] */ const String& className)
{
    if (className.StartWith(packageName)) {
        Int32 PN = packageName.GetLength();
        Int32 CN = className.GetLength();
        if (CN > PN && className.GetChar(PN) == '.') {
            AutoPtr<ArrayOf<Char32> > chars = className.GetChars();
            IWriter::Probe(pw)->Write(chars, PN, CN - PN);
            return NOERROR;
        }
    }
    pw->Print(className);
    return NOERROR;
}

ECode CComponentName::FlattenToString(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)
    StringBuilder sb;
    sb.Append(mPackage);
    sb.AppendChar('/');
    sb.Append(mClass);
    return sb.ToString(name);
}

ECode CComponentName::FlattenToShortString(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)
    StringBuilder sb(mPackage.GetLength() + mClass.GetLength());
    AppendShortString(&sb, mPackage, mClass);
    *name = sb.ToString();
    return NOERROR;
}

ECode CComponentName::AppendShortString(
    /* [in] */ IStringBuilder* sb)
{
    return AppendShortString(sb, mPackage, mClass);
}

ECode CComponentName::AppendShortString(
    /* [in] */ IStringBuilder* sb,
    /* [in] */ const String& packageName,
    /* [in] */ const String& className)
{
    VALIDATE_NOT_NULL(sb)
    sb->Append(packageName);
    sb->AppendChar('/');
    return AppendShortClassName(sb, packageName, className);
}

ECode CComponentName::PrintShortString(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ const String& packageName,
    /* [in] */ const String& className)
{
    VALIDATE_NOT_NULL(pw)
    pw->Print(packageName);
    pw->PrintChar('/');
    return PrintShortClassName(pw, packageName, className);
}

ECode CComponentName::UnflattenFromString(
    /* [in] */ const String& str,
    /* [out] */ IComponentName** componentName)
{
    VALIDATE_NOT_NULL(componentName);
    Int32 sep = str.IndexOf('/');
    if (sep < 0 || (sep + 1) >= str.GetLength()) {
        *componentName = NULL;
        return NOERROR;
    }
    String pkg = str.Substring(0, sep);
    String cls = str.Substring(sep + 1);
    if (!cls.IsNullOrEmpty() && cls.GetChar(0) == '.') {
        cls = pkg + cls;
    }
    return CComponentName::New(pkg, cls, componentName);
}

ECode CComponentName::ToShortString(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)
    StringBuilder sb;
    sb.AppendChar('{');
    sb.Append(mPackage);
    sb.AppendChar('/');
    sb.Append(mClass);
    sb.AppendChar('}');
    return sb.ToString(name);
}

ECode CComponentName::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    StringBuilder sb;
    sb.Append(String("ComponentInfo{"));
    sb.Append(mPackage);
    sb.AppendChar('/');
    sb.Append(mClass);
    sb.AppendChar('}');
    sb.ToString(str);
    return NOERROR;
}

ECode CComponentName::Equals(
    /* [in] */ IInterface* _obj,
    /* [out] */ Boolean* isEqual)
{
    VALIDATE_NOT_NULL(isEqual)

    IComponentName* obj = IComponentName::Probe(_obj);
    if (NULL != obj) {
        // Note: no null checks, because mPackage and mClass can
        // never be null.
        String pkg;
        String cls;
        obj->GetPackageName(&pkg);
        obj->GetClassName(&cls);
        if (mPackage.Equals(pkg) && mClass.Equals(cls)) {
            *isEqual = TRUE;
            return NOERROR;
        }
    }
    *isEqual = FALSE;
    return NOERROR;
}

ECode CComponentName::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);

    *hashCode = 0;
    if (!mPackage.IsNull()) {
        *hashCode += mPackage.GetHashCode() * 5;
    }
    if (!mClass.IsNull()) {
        *hashCode += mClass.GetHashCode();
    }
    return NOERROR;
}

ECode CComponentName::CompareTo(
    /* [in] */ IInterface* thatObj,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = -1;

    IComponentName* that = IComponentName::Probe(thatObj);
    VALIDATE_NOT_NULL(that)

    Int32 v = 0;
    String pkg;
    that->GetPackageName(&pkg);
    v = mPackage.Compare(pkg);
    if (v != 0) {
        *result = v;
        return NOERROR;
    }
    String cls;
    that->GetClassName(&cls);
    *result = mClass.Compare(cls);
    return NOERROR;
}

ECode CComponentName::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    FAIL_RETURN(source->ReadString(&mPackage));
    FAIL_RETURN(source->ReadString(&mClass));
    return NOERROR;
}

ECode CComponentName::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    FAIL_RETURN(dest->WriteString(mPackage));
    FAIL_RETURN(dest->WriteString(mClass));
    return NOERROR;
}

ECode CComponentName::WriteToParcel(
    /* [in] */ IComponentName* cn,
    /* [in] */ IParcel* dest)
{
    if (NULL != cn) {
        return IParcelable::Probe(cn)->WriteToParcel(dest);
    }
    else {
        return dest->WriteString(String(NULL));
    }
}

ECode CComponentName::ReadFromParcel(
    /* [in] */ IParcel* source,
    /* [out] */ IComponentName** cn)
{
    VALIDATE_NOT_NULL(cn)
    String pkg, cls;
    source->ReadString(&pkg);
    if (!pkg.IsNull()) {
        source->ReadString(&cls);
        return CComponentName::New(pkg, cls, cn);
    }
    *cn = NULL;
    return NOERROR;
}

} // namespace Content
} // namespace Droid
} // namespace Elastos
