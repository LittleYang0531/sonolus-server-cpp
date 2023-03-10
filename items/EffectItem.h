#ifndef EFFECTITEM_H
#define EFFECTITEM_H

using namespace std;

class EffectItem {
    public:

    int id;
    string name;
    int version = 4;
    string title;
    string subtitle;
    string author;
    SRL<EffectThumbnail> thumbnail;
    SRL<EffectData> data;
    SRL<EffectAudio> audio;

    EffectItem(){}
    EffectItem(int id, string name, string title, string subtitle, string author,
        SRL<EffectThumbnail> thumbnail, SRL<EffectData> data, SRL<EffectAudio> audio):
        id(id), name(name), title(title), subtitle(subtitle), author(author),
        thumbnail(thumbnail), data(data), audio(audio){}
    EffectItem(int effect_id, Json::Value arr) {
        id = effect_id;
        name = arr["name"].asString();
        version = arr["version"].asInt();
        title = arr["title"].asString();
        subtitle = arr["subtitle"].asString();
        author = arr["author"].asString();
        thumbnail = SRL<EffectThumbnail>(arr["thumbnail"]);
        data = SRL<EffectData>(arr["data"]);
        audio = SRL<EffectAudio>(arr["audio"]);
    }
    
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

    argvar fetchParamList() {
        argvar args;
        args["name"] = name;
        args["version"] = to_string(version);
        args["title"] = title;
        args["subtitle"] = subtitle;
        args["author"] = author;
        args["thumbnail"] = thumbnail.url;
        args["data"] = data.url;
        args["audio"] = audio.url;
        args["url"] = "/effects/" + name;
        args["sonolus.url"] = "sonolus:" + appConfig["server.rootUrl"].asString() + "/effects/" + name;
        return args;
    }

    H toHTMLObject() {
        string buffer = readFile("./web/html/components/effects.html");
        buffer = str_replace(buffer, fetchParamList());
        return H(buffer);
    }
};

int effectNumber(string filter) {
    string sql = "SELECT COUNT(*) AS sum FROM Effect";
    if (filter != "") sql += " WHERE (" + filter + ")";
    dbres res = (new DB_Controller)->query(sql.c_str());
    return atoi(res[0]["sum"].c_str());
}

Section<EffectItem> effectList(string filter, int st = 1, int en = 20) {
    // ??????????????????
    int pageCount = ceil(1.0 * effectNumber(filter) / 20);

    // ????????????
    string sql = "SELECT * FROM Effect";
    if (filter != "") sql += " WHERE (" + filter + ")";
    sql += " ORDER BY id DESC LIMIT " + to_string(st - 1) + ", " + to_string(en - st + 1);
    dbres res = (new DB_Controller)->query(sql.c_str());
    Section<EffectItem> list = Section<EffectItem>(pageCount, EffectSearch);
    for (int i = 0; i < res.size(); i++) {
        EffectItem data = EffectItem(
            atoi(res[i]["id"].c_str()),
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

string effectFilter(argvar arg) {
    string filter = "";
    if (arg["keywords"] != "") filter = "title like \"%" + str_replace("\"", "\\\"", urldecode(arg["keywords"])) + "%\"";
    return filter;
}

int effectCreate(EffectItem item) {
    stringstream sqlbuffer;
    auto res = (new DB_Controller)->query("SELECT id FROM Effect WHERE name = \"" + item.name + "\"");
    if (res.size() != 0) return 0;
    int id = atoi((new DB_Controller)->query("SELECT COUNT(*) AS sum FROM Effect;")[0]["sum"].c_str()) + 1;
    sqlbuffer << "INSERT INTO Effect (id, name, version, title, subtitle, author, thumbnail, data, audio) VALUES (";
    sqlbuffer << id << ", \"" << item.name << "\", " << item.version << ", \"" << item.title << "\", ";
    sqlbuffer << "\"" << item.subtitle << "\", \"" << item.author << "\", \"" << item.thumbnail.hash << "\", ";
    sqlbuffer << "\"" << item.data.hash << "\", \"" << item.audio.hash << "\")";
    return (new DB_Controller)->execute(sqlbuffer.str());
}

#endif