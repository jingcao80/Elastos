#ifndef __ELASTOS_DROID_VIEW_CDRAGEVENTHELPER_H__
#define __ELASTOS_DROID_VIEW_CDRAGEVENTHELPER_H__

#include "_Elastos_Droid_View_CDragEventHelper.h"

using Elastos::Droid::Content::IClipData;
using Elastos::Droid::Content::IClipDescription;
using Elastos::Droid::View::IDragEvent;

namespace Elastos {
namespace Droid {
namespace View {

CarClass(CDragEventHelper)
{
public:
    CARAPI Obtain(
           /* [out] */  IDragEvent** event);

    CARAPI Obtain(
        /* [in] */ Int32 action,
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ IInterface* localState,
        /* [in] */ IClipDescription* description,
        /* [in] */ IClipData* data,
        /* [in] */ Boolean result,
        /* [out] */ IDragEvent** event);

    CARAPI Obtain(
        /* [in] */ IDragEvent* source,
        /* [out] */ IDragEvent** event);
};

} //view
} //Droid
} //Elastos

#endif // __ELASTOS_DROID_VIEW_CDRAGEVENTHELPER_H__
