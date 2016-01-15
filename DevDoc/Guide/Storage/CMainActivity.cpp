
#include <ext/frameworkdef.h>
#include "CMainActivity.h"
#include "R.h"
#include <elastos/Logger.h>
#include <elastos/StringUtils.h>
#include <elastos/StringBuilder.h>

using Elastos::Core::StringBuilder;
using Elastos::IO::CBufferedReader;
using Elastos::IO::Charset::CCharsets;
using Elastos::IO::Charset::ICharset;
using Elastos::IO::Charset::ICharsets;
using Elastos::IO::CInputStreamReader;
using Elastos::IO::COutputStreamWriter;
using Elastos::IO::IBufferedReader;
using Elastos::IO::IFile;
using Elastos::IO::IFileInputStream;
using Elastos::IO::IFileOutputStream;
using Elastos::IO::IFileReader;
using Elastos::IO::IFileWriter;
using Elastos::IO::IInputStreamReader;
using Elastos::IO::IOutputStreamWriter;
using Elastos::IO::IReader;
using Elastos::Utility::Logging::Logger;
using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::ISharedPreferences;
using Elastos::Droid::Content::ISharedPreferencesEditor;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Database::Sqlite::CSQLiteDatabaseHelper;
using Elastos::Droid::Database::Sqlite::ISQLiteDatabase;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::Widget::EIID_IRadioGroupOnCheckedChangeListener;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::IEditText;
using Elastos::Droid::Widget::IRadioGroup;
using Elastos::Droid::Widget::IRadioGroupOnCheckedChangeListener;
using Elastos::Droid::Widget::ITextView;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace Storage {

CMainActivity::ButtonClickListener::ButtonClickListener(
    /* [in] */CMainActivity* host)
    : mHost(host)
{}

CAR_INTERFACE_IMPL_2(CMainActivity::ButtonClickListener, IViewOnClickListener, IRadioGroupOnCheckedChangeListener)


const String CMainActivity::TAG("Storage.CMainActivity");
const String CMainActivity::ID = String("id");
const String CMainActivity::NAME = String("name");
const String CMainActivity::ADDRESS = String("addr");
const String CMainActivity::DATABASE_NAME = String("person.db");
const Int32  CMainActivity::DATABASE_VERSION = 1;
const String CMainActivity::TABLE_NAME = String("person_table");
const String CMainActivity::FILE_NAME = String("person.txt");

ECode CMainActivity::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);
    SetContentView(R::layout::activity_main);
    mButtonListener = new ButtonClickListener(this);

    AutoPtr<IViewOnClickListener> clickListener = IViewOnClickListener::Probe(mButtonListener);
    AutoPtr<IRadioGroupOnCheckedChangeListener> radioListener = IRadioGroupOnCheckedChangeListener::Probe(mButtonListener);
    AutoPtr<IView> name_edit;
    FindViewById(R::id::input_name, (IView**) &name_edit);
    mName = IEditText::Probe(name_edit);
    mNameValue = "";

    AutoPtr<IView> addr_edit;
    FindViewById(R::id::input_addr, (IView**) &addr_edit);
    mAddress = IEditText::Probe(addr_edit);
    mAddrValue = "";

    AutoPtr<IView> save_button;
    FindViewById(R::id::save, (IView**) &save_button);
    save_button->SetOnClickListener(clickListener);

    AutoPtr<IView> show_button;
    FindViewById(R::id::show, (IView**) &show_button);
    show_button->SetOnClickListener(clickListener);

    AutoPtr<IView> view;
    FindViewById(R::id::choice, (IView**)&view);
    AutoPtr<IRadioGroup> radio_button = IRadioGroup::Probe(view);
    radio_button->SetOnCheckedChangeListener(radioListener);

    mRadioButtonId = R::id::radio_preference;
    AutoPtr<IView> msg_view;
    FindViewById(R::id::msg, (IView**)&msg_view);
    mShwmsg = ITextView::Probe(msg_view);

    OpenOrCreateDatabase(DATABASE_NAME,MODE_PRIVATE, NULL, (ISQLiteDatabase**)&mDB);

    Logger::D(TAG,"oncreate");
    return NOERROR;
}

