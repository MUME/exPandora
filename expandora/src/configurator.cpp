

/* configuration reader/saver and handler */
#include <QFile>
#include <QImage>
#include <QXmlDefaultHandler>
#include <QGLWidget>
#include <QMutex>

#include "defines.h"
#include "CRoom.h"
#include "Map.h"

#include "configurator.h"
#include "utils.h"
#include "engine.h"
#include "event.h"
//#include "renderer.h"

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
    
    set_name_quote(10);
    set_desc_quote(10);

    
    
    /* colours */
    set_look_col("[32m");
    set_prompt_col("");
    set_end_col("[0m");
    
    /* patterns */
    set_exits_pat("Exits:");
    
    
    /* regexps - the working pony */
    flush_all_exps();

    /* setup the config readers table */
/*
    add_command("localport",         parse_localport,        update_localport);
    add_command("remoteport",        parse_remoteport,       update_remoteport);
    add_command("remotehost",        parse_remotehost,       update_remotehost);
    add_command("basefile",          parse_basefile,         update_basefile);
    add_command("roomcolour",        parse_roomcolour,       update_roomcolour);
    add_command("autocheckexits",    parse_autocheckexits,   update_autocheckexits);
    add_command("autocheckterrain",  parse_autocheckterrain, update_autocheckterrain);
    add_command("pattern",           parse_pattern,          update_pattern);
    add_command("mapperbriefmode",   parse_mapperbriefmode,  update_mapperbriefmode);
    add_command("exitspattern",      parse_exitspattern,     update_exitspattern);
    add_command("terraintype",       parse_terraintype,      update_terraintype);
    add_command("leadersetup",       parse_leadersetup,      update_leadersetup);
    add_command("debug",             parse_debug,            update_debug);
    add_command("include",           parse_include,          update_include);
    add_command("engine",            parse_engine,           update_engine);
    add_command("roomnamesimilarityquote",parse_roomnamequote,update_roomnamequote);
    add_command("descsimilarityquote",parse_descquote,       update_descquote);
    add_command("autorefresh",       parse_autorefresh,      update_autorefresh);
    add_command("automerge",		parse_automerge,        update_automerge);
    add_command("texturesvisibilityrange", parse_texturesvisibility, update_texturesvisibility);
    add_command("detailsvisibilityrange",  parse_detailsvisibility, update_detailsvisibility);
    add_command("angrylinker",       parse_angrylinker,      update_angrylinker);
*/

    struct room_sectors_data first;
        
    first.pattern = 0;
    first.desc = "NONE";
    first.texture = 1;
    first.gllist = 1;
    sectors.push_back(first);
}


void Cconfigurator::reset_current_config()
{
    patterns.clear();
    sectors.clear();
    
    struct room_sectors_data first;
        
    first.pattern = 0;
    first.desc = "NONE";
    first.texture = 1;
    first.gllist = 1;
    sectors.push_back(first);
}

/* ---------------- PATTERNS and REGEXPS GENERATION --------------- */
void Cconfigurator::set_exits_pat(QByteArray str) 
{ 
    exits_pat = str; 
    exits_exp.setPattern(QRegExp::escape(str) );
    
    set_conf_mod(true);
}


