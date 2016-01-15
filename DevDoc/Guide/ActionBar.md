## 简单ActionBar ##

操作栏是一个窗口特性,确定用户的位置,并提供用户操作和导航模式。使用操作栏为用户在不同屏幕的应用中提供了一个熟悉的界面。

**添加ActionBar**

- 添加action项
当Activity启动时，系统通过调用Activity的OnCreateOptionsMenu()方法安放action项，并用定义在菜单资源中的action项来填充。如资源定义：
```xml
<menu xmlns:android="http://schemas.android.com/apk/res/android" >
    <item
        android:id="@+id/action_settings"
        android:orderInCategory="100"
        android:showAsAction="never"
        android:title="@string/action_settings"/>

    <item android:id="@+id/action_search"
        android:icon="@drawable/ic_action_search"
        android:title="@string/action_search"
        android:showAsAction="always" />
</menu>
```
- 在方法中填充各个项
```c++
ECode CActionBarActivity::OnCreateOptionsMenu(
    /* [in] */ IMenu* menu,
    /* [out] */ Boolean* allowToShow)
{
    AutoPtr<IMenuInflater> menuFlater;
    GetMenuInflater((IMenuInflater**)&menuFlater);
    menuFlater->Inflate(R::menu::action_bar, menu);
    Activity::OnCreateOptionsMenu(menu, allowToShow);
    Logger::D(TAG, "menu return:%s", StringUtils::BooleanToString(*allowToShow).string());

    return NOERROR;
}
```
- 处理各个菜单项上的事件
```c++
ECode CActionBarActivity::OnOptionsItemSelected(
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* res)
{
    Int32 itemId = 0;
    item->GetItemId(&itemId);

    AutoPtr<ICharSequence> csq;
    item->GetTitle((ICharSequence**)&csq);

    String title;
    csq->ToString(&title);

    switch(itemId){
        case R::id::action_settings:
            Logger::D(TAG, "select menu:%s", title.string());
            break;
        case Elastos::Droid::R::id::homeAsUp:
            Logger::D(TAG, "home");
            break;
        default:
            Activity::OnOptionsItemSelected(item, res);

    }
    Logger::D(TAG, "item id:%s", StringUtils::Int32ToOctalString(itemId).string());

    return NOERROR;
}
```

以上便是简单的Action Bar处理。