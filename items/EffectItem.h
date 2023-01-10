#ifndef EFFECTITEM_H
#define EFFECTITEM_H

using namespace std;

class EffectItem {
    public:

    string name;
    int version = 4;
    string title;
    string subtitle;
    string author;
    SRL<EffectThumbnail> thumbnail;
    SRL<EffectData> data;
    SRL<EffectAudio> audio;

    EffectItem(){}
    EffectItem(string name, string title, string subtitle, string author,
        SRL<EffectThumbnail> thumbnail, SRL<EffectData> data, SRL<EffectAudio> audio):
        name(name), title(title), subtitle(subtitle), author(author),
        thumbnail(thumbnail), data(data), audio(audio){}
    
    Json::Value toJsonObject() {
        Json::Value res;
        res["name"] = name;
        res["version"] = version;
        res["title"] = title;
        res["subtitle"] = subtitle;
        res["author"] = author;
        res["thumbnail"] = thumbnail.toJsonObject();
        res["data"] = data.toJsonObject();
        res["audio"] = audio.toJsonObject();
        return res;
    }
};

Section<EffectItem> effectList(string filter, int st = 1, int en = 20) {
    // 获取数据条数
    string sql = "SELECT COUNT(*) AS sum FROM Effect";
    if (filter != "") sql += " WHERE (" + filter + ")";
    sql += " ORDER BY id ASC LIMIT " + to_string(st - 1) + ", " + to_string(en - st + 1);
    mysqld res = mysqli_query(mysql, sql.c_str());
    int pageCount = atoi(res[0]["sum"].c_str()) / 20;

    // 获取数据
    sql = "SELECT * FROM Effect";
    if (filter != "") sql += " WHERE (" + filter + ")";
    sql += " ORDER BY id ASC LIMIT " + to_string(st - 1) + ", " + to_string(en - st + 1);
    res = mysqli_query(mysql, sql.c_str());
    Section<EffectItem> list = Section<EffectItem>(pageCount, EffectSearch);
    for (int i = 0; i < res.size(); i++) {
        EffectItem data = EffectItem(
            res[i]["name"],
            res[i]["title"],
            res[i]["subtitle"],
            res[i]["author"],
            SRL<EffectThumbnail>(res[i]["thumbnail"], "/data/" + res[i]["thumbnail"]),
            SRL<EffectData>(res[i]["data"], "/data/" + res[i]["data"]),
            SRL<EffectAudio>(res[i]["audio"], "/data/" + res[i]["audio"])
        ); list.append(data);
    } return list;
}

#endif