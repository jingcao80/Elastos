#include "CEventLogTagsDescription.h"

namespace Elastos {
namespace Droid {
namespace Utility {

CEventLogTagsDescription::CEventLogTagsDescription()
    : mTag(-1)
    , mName(NULL)
{
}

CEventLogTagsDescription::~CEventLogTagsDescription()
{
}

ECode CEventLogTagsDescription::constructor(
    /* [in] */ Int32 * tag,
    /* [in] */ const String& * name)
{
    mTag = tag;
    mName = name;
    return NOERROR;
}

}
}
}