ECode CMainActivity::ButtonClickListener::OnClick(
    /*in*/ IView* view)
{
    Int32 viewId = 0;
    view->GetId(&viewId);
    AutoPtr<ICharSequence> name ;
    mHost->mName->GetText((ICharSequence**)&name);
    name->ToString(&mHost->mNameValue);

    AutoPtr<ICharSequence> addr;
    mHost->mAddress->GetText((ICharSequence**)&addr);
    addr->ToString(&mHost->mAddrValue);

    switch(viewId) {
        case R::id::save:
            switch(mHost->mRadioButtonId) {
                case R::id::radio_preference:
                    mHost->SaveToPreference();
                    break;
                case R::id::radio_file:
                    mHost->SaveToFile();
                    break;
                case R::id::radio_db:
                    mHost->SaveToDB();
                    break;
                default:
                    break;
            }
            break;

        case R::id::show:
            switch(mHost->mRadioButtonId) {
                case R::id::radio_preference:
                    mHost->ReadFromPreference();
                    break;
                case R::id::radio_file:
                    mHost->ReadFromFile();
                    break;
                case R::id::radio_db:
                    mHost->ReadFromDB();
                    break;
                default:
                    break;
            }
            break;

        default:
            break;
    }

    return NOERROR;
}

ECode CMainActivity::ButtonClickListener::OnCheckedChanged(
    /* [in] */ IRadioGroup* radioGroup,
    /* [in] */ Int32 checkedId)
{
    Int32 groupId = 0;
    radioGroup->GetId(&groupId);
    mHost->mRadioButtonId = checkedId;
    if (groupId == R::id::choice) {
        switch(checkedId) {
            case R::id::radio_preference:
                Logger::D(TAG,"select preference");
                break;
            case R::id::radio_file:
                Logger::D(TAG,"select file");
                break;
            case R::id::radio_db:
                Logger::D(TAG,"select database");
                break;
            default:
                break;
        }
    }
    return NOERROR;
}

ECode CMainActivity::SaveToPreference()
{
    AutoPtr<ISharedPreferences> pref;
    GetPreferences(MODE_PRIVATE, (ISharedPreferences**)&pref);
    AutoPtr<ISharedPreferencesEditor> editor;
    pref->Edit((ISharedPreferencesEditor**)&editor);
    editor->PutString(String("name"), mNameValue);
    editor->PutString(String("addr"), mAddrValue);
    Boolean result;
    editor->Commit(&result);
    Logger::D(TAG, "save to preferences: %s---%s", mNameValue.string(), mAddrValue.string());
    return NOERROR;
}

ECode CMainActivity::SaveToFile()
{
    AutoPtr<ICharsets> charsets;
    CCharsets::AcquireSingleton(((ICharsets**)&charsets));
    AutoPtr<ICharset> charset;
    charsets->GetUTF_8((ICharset**)&charset);
    String utf8;
    charset->GetName(&utf8);
    AutoPtr<IFileOutputStream> fos;
    OpenFileOutput(FILE_NAME,MODE_PRIVATE, (IFileOutputStream**)&fos);
    AutoPtr<IOutputStreamWriter> osw;
    COutputStreamWriter::New(fos,utf8, (IOutputStreamWriter**)&osw);
    osw->WriteString(mNameValue + String("\n") + mAddrValue);
    fos->Close();
    Logger::D(TAG, "save to file: %s---%s", mNameValue.string(), mAddrValue.string());
    return NOERROR;
}

