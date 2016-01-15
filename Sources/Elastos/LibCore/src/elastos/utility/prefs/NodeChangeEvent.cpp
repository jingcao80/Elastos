
#include "NodeChangeEvent.h"

namespace Elastos {
namespace Utility {
namespace Prefs {


CAR_INTERFACE_IMPL(NodeChangeEvent, EventObject, INodeChangeEvent);

NodeChangeEvent::NodeChangeEvent()
{
}

NodeChangeEvent::~NodeChangeEvent()
{
}

ECode NodeChangeEvent::constructor(
    /* [in] */ IPreferences* p,
    /* [in] */ IPreferences* c)
{
    FAIL_RETURN(EventObject::constructor(IObject::Probe(p)))

    mParent = p;
    mChild = c;
    return NOERROR;
}

ECode NodeChangeEvent::GetParent(
    /* [out] */ IPreferences** parent)
{
    VALIDATE_NOT_NULL(parent);
    *parent = mParent;
    REFCOUNT_ADD(*parent);
    return NOERROR;
}

ECode NodeChangeEvent::GetChild(
    /* [out] */ IPreferences** child)
{
    VALIDATE_NOT_NULL(child);
    *child = mChild;
    REFCOUNT_ADD(*child);
    return NOERROR;
}

ECode NodeChangeEvent::WriteObject(
    /* [in] */ IObjectOutputStream* out)/* throws IOException */
{
    return E_NOT_SERIALIZABLE_EXCEPTION;
}

/**
 * This method always throws a <code>NotSerializableException</code>,
 * because this object cannot be serialized,
 */
ECode NodeChangeEvent::ReadObject(
    /* [in] */ IObjectInputStream* in) /*throws IOException, ClassNotFoundException*/
{
    return E_NOT_SERIALIZABLE_EXCEPTION;
}

} // namespace Prefs
} // namespace Utility
} // namespace Elastos
