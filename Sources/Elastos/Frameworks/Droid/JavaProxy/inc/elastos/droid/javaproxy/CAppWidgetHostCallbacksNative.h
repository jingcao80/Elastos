
#ifndef __ELASTOS_DROID_JAVAPROXY_CAPPWIDGETHOSTCALLBACKSNATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CAPPWIDGETHOSTCALLBACKSNATIVE_H__

#include "_Elastos_Droid_JavaProxy_CAppWidgetHostCallbacksNative.h"

#include <jni.h>
#include "elastos/droid/ext/frameworkdef.h"

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CAppWidgetHostCallbacksNative)
{
public:
    CAppWidgetHostCallbacksNative();
    ~CAppWidgetHostCallbacksNative();

    CARAPI constructor(
        /* [in] */ Handle32 jVM,
        /* [in] */ Handle32 jInstance);

    CARAPI UpdateAppWidget(
        /* [in] */ Int32 appWidgetId,
        /* [in] */ IParcelable * pViews);

    CARAPI ProviderChanged(
        /* [in] */ Int32 appWidgetId,
        /* [in] */ Elastos::Droid::AppWidget::IAppWidgetProviderInfo * pInfo);

    CARAPI ProvidersChanged();

    CARAPI ViewDataChanged(
        /* [in] */ Int32 appWidgetId,
        /* [in] */ Int32 viewId);

private:
    static const String TAG;

    JavaVM* mJVM;
    jobject mJInstance;
};

}
}
}

#endif // __ELASTOS_DROID_JAVAPROXY_CAPPWIDGETHOSTCALLBACKSNATIVE_H__
