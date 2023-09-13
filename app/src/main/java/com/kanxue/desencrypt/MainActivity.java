package com.kanxue.desencrypt;

import androidx.annotation.RequiresApi;
import androidx.appcompat.app.AppCompatActivity;

import android.Manifest;
import android.annotation.SuppressLint;
import android.app.AppOpsManager;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.pm.PackageManager;
import android.net.Uri;
import android.os.Binder;
import android.os.Build;
import android.os.Bundle;
import android.provider.Settings;
import android.util.Log;
import android.view.Gravity;
import android.view.View;
import android.widget.CheckBox;
import android.widget.ListView;
import android.widget.Switch;
import android.widget.TextView;
import android.widget.Toast;

import com.alibaba.fastjson.JSON;
import com.hjq.xtoast.XToast;
import com.hjq.xtoast.draggable.MovingDraggable;
import com.kanxue.dialog.CallbackBundle;
import com.kanxue.dialog.OpenFileDialog;
import com.kanxue.utils.AppData;
import com.kanxue.utils.LoadAppList;

import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.lang.reflect.Field;
import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Objects;
import java.util.zip.ZipEntry;
import java.util.zip.ZipInputStream;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }
    String dataPath;
    String extDataPath;

    static final String TAG = "qqq";
    static final String _dir = "yyf";
    private static final int REQUEST_DIALOG_PERMISSION = 1002;
    ListView AppList = null;
    List<AppData> dataList = new ArrayList<>();

    PersistListAppAdapter dataAdapter = null;
    AppData appData = new AppData();
    String AppUID=null;
    boolean swc=false;
    public static HashMap<String, String> mAppJsPathMap = new HashMap<>();
    public native String myNativeMethod1(String input);
    public  native  String teststring();
    @Override
    protected void onDestroy() {
        super.onDestroy();
        appData.mxToast.cancel();

    }

    public void testwight(Bundle savedInstanceState){
//        ArrayList<Map<String, Object>> test = new ArrayList<Map<String, Object>>();
        ArrayList<Map<String,Object>> test = new ArrayList<Map<String,Object>>();
        HashMap<String,Object>   testlist = new HashMap<String, Object>();
        testlist.put("name", "Path");
        testlist.put("path", "sad");
        testlist.put("img", "Asd");
        test.add(testlist);
        Log.i("qqq",testlist.toString()+test.toString());
        String a1 = new String("asd");
        String a2 = teststring();
        Log.i("qqqq",a1+a2);
    }
    @RequiresApi(api = Build.VERSION_CODES.M)
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        stringFromJNI();
        myNativeMethod1("666");
        testwight(savedInstanceState);
        if (!checkFloatPermission(this)){
            requestSettingCanDrawOverlays();
        }
