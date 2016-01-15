
#include "CActivityOne.h"
#include "R.h"
#include <elastos/utility/logging/Slogger.h>
#include "elastos/droid/os/ServiceManager.h"

using Elastos::IO::CFile;
using Elastos::IO::IDataInput;
using Elastos::IO::IDataOutput;
using Elastos::Utility::Logging::Slogger;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Widget::EIID_IRadioGroupOnCheckedChangeListener;
using Elastos::Droid::Widget::ICheckable;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace BackupManagerDemo {

CActivityOne::MyListener::MyListener(
    /* [in] */ CActivityOne* host)
    :mHost(host)
{

}

PInterface CActivityOne::MyListener::Probe(
    /* [in]  */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)(IRadioGroupOnCheckedChangeListener*)this;
    }
    else if (riid == EIID_IRadioGroupOnCheckedChangeListener) {
        return (IRadioGroupOnCheckedChangeListener*)this;
    }
    return NULL;
}

UInt32 CActivityOne::MyListener::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 CActivityOne::MyListener::Release()
{
    return ElRefBase::Release();
}

ECode CActivityOne::MyListener::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    if (pIID == NULL) {
        return E_INVALID_ARGUMENT;
    }

    if (pObject == (IInterface*)(IRadioGroupOnCheckedChangeListener*)this) {
        *pIID = EIID_IRadioGroupOnCheckedChangeListener;
    }
    else {
        return E_INVALID_ARGUMENT;
    }
    return NOERROR;
}

ECode CActivityOne::MyListener::OnCheckedChanged(
    /* [in] */  IRadioGroup* group,
    /* [in] */  Int32 checkedId)
{
    Slogger::E(TAG, "New radio item selected: %d", checkedId);
    mHost->RecordNewUIState();
    return NOERROR;
}

ECode CActivityOne::MyListener::OnCheckedChanged(
            /* [in] */ ICompoundButton* buttonView,
            /* [in] */ Boolean isChecked)
{
    Slogger::E(TAG, "ICompoundButton selected: %d", isChecked);
    mHost->RecordNewUIState();
    return NOERROR;
}

Mutex CActivityOne::sDatalock;
const String CActivityOne::TAG("Backup-->CActivityOne");
const String CActivityOne::DATA_FILE_NAME("saved_data");
const String CActivityOne::PKG("BackupManagerDemo");

ECode CActivityOne::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);
    SetContentView(R::layout::backup_restore);
    /** Once the UI has been inflated, cache the controls for later */
    AutoPtr<IView> temp = FindViewById(R::id::filling_group);
    mFillingGroup = IRadioGroup::Probe(temp.Get());
    assert(mFillingGroup != NULL);
    temp = NULL;
    temp = FindViewById(R::id::mayo);
    mAddMayoCheckbox = ICheckBox::Probe(temp.Get());
    assert(mAddMayoCheckbox != NULL);

    /*
     *temp = NULL;
     *temp = FindViewById(R::id::myEditText);
     *mEditText = IEditText::Probe(temp);
     *assert(mEditText != NULL);
     */

    temp = NULL;
    temp = FindViewById(R::id::tomato);
    mAddTomatoCheckbox = ICheckBox::Probe(temp.Get());
    assert(mAddTomatoCheckbox != NULL);

    AutoPtr<IFile> path;
    GetFilesDir((IFile**)&path);
    CFile::New(path, DATA_FILE_NAME, (IFile**)&mDataFile);
    String str;
    path->ToString(&str);
    Slogger::V(TAG, "Onctreate path[%s]", str.string());

    //new mBackupManager
    AutoPtr<IInterface> backupManager = ServiceManager::GetService(String("backup"));
    mBackupManager = IIBackupManager::Probe(backupManager);
    if (mBackupManager == NULL) {
        printf("BackupManagerDemo::Init mBackupManager is NULL\n");
        assert(0);
    }

    /**
     * Finally, build the UI from the persistent store
     */
    PopulateUI();
    return NOERROR;
}

