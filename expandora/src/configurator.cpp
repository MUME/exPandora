

/* configuration reader/saver and handler */

#include "configurator.h"

class Cconfigurator conf;

Cconfigurator::Cconfigurator()
{
    /* here we set the default configuration */
    
    /* data */
    base_file = "";
    local_port = 0;
    remote_host = "";
    remote_port = 0;
    db_modified = false;
    set_conf_mod(false);
    
    set_autorefresh(true);          /* default values */ 
    set_automerge(true);
    set_angrylinker(true);
    set_exits_check(false);
    set_terrain_check(true);
    
    set_details_vis(500);
    set_texture_vis(300);
    
    set_roomname_quote(10);
    set_desc_quote(10);

    
    
    /* colours */
    set_look_col("[32m");
    set_prompt_col("");
    set_end_col("[0m");
    
    /* patterns */
    set_exits_pat("Exits:");
    
    
    /* regexps - the working pony */
    flush_all_exps();

}


/* ---------------- PATTERNS and REGEXPS GENERATION --------------- */
void Cconfigurator::set_exits_pat(QString str) 
{ 
    exits_pat = str; 
    exits_exp.setPattern(QRegExp::escape(str) );
    
    set_conf_mod(true);
}


void Cconfigurator::add_pattern(QString pattern, QString data, char marker, char type)
{
    TPattern p;
    
    p.pattern = pattern;
    p.data = data;
    p.marker = marker;
    p.type = type;
    p.rexp.setWildcard(true);
    p.rexp.setPattern(pattern);

    patterns.push_back(p);
}



/* ----------------- REGULAR EXPRESSIONS SECTION ---------------- */
void Cconfigurator::flush_all_exps()
{
    refresh_roomname_exp();
    refresh_prompt_exp();
}

void Cconfigurator::refresh_roomname_exp()
{
    roomname_exp.setPattern("^"+ QRegExp::escape(get_look_col()) + ".*" 
                    + QRegExp::escape(get_end_col())+"$");
}

void Cconfigurator::refresh_prompt_exp()
{
    prompt_exp.setPattern("^"+ QRegExp::escape(get_prompt_col()) + ".*>" 
                    + QRegExp::escape(get_end_col()));
}



/* ----------- COLOURS SECTION ---------------*/
void Cconfigurator::set_look_col(QString str)
{ 
    look_col = str; 
    refresh_roomname_exp();
    set_conf_mod(true);
}

void Cconfigurator::set_prompt_col(QString str) 
{ 
    prompt_col = str; 
    set_conf_mod(true);
}

void Cconfigurator::set_end_col(QString str) 
{ 
    /* I cant imagine this happening, but hell ... */
    end_col = str; 
    flush_all_exps();
    set_conf_mod(true);
}

/* ------------------- DATA ------------------- */
void Cconfigurator::set_base_file(QString str)
{
    base_file = str;
    set_conf_mod(true);
}

void Cconfigurator::set_remote_host(QString str)
{
    remote_host = str;
    set_conf_mod(true);
}

void Cconfigurator::set_remote_port(int i)
{
    remote_port = i;
    set_conf_mod(true);
}

void Cconfigurator::set_local_port(int i)
{
    local_port = i;
    set_conf_mod(true);
}

void Cconfigurator::set_autorefresh(bool b)
{
    autorefresh = b;
    set_conf_mod(true);
}

void Cconfigurator::set_automerge(bool b)
{
    automerge = b;
    set_conf_mod(true);
}

void Cconfigurator::set_angrylinker(bool b)
{
    angrylinker = b;
    set_conf_mod(true);
}

void Cconfigurator::set_exits_check(bool b)
{
    exits_check = b;
//    set_conf_mod(true);       /* this option changes repeatedly when you turn */
                                /* mapping on and off */
}

void Cconfigurator::set_terrain_check(bool b)
{
    terrain_check = b;
    set_conf_mod(true);
}

void Cconfigurator::set_details_vis(int i)
{
    details_visibility_range = i;
    set_conf_mod(true);
}

void Cconfigurator::set_texture_vis(int i)
{
    texture_visibilit_range = i;
    set_conf_mod(true);
}

void Cconfigurator::set_brief_mode(bool b)
{
    brief_mode = b;
    set_conf_mod(true);
}

void Cconfigurator::set_roomname_quote(int i)
{
    name_quote = i;
    set_conf_mod(true);
}

void Cconfigurator::set_desc_quote(int i)
{
    desc_quote = i;
    set_conf_mod(true);
}
