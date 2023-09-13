package com.kanxue.utils;

import static com.kanxue.dialog.OpenFileDialog.TAG;

import android.content.Context;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.text.TextUtils;
import android.util.Log;


import com.kanxue.desencrypt.PersistSettings;

import java.util.ArrayList;
import java.util.List;

public class LoadAppList {

    public static List<AppData> loadAllInstalledApps(Context context){
        PackageManager pm = context.getPackageManager();
        List<AppData> modulePathList = new ArrayList<>();
        for (PackageInfo pkg : pm.getInstalledPackages(PackageManager.GET_ACTIVITIES | PackageManager.GET_SERVICES)) {
            AppData moduleData = new AppData();
            ApplicationInfo app = pkg.applicationInfo;
            if (!app.enabled)
                continue;
            String apkPath = pkg.applicationInfo.publicSourceDir;
            String apkName = context.getPackageManager().getApplicationLabel(pkg.applicationInfo).toString();
            if (TextUtils.isEmpty(apkPath)) {
                apkPath = pkg.applicationInfo.sourceDir;
            }
            if (!TextUtils.isEmpty(apkPath)) {
                if (apkPath.startsWith("/data/app")) {
                    moduleData.apkPath = apkPath;
                    moduleData.appName = apkName;
                    moduleData.pkgName = app.packageName;
                    moduleData.icon = app.loadIcon(pm);
                    moduleData.uid = app.uid + "";
                    moduleData.versionName = pkg.versionName;
                    moduleData.mxToast=null;
                    moduleData.JsPath =null;
//                    Log.i("aaa", "loadAllInstalledApps: "+moduleData);
                    modulePathList.add(moduleData);
                }
            }
        }
        return modulePathList;
    }
}
