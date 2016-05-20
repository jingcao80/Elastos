
#include "PreferenceChangeEvent.h"

namespace Elastos {
namespace Utility {
namespace Prefs {

CAR_INTERFACE_IMPL(PreferenceChangeEvent, EventObject, IPreferenceChangeEvent)

PreferenceChangeEvent::PreferenceChangeEvent()
{
}

ECode PreferenceChangeEvent::constructor(
    /* [in] */ Preferences* p,
    /* [in] */ const String& k,
    /* [in] */ const String& v)
{
    FAIL_RETURN(EventObject::constructor(p))
    mNode = p;
    mKey = k;
    mValue = v;
    return NOERROR;
}

ECode PreferenceChangeEvent::GetKey(
    /* [out] */ String* key)
{
    VALIDATE_NOT_NULL(key);
    *key = mKey;
    return NOERROR;
}

ECode PreferenceChangeEvent::GetNewValue(
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value);
    *value = mValue;
    return NOERROR;
}

ECode PreferenceChangeEvent::GetNode(
    /* [out] */ IPreferences** pf)
{
    VALIDATE_NOT_NULL(pf);
    *pf = mNode;
    REFCOUNT_ADD(*pf);
    return NOERROR;
}

ECode PreferenceChangeEvent::WriteObject(
    /* [in] */ IObjectOutputStream* out) /*throws IOException*/
{
    // throw new NotSerializableException();
    return E_NOT_SERIALIZABLE_EXCEPTION;
}

/**
 * This method always throws a <code>NotSerializableException</code>,
 * because this object cannot be serialized,
 */
ECode PreferenceChangeEvent::ReadObject(
    /* [in] */ IObjectInputStream* in) /*throws IOException*/
{
    // throw new NotSerializableException();
    return E_NOT_SERIALIZABLE_EXCEPTION;
}

ECode PreferenceChangeEvent::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info);
    *info = String("Elastos.Utility.Prefs.PreferenceChangeEvent");
    return NOERROR;
}

} // namespace Prefs
} // namespace Utility
} // namespace Elastos
