
#ifndef __ELASTOS_DROID_INCALLUI_BASEFRAGMENT_H__
#define __ELASTOS_DROID_INCALLUI_BASEFRAGMENT_H__

#include "_Elastos.Droid.Dialer.h"
#include <elastos/droid/app/Fragment.h>

using Elastos::Droid::App::Fragment;

namespace Elastos {
namespace Droid {
namespace InCallUI {

/**
 * Parent for all fragments that use Presenters and Ui design.
 */
class BaseFragment
    : public Fragment
{
public:
    virtual CARAPI_(AutoPtr<IPresenter>) CreatePresenter() = 0;

    virtual CARAPI_(AutoPtr<IUi>) GetUi() = 0;

    virtual CARAPI_(AutoPtr<IPresenter>) GetPresenter();

    // @Override
    CARAPI OnActivityCreated(
        /* [in] */ IBundle* savedInstanceState);

    // @Override
    CARAPI OnDestroyView();

protected:
    CARAPI constructor();

private:
    AutoPtr<IPresenter> mPresenter;
};

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INCALLUI_BASEFRAGMENT_H__
