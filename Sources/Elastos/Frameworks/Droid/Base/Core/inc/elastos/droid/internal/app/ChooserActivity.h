
#ifndef __ELASTOS_DROID_INTERNAL_APP_CHOOSERACTIVITY_H__
#define __ELASTOS_DROID_INTERNAL_APP_CHOOSERACTIVITY_H__

#include "elastos/droid/internal/app/ResolverActivity.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace App {

class ChooserActivity
    : public ResolverActivity
    , public IChooserActivity
{
public:
    CAR_INTERFACE_DECL()

    CARAPI GetReplacementIntent(
        /* [in] */ const String& packageName,
        /* [in] */ IIntent* defIntent,
        /* [out] */ IIntent** outIntent);

protected:
    // @Override
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

private:
    CARAPI_(void) ModifyTargetIntent(
        /* [in] */ IIntent* in);

private:
    AutoPtr<IBundle> mReplacementExtras;
};

} // namespace App
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_APP_CHOOSERACTIVITY_H__
