#ifndef BACKGROUNDITEM_H
#define BACKGROUNDITEM_H

using namespace std;

class BackgroundItem {
    public:

    string name;
    int version = 2;
    string title;
    string subtitle;
    string author;
    SRL<BackgroundThumbnail> thumbnail;
    SRL<BackgroundData> data;
    SRL<BackgroundImage> image;
    SRL<BackgroundConfiguration> configuration;

    BackgroundItem(){}
    BackgroundItem(string name, string title, string subtitle, string author, 
        SRL<BackgroundThumbnail> thumbnail, SRL<BackgroundData> data, SRL<BackgroundImage> image, SRL<BackgroundConfiguration> configuration):
        name(name), title(title), subtitle(subtitle), author(author),
        thumbnail(thumbnail), data(data), image(image), configuration(configuration){}
    
    Json::Value toJsonObject() {
        Json::Value res;
        res["name"] = name;
        res["version"] = version;
        res["title"] = title;
        res["subtitle"] = subtitle;
        res["author"] = author;
        res["thumbnail"] = thumbnail.toJsonObject();
        res["data"] = data.toJsonObject();
        res["image"] = image.toJsonObject();
        res["configuration"] = configuration.toJsonObject();
        return res;
    }
};

Section<BackgroundItem> backgroundList(string filter, int st = 1, int en = 20) {
    // 获取数据条数
    string sql = "SELECT COUNT(*) AS sum FROM Background";
    if (filter != "") sql += " WHERE (" + filter + ")";
    sql += " ORDER BY id ASC LIMIT " + to_string(st - 1) + ", " + to_string(en - st + 1);
    mysqld res = mysqli_query(mysql, sql.c_str());
    int pageCount = atoi(res[0]["sum"].c_str()) / 20;

    // 获取数据
    sql = "SELECT * FROM Background";
    if (filter != "") sql += " WHERE (" + filter + ")";
    sql += " ORDER BY id ASC LIMIT " + to_string(st - 1) + ", " + to_string(en - st + 1);
    res = mysqli_query(mysql, sql.c_str());
    Section<BackgroundItem> list = Section<BackgroundItem>(pageCount, BackgroundSearch);
    for (int i = 0; i < res.size(); i++) {
        BackgroundItem data = BackgroundItem(
            res[i]["name"], 
            res[i]["title"], 
            res[i]["subtitle"],
            res[i]["author"],
            SRL<BackgroundThumbnail>(res[i]["thumbnail"], "/data/" + res[i]["thumbnail"]),
            SRL<BackgroundData>(res[i]["data"], "/data/" + res[i]["data"]),
            SRL<BackgroundImage>(res[i]["image"], "/data/" + res[i]["image"]),
            SRL<BackgroundConfiguration>(res[i]["configuration"], "/data/" + res[i]["configuration"])
        ); list.append(data);
    } return list;
}

#endif