ECode CMainActivity::SaveToDB()
{
    OpenOrCreateDatabase(DATABASE_NAME,MODE_PRIVATE, NULL, (ISQLiteDatabase**)&mDB);
    StringBuilder sb;
    sb += "CREATE TABLE IF NOT EXISTS ";
    sb += TABLE_NAME;
    sb += " (";
    sb += ID;
    sb += " INTEGER primary key autoincrement, ";
    sb += NAME;
    sb += " text, ";
    sb += ADDRESS;
    sb += " text);";
    mDB->ExecSQL(sb.ToString());

    sb.Reset();
    AutoPtr<IContentValues> cv;
    CContentValues::New((IContentValues**)&cv);
    sb += mNameValue;
    cv->PutString(NAME, sbn.ToCharSequence());

    sb.Reset();
    sb += mAddrValue;
    cv->PutString(ADDRESS, sbv.ToCharSequence());
    Int64 row;
    FAIL_RETURN(mDB->Insert(TABLE_NAME, String(NULL), cv, &row))
    mDB->Close();
    Logger::D(TAG, "save to database: %s---%s", mNameValue.string(), mAddrValue.string());
    return NOERROR;
}

ECode CMainActivity::ReadFromPreference()
{
    AutoPtr<ISharedPreferences> pref;
    GetPreferences(MODE_PRIVATE, (ISharedPreferences**)&pref);
    String showMsg;
    pref->GetString(String("name"), String("default Name"), &showMsg);
    StringBuilder sb(showMsg);
    pref->GetString(String("addr"), String("default Address"), &showMsg);
    sb += "\n";
    sb += showMsg;
    AutoPtr<ICharSequence> msg = sb.ToCharSequence();
    mShwmsg->SetText(msg);
    Logger::D(TAG, "read from preference: %s", sb.ToString().string());
    return NOERROR;
}

ECode CMainActivity::ReadFromFile()
{
    AutoPtr<ICharsets> charsets;
    CCharsets::AcquireSingleton(((ICharsets**)&charsets));
    AutoPtr<ICharset> charset;
    charsets->GetUTF_8((ICharset**)&charset);
    String utf8;
    charset->GetName(&utf8);
    AutoPtr<IFileInputStream> fis;
    OpenFileInput(FILE_NAME,(IFileInputStream**)&fis);
    AutoPtr<IInputStreamReader> isr;
    CInputStreamReader::New(fis,utf8,(IInputStreamReader**)&isr);
    AutoPtr<IBufferedReader> br;
    CBufferedReader::New(isr,(IBufferedReader**)&br);
    StringBuilder sb("");
    String line;
    br->ReadLine(&line);
    while (!line.IsNull()) {
        sb += line;
        br->ReadLine(&line);
    }
    fis->Close();
    mShwmsg->SetText(sb.ToCharSequence());
    Logger::D(TAG, "read from file: %s---%s", sb.ToString().string(), utf8.string());
    return NOERROR;
}

ECode CMainActivity::ReadFromDB()
{
    OpenOrCreateDatabase(DATABASE_NAME,MODE_PRIVATE, NULL, (ISQLiteDatabase**)&mDB);
    AutoPtr<ICursor> cursor;
    mDB->QueryEx2(TABLE_NAME, NULL, String(NULL), NULL, String(NULL),
            String(NULL), String(NULL), (ICursor**)&cursor);
    Boolean notempty;
    cursor->MoveToFirst(&notempty);
    //false empty
    if (!notempty) {
        Logger::E(TAG,"database is empty");
    }
    else {
        Int32 count, var, colIndex;
        String outValue;
        StringBuilder sb("");
        cursor->GetCount(&count);
        Boolean flag;
        for (var = 0; var < count ; ++var) {
            cursor->GetColumnIndex(ID, &colIndex);
            cursor->GetString(colIndex, &outValue);
            sb += outValue;
            cursor->GetColumnIndex(NAME, &colIndex);
            cursor->GetString(colIndex, &outValue);
            sb += "-";
            sb += outValue;
            cursor->GetColumnIndex(ADDRESS, &colIndex);
            cursor->GetString(colIndex, &outValue);
            sb += "-";
            sb += outValue;
            sb += "\n";
            cursor->MoveToNext(&flag);
        }
        mShwmsg->SetText(sb.ToCharSequence());
        Logger::D(TAG, "read from database: %s", sb.ToString().string());
    }
    cursor->Close();
    mDB->Close();
    return NOERROR;
}

} //namespace Storage
} //namespace DevSamples
} //namespace Droid
} //namespace Elastos
