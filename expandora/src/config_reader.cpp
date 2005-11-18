/* config file parser and reader */
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <qmutex.h>



#include "rooms.h"
#include "defines.h"
#include "struct.h"
#include "configurator.h"

#include "utils.h"
#include "renderer.h"
#include "config_reader.h"
#include "engine.h"
#include "dispatch.h"
#include "event.h"



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

  int open(char *fname);
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

int buffered_file::open(char *fname)
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
char * path = NULL;

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

   
ACMD(parse_localport);
ACMD(parse_remoteport);
ACMD(parse_remotehost);
ACMD(parse_basefile);
ACMD(parse_roomcolour);
ACMD(parse_autocheckexits);
ACMD(parse_autocheckterrain);
ACMD(parse_pattern);
ACMD(parse_mapperbriefmode);
ACMD(parse_exitspattern);
ACMD(parse_terraintype);
ACMD(parse_leadersetup);
ACMD(parse_debug);
ACMD(parse_include);
ACMD(parse_engine);
ACMD(parse_roomnamequote);
ACMD(parse_descquote);
ACMD(parse_autorefresh);
ACMD(parse_automerge);
ACMD(parse_texturesvisibility);
ACMD(parse_detailsvisibility);
ACMD(parse_angrylinker);

ACMD(update_localport);
ACMD(update_remoteport);
ACMD(update_remotehost);
ACMD(update_basefile);
ACMD(update_roomcolour);
ACMD(update_autocheckexits);
ACMD(update_autocheckterrain);
ACMD(update_pattern);
ACMD(update_mapperbriefmode);
ACMD(update_exitspattern);
ACMD(update_terraintype);
ACMD(update_leadersetup);
ACMD(update_debug);
ACMD(update_include);
ACMD(update_engine);
ACMD(update_roomnamequote);
ACMD(update_descquote);
ACMD(update_autorefresh);
ACMD(update_automerge);
ACMD(update_texturesvisibility);
ACMD(update_detailsvisibility);
ACMD(update_angrylinker);



struct config_commands_type {
  const char *command;
  void (*parse_command) (char *line);
  void (*update_command) (char *line);
    
};


const config_commands_type commands[] = {
  {"localport",         parse_localport, update_localport},
  {"remoteport",        parse_remoteport, update_remoteport},
  {"remotehost",        parse_remotehost, update_remotehost},
  {"basefile",          parse_basefile, update_basefile},
  {"roomcolour",        parse_roomcolour, update_roomcolour},
  {"autocheckexits",    parse_autocheckexits, update_autocheckexits},
  {"autocheckterrain",  parse_autocheckterrain, update_autocheckterrain},
  {"pattern",           parse_pattern, update_pattern},
  {"mapperbriefmode",   parse_mapperbriefmode, update_mapperbriefmode},
  {"exitspattern",      parse_exitspattern, update_exitspattern},
  {"terraintype",       parse_terraintype, update_terraintype},
  {"leadersetup",       parse_leadersetup, update_leadersetup},
  {"debug",             parse_debug, update_debug},
  {"include",           parse_include, update_include},
  {"engine",            parse_engine, update_engine},
  {"roomnamesimilarityquote",     parse_roomnamequote, update_roomnamequote},
  {"descsimilarityquote",         parse_descquote, update_descquote},
  {"autorefresh",       parse_autorefresh, update_autorefresh},
  {"automerge",			parse_automerge, update_automerge},
  {"texturesvisibilityrange",      parse_texturesvisibility, update_texturesvisibility},
  {"detailsvisibilityrange",       parse_detailsvisibility, update_detailsvisibility},
  {"angrylinker",       parse_angrylinker, update_angrylinker},
  {NULL, NULL, NULL}
};




void parse_line(char *line);


/* PARSE SECTION */


ACMD(parse_leadersetup)
{
  char *p;
  char arg[MAX_STR_LEN];
  
  GET_NEXT_ARGUMENT(p, line, arg, 0);
  
  dispatcher.set_leaderpatter(arg);
}