//        String filepath = savedInstanceState.getString("path");
//        Log.d(TAG, filepath);

        //SD卡读写权限
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q) {
            if (checkSelfPermission(Manifest.permission.WRITE_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED) {
                requestPermissions(new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE}, 1000);
                //悬浮框权限
            } else {
                Toast.makeText(this, "授权读写外置卡成功", Toast.LENGTH_LONG).show();
            }

        }
        loadAppJsPath();
        dataList = LoadAppList.loadAllInstalledApps(this);
        AppList = findViewById(R.id.listViewInstalledListApp);
        dataAdapter = new PersistListAppAdapter(this, dataList, new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                switch (view.getId()){
                    case R.id.ForcedInterpretOnly:{
                        String methodType = PersistSettings.INTERPRET_TYPE;  //取方法类型
                        boolean isActive = ((CheckBox) view).isChecked();//是否开启
                        if (isActive){
                            AppData aData = dataList.get((int)view.getTag());//取应用信息
                            //Log.d(TAG, "AppUID: "+aData.uid+"   "+aData.pkgName);
                            if (appData.mxToast==null){
                                appData = aData;
                                if (appData != null) {
                                    AppUID=aData.uid;
                                    if (!PersistSettings.CreateFile(MainActivity.this,appData.pkgName, methodType, isActive)) {
                                        Toast.makeText(MainActivity.this, "配置失败", Toast.LENGTH_SHORT).show();
                                        appData.mxToast=null;
                                        refreshData();
                                        break;
                                    } else {
                                        Toast.makeText(MainActivity.this, "配置成功", Toast.LENGTH_SHORT).show();
                                        appData.mxToast = showWindowEvents(aData.pkgName, view);
                                        if (appData.mxToast != null) {
                                            appData.mxToast.show();
                                        }
                                    }
                                } else {
                                    refreshData();
                                    Toast.makeText(MainActivity.this, "未获取到配置的App信息", Toast.LENGTH_SHORT).show();
                                }
                            }else {
                                ((CheckBox) view).setChecked(false);
                            }
                        }else {
                            //判断对象是否属于它
                            AppData aData = dataList.get((int)view.getTag());//取应用信息
                            Log.d(TAG, "AppUID: "+aData.uid+"   "+aData.pkgName +"  isActive "+ isActive);
                            if (Objects.equals(AppUID, aData.uid)){
                                if(PersistSettings.CreateFile(MainActivity.this,appData.pkgName, PersistSettings.INTERPRET_TYPE, false)){
                                    Toast.makeText(MainActivity.this, "停用成功", Toast.LENGTH_SHORT).show();
                                    if (appData.mxToast != null){
                                        appData.mxToast.cancel();
                                        appData.mxToast = null;
                                        AppUID=null;
                                    }
                                }
                                PersistSettings.clearAllFile(aData.pkgName);
                            }else {
                                ((CheckBox) view).setChecked(isActive);
                            }
                        }
                        break;
                    }
                    //选择js文件
                    case R.id.buttonChooseJsFile:{  //选择js文件
                        Map<String, Integer> images = new HashMap<>();
                        images.put(OpenFileDialog.sRoot, R.drawable.filedialog_root);
                        images.put(OpenFileDialog.sParent, R.drawable.filedialog_folder_up);
                        images.put(OpenFileDialog.sFolder, R.drawable.filedialog_folder);
                        images.put("wav", R.drawable.filedialog_wavfile);
                        images.put(OpenFileDialog.sEmpty, R.drawable.filedialog_root);
                        OpenFileDialog.createDialog(MainActivity.this, "文件选择", new CallbackBundle() {
                            @Override
                            public void callback(Bundle bundle) {
                                String filepath = bundle.getString("path");
                                AppData moduleData = dataList.get((int) view.getTag());
                                Log.d(TAG, "packageName:" + moduleData.pkgName + " file path:" + filepath);

                                mAppJsPathMap.put(moduleData.pkgName, filepath);
                                saveAppJsPath();
                                refreshData();
                            }
                        }, ".js;", images).show();
                        break;
                    }
                }
            }
        });
        //刷新app点击事件
        AppList.setAdapter(dataAdapter);
        findViewById(R.id.refreshAppList).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                refreshData();
                Toast.makeText(MainActivity.this, "刷新成功", Toast.LENGTH_SHORT).show();
            }
        });

    }

    //判断是否开启悬浮窗权限   context可以用你的Activity.或者tiis
    public static boolean checkFloatPermission(Context context) {
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.KITKAT)
            return true;
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.M) {
            try {
                Class cls = Class.forName("android.content.Context");
                Field declaredField = cls.getDeclaredField("APP_OPS_SERVICE");
                declaredField.setAccessible(true);
                Object obj = declaredField.get(cls);
                if (!(obj instanceof String)) {
                    return false;
                }
                String str2 = (String) obj;
                obj = cls.getMethod("getSystemService", String.class).invoke(context, str2);
                cls = Class.forName("android.app.AppOpsManager");
                Field declaredField2 = cls.getDeclaredField("MODE_ALLOWED");
                declaredField2.setAccessible(true);
                Method checkOp = cls.getMethod("checkOp", Integer.TYPE, Integer.TYPE, String.class);
                int result = (Integer) checkOp.invoke(obj, 24, Binder.getCallingUid(), context.getPackageName());
                return result == declaredField2.getInt(cls);
            } catch (Exception e) {
                return false;
            }
        } else {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
                AppOpsManager appOpsMgr = (AppOpsManager) context.getSystemService(Context.APP_OPS_SERVICE);
                if (appOpsMgr == null)
                    return false;
                int mode = appOpsMgr.checkOpNoThrow("android:system_alert_window", android.os.Process.myUid(), context
                        .getPackageName());
                return mode == AppOpsManager.MODE_ALLOWED || mode == AppOpsManager.MODE_IGNORED;
            } else {
                return Settings.canDrawOverlays(context);
            }
        }
    }

    //权限打开
    private void requestSettingCanDrawOverlays() {
        int sdkInt = Build.VERSION.SDK_INT;
        if (sdkInt >= 30) {
            Intent intent = new Intent(Settings.ACTION_MANAGE_ALL_FILES_ACCESS_PERMISSION);
            startActivity(intent);

        }
        if (sdkInt >= Build.VERSION_CODES.O) {//8.0以上
            //startActivityForResult(new Intent(Settings.ACTION_MANAGE_OVERLAY_PERMISSION, Uri.parse("package:" + getPackageName())), REQUEST_CODE);
            Intent intent = new Intent(Settings.ACTION_MANAGE_OVERLAY_PERMISSION, Uri.parse("package:" + getPackageName()));
            startActivityForResult(intent, REQUEST_DIALOG_PERMISSION);//ACTION_MANAGE_OVERLAY_PERMISSION
        } else if (sdkInt >= Build.VERSION_CODES.M) {//6.0-8.0
            Intent intent = new Intent(Settings.ACTION_MANAGE_OVERLAY_PERMISSION);
            intent.setData(Uri.parse("package:" + getPackageName()));
            startActivityForResult(intent, REQUEST_DIALOG_PERMISSION);
        } else {//4.4-6.0以下
            //无需处理了
        }
    }


    //悬浮窗口的对象创建
    public XToast<?> showWindowEvents(String pkgname, View view){
        return(new XToast<>(getApplication())
                .setContentView(R.layout.window)
                .setAnimStyle(R.style.RightAnimStyle)
                .setGravity(Gravity.RIGHT | Gravity.BOTTOM)
                .setText(R.id.pkgname,pkgname)
                .setYOffset(400)
                .setDraggable(new MovingDraggable())
                .setOnClickListener(R.id.frida, new XToast.OnClickListener<Switch>() {
                    @Override
                    public void onClick(XToast<?> toast, @SuppressLint("UseSwitchCompatOrMaterialCode") Switch frida) { //持久
                      fridaPersist(frida,view);
                    }
                })
                .setOnClickListener(R.id.frida1, new XToast.OnClickListener<Switch>() {
                    @Override
                    public void onClick(XToast<?> toast, @SuppressLint("UseSwitchCompatOrMaterialCode") Switch frida1) {//监听
                        openCreateFile(frida1,view,PersistSettings.MONITORMODE_TYPE);
                    }
                })
                .setOnClickListener(R.id.dump_File, new XToast.OnClickListener<Switch>() {
                    @Override
                    public void onClick(XToast<?> toast, @SuppressLint("UseSwitchCompatOrMaterialCode") Switch dump_file) { //脱壳
                     openCreateFile(dump_file,view,PersistSettings.DUMPFILE_TYPE);
                    }
                })
                .setOnClickListener(R.id.Call_process, new XToast.OnClickListener<Switch>() {
                    @Override
                    public void onClick(XToast<?> toast, @SuppressLint("UseSwitchCompatOrMaterialCode") Switch Call_process) {
                     openCreateFile(Call_process,view,PersistSettings.CALLPROCESS_TYPE);
                    }
                })
                .setOnClickListener(R.id.Call_process1, new XToast.OnClickListener<Switch>() {

                    @Override
                    public void onClick(XToast<?> toast, @SuppressLint("UseSwitchCompatOrMaterialCode") Switch Call_process1) {
                      openCreateFile(Call_process1,view,PersistSettings.SMAIL_TYPE);

                    }

                }));
    }

    private void saveAppJsPath() {
        String jsonString = JSON.toJSONString(mAppJsPathMap);
        SharedPreferences sharedPreferences = getSharedPreferences(_dir, 0);
        SharedPreferences.Editor editor = sharedPreferences.edit();
        editor.putString("JSPath", jsonString);
        Log.d(TAG, "saveAppJSPath  commit ");
        editor.commit();
    }

    private void loadAppJsPath() {
        SharedPreferences sharedPreferences = getSharedPreferences(_dir, 0);
        String tempString = sharedPreferences.getString("JSPath", JSON.toJSONString(new HashMap<>()));
        Log.d(TAG, "tempString: "+tempString);
        Map maps = (Map) JSON.parse(tempString);
        mAppJsPathMap.clear();
        mAppJsPathMap.putAll(maps);
    }
    private void refreshData() {
        dataList = LoadAppList.loadAllInstalledApps(this);
        dataAdapter.datas = dataList;
        dataAdapter.notifyDataSetChanged();
    }
    //frida 持久模式
    private void fridaPersist(@SuppressLint("UseSwitchCompatOrMaterialCode") Switch frida, View view){
        String methodType = PersistSettings.PERSIST_TYPE;  //取方法类型
        boolean isActive = frida.isChecked();//是否开启
        AppData appData = dataList.get((int)view.getTag());//取应用信息
        Log.d(TAG, "AppData: "+appData.uid+"   "+appData.pkgName+" isActive: "+isActive);
        if (appData != null) {
            //获取js路径
            String jsPath = mAppJsPathMap.get(appData.pkgName);
            Log.d(TAG, "jsPath: "+jsPath+mAppJsPathMap.toString());//  /sdcard/gqghj/ceshi1.jsjs

            if (jsPath == null) {
                Toast.makeText(MainActivity.this, "未配置对应的js文件路径", Toast.LENGTH_SHORT).show();
                frida.setChecked(false);
                return;
            }

            if(!PersistSettings.CreateFile(MainActivity.this,appData.pkgName, methodType, isActive)) {
                Toast.makeText(MainActivity.this, "配置失败", Toast.LENGTH_SHORT).show();
                frida.setChecked(false);
                return;
            };
            if (isActive) {
                //关闭其勾选的
                if (!methodType.equals(PersistSettings.PERSIST_TYPE)) {
                    PersistSettings.CreateFile(MainActivity.this,appData.pkgName, PersistSettings.PERSIST_TYPE, false);
                }
            }

            if (isActive) {
                // 先复制到 /data/system/xsettings/xxxx/jscfg/pkgname/config.js
                // pkgPath /data/system/xsettings/xxxx/jscfg/pkgname/config.js
                // jsPath  /sdcard/gqghj/sohook.js
                String pkgPath = PersistSettings.getAppJSPath(appData.pkgName);
                if (PersistSettings.copyJSFileToAppJSPath(jsPath, pkgPath)) {
                    Toast.makeText(MainActivity.this, "配置成功", Toast.LENGTH_SHORT).show();
                } else {
                    Toast.makeText(MainActivity.this, "配置失败", Toast.LENGTH_SHORT).show();
                    frida.setChecked(false);
                    return;
                }
                refreshData();
            } else {
                String appJsPath = PersistSettings.getAppJSPath(appData.pkgName);
                File file = new File(appJsPath);
                if (file.exists()) {
                    file.delete();
                }
                Toast.makeText(MainActivity.this, "Frida 停用成功", Toast.LENGTH_SHORT).show();
                frida.setChecked(false);
            }
            saveAppJsPath();
        } else {
            frida.setChecked(false);
            Toast.makeText(MainActivity.this, "未获取到配置的App信息", Toast.LENGTH_SHORT).show();
        }
    }
    private void openCreateFile(@SuppressLint("UseSwitchCompatOrMaterialCode") Switch sw, View view,String typeName){
        AppData appData = dataList.get((int) view.getTag());
        boolean isActive = sw.isChecked();
        if (appData != null) {
            Log.d(TAG, "MonitorMode:" + appData.pkgName +"  "+isActive+typeName);
            if(!PersistSettings.CreateFile(MainActivity.this,appData.pkgName, typeName, isActive)) {
                Toast.makeText(MainActivity.this, "配置失败", Toast.LENGTH_SHORT).show();
                refreshData();
            }
            if (isActive) {
                if (!typeName.equals(typeName)) {
                    PersistSettings.CreateFile(MainActivity.this,appData.pkgName,typeName, false);
                }
            }
        }
    }
    public native String stringFromJNI();
}




