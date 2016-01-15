## 数据保存 ##

### 保存键-值对 ###
如果你有一个相对小的键值数据需要保存，你应该使用ISharedPreferences API。
- 获取ISharedPreferences引用
通过以下两个方法你可以创建或访问已经存在的Preferences文件：
```c++
1. GetSharedPreferences()
2. GetPreferences()
```
- 获得文件引用后可通过ISharedPreferencesEditor来提保存数据：
```c++
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
```
- 从ISharedPreferences中读取数据：
```c++
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
    Logger::D(TAG, read from preference: %s", sb.ToString().string());
    return NOERROR;
}
```
### 保存到文件 ###
可以进行如下操作
- 保存文件到内部存储
利用OpenFileOutput()方法获得内部存储流，调用IOutputStreamWriter的方法将文件保存到内部存储：
```c++
ECode CMainActivity::SaveToFile()
{
    AutoPtr<ICharsets> charsets;
    CCharsets::AcquireSingleton(((ICharsets**)&charsets));
    AutoPtr<ICharset> charset;
    charsets->GetUTF_8((ICharset**)&charset);
    String utf8;
    charset->GetName(&utf8);
    AutoPtr<IFileOutputStream> fos;
    OpenFileOutput(FILE_NAME, MODE_PRIVATE, (IFileOutputStream**)&fos);
    AutoPtr<IOutputStreamWriter> osw;
    COutputStreamWriter::New(fos, utf8, (IOutputStreamWriter**)&osw);
    osw->WriteString(mNameValue + String("\n") + mAddrValue);
    fos->Close();
    Logger::D(TAG, "save to file: %s---%s", mNameValue.string(), mAddrValue.string());
    return NOERROR;
}
```
- 保存到外部存储
要保存数据到外部存储，首先你得有外部存储的读写权限，通过manifest文件配置权限：
```xml
<manifest ...>
    <uses-permission android:name="android.permission.WRITE_EXTERNAL_STORAGE" />
</manifest>
```
获得权限后，通过调用GetExternalFilesDir()方法来获得外部存储的指定目录，其它操作与内部存储类似。
- 读取文件
利用OpenFileInput()获得文件输入流，有IInputStreamReader来读取流中数据：
```c++
ECode CMainActivity::ReadFromFile()
{
    AutoPtr<ICharsets> charsets;
    CCharsets::AcquireSingleton(((ICharsets**)&charsets));
    AutoPtr<ICharset> charset;
    charsets->GetUTF_8((ICharset**)&charset);
    String utf8;
    charset->GetName(&utf8);
    AutoPtr<IFileInputStream> fis;
    OpenFileInput(FILE_NAME, (IFileInputStream**)&fis);
    AutoPtr<IInputStreamReader> isr;
    CInputStreamReader::New(fis, utf8, (IInputStreamReader**)&isr);
    AutoPtr<IBufferedReader> br;
    CBufferedReader::New(isr, (IBufferedReader**)&br);
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
```
### 保存到SQL数据库 ###
利用OpenOrCreateDatabase()方法快速获得数据库ISQLiteDatabase，调用其方法进行数据库操作。
- 保存到数据库
```c++
ECode CMainActivity::SaveToDB()
{
    OpenOrCreateDatabase(DATABASE_NAME, MODE_PRIVATE, NULL,
            (ISQLiteDatabase**)&mDB);
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
```
- 读取数据
```c++
ECode CMainActivity::ReadFromDB()
{
    OpenOrCreateDatabase(DATABASE_NAME, MODE_PRIVATE, NULL,
            (ISQLiteDatabase**)&mDB);
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
        cursor->GetCount(&count);
        String outValue;
        StringBuilder sb("");
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
        }//end for
        mShwmsg->SetText(sb.ToCharSequence());
        Logger::D(TAG, "read from database: %s", sb.ToString());
    }//end if
    cursor->Close();
    mDB->Close();

    return NOERROR;
}
```
到此，主要简单介绍3种主要保存数据的方式。
