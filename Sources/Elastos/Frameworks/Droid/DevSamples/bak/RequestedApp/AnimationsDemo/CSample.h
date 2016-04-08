
#ifndef __CSAMPLE_H__
#define __CSAMPLE_H__

#include "_CSample.h"
#include "CMainActivity.h"

using Elastos::Droid::App::IActivity;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace AnimationsDemo {

/**
 * This class describes an individual sample (the sample title, and the activity class that
 * demonstrates this sample).
 */
CarClass(CSample)
{
public:
    CARAPI constructor(
        /* [in] */ IActivity* host,
        /* [in] */ Int32 titleResId,
        /* [in] */ IClassInfo* activityClass);

    ~CSample();

    CARAPI ToString(
        /* [out] */ String* str);

private:
    CMainActivity* mHost;
    String mTitle;
    AutoPtr<IClassInfo> mActivityClass;
    friend class CMainActivity;
};

} // namespace AnimationsDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos

#endif // __CSAMPLE_H__