void Cconfigurator::add_pattern(QByteArray pattern, QByteArray data, char marker, char type, bool is_regexp)
{
    TPattern p;
    
    p.is_regexp = is_regexp;
    p.pattern = pattern;
    p.event.data = data;
    p.event.group = marker;
    p.event.type = type;
    if (is_regexp) 
      p.rexp.setPatternSyntax(QRegExp::RegExp);
    else 
      p.rexp.setPatternSyntax(QRegExp::Wildcard);
    p.rexp.setPattern(pattern);

    patterns.push_back(p);
    set_conf_mod(true);
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
void Cconfigurator::set_look_col(QByteArray str)
{ 
    look_col = str; 
    refresh_roomname_exp();
    set_conf_mod(true);
}

void Cconfigurator::set_prompt_col(QByteArray str) 
{ 
    prompt_col = str; 
    set_conf_mod(true);
}

void Cconfigurator::set_end_col(QByteArray str) 
{ 
    /* I cant imagine this happening, but hell ... */
    end_col = str; 
    flush_all_exps();
    set_conf_mod(true);
}

/* ------------------- DATA ------------------- */
char Cconfigurator::get_pattern_by_room(CRoom *r)
{
    return sectors.at(r->sector).pattern;
}

int Cconfigurator::get_sector_by_desc(QByteArray desc)
{
    unsigned int i;
    for (i = 0; i < sectors.size(); ++i) {
        if (sectors[i].desc == desc)
            return i;
    }
    return -1;
}

GLuint Cconfigurator::get_texture_by_desc(QByteArray desc)
{
    int i;
    i = get_sector_by_desc(desc);
    if (i == -1)
        return 0;
    return sectors[i].texture;
}


void Cconfigurator::add_texture(QByteArray desc, QByteArray filename, char pattern)
{
    struct room_sectors_data s;
        
    s.desc = desc;
    s.filename = filename;
    s.pattern = pattern;

    sectors.push_back(s);
//    printf("added texture with pattern %c.\r\n", pattern);
}

int Cconfigurator::get_sector_by_pattern(char pattern)
{
    unsigned int i;
    for (i = 0; i < sectors.size(); ++i) {
        if (sectors[i].pattern == pattern)
            return i;
    }
    return 0;
}


void Cconfigurator::set_base_file(QByteArray str)
{
    base_file = str;
    set_conf_mod(true);
}

void Cconfigurator::set_remote_host(QByteArray str)
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

void Cconfigurator::set_name_quote(int i)
{
    name_quote = i;
    set_conf_mod(true);
}

void Cconfigurator::set_desc_quote(int i)
{
    desc_quote = i;
    set_conf_mod(true);
}


int Cconfigurator::load_texture(struct room_sectors_data *p)
{
    QImage tex1, buf1;

    glGenTextures(1, &p->texture);
    print_debug(DEBUG_RENDERER, "loading texture %s", (const char *) p->filename);
    
    if (!buf1.load( p->filename )) {
        printf("Failed to load the %s!\r\n", (const char *) p->filename);
        return -1;
    }
    tex1 = QGLWidget::convertToGLFormat( buf1 );
    glGenTextures(1, &p->texture );
    glBindTexture(GL_TEXTURE_2D, p->texture );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex1.width(), tex1.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, tex1.bits() );
    
    
    
    p->gllist = glGenLists(1);
    if (p->gllist != 0) {
        glNewList(p->gllist, GL_COMPILE);
        
        glEnable(GL_TEXTURE_2D);
        
        glBindTexture(GL_TEXTURE_2D, p->texture);
        
        glBegin(GL_QUADS);
            glTexCoord2f(0.0, 1.0);
            glVertex3f(-ROOM_SIZE,  ROOM_SIZE, 0.0f);
            glTexCoord2f(0.0, 0.0);
            glVertex3f(-ROOM_SIZE, -ROOM_SIZE, 0.0f);
            glTexCoord2f(1.0, 0.0);
            glVertex3f( ROOM_SIZE, -ROOM_SIZE, 0.0f);
            glTexCoord2f(1.0, 1.0);
            glVertex3f( ROOM_SIZE,  ROOM_SIZE, 0.0f);
            
        glEnd();
        glDisable(GL_TEXTURE_2D);
        
        
        
        glEndList();
    }
    return 1;
}


int Cconfigurator::load_config(QByteArray path, QByteArray filename)
{
  QFile xmlFile(path+filename);
  QXmlInputSource source( &xmlFile );

  if (xmlFile.exists() == false) {
      printf("ERROR: The config file %s does NOT exist!\r\n", (const char*) (path+filename) );
      return 0;
  }

  QXmlSimpleReader reader;

  ConfigParser * handler = new ConfigParser();
  reader.setContentHandler( handler );
    
  reset_current_config();
	
    
  printf("Reading the config file %s\r\n", (const char *) (path+filename));
  fflush(stdout);
  reader.parse( source );
  printf("done.\r\n");
  set_conf_mod(false);


  config_path = path;
  config_file = filename;
  return 1;
}

