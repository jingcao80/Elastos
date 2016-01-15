
#include "Elastos.CoreLibrary.External.h"
#include "CDOMSource.h"

namespace Elastosx {
namespace Xml {
namespace Transform {
namespace Dom {

CAR_OBJECT_IMPL(CDOMSource);
CAR_INTERFACE_IMPL_2(CDOMSource, Object, IDOMSource, ISource);

ECode CDOMSource::constructor()
{
    return NOERROR;
}

ECode CDOMSource::constructor(
    /* [in] */ INode* n)
{
    return SetNode(n);
}

ECode CDOMSource::constructor(
    /* [in] */ INode* node,
    /* [in] */ const String& systemID)
{
    SetNode(node);
    return SetSystemId(systemID);
}

ECode CDOMSource::SetNode(
    /* [in] */ INode* node)
{
    mNode = node;
    return NOERROR;
}

ECode CDOMSource::GetNode(
    /* [out] */ INode** node)
{
    VALIDATE_NOT_NULL(node);
    *node = mNode;
    REFCOUNT_ADD(*node);
    return NOERROR;
}

ECode CDOMSource::SetSystemId(
    /* [in] */ const String& systemID)
{
    mSystemID = systemID;
    return NOERROR;
}

ECode CDOMSource::GetSystemId(
    /* [out] */ String* id)
{
    VALIDATE_NOT_NULL(id);
    *id = mSystemID;
    return NOERROR;
}

} // namespace Dom
} // namespace Transform
} // namespace Xml
} // namespace Elastosx
