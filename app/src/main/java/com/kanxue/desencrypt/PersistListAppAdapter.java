package com.kanxue.desencrypt;

import android.content.Context;
import android.graphics.Color;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.ImageView;
import android.widget.TextView;

import com.kanxue.utils.AppData;


import java.util.List;


public class PersistListAppAdapter extends BaseAdapter {
    private LayoutInflater inflater;
    public List<AppData> datas;
    Context mContext;
    View.OnClickListener onClickListener;



    public PersistListAppAdapter(Context context, List<AppData> datas, View.OnClickListener onClickListener) {
        super();
        mContext = context;
        inflater = LayoutInflater.from(context);
        this.datas = datas;
        this.onClickListener = onClickListener;
    }

    @Override
    public int getCount() {
        if (datas!=null){
            return datas.size();
        }
        return 0;
    }

    @Override
    public Object getItem(int position) {
        if (datas!=null){
            return datas.get(position);
        }
        return 0;

    }

    @Override
    public long getItemId(int position) {
        return position;
    }

    @Override
    public View getView(final int position, View convertView, ViewGroup parent) {
        ViewHolder holder;
        if (convertView == null) {
            convertView = inflater.inflate(R.layout.app_installed_persist_list_item_layout, null);
            holder = new ViewHolder();
            holder.icon =  convertView.findViewById(R.id.appInstalledIcon);
            holder.label =  convertView.findViewById(R.id.appInstalledName);
            holder.buttonConfigJsPath =convertView.findViewById(R.id.buttonChooseJsFile);
            holder.jsPath = convertView.findViewById(R.id.textViewFridaInjectJsPath);
            holder.ForcedInterpretOnly=convertView.findViewById(R.id.ForcedInterpretOnly);
            convertView.setTag(holder);
        } else {
            holder = (ViewHolder) convertView.getTag();
        }

        holder.icon.setImageDrawable(datas.get(position).icon);
        holder.label.setText(datas.get(position).appName + " " + datas.get(position).versionName);

        String pkgName = datas.get(position).pkgName;
        String jsPath = MainActivity.mAppJsPathMap.get(pkgName);

        holder.jsPath.setTextColor(Color.RED);
        if (jsPath == null) {
            holder.jsPath.setText("未配置js文件");
        } else {
            holder.jsPath.setText("" + jsPath);
        }

        holder.label.setTag(position);
        holder.label.setOnClickListener(onClickListener);

        holder.buttonConfigJsPath.setTag(position);
        holder.buttonConfigJsPath.setOnClickListener(onClickListener);



        holder.ForcedInterpretOnly.setTag(position);
        holder.ForcedInterpretOnly.setChecked(datas.get(position).isForcedInterpret);

        holder.ForcedInterpretOnly.setOnClickListener(onClickListener);
        return convertView;
    }

    class ViewHolder {
        private ImageView icon;
        private TextView label;
        private TextView jsPath;
        private CheckBox ForcedInterpretOnly;
        private Button buttonConfigJsPath;
    }


}
