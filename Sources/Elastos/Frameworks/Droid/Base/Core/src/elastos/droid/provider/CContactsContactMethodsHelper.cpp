#include "elastos/droid/provider/CContactsContactMethodsHelper.h"
#include <elastos/coredef.h>

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CContactsContactMethodsHelper)

CAR_INTERFACE_IMPL(CContactsContactMethodsHelper, Singleton, IContactsContactMethodsHelper)

ECode CContactsContactMethodsHelper::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    return CContactsContactMethods::GetCONTENT_URI(uri);
}

ECode CContactsContactMethodsHelper::GetRAW_CONTENT_EMAIL_URL(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    return CContactsContactMethods::GetRAW_CONTENT_EMAIL_URL(uri);
}

ECode CContactsContactMethodsHelper::EncodePredefinedImProtocol(
    /* [in] */ Int32 protocol,
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value);

    return CContactsContactMethods::EncodePredefinedImProtocol(protocol, value);
}

ECode CContactsContactMethodsHelper::EncodeCustomImProtocol(
    /* [in] */ const String& protocolString,
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value);

    return CContactsContactMethods::EncodeCustomImProtocol(protocolString, value);
}

ECode CContactsContactMethodsHelper::DecodeImProtocol(
    /* [in] */ const String& encodedString,
    /* [out] */ IInterface** value)
{
    VALIDATE_NOT_NULL(value);

    return CContactsContactMethods::DecodeImProtocol(encodedString, value);
}

ECode CContactsContactMethodsHelper::LookupProviderNameFromId(
    /* [in] */ Int32 protocol,
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);

    return CContactsContactMethods::LookupProviderNameFromId(protocol, name);
}

ECode CContactsContactMethodsHelper::GetDisplayLabel(
    /* [in] */ IContext* context,
    /* [in] */ Int32 kind,
    /* [in] */ Int32 type,
    /* [in] */ ICharSequence* label,
    /* [out] */ ICharSequence** lb)
{
    VALIDATE_NOT_NULL(lb);

    return CContactsContactMethods::GetDisplayLabel(context, kind, type, label, lb);
}

} // Provider
} // Droid
} // Elastos