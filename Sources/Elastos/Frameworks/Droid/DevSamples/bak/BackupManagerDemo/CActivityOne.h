#ifndef __CACTIVITYONE_H__
#define __CACTIVITYONE_H__

#include "elastos/droid/app/Activity.h"
#include "_CActivityOne.h"
#include "elastos/droid/content/BroadcastReceiver.h"

using Elastos::IO::IRandomAccessFile;
using Elastos::IO::CRandomAccessFile;
using Elastos::Droid::App::Backup::IIBackupManager;
using Elastos::Droid::App::Activity;
using Elastos::Droid::Content::BroadcastReceiver;

using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::IEditText;
using Elastos::Droid::Widget::IRadioGroup;
using Elastos::Droid::Widget::ICheckBox;
using Elastos::Droid::Widget::IRadioGroupOnCheckedChangeListener;
using Elastos::Droid::Widget::ICompoundButtonOnCheckedChangeListener;
using Elastos::Droid::Widget::ICompoundButton;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace BackupManagerDemo {

class CActivityOne : public Activity
{
public:
    class MyListener
        : public IRadioGroupOnCheckedChangeListener
        , public ICompoundButtonOnCheckedChangeListener
        , public ElRefBase
    {
    public:
        MyListener(
            /* [in] */ CActivityOne* host);

        CARAPI_(PInterface) Probe(
            /* [in]  */ REIID riid);

        CARAPI_(UInt32) AddRef();

        CARAPI_(UInt32) Release();

        CARAPI GetInterfaceID(
            /* [in] */ IInterface *pObject,
            /* [out] */ InterfaceID *pIID);

        CARAPI OnCheckedChanged(
            /* [in] */  IRadioGroup* group,
            /* [in] */  Int32 checkedId);
        CARAPI OnCheckedChanged(
            /* [in] */ ICompoundButton* buttonView,
            /* [in] */ Boolean isChecked);
    private:
        AutoPtr<CActivityOne> mHost;
    };

protected:
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    CARAPI OnStart();

    CARAPI OnResume();

    CARAPI OnPause();

    CARAPI OnStop();

    CARAPI OnDestroy();

private:
    /*
     * configure the UI base on our persistent data
     */
    CARAPI_(void) PopulateUI();

    CARAPI_(void) WriteDataToFileLocked(
        /* [in] */ IRandomAccessFile* file,
        /* [in] */ Boolean addMayo,
        /* [in] */ Boolean addTomato,
        /* [in] */ Int32 whichFilling);

    CARAPI_(void) RecordNewUIState();

public:
    static const String TAG;
    static const String DATA_FILE_NAME;
    static const String PKG;

    AutoPtr<IFile> mDataFile;

    AutoPtr<IIBackupManager> mBackupManager;

    AutoPtr<IRadioGroup> mFillingGroup;
    AutoPtr<ICheckBox> mAddMayoCheckbox;
    AutoPtr<ICheckBox> mAddTomatoCheckbox;
    AutoPtr<IEditText> mEditText;

    //mutex
    static Object sDatalock;
};

} // namespace BackupManagerDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos

#endif // __CACTIVITYONE_H__
