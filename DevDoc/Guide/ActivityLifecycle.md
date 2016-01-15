## Activity生命周期管理 ##
**Activity生命周期如下图所示：**

![](http://i.imgur.com/6x3rlIS.png)

**通过实现以下方法来查看生命周期状态：**
- CARAPI OnCreate();
- CARAPI OnStart();
- CARAPI OnRestart();
- CARAPI OnPause();
- CARAPI OnResume();
- CARAPI OnStop();
- CARAPI OnDestroy();

通过2个不同的Activity切换来查看声明周期：
1. 在action_bar中新建一个名为SecondBarActivity的activity
2. 在res/layout/action_bar.xml中添加按钮来启动SecondBarActivity。如：
```xml
<LinearLayout xmlns:android="http://schemas.android.com/apk/res/android"
    xmlns:tools="http://schemas.android.com/tools"
    android:layout_width="match_parent"
    android:layout_height="match_parent"
    android:paddingBottom="@dimen/activity_vertical_margin"
    android:paddingLeft="@dimen/activity_horizontal_margin"
    android:paddingRight="@dimen/activity_horizontal_margin"
    android:paddingTop="@dimen/activity_vertical_margin"
    tools:context=".CActionBarActivity" >

    <Button android:id="@+id/secondBar"
        android:layout_width="wrap_content"
        android:layout_height="wrap_content"
        android:text="@string/second_button"
        android:onClick="goSecondActivity"/>
    <Button
        android:id="@+id/send_text"
        android:layout_width="wrap_content"
        android:layout_height="wrap_content"
        android:text="@string/sent_text" />
</LinearLayout>
```
3. 在actionbar的头文件中注册监听器以响应button单击事件，比如：
```c++
class MenuItemClickListener
    : public IViewOnClickListener
    , public ElRefBase
{
public:
    MenuItemClickListener(
        /* [in] */ CActionBarActivity* host);

    CAR_INTERFACE_DECL();

    CARAPI OnClick(
        /* [in] */ IView* v);

private:
    AutoPtr<CActionBarActivity> mHost;
};
```
4. 在源代码中绑定事件并处理，如：
```c++
ECode CActionBarActivity::MenuItemClickListener::OnClick(
    /*[in]*/ IView* view)
{
    Int32 viewId;
    view->GetId(&viewId);
    switch(viewId){
    case R::id::secondBar:
        mHost->goSecondActivity(view);
        break;
    case R::id::send_text:
        AutoPtr<IIntent> intent;
        CIntent::New((IIntent**)&intent);
        intent->SetAction(IIntent::ACTION_SEND);
        intent->SetType(String("text/plain"));
        mHost->StartActivity(intent);
        Logger::D(TAG, "CActionBarActivity::sent text\n");

        break;
    }

    return NOERROR;
}

ECode CActionBarActivity::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);
    SetContentView(R::layout::activity_action_bar);

    AutoPtr<IView> button = FindViewById(R::id::secondBar);
    button->SetOnClickListener(mItemClickListener);

    button = FindViewById(R::id::send_text);
    button->SetOnClickListener(mItemClickListener);

    Logger::D(TAG, "CActionBarActivity::OnCreate\n");

    return NOERROR;
}
```

到此acitivity各个状态皆实现，从logcat中可以跟踪到生命周期状态。
