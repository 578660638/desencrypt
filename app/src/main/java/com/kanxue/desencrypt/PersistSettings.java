package com.kanxue.desencrypt;

import android.content.Context;
import android.system.Os;
import android.util.Log;
import android.widget.Toast;




import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.ArrayList;

public class PersistSettings {
    private static final String _dir = "yyf";   //自定义目录名称
    private static final String TAG = "gqghj";

    private static final String SETTINGS_DIR = "/data/system/xsetting/yyf/persist/";
    private static final String CONFIG_JS_DIR = "/data/system/xsetting/yyf/jscfg/";

    public static final String INTERPRET_TYPE = "yyf_ForcedInterpretOnly";
    public static final String DUMPFILE_TYPE = "yyf_dumpfile";
    public static final String CALLPROCESS_TYPE = "yyf_Call";
    public static final String PERSIST_TYPE = "persist";
    public static final String SMAIL_TYPE = "yyf_smailCall";
    public static final String MONITORMODE_TYPE = "yyf_MonitorMode";

    //test
//    private static final String SETTINGS_DIR = "/sdcard/xsetting/"+_dir+"/persist";
//    private static final String CONFIG_JS_DIR = "/sdcard/xsetting/"+_dir+"/jscfg";
    //test

// /sdcard/gqghj/test2.js       /sdcard/xsettings/yyf/jscfg/com.Persist.persistdemo/Config.js
    public static boolean copyJSFileToAppJSPath(String srcjsFilePath, String dstJsPath) {
        try {
            File file = new File(srcjsFilePath);
            if (!file.exists()) {
                return false;
            }
            file = new File(dstJsPath);
            if (file.exists()) {
                file.delete();
            }

            FileInputStream fileInputStream = new FileInputStream(new File(srcjsFilePath));
            FileOutputStream fileOutputStream = new FileOutputStream(new File(dstJsPath));
            byte[] dataBytes = new byte[4 * 1024];
            int len = -1;
            while ((len = fileInputStream.read(dataBytes)) != -1) {
                fileOutputStream.write(dataBytes, 0, len);
                fileOutputStream.flush();
            }
            try {
                file = new File(dstJsPath);
                Os.chmod(file.getAbsolutePath(), 0775);
            } catch (Exception e) {
                e.printStackTrace();
            }
            fileOutputStream.close();
            fileInputStream.close();
            return true;
        } catch (Exception e) {
            e.printStackTrace();
            return false;
        }
    }



    public static String getAppJSPath(String pkgName) {
        File file = new File(CONFIG_JS_DIR);
        if (!file.exists()) {
            file.mkdirs();
            try {
                Os.chmod(file.getAbsolutePath(), 0775);
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
        file = new File(CONFIG_JS_DIR + "/" + pkgName);
        if (!file.exists()) {
            file.mkdirs();
            try {
                Os.chmod(file.getAbsolutePath(), 0775);
            } catch (Exception e) {
                e.printStackTrace();
            }
        }

        file = new File(CONFIG_JS_DIR + "/" + pkgName + "/config.js");
        return file.getAbsolutePath();
    }


    /*************************创建文件标签**************************/
    public static boolean CreateFile(Context context, String pkgName, String methodType, boolean isEnable) {
        File pkgFile = new File(SETTINGS_DIR, pkgName);//文件路径需要在系统内部创建    "/data/system/xsetting/yyf/persist"

        if (!pkgFile.exists()) {
            try {
                boolean mkdirIsOk = pkgFile.mkdir();
                if(!mkdirIsOk){
                    Log.d(TAG, "创建包名文件1:  "+pkgFile+"      "+mkdirIsOk);
                    return false;
                } else {
                    Log.d(TAG, "创建包名文件2:  "+pkgFile+"      "+mkdirIsOk);
                }
                Os.chmod(pkgFile.getAbsolutePath(), 0775);
            } catch (Exception e) {
                Log.d(TAG, "创建包名文件失败 :" + pkgFile.getAbsolutePath());
                e.printStackTrace();
                return false;
            }
        }
        File enableFile = new File(pkgFile, methodType);
        if (isEnable) {
            Log.d(TAG, "创建文件: "+ enableFile);
            if (!enableFile.exists()) {
                try {
                    boolean createIsOk = enableFile.createNewFile(); // /data/system/xsettings/gqghj/persist/com.Persist.persistdemo/gqghj_dumpfile
                    if(!createIsOk) {
                        return false;
                    }
                    Os.chmod(enableFile.getAbsolutePath(), 0775);
                    Toast.makeText(context, "功能配置成功", Toast.LENGTH_SHORT).show();
                } catch (Exception e) {
                    Log.d(TAG, "创建文件失败:" + e.toString());

                    return false;
                }
            }
        } else {
            Log.d(TAG, "删除文件: "+ enableFile);
            if (enableFile.exists()) {
                try {
                    Log.d(TAG, "删除文件:" + enableFile.getAbsolutePath());
                    boolean deleteIsOk = enableFile.delete();
                    if(!deleteIsOk) {
                        Log.d(TAG, "删除文件失败 ");
                        return false;
                    }else {
                        Log.d(TAG, "删除文件成功 ");
                        Toast.makeText(context, "配置解除成功", Toast.LENGTH_SHORT).show();
                    }
                } catch (Exception e) {
                    Log.d(TAG, "删除文件失败:" + enableFile.getAbsolutePath());
                    e.printStackTrace();
                    return false;
                }
            }
        }

        return true;
    }



    public static boolean isForcedInterpretOnly(String pkgName, String methodType) {
        File enableFile = new File(SETTINGS_DIR, pkgName + "/" + methodType);
        return enableFile.exists();
    }

    public static void clearAllFile(String pkgName){
        File[] filePaths = new File(SETTINGS_DIR,pkgName).listFiles();
        for (int i=0;i<filePaths.length;i++){
            if (filePaths[i].exists()) {
                boolean deleteIsOk = filePaths[i].delete();
                Log.d(TAG, "clearAllFile: "+filePaths[i]+"  "+deleteIsOk);
            }
        }

    }
}