int Cconfigurator::save_config_as(QByteArray path, QByteArray filename)
{
  FILE *f;
  unsigned int i;

  config_file = filename;
  config_path = path;

  f = fopen((const char *) path + filename, "w");
  if (f == NULL) {
    printf("XML: Error - can not open the file: %s.\r\n", (const char *) filename);
    return -1;
  }    
  
  fprintf(f, "<config>\r\n");
  fprintf(f, "  <localport port=\"%i\">\r\n", get_local_port());
  fprintf(f, "  <remotehost hostname=\"%s\" port=\"%i\">\r\n", 
                  (const char *) get_remote_host(), 
                  (const char *) get_remote_port() );
  fprintf(f, "  <basefile filename=\"%s\">\r\n", 
                  (const char *) get_base_file() );
  fprintf(f, "  <roomnamecolour>%s</roomnamecolour>\r\n", 
                  (const char *) get_look_col() );
  fprintf(f, "  <promptcolour>%s</promptcolour>\r\n", 
                  (const char *) get_prompt_col() );
  fprintf(f, "  <GLvisibility textures=\"%i\" details=\"%i\">\r\n", 
                  get_texture_vis(),  get_details_vis() );
  
  fprintf(f, "  <analyzers desc=\"%s\" exits=\"%s\"  terrain=\"%s\">\r\n", 
                  "ON", ON_OFF(get_exits_check() ), ON_OFF(get_terrain_check() ) );

  fprintf(f, "  <engineflags briefmode=\"%s\" automerge=\"%s\"  angrylinker=\"%s\">\r\n", 
                  ON_OFF(get_brief_mode()), 
                  ON_OFF(get_automerge() ), ON_OFF( get_angrylinker()) );

  fprintf(f, "  <refresh auto=\"%s\" roomnamequote=\"%i\" descquote=\"%i\">\r\n",
                  ON_OFF( get_autorefresh() ), get_name_quote(), get_desc_quote() );
  
  QString ch;
  for (i = 1; i < sectors.size(); i++) {
      if (sectors[i].pattern == '<')
          ch = "&lt;";
      else if (sectors[i].pattern == '>')
          ch = "&gt;";
      else if (sectors[i].pattern == '&')
          ch = "&amp;";
      else if (sectors[i].pattern == '\'')
          ch = "&apos;";
      else if (sectors[i].pattern == '"')
          ch = "&quot";
      else
          ch = sectors[i].pattern;
      
      fprintf(f, "  <texture handle=\"%s\" file=\"%s\" pattern=\"%s\">\r\n",
                  (const char *) sectors[i].desc, 
                  (const char *) sectors[i].filename, qPrintable(ch));
  }
  
  for (i = 0; i < patterns.size(); i++) {
        fprintf(f, "  <pattern regexp=\"%s\" type=\"%s\" data=\"%s\">%s</pattern>\r\n",
                  YES_NO(patterns[i].is_regexp), 
                  (const char *) Events[patterns[i].event.type].data, 
                  (const char *) patterns[i].event.data, 
                  (const char *) patterns[i].pattern );
  }
  i = 0;
  while (debug_data[i].name) {
      fprintf(f, "  <debug name=\"%s\"  state=\"%s\">\r\n", debug_data[i].name, ON_OFF(debug_data[i].state));
      i++;
  }

  /* PUT ENGINE CONFIG SAVING THERE ! */
  
  set_conf_mod(false);
  fprintf(f, "</config>\r\n");
  fflush(f);
  fclose(f);
  return 1;
}


/* --------------- HERE COMES THE XML READER FOR CONFIG FILES -------------- */


ConfigParser::ConfigParser()
  : QXmlDefaultHandler()
{
}


bool ConfigParser::endElement( const QString& , const QString& , const QString& qName)
{
  if (qName == "pattern" && flag == PATTERN) {
    conf.add_pattern(pattern.pattern, pattern.event.data, pattern.event.group, 
                                      pattern.event.type, pattern.is_regexp);          
  }
  flag = 0;    
  return TRUE;
}

bool ConfigParser::characters( const QString& ch)
{
  s = ch;  
  if (ch == NULL || ch == "")
    return TRUE;
    
  if (flag == ROOMCOLOUR) {
      conf.set_look_col(s.toAscii());
      printf("The room name colour : %s Test.%s\r\n", (const char *) conf.get_look_col(), (const char *)conf.get_end_col() );
      printf("And the received str : %s str %s.\r\n", qPrintable(ch), (const char *)conf.get_end_col() );
  } else if (flag == PROMPTCOLOUR) {
      conf.set_prompt_col(s.toAscii());
  } else /*if (flag == TEXTURE) {
      texture.pattern = ch[0].toAscii();  
  } else */
  if (flag == PATTERN) {
      ch.toUpper();
      pattern.pattern = ch.toAscii();
      
  }

  return TRUE;
} 


