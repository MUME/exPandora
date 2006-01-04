#ifndef CONFIGURATOR_H 
#define CONFIGURATOR_H 
/* configurator */

#include <QString>
#include <QRegExp>
#include <vector>
#include <QXmlDefaultHandler>
#include <QGLWidget>
#include <map>

#include "CRoom.h"
#include "event.h"

#define ACMD(name)  void Cconfigurator::name(char *line)
#define DEF_ACMD(name) void name(char *line)

#define NUM_CONFIG_COMMANDS     23

using namespace std;

typedef struct {
  QByteArray pattern;                /* pattern */
  QRegExp rexp;                   /* expression to match the pattern/WILDCAD */
  bool    is_regexp;            /* is it regexp or wildcard ? */
  TEvent  event;
} TPattern;

struct room_sectors_data {
  QByteArray desc;             /* name of this flag */
  QByteArray filename;         /* appropriate texture's filename */
  char   pattern;           /* appropriate pattern */
  GLuint texture;          /* and texture handler for renderer */
  GLuint gllist;            /* OpenGL display list */
};

class Cconfigurator {
    /* general */
    bool        conf_mod;       /* if the config was modified */
    QByteArray  config_file;
    QByteArray  config_path;
    
    /* colours */
    QByteArray     look_col;
    QByteArray     prompt_col;
    QByteArray     end_col;
    
    
    /* patterns/regexps */
    QByteArray exits_pat;
    
    
    /* regexps */
    QRegExp     roomname_exp;
    QRegExp     exits_exp;
    QRegExp     prompt_exp;
    
    /* data */
    QByteArray  database_path;
    QByteArray  base_file;
    int         local_port;
    QByteArray  remote_host;
    int         remote_port;
    bool        db_modified;
    

    bool autorefresh;             /* automatic room desc refresh */
    bool automerge;               /* automatic twins merging based on roomdesc */
    bool angrylinker;             /* automatic linking based on coordinates */
    bool exits_check;             /* apply exits check to stacks */
    bool terrain_check;           /* apply terrain check to stacks */
    bool brief_mode;

    
    int texture_visibilit_range;
    int details_visibility_range;

    
    int desc_quote;        /* quote for description - in percents */
    int name_quote;        /* quote for roomname - in percents */
    
    void parse_engine(char *line);
    void parse_line(char *line);
    
    void reset_current_config();

public:

    /* this patterns data should be public for easier read access, write access
        will be implemented via functions anyway */
    vector<TPattern> patterns;
    void add_pattern(QByteArray pattern, QByteArray data, char grp, char type, bool is_regexp);



    /* texture and sectors stuff */
    vector<struct room_sectors_data> sectors;
    int get_sector_by_desc(QByteArray desc);
    int get_sector_by_pattern(char pattern);

    int load_texture(struct room_sectors_data *p);
    char get_pattern_by_room(CRoom *r);
    GLuint get_texture_by_desc(QByteArray desc);
    void add_texture(QByteArray desc, QByteArray filename, char pattern);
    
    /* */
    
    int load_engine_config(QByteArray path, QByteArray filename);
    
    Cconfigurator();


    int load_config(QByteArray path, QByteArray filename);
    int save_config_as(QByteArray path, QByteArray filename);
    int save_config() { return save_config_as(config_path, config_file); }
    

    void set_look_col(QByteArray str);
    void set_prompt_col(QByteArray str);
    void set_end_col(QByteArray str);
    
    QByteArray get_look_col() { return look_col; }
    QByteArray get_prompt_col() { return prompt_col; }
    QByteArray get_end_col() { return end_col; }
    int get_prompt_col_len() { return prompt_col.length(); }
    
    /* patterns */
    QByteArray get_exits_pat() { return exits_pat; }
    void set_exits_pat(QByteArray str);
    
    
    
    /* regexps */
    void refresh_roomname_exp();
    void refresh_prompt_exp();
    
    QRegExp get_roomname_exp() { return roomname_exp; }
    QRegExp get_prompt_exp() { return prompt_exp; }
    QRegExp get_exits_exp() { return exits_exp; }
    
    void flush_all_exps();
    
    
    /* data / global flags */
    void set_base_file(QByteArray str); 
    void set_data_mod(bool b) { db_modified = b; }
    void set_remote_host(QByteArray str);
    void set_remote_port(int i);
    void set_local_port(int i);
    void set_conf_mod(bool b) { conf_mod = b; }

    void set_config_file(QByteArray p, QByteArray f) { config_file = f; config_path = p; }

    void set_autorefresh(bool b); 
    void set_automerge(bool b);
    void set_angrylinker(bool b);
    void set_exits_check(bool b);
    void set_terrain_check(bool b);
    void set_details_vis(int i);
    void set_texture_vis(int i);    
    void set_brief_mode(bool b);
    void set_desc_quote(int i);
    void set_name_quote(int i);
    
    /*--*/
    bool get_data_mod() { return db_modified; } 
    QByteArray get_base_file() { return base_file; } 
    QByteArray get_remote_host() { return remote_host;}
    int get_remote_port() {return remote_port;}
    int get_local_port() {return local_port;}
    bool get_conf_mod() { return conf_mod; }

    bool get_autorefresh() { return autorefresh; }
    bool get_automerge()  { return automerge; }
    bool get_angrylinker() { return angrylinker; }
    bool get_exits_check() { return exits_check; }
    bool get_terrain_check() { return terrain_check; } 
    bool get_brief_mode()     {return brief_mode;}
    
    int get_details_vis() { return details_visibility_range; }
    int get_texture_vis() { return texture_visibilit_range; }
    
    int get_desc_quote() { return desc_quote; }
    int get_name_quote() { return name_quote; }
};

extern class Cconfigurator conf;


class ConfigParser : public QXmlDefaultHandler {
public:
  ConfigParser();
  bool characters(const QString& ch);
    
  bool startElement( const QString&, const QString&, const QString& ,
		     const QXmlAttributes& );
  bool endElement( const QString&, const QString&, const QString& );
private:
  enum {ROOMCOLOUR = 1, PROMPTCOLOUR, TEXTURE, PATTERN };
  /* some flags */
  int flag;
  QString s;

  struct room_sectors_data texture;
  TPattern pattern;
  
  int i;
};



#endif