ACMD(parse_include)
{
  char *p;
  char arg[MAX_STR_LEN];
  class buffered_file *old_buffered_reader;

  
  GET_NEXT_ARGUMENT(p, line, arg, 0);
  
  old_buffered_reader = current_buffered_reader;
  parse_config(path, arg);
  current_buffered_reader = old_buffered_reader;
}

ACMD(parse_engine)
{
  char *p;
  char arg[MAX_STR_LEN];
  char cause_type, result_type;
  
  
  
  p = skip_spaces(line);                        
  if (!*p) {                                    
    CONFIG_ERROR("missing cause entry");
    return;                                     
  }                                             
  p = one_argument(p, arg, 2);         /* to upper case */

  
  cause_type = get_cause_type_by_line(arg);
    
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
  
  result_type = get_result_type_by_line(arg);
  if (result_type == -1) {
    CONFIG_ERROR("bad result type");
    return;                                     
  } 
  
  p = skip_spaces(p);                        
  if (!*p) {                                    
    CONFIG_ERROR("missing command line");
    return;                                     
  }                       

  if (engine_parse_command_line(cause_type, result_type, p) != 0) 
    CONFIG_ERROR("cant parse command");
}


ACMD(parse_debug)
{
  char *p;
  char arg[MAX_STR_LEN];
  int desired;
  int i;

  GET_NEXT_ARGUMENT(p, line, arg, 0);
  
  for (i = 0; debug_data[i].name; i++)
    if (is_abbrev(arg, debug_data[i].name) ) {
      
      GET_NEXT_ARGUMENT(p, p, arg, 0);
      
      desired = get_input_boolean(arg);
      if (desired == -1) {
        CONFIG_ERROR("bad boolean");
        return;                                     
      }
      
      debug_data[i].state = desired;
      printf("[Debug config] Setting %s to %s.\r\n", debug_data[i].desc, ON_OFF(debug_data[i].state));
      return;
    }
  
  CONFIG_ERROR("incorrect debug option");
}

ACMD(parse_pattern)
{
  char *p;
  char arg[MAX_STR_LEN];
  QString data;
  char marker, r;
    
  p = skip_spaces(line);                        
  if (!*p) {               
    CONFIG_ERROR("missing movement failures label");
    return;                                     
  }                                             
  p = one_argument(p, arg, 2); 
  
  r = get_result_type_by_line(arg);
  if (r != -1) {
      marker = 'R';
  } else {
    r = get_cause_type_by_line(arg);
    if (r == -1) {
      CONFIG_ERROR("wrong type of the pattern");
      return;                                     
    }                                             
    marker = 'C';
  }    
      
  p = skip_spaces(p);                        
  p = one_argument(p, arg, 0); 
  
  data = arg;
  
  p = skip_spaces(p);                        
  if (!*p) {                                 
    CONFIG_ERROR("missing pattern");
    return;                                     
  }                                             
  conf.add_pattern(p, data, marker, r);
  return;
      
}


ACMD(parse_terraintype)
{
  char *p;
  char arg[MAX_STR_LEN];
  struct room_sectors_data *terrain = new room_sectors_data;
  
  p = skip_spaces(line);                        
  if (!*p) {                                    
    CONFIG_ERROR("missing terrain description");
    delete terrain;
    return;                                     
  }                                             
  p = one_argument(p, arg, 2);          /* upper case */
  
  

  terrain->desc = (char *) malloc(strlen(arg)+1);
  strcpy(terrain->desc, arg);
  
  p = skip_spaces(p);                        
  if (!*p) {                                    
    CONFIG_ERROR("missing filename");
    free(terrain->desc);
    delete terrain; // things created with new should ALWAYS be destroyed with delete
    return;                                     
  }                                             
  p = one_argument(p, arg, 1);          /* get as is - file name */ 
  

  terrain->filename = (char *) malloc(strlen(arg)+strlen(path)+1);
  strcpy(terrain->filename, path);
  strcat(terrain->filename, arg);

  p = skip_spaces(p);                        
  if (!*p) {                                    
    CONFIG_ERROR("missing terrain marker");
    free(terrain->filename);
    delete terrain;
    return;                                     
  }                                             
  p = one_argument(p, arg, 1);          /* get as is */ 

  terrain->pattern = arg[0];
  
  terrain->next = room_sectors;
  room_sectors = terrain;
}


