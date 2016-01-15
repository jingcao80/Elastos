
#include "CName.h"
#include "CManifest.h"

namespace Elastos {
namespace Utility {
namespace Jar {

static AutoPtr<IName> CreateName(const String& str)
{
    AutoPtr<CName> name;
    ASSERT_SUCCEEDED(CName::NewByFriend(str, (CName**)&name));
    return (IName*)name.Get();
}

const AutoPtr<IName> CName::MANIFEST_VERSION = CreateName(String("Manifest-Version"));
const AutoPtr<IName> CName::MAIN_CLASS = CreateName(String("Main-Class"));
const AutoPtr<IName> CName::SIGNATURE_VERSION = CreateName(String("Signature-Version"));
const AutoPtr<IName> CName::CONTENT_TYPE = CreateName(String("Content-Type"));
const AutoPtr<IName> CName::SEALED = CreateName(String("Sealed"));
const AutoPtr<IName> CName::IMPLEMENTATION_TITLE = CreateName(String("Implementation-Title"));
const AutoPtr<IName> CName::IMPLEMENTATION_VERSION = CreateName(String("Implementation-Version"));
const AutoPtr<IName> CName::IMPLEMENTATION_VENDOR = CreateName(String("Implementation-Vendor"));
const AutoPtr<IName> CName::SPECIFICATION_TITLE = CreateName(String("Specification-Title"));
const AutoPtr<IName> CName::SPECIFICATION_VERSION = CreateName(String("Specification-Version"));
const AutoPtr<IName> CName::SPECIFICATION_VENDOR = CreateName(String("Specification-Vendor"));
const AutoPtr<IName> CName::EXTENSION_LIST = CreateName(String("Extension-List"));
const AutoPtr<IName> CName::EXTENSION_NAME = CreateName(String("Extension-Name"));
const AutoPtr<IName> CName::EXTENSION_INSTALLATION = CreateName(String("Extension-Installation"));
const AutoPtr<IName> CName::IMPLEMENTATION_VENDOR_ID = CreateName(String("Implementation-Vendor-Id"));
const AutoPtr<IName> CName::IMPLEMENTATION_URL = CreateName(String("Implementation-URL"));
const AutoPtr<IName> CName::NAME = CreateName(String("Name"));
const AutoPtr<IName> CName::CLASS_PATH = CreateName(String("Class-Path"));

CAR_INTERFACE_IMPL(CName, Object, IName)

CAR_OBJECT_IMPL(CName)

CName::CName()
    : mHashCode(0)
{
}

ECode CName::GetName(
    /* [out] */ String * pName)
{
    VALIDATE_NOT_NULL(pName)
    *pName = mName;
    return NOERROR;
}

ECode CName::Equals(
    /* [in] */ IInterface * pObject,
    /* [out] */ Boolean * pResult)
{
    VALIDATE_NOT_NULL(pResult)

    IName* otherName = IName::Probe(pObject);
    if (otherName) {
        *pResult =  ((CName*)otherName)->mName.EqualsIgnoreCase(mName);
    }
    else {
        *pResult = FALSE;
    }
    return NOERROR;
}

ECode CName::GetHashCode(
    /* [out] */ Int32 * pHashCode)
{
    VALIDATE_NOT_NULL(pHashCode)
    if (mHashCode == 0) {
        mHashCode = mName.ToLowerCase().GetHashCode();
    }
    *pHashCode = mHashCode;
    return NOERROR;
}

ECode CName::ToString(
    /* [out] */ String * pName)
{
    VALIDATE_NOT_NULL(pName)
    *pName = mName;
    return NOERROR;
}

ECode CName::constructor(
    /* [in] */ const String& name)
{
    return constructor(name, TRUE);
}

ECode CName::constructor(
    /* [in] */ const String& name,
    /* [in] */ Boolean validateName)
{
    Int32 length = name.GetByteLength();
    if (length == 0 || length > CManifest::LINE_LENGTH_LIMIT - 2) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (validateName) {
        const char* p = name.string();
        char ch;
        Int32 i = 0;
        while (i < length) {
            ch = *(p + i);
            if (!((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')
                    || ch == '_' || ch == '-' || (ch >= '0' && ch <= '9'))) {
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
            ++i;
        }
    }

    mName = name;
    return NOERROR;
}

} // namespace Jar
} // namespace Utility
} // namespace Elastos

