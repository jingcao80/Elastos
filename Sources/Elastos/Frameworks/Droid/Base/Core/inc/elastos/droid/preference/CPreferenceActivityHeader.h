
#ifndef __ELASTOS_DROID_PREFERENCE_CPREFERENCEACTIVITYHEADER_H__
#define __ELASTOS_DROID_PREFERENCE_CPREFERENCEACTIVITYHEADER_H__

#include "_Elastos_Droid_Preference_CPreferenceActivityHeader.h"
#include "elastos/droid/preference/PreferenceActivity.h"

namespace Elastos {
namespace Droid {
namespace Preference {

CarClass(CPreferenceActivityHeader)
    , public PreferenceActivity::Header
{
public:
    CAR_OBJECT_DECL()
};

}
}
}

#endif // __ELASTOS_DROID_PREFERENCE_CPREFERENCEACTIVITYHEADER_H__
