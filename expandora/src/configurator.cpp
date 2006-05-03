

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
    set_always_on_top(true);
    
    set_details_vis(500);
    set_texture_vis(300);
    
    set_name_quote(10);
    set_desc_quote(10);

    
    
    /* colours */
    
    /* patterns */
    set_exits_pat("Exits:");
    
    struct room_sectors_data first;
        
    first.pattern = 0;
    first.desc = "NONE";
    first.texture = 1;
    first.gllist = 1;
    sectors.push_back(first);
    
    spells_pattern = "Affected by:";
}


void Cconfigurator::reset_current_config()
{
    sectors.clear();
    spells.clear();
    
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

/* --------------------------------------- spells ----------------------------------------- */
void Cconfigurator::add_spell(QByteArray spellname, QByteArray up, QByteArray down, QByteArray refresh, bool addon)
{
    TSpell spell;
    
    spell.name = spellname;
    spell.up_mes = up;
    spell.down_mes = down;
    spell.refresh_mes = refresh;
    spell.addon = addon;
    spell.up = false;
    
    spells.push_back(spell);
    set_conf_mod(true);
}

void Cconfigurator::add_spell(TSpell spell)
{
    spells.push_back(spell);
    set_conf_mod(true);
}

QString Cconfigurator::spell_up_for(unsigned int p)
{
    QString s;
    int min;
    int sec;
    
    if (p > spells.size())
        return "";
        
    sec = spells[p].timer.elapsed() / (1000);
    min = sec / 60;
    sec = sec % 60;    
        
    s = QString("- %1%2:%3%4")
            .arg( min / 10 )
            .arg( min % 10 )
            .arg( sec / 10 )
            .arg( sec % 10 );

    return s;
}


/* ----------------- REGULAR EXPRESSIONS SECTION ---------------- */
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
    return 0;
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

void Cconfigurator::set_always_on_top(bool b)
{
    always_on_top = b;
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
    if (p->filename == "")
        return -1;
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
                  get_remote_port() );
  fprintf(f, "  <basefile filename=\"%s\">\r\n", 
                  (const char *) get_base_file() );
  
  fprintf(f, "  <GLvisibility textures=\"%i\" details=\"%i\">\r\n", 
                  get_texture_vis(),  get_details_vis() );
  
  fprintf(f, "  <analyzers desc=\"%s\" exits=\"%s\"  terrain=\"%s\">\r\n", 
                  "ON", ON_OFF(get_exits_check() ), ON_OFF(get_terrain_check() ) );

  fprintf(f, "  <engineflags briefmode=\"%s\" automerge=\"%s\"  angrylinker=\"%s\">\r\n", 
                  ON_OFF(get_brief_mode()), 
                  ON_OFF(get_automerge() ), ON_OFF( get_angrylinker()) );

  fprintf(f, "  <guisettings always_on_top=\"%s\">\r\n", 
                  ON_OFF(get_always_on_top()) );

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
  
  for (i = 0; i < spells.size(); i++) {
        fprintf(f, "  <spell addon=\"%s\" name=\"%s\" up=\"%s\" refresh=\"%s\" down=\"%s\">\r\n",
                    YES_NO(spells[i].addon), 
                    (const char *) spells[i].name,
                    (const char *) spells[i].up_mes,
                    (const char *) spells[i].refresh_mes,
                    (const char *) spells[i].down_mes);
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


/*bool ConfigParser::endElement( const QString& , const QString& , const QString&)
{
    return TRUE;
}
*/
/*bool ConfigParser::characters( const QString& ch)
{
  return TRUE;
} 
*/

bool ConfigParser::startElement( const QString& , const QString& , 
                                    const QString& qName, 
                                    const QXmlAttributes& attributes)
{
    if (qName == "localport") {
        if (attributes.length() < 1) {
            printf("(localport token) Not enough attributes in XML file!");
            exit(1);
        }        
        
        s = attributes.value("port");
        conf.set_local_port(s.toInt() );
//        printf("Using local port %i. \r\n", conf.get_local_port() );

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
//        printf("Using remote host %s:%i\r\n", (const char *)conf.get_remote_host(), 
//                                            conf.get_remote_port() );

        return TRUE;
    } else if (qName == "basefile") {
        if (attributes.length() < 1) {
            printf("(basefile token) Not enough attributes in XML file!");
            exit(1);
        }        
        
        s = attributes.value("filename");
        conf.set_base_file(s.toAscii() );
//        printf("Using the database file: %s\r\n", qPrintable(s) );
        
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
    } else if (qName == "guisettings") {
        if (attributes.length() < 1) {
            printf("(guisettings token) Not enough attributes in XML file!");
            exit(1);
        }        
        
        s = attributes.value("always_on_top");
        s = s.toLower();
        if (s == "on") 
            conf.set_always_on_top(true);
        else 
            conf.set_always_on_top(false);

        printf("GUI settings: always_on_top %s.\r\n", ON_OFF(conf.get_always_on_top()) );

        return TRUE;
    } else if (qName == "engineflags") {
        if (attributes.length() < 3) {
            printf("(engineflags token) Not enough attributes in XML file!");
            exit(1);
        }        
        
        s = attributes.value("briefmode");
        s = s.toLower();
//        printf("The brief mode setting : %s\r\n", qPrintable(s) );
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
    } else if (qName == "spell") {
        if (attributes.length() < 4) {
            printf("(pattern token) Not enough attributes in XML file!");
            exit(1);
        }        

        spell.up = false;
        s = attributes.value("addon");
        s = s.toLower();
        spell.addon = false;
        if (s == "yes") 
          spell.addon = true;
        
        
        s = attributes.value("name");
        spell.name = s.toAscii();
        
        s = attributes.value("up");
        spell.up_mes = s.toAscii();

        s = attributes.value("refresh");
        spell.refresh_mes = s.toAscii();

        s = attributes.value("down");
        spell.down_mes = s.toAscii();

        conf.add_spell(spell);
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

