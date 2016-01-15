## Activity交互 ##

#### 利用Intent进行不同应用间数据通信 ####

1. **在A应用中创建一个Intent请求数据**

    - 创建一个隐式的Intent向其它应用请求数据，如：
    ```c++
    ECode CMainActivity::ButtonClickListener::OnClick(
        /*[in]*/ IView* view)
    {
        Int32 viewId;
        view->GetId(&viewId);
        switch(viewId) {
            case R::id::btn_getmsg:
                AutoPtr<IIntent> intent;
                CIntent::New((IIntent**)&intent);
                intent->SetAction(IIntent::ACTION_SEND);
                intent->SetType(String("text/plain"));
                mHost->StartActivityForResult(intent, REQUEST_CODE);
                Logger::D(TAG, "get text\n");
                break;
            default:
                break;
        }

        return NOERROR;
    }
    ```
    -  处理请求结果,如：
    ```c++
    ECode CMainActivity::OnActivityResult(
        /* [in] */ Int32 requestCode,
        /* [in] */ Int32 resultCode,
        /* [in] */ IIntent *data)
    {
        if (requestCode == REQUEST_CODE) {
            if (resultCode == RESULT_OK) {
                String msg;
                data->GetStringExtra(String("msg"), &msg);
                AutoPtr<IView> view = FindViewById(R::id::message);
                AutoPtr<ITextView> textview = ITextView::Probe(view);
                AutoPtr<ICharSequence> cs;
                CStringWrapper::New(msg, (ICharSequence**)&cs);
                textview->SetText(cs);
            }
        }

        return NOERROR;
    }
    ```
    - 在UI展示处理结果
    ```xml
    <RelativeLayout xmlns:android="http://schemas.android.com/apk/res/android"
        xmlns:tools="http://schemas.android.com/tools"
        android:layout_width="match_parent"
        android:layout_height="match_parent"
        android:paddingBottom="@dimen/activity_vertical_margin"
        android:paddingLeft="@dimen/activity_horizontal_margin"
        android:paddingRight="@dimen/activity_horizontal_margin"
        android:paddingTop="@dimen/activity_vertical_margin"
        tools:context=".CMainActivity" >

        <TextView android:id="@+id/message"
            android:layout_width="match_parent"
            android:layout_height="wrap_content"
            android:hint="show message here"/>

        <Button
            android:id="@+id/btn_getmsg"
            android:layout_width="wrap_content"
            android:layout_height="wrap_content"
            android:layout_alignRight="@+id/message"
            android:layout_below="@+id/message"
            android:layout_marginRight="47dp"
            android:layout_marginTop="25dp"
            android:text="get message" />

    </RelativeLayout>
    ```

2. **在B应用中响应A的事件并返回结果**

    - 在mainfest文件中声明可以接收并处理事件
    ```xml
    <application
        android:allowBackup="true"
        android:icon="@drawable/ic_launcher"
        android:label="@string/app_name"
        android:theme="@style/AppTheme" >
        <activity
            android:name="CMainActivity"
            android:label="@string/app_name" >
            <intent-filter>
                <action android:name="android.intent.action.MAIN" />
                <category android:name="android.intent.category.DEFAULT" />
                <category android:name="android.intent.category.LAUNCHER" />
            </intent-filter>
            <intent-filter>
                <action android:name="android.intent.action.SEND"/>
                <category android:name="android.intent.category.DEFAULT"/>
                <data android:mimeType="image/*"/>
                <data android:mimeType="text/plain"/>
            </intent-filter>
        </activity>
    </application>
    ```
    - 处理请求并封装结果
    由文本框将用户输入封装到结果对象中，在layout中声明文本框：
    ```xml
    <RelativeLayout xmlns:android="http://schemas.android.com/apk/res/android"
        xmlns:tools="http://schemas.android.com/tools"
        android:layout_width="match_parent"
        android:layout_height="match_parent"
        android:paddingBottom="@dimen/activity_vertical_margin"
        android:paddingLeft="@dimen/activity_horizontal_margin"
        android:paddingRight="@dimen/activity_horizontal_margin"
        android:paddingTop="@dimen/activity_vertical_margin"
        tools:context=".CMainActivity" >

        <EditText
            android:id="@+id/edt_text"
            android:layout_width="wrap_content"
            android:layout_height="wrap_content"
            android:layout_alignParentLeft="true"
            android:layout_alignParentTop="true"
            android:ems="10" />

        <Button
            android:id="@+id/btn_finish"
            android:layout_width="wrap_content"
            android:layout_height="wrap_content"
            android:layout_alignParentRight="true"
            android:layout_below="@+id/editText1"
            android:layout_marginRight="42dp"
            android:layout_marginTop="38dp"
            android:text="finish" />

    </RelativeLayout>
    ```
    单击finish按钮完成输入进行处理：
    ```c++
    ECode CMainActivity::ButtonClickListener::OnClick(
        /*[in]*/ IView* view)
    {
        Int32 viewId;
        view->GetId(&viewId);
        switch(viewId){
        case R::id::btn_finish:
            AutoPtr<IIntent> intent;
            CIntent::New((IIntent**)&intent);
            AutoPtr<IView> msg = mHost->FindViewById(R::id::edt_text);
            AutoPtr<IEditText> text = IEditText::Probe(msg);
            AutoPtr<ICharSequence> msg_text;
            text->GetText((ICharSequence**)&msg_text);
            String msgs;
            msg_text->ToString(&msgs);
            intent->PutStringExtra(String("msg"), msgs);
            mHost->SetResultEx(RESULT_OK, intent);
            mHost->Finish();
            break;
        }

        return NOERROR;
    }
    ```
    将数据封装到intent中，再将intent设置到结果中，由Finish方法触发操作完成，结果由A的OnActivityResult()回调函数进行处理。

到此简单的2个应用之间交互就完成了。