bool ConfigParser::startElement( const QString& , const QString& , 
                                    const QString& qName, 
                                    const QXmlAttributes& attributes)
{
    unsigned int i;


    if (qName == "localport") {
        if (attributes.length() < 1) {
            printf("(localport token) Not enough attributes in XML file!");
            exit(1);
        }        
        
        s = attributes.value("port");
        conf.set_local_port(s.toInt() );
        printf("Using local port %i. \r\n", conf.get_local_port() );

        return TRUE;
    } else if (qName == "remotehost") {
        if (attributes.length() < 2) {
            printf("(remotehost token) Not enough attributes in XML file!");
            exit(1);
        }        
        
        s = attributes.value("hostname");
        conf.set_remote_host(s.toAscii() );
        
        s = attributes.value("port");
        conf.set_remote_port(s.toInt() );
        printf("Using remote host %s:%i\r\n", (const char *)conf.get_remote_host(), 
                                            conf.get_remote_port() );

        return TRUE;
    } else if (qName == "basefile") {
        if (attributes.length() < 1) {
            printf("(basefile token) Not enough attributes in XML file!");
            exit(1);
        }        
        
        s = attributes.value("filename");
        conf.set_base_file(s.toAscii() );
        printf("Using the database file: %s\r\n", qPrintable(s) );
        
        return TRUE;
    } else if (qName == "roomnamecolour") {
        flag = ROOMCOLOUR;
        return TRUE;
    } else if (qName == "promptcolour") {
        flag = PROMPTCOLOUR;
        return TRUE;
    } else if (qName == "GLvisibility") {
        if (attributes.length() < 2) {
            printf("(GLvisibility token) Not enough attributes in XML file!");
            exit(1);
        }        
        
        s = attributes.value("textures");
        conf.set_texture_vis(s.toInt() );
        s = attributes.value("details");
        conf.set_details_vis(s.toInt() );
        
        printf("OpenGL visibility ranges set to %i (texture) and %i (details).\r\n",
                    conf.get_texture_vis(), conf.get_details_vis() );
        
        return TRUE;
    } else if (qName == "analyzers") {
        if (attributes.length() < 3) {
            printf("(analyzers token) Not enough attributes in XML file!");
            exit(1);
        }        
        
        s = attributes.value("exits");
        s = s.toLower();
        if (s == "on") 
            conf.set_exits_check(true);
        else 
            conf.set_exits_check(false);
        
        s = attributes.value("terrain");
        s = s.toLower();
        if (s == "on") 
            conf.set_terrain_check(true);
        else 
            conf.set_terrain_check(false);
        
        
        printf("Analyzers: desc ON, exits %s, terrain %s.\r\n",
                    ON_OFF(conf.get_exits_check() ), ON_OFF(conf.get_terrain_check()) );
        
        return TRUE;
    } else if (qName == "engineflags") {
        if (attributes.length() < 3) {
            printf("(engineflags token) Not enough attributes in XML file!");
            exit(1);
        }        
        
        s = attributes.value("briefmode");
        s = s.toLower();
        printf("The brief mode setting : %s\r\n", qPrintable(s) );
        if (s == "on") 
            conf.set_brief_mode(true);
        else 
            conf.set_brief_mode(false);
        
        s = attributes.value("automerge");
        s = s.toLower();
        if (s == "on") 
            conf.set_automerge(true);
        else 
            conf.set_automerge(false);

        s = attributes.value("angrylinker");
        s = s.toLower();
        if (s == "on") 
            conf.set_angrylinker(true);
        else 
            conf.set_angrylinker(false);

        printf("Engine flags: briefmode %s, automerge %s, angrylinker %s.\r\n",
               ON_OFF(conf.get_brief_mode()), ON_OFF(conf.get_automerge()), 
                ON_OFF(conf.get_angrylinker()) );
        
        return TRUE;
    } else if (qName == "refresh") {
        if (attributes.length() < 3) {
            printf("(refresh token) Not enough attributes in XML file!");
            exit(1);
        }        
        
        s = attributes.value("auto");
        s = s.toLower();
        if (s == "on") 
            conf.set_autorefresh(true);
        else 
            conf.set_autorefresh(false);
        
        s = attributes.value("roomnamequote");
        conf.set_name_quote(s.toInt());
        s = attributes.value("descquote");
        conf.set_desc_quote(s.toInt());

        printf("Autorefresh settings: automatic refresh %s, roomname quote %i, desc quote %i.\r\n",
                ON_OFF(conf.get_autorefresh()), conf.get_name_quote(), 
                conf.get_desc_quote() );
        return TRUE;
    } else if (qName == "texture") {
        if (attributes.length() < 2) {
            printf("(texture token) Not enough attributes in XML file!");
            exit(1);
        }        
        
        s = attributes.value("handle");
        s.toUpper();
        texture.desc = s.toAscii();
        
        s = attributes.value("file");
        texture.filename = s.toAscii();

        s = attributes.value("pattern");
        texture.pattern = s[0].toAscii();

        conf.add_texture(texture.desc, texture.filename, texture.pattern);
//        printf("Added texture: desc %s, file %s, pattern %c.\r\n", 
//              (const char *) texture.desc, (const char *) texture.filename, texture.pattern);

        
//        flag = TEXTURE;         /* get the inner data ! */
        return TRUE;
    } else if (qName == "pattern") {
        if (attributes.length() < 2) {
            printf("(pattern token) Not enough attributes in XML file!");
            exit(1);
        }        

        s = attributes.value("regexp");
        s = s.toLower();
        pattern.is_regexp = false;
        if (s == "yes") 
          pattern.is_regexp = true;
        
        
        s = attributes.value("type");
        pattern.event.type = 0;
        for (i = 0; i < Events.size(); i++) 
            if ( s == Events[i].data) {
                pattern.event.type = Events[i].type;
                pattern.event.group = Events[i].group;
                break;
            }
        
        if (pattern.event.type == -1) {
            printf("Bad type descriptor.\r\n");
            return TRUE;                                     
        } 

        s = attributes.value("data");
        pattern.event.data = s.toAscii();
        
        flag = PATTERN;         /* get the inner data ! */
        return TRUE;
    } else if (qName == "debug") {
        if (attributes.length() < 1) {
            printf("(texture token) Not enough attributes in XML file!");
            exit(1);
        }        
        
        s = attributes.value("name");
        unsigned int i = 0;
        while (debug_data[i].name != NULL) {
            if (debug_data[i].name == s) 
                break;
            i++;
        }
        if (debug_data[i].name == NULL) {
            printf("Warning, %s is a wrong debug descriptor/name!\r\n", qPrintable(s));
            return TRUE;
        }
        
        s = attributes.value("state");
        s = s.toLower();
        if (s == "on") 
            debug_data[i].state = 1;
        else 
            debug_data[i].state = 0;
        
//        printf("Debug option %s is now %s.\r\n", debug_data[i].name, ON_OFF(debug_data[i].state) );
        return TRUE;
    } 
    
  return TRUE;
}

