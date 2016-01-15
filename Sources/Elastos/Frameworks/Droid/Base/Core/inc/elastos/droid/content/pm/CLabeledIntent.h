
#ifndef __ELASTOS_DROID_CONTENT_PM_CLABELEDINTENT_H__
#define __ELASTOS_DROID_CONTENT_PM_CLABELEDINTENT_H__

#include "_Elastos_Droid_Content_Pm_CLabeledIntent.h"
#include "elastos/droid/content/pm/LabeledIntent.h"

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

/**
* A special subclass of Intent that can have a custom label/icon
* associated with it.  Primarily for use with {@link Intent#ACTION_CHOOSER}.
 */
CarClass(CLabeledIntent)
    , public LabeledIntent
{
public:
    CAR_OBJECT_DECL()
};

}
}
}
}

#endif // __ELASTOS_DROID_CONTENT_PM_CLABELEDINTENT_H__
