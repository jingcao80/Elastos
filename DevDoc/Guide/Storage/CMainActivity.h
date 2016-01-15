#ifndef __CMAINACTIVITY_H__
#define __CMAINACTIVITY_H__

#include "app/Activity.h"
#include "_CMainActivity.h"

using Elastos::Droid::App::Activity;
using Elastos::Droid::Database::Sqlite::ISQLiteDatabase;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::Widget::IEditText;
using Elastos::Droid::Widget::IRadioButton;
using Elastos::Droid::Widget::IRadioGroup;
using Elastos::Droid::Widget::IRadioGroupOnCheckedChangeListener;
using Elastos::Droid::Widget::ITextView;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace Storage {

class CMainActivity : public Activity
{
public:
    class ButtonClickListener
        : public ElRefBase
        , public IViewOnClickListener
        , public IRadioGroupOnCheckedChangeListener
    {
    public:
        ButtonClickListener(
            /* [in] */ CMainActivity* host);

        CAR_INTERFACE_DECL();

        CARAPI OnClick(
            /* [in] */ IView* v);

        CARAPI OnCheckedChanged (
            /* [in] */ IRadioGroup* group,
            /* [in] */ Int32 checkedId);

    private:
        CMainActivity* mHost;
    };

public:
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    CARAPI SaveToPreference();

    CARAPI SaveToFile();

    CARAPI SaveToDB();

    CARAPI ReadFromPreference();

    CARAPI ReadFromFile();

    CARAPI ReadFromDB();

private :
    AutoPtr<ButtonClickListener> mButtonListener;
    AutoPtr<IEditText> mName;
    AutoPtr<IEditText> mAddress;
    Int32 mRadioButtonId;
    AutoPtr<ITextView> mShwmsg;
    AutoPtr<ISQLiteDatabase> mDB;
    String mNameValue;
    String mAddrValue;

    static const String TAG;
    static const String ID;
    static const String NAME;
    static const String ADDRESS;

    static const String DATABASE_NAME;
    static const Int32  DATABASE_VERSION;
    static const String TABLE_NAME;

    static const String FILE_NAME;
};

}//namespace Storage
}//namespace DevSamples
}//namespace Droid
}//namespace Elastos

#endif // __CMAINACTIVITY_H__
