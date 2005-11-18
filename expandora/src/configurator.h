#ifndef CONFIGURATOR_H 
#define CONFIGURATOR_H 
/* configurator */

#include <qstring.h>
#include <qregexp.h>
#include <vector>

using namespace std;

typedef struct {
  QString pattern;                /* pattern */
  QString data;                   /* data for event structure */
  QRegExp rexp;                   /* expression to match the pattern/WILDCAD */
  char    marker;                 /* R_FAIL, R_BLIND etc */
  char    type;                   /* R_something or C_something */
} TPattern;

class Cconfigurator {
    /* general */
    bool        conf_mod;       /* if the config was modified */
    
    /* colours */
    QString     look_col;
    QString     prompt_col;
    QString     end_col;
    
    
    /* patterns/regexps */
    QString exits_pat;
    
    
    /* regexps */
    QRegExp     roomname_exp;
    QRegExp     exits_exp;
    QRegExp     prompt_exp;
    
    /* data */
    QString     base_file;
    int         local_port;
    QString     remote_host;
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
    

public:

    /* this patterns data should be public for easier read access, write access
        will be implemented via functions anyway */
    vector<TPattern> patterns;
    void add_pattern(QString pattern, QString data, char marker, char type);

    Cconfigurator();
    
    

    void set_look_col(QString str);
    void set_prompt_col(QString str);
    void set_end_col(QString str);
    
    QString get_look_col() { return look_col; }
    QString get_prompt_col() { return prompt_col; }
    QString get_end_col() { return end_col; }
    
    /* patterns */
    QString get_exits_pat() { return exits_pat; }
    void set_exits_pat(QString str);
    
    
    
    /* regexps */
    void refresh_roomname_exp();
    void refresh_prompt_exp();
    
    QRegExp get_roomname_exp() { return roomname_exp; }
    QRegExp get_prompt_exp() { return prompt_exp; }
    QRegExp get_exits_exp() { return exits_exp; }
    
    void flush_all_exps();
    
    
    /* data / global flags */
    void set_base_file(QString str); 
    void set_data_mod(bool b) { db_modified = b; }
    void set_remote_host(QString str);
    void set_remote_port(int i);
    void set_local_port(int i);
    void set_conf_mod(bool b) { conf_mod = b; }

    void set_autorefresh(bool b); 
    void set_automerge(bool b);
    void set_angrylinker(bool b);
    void set_exits_check(bool b);
    void set_terrain_check(bool b);
    void set_details_vis(int i);
    void set_texture_vis(int i);    
    void set_brief_mode(bool b);
    void set_desc_quote(int i);
    void set_roomname_quote(int i);
    
    /*--*/
    bool get_data_mod() { return db_modified; } 
    QString get_base_file() { return base_file; } 
    QString get_remote_host() { return remote_host;}
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



#endif