/************************************************/
/*                                              */
/*                                              */
/*       OLD config reader for engine.cfg file  */
/*                                              */
/*                                              */
/************************************************/

#define BUFFER_SIZE 4096


class buffered_file {
  FILE *f; 

  char filename[MAX_STR_LEN];
  char buffered_buffer[BUFFER_SIZE];
  int buffered_amount;
  int buffered_pos; /* current position in buffer */

  int line_number;

public:
  buffered_file();    

  int open(const char *fname);
  int readln(char *line, int maxlen);
  int get_linenum();

  void close() { fclose(f); };
  int eof();
  char *get_filename();

};

buffered_file::buffered_file()
{
  f = NULL;
  filename[0] = 0;
  buffered_pos = 0;
  buffered_amount = 0;
  line_number = 0;
}

char * buffered_file::get_filename()
{
  return filename;
}

int buffered_file::eof()
{
  if (feof(f) && (buffered_pos==buffered_amount))
    return 1;
  return 0;
}


int buffered_file::readln(char *line, int maxlen)
{
  int i, j;
  
  i = buffered_pos;
  if (buffered_pos == buffered_amount) {
    if ( feof(f) ) {
      line[0]=0;
      return 0;
    }
    buffered_amount = fread(buffered_buffer, 1, BUFFER_SIZE, f);
    buffered_pos=0;
    i=0;
  }
  j=0;
  while (buffered_buffer[i] != 0x0a) {
    line[j++]=buffered_buffer[i++];
    if (j==maxlen) 
      break;
    if (i==buffered_amount) {
      if (feof(f))
        break;
      buffered_amount=fread(buffered_buffer, 1, BUFFER_SIZE, f);
      buffered_pos=0;
      i=0;
    }
  }
  buffered_pos=++i;
  line[j]=0;
  
  line_number++;
  return j;
}

