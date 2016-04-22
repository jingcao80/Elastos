
#include "CAm.h"
#include "Am.h"

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Am {

CAR_SINGLETON_IMPL(CAm);

CAR_INTERFACE_IMPL(CAm, Singleton, IAm);

ECode CAm::Main(
    /* [in] */ const ArrayOf<String>& args)
{
    Am* am = new Am;
    if (!am) return E_OUT_OF_MEMORY;

    return am->Run(&const_cast<ArrayOf<String>&>(args));
}

}
}
}
}