ACMD(parse_autocheckexits)
{
  char *p;
  char arg[MAX_STR_LEN];

  GET_NEXT_ARGUMENT(p, line, arg, 0);

  char c = get_input_boolean(arg);
    
  if (c == -1) {
    CONFIG_ERROR("bad boolean value");
    return;
  } else {
    conf.set_exits_check(c);
  }
  
  printf("Exits check is %s.\r\n", ON_OFF(conf.get_exits_check() ) );
}


ACMD(parse_automerge)
{
  char *p;
  char arg[MAX_STR_LEN];

  GET_NEXT_ARGUMENT(p, line, arg, 0);
  
  if (char c = get_input_boolean(arg) == -1) {
    CONFIG_ERROR("bad boolean value");
    return;
  } else 
    conf.set_automerge(c);
  
  printf("AutoMerge check is %s.\r\n", ON_OFF(conf.get_automerge() ) );
}


ACMD(parse_angrylinker)
{
  char *p;
  char arg[MAX_STR_LEN];

  GET_NEXT_ARGUMENT(p, line, arg, 0);

  if (char c = get_input_boolean(arg) == -1) {
    CONFIG_ERROR("bad boolean value");
    return;
  } else 
    conf.set_angrylinker(c);
  
  printf("AngryLinker is %s.\r\n", ON_OFF(conf.get_angrylinker() ) );
}

ACMD(parse_autocheckterrain)
{
  char *p;
  char arg[MAX_STR_LEN];

  GET_NEXT_ARGUMENT(p, line, arg, 0);
  
  if (char c = get_input_boolean(arg) == -1) {
    CONFIG_ERROR("bad boolean value");
    return;
  } else 
    conf.set_terrain_check(c);
  
  printf("Terrain check is %s.\r\n", ON_OFF(conf.get_terrain_check() ) );
}


ACMD(parse_autorefresh)
{
  char *p;
  char arg[MAX_STR_LEN];

  GET_NEXT_ARGUMENT(p, line, arg, 0);
  
  if (char c = get_input_boolean(arg) == -1) {
    CONFIG_ERROR("bad boolean value");
    return;
  } else 
    conf.set_autorefresh(c);
  
  printf("AutoRefresh is %s.\r\n", ON_OFF( conf.get_autorefresh() ) );
}


ACMD(parse_mapperbriefmode)
{
  char *p;
  char arg[MAX_STR_LEN];
  int i;
  
  
  GET_NEXT_ARGUMENT(p, line, arg, 0);
  
  i = get_input_boolean(arg);
  
  if (i == -1) {
    CONFIG_ERROR("bad boolean value");
    return;
  }
  
  conf.set_brief_mode(i);
  printf("MBriefMode is %s.\r\n", ON_OFF( conf.get_brief_mode() ) );
}  
  
ACMD(parse_localport)
{
  char *p;
  char arg[MAX_STR_LEN];
  
  GET_NEXT_ARGUMENT(p, line, arg, 0);
  
  conf.set_local_port(atoi(arg));
}


ACMD(parse_remoteport)
{
  char *p;
  char arg[MAX_STR_LEN];
  
  GET_NEXT_ARGUMENT(p, line, arg, 0);
  
  conf.set_remote_port(atoi(arg));
}

ACMD(parse_remotehost)
{
  char *p;
  char arg[MAX_STR_LEN];
  
  GET_NEXT_ARGUMENT(p, line, arg, 0);
  
  conf.set_remote_host(arg);
}



ACMD(parse_descquote)
{
  char *p;
  char arg[MAX_STR_LEN];
  
  GET_NEXT_ARGUMENT(p, line, arg, 0);
  
  conf.set_desc_quote(atoi(arg));
}