int buffered_file::get_linenum()
{
  return line_number;
}

int buffered_file::open(const char *fname)
{
  f = fopen(fname, "r");
  strcpy(filename, fname);
  if (!f)
    return -1;
  else 
    return 0;
  
  buffered_amount=fread(buffered_buffer, 1, BUFFER_SIZE, f);
  buffered_pos = 0;
  line_number = 0;
  
}


class buffered_file *current_buffered_reader = NULL;

#define CONFIG_ERROR(message) \
  printf("-!!- %s:%i: Error : %s.\r\n", \
        current_buffered_reader->get_filename(),        \
        current_buffered_reader->get_linenum(),         \
        message); 



#define GET_NEXT_ARGUMENT(p, line, argument, mode)	\
  p = skip_spaces(line);                        \
  if (!*p) {                                    \
    CONFIG_ERROR("missing argument")            \
    return;                                     \
  }                                             \
  p = one_argument(p, arg, mode); 


void Cconfigurator::parse_engine(char *line)
{
  char *p;
  char arg[MAX_STR_LEN];
  unsigned int i;
  char cause_type, result_type;
  
  
  
  p = skip_spaces(line);                        
  if (!*p) {                                    
    CONFIG_ERROR("missing cause entry");
    return;                                     
  }                                             
  p = one_argument(p, arg, 2);         /* to upper case */

  
  cause_type = -1;
  for (i = 0; i < Events.size(); i++) 
    if ( (Events[i].group == E_CAUSE) &&               
         (Events[i].data == arg) ) 
    {
      cause_type = Events[i].type;
      break;
    }
    
  if (cause_type == -1) {
    CONFIG_ERROR("bad cause type");
    return;                                     
  } 
  
    

  p = skip_spaces(p);                        
  if (!*p) {                                    
    CONFIG_ERROR("missing result entry in engine config line");
    return;                                     
  }                                             
  p = one_argument(p, arg, 2);  /* to upper case */          
  
  result_type = -1;
  for (i = 0; i < Events.size(); i++) 
    if ( (Events[i].group == E_RESULT) &&               
         (Events[i].data == arg) ) 
    {
      result_type = Events[i].type;
      break;
    }

  if (result_type == -1) {
    CONFIG_ERROR("bad result type");
    return;                                     
  } 
  
  p = skip_spaces(p);                        
  if (!*p) {                                    
    CONFIG_ERROR("missing command line");
    return;                                     
  }                       

  if (Engine.parse_command_line(cause_type, result_type, p) != 0) 
    CONFIG_ERROR("cant parse command");
}

int Cconfigurator::load_engine_config(QByteArray path, QByteArray fn)
{
  char line[MAX_STR_LEN];
  class buffered_file *buffered;
  
  
  buffered = new class buffered_file;

  QByteArray filename = path + fn;
  printf("Loading the engine script file %s\r\n", (const char*) filename);

  current_buffered_reader = buffered;
  
  if (buffered->open((const char *) filename) != 0) 
  {
    printf("ERROR: Cant read config file %s!\r\n", (const char *) filename);
    return -1;
  }
  
  /* buffered reading and line parsing cycle */
  do {
    buffered->readln(line, MAX_STR_LEN);
    parse_line(line);
  } while (!buffered->eof());
  
  
  buffered->close();
  delete buffered;
  
  return 0;
}

void Cconfigurator::parse_line(char *line)
{
  char *p;
  char arg[MAX_STR_LEN];
  
  p = skip_spaces(line);

  /* empty line */
  if (strlen(p) == 0)
    return;

  /* comment */
  if (p[0] == '#')
    return;
  
  arg[0] = 0;
  p = one_argument(p, arg, 0);
  
  if (!*arg)
    return;
  
  if (strcmp(arg, "engine") == 0 ) {
      parse_engine(p);
  }
}