ECode CActivityOne::OnStart()
{
    return Activity::OnStart();
}

ECode CActivityOne::OnResume()
{
    return Activity::OnResume();
}

ECode CActivityOne::OnPause()
{
    return Activity::OnPause();
}

ECode CActivityOne::OnStop()
{
    return Activity::OnStop();
}

ECode CActivityOne::OnDestroy()
{
    return Activity::OnDestroy();
}

void CActivityOne::PopulateUI()
{

    AutoPtr<IRandomAccessFile> file;
    Int32 whichFilling = R::id::pastrami;
    Boolean addMayo = FALSE;
    Boolean addTomato = FALSE;

    {
        AutoLock lock(sDatalock);
        Boolean exists = FALSE;
        mDataFile->Exists(&exists);
        CRandomAccessFile::New(mDataFile, String("rw"), (IRandomAccessFile**)&file);
        if (exists) {
            Slogger::V(TAG, "datafile exists");
            AutoPtr<IDataInput> dataInput = IDataInput::Probe(file);
            dataInput->ReadInt32(&whichFilling);
            dataInput->ReadBoolean(&addMayo);
            dataInput->ReadBoolean(&addTomato);
            Slogger::V(TAG, "mayo=%d, tomato=%d, filling=%d", addMayo, addTomato, whichFilling);
        }
        else {
            Slogger::V(TAG, "creating default datafile");
            WriteDataToFileLocked(file, addMayo, addTomato, whichFilling);
            mBackupManager->DataChanged(PKG);
        }
    }

    /** Now that we've processed the file, build the UI outside the lock */
    mFillingGroup->Check(whichFilling);
    ICheckable::Probe(mAddMayoCheckbox)->SetChecked(addMayo);
    ICheckable::Probe(mAddTomatoCheckbox)->SetChecked(addMayo);

    AutoPtr<MyListener> l = new MyListener(this);
    mAddMayoCheckbox->SetOnCheckedChangeListener(l.Get());
    mAddTomatoCheckbox->SetOnCheckedChangeListener(l.Get());
    mFillingGroup->SetOnCheckedChangeListener(l.Get());
}

void CActivityOne::WriteDataToFileLocked(
    /* [in] */ IRandomAccessFile* file,
    /* [in] */ Boolean addMayo,
    /* [in] */ Boolean addTomato,
    /* [in] */ Int32 whichFilling)
{
    file->SetLength(0);
    AutoPtr<IDataOutput> dataOutput = IDataOutput::Probe(file);
    assert(dataOutput != NULL);
    dataOutput->WriteInt32(whichFilling);
    dataOutput->WriteBoolean(addMayo);
    dataOutput->WriteBoolean(addTomato);
    Slogger::V(TAG, "NEW State: mayo=%d, tomato = %d, filling=%d", addMayo, addTomato, whichFilling);
}

/**
 * this one reads the current UI state and writes that
 * to the persistent store, then tells the backup manager that we need
 * a backup.
 */
void CActivityOne::RecordNewUIState()
{
    Slogger::E(TAG, "RecordNewUIState");
    Boolean addMayo; ICheckable::Probe(mAddMayoCheckbox)->IsChecked(&addMayo);

    Boolean addTomato; ICheckable::Probe(mAddTomatoCheckbox)->IsChecked(&addTomato);

    Int32 whichFilling; mFillingGroup->GetCheckedRadioButtonId(&whichFilling);
    {
        AutoLock lock(sDatalock);
        AutoPtr<IRandomAccessFile> file;
        CRandomAccessFile::New(mDataFile, String("rw"), (IRandomAccessFile**)&file);
        WriteDataToFileLocked(file, addMayo, addTomato, whichFilling);
    }
    //
    mBackupManager->DataChanged(PKG);
}

} // namespace BackupManagerDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