ACMD(parse_roomnamequote)
{
  char *p;
  char arg[MAX_STR_LEN];
  
  GET_NEXT_ARGUMENT(p, line, arg, 0);
  
  conf.set_roomname_quote(atoi(arg));
}

ACMD(parse_texturesvisibility)
{
  char *p;
  char arg[MAX_STR_LEN];
  
  GET_NEXT_ARGUMENT(p, line, arg, 0);
  
  conf.set_texture_vis(atoi(arg));
}

ACMD(parse_detailsvisibility)
{
  char *p;
  char arg[MAX_STR_LEN];
  
  GET_NEXT_ARGUMENT(p, line, arg, 0);
  
  conf.set_details_vis(atoi(arg));
}


ACMD(parse_exitspattern)
{
  char *p;
  char arg[MAX_STR_LEN];
  
  GET_NEXT_ARGUMENT(p, line, arg, 1);   /* get as is */
  
  printf("New exits pattern: \"%s\".\r\n", arg); 
  conf.set_exits_pat(arg);
}


ACMD(parse_basefile)
{
  char *p;
  char arg[MAX_STR_LEN];
  
  GET_NEXT_ARGUMENT(p, line, arg, 1); /* get as is */
  
  QString str = path;
  str += arg;
  conf.set_base_file(str);
}

ACMD(parse_roomcolour)
{
  char *p;
  char arg[MAX_STR_LEN];
  
  GET_NEXT_ARGUMENT(p, line, arg, 1);   /* as is */
  
  conf.set_look_col(arg);
  
  GET_NEXT_ARGUMENT(p, p, arg, 1);      /* as is */

  //strcpy(roomname_end, arg);
  
  printf("Roomname colour check: %sRoomname colour example%s.\r\n", 
        (const char *) conf.get_look_col(), (const char *) conf.get_end_col());
}

int parse_config(char * in_path, char *fn)
{
  char * old_path = path;
  path = in_path;
  char line[MAX_STR_LEN];
  class buffered_file *buffered;
  
  
  buffered = new class buffered_file;

  char * filename = (char *) malloc(strlen(fn)+strlen(path)+1);
  strcpy(filename, path);
  strcat(filename, fn);  

  printf("Loading config file %s\r\n", filename);

  current_buffered_reader = buffered;
  
  if (buffered->open(filename) != 0) 
  {
    printf("ERROR: Cant read config file %s!\r\n", filename);
    return -1;
  }
  
  /* buffered reading and line parsing cycle */
  do {
    buffered->readln(line, MAX_STR_LEN);
    parse_line(line);
  } while (!buffered->eof());
  
  
  buffered->close();
  delete buffered;
  
  free(filename);
  path = old_path;
  return 0;
}

void parse_line(char *line)
{
  char *p;
  int i;
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
  
  for (i = 0; commands[i].command != NULL; i++) {
    if (strcmp(arg, commands[i].command) == 0 && commands[i].parse_command !=NULL ) 
    {
      ((*commands[i].parse_command) (p));
    }
  }
  
  
}




/* ------------- UPDATE SECTION ---------------- */

ACMD(update_localport) 
{
    
}

ACMD(update_remoteport)
{
    
}

ACMD(update_remotehost)
{
    
}

ACMD(update_basefile)
{
    
}

ACMD(update_roomcolour)
{
    
}

ACMD(update_autocheckexits)
{
    
}

ACMD(update_autocheckterrain)
{
    
}

ACMD(update_pattern)
{
    
}

ACMD(update_mapperbriefmode)
{
    
}

ACMD(update_exitspattern)
{
    
}

ACMD(update_terraintype)
{
    
}

ACMD(update_leadersetup)
{
    
}

ACMD(update_debug)
{
    
}

ACMD(update_include)
{
    
}

ACMD(update_engine)
{
    
}

ACMD(update_roomnamequote)
{
    
}

ACMD(update_descquote)
{
    
}

ACMD(update_autorefresh)
{
    
}

ACMD(update_automerge)
{
    
}

ACMD(update_texturesvisibility)
{
    
}

ACMD(update_detailsvisibility)
{
    
}

ACMD(update_angrylinker)
{
    
}
