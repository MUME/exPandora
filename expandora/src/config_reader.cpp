/* config file parser and reader */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defines.h"

#include <qmutex.h>


#include "struct.h"
#include "CharacterTable.h"
#include "utils.h"
#include "event.h"
#include "dispatch.h"
#include "rooms.h"
#include "renderer.h"
#include "config_reader.h"
#include "engine.h"

#ifdef NEW_ENGINE
#include "Terrain.h"
#include <qstring.h>
//#include <pair>
//using namespace std;
#endif

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
ACMD(parse_movementfailure);
ACMD(parse_mapperbriefmode);
ACMD(parse_exitspattern);
ACMD(parse_terraintype);
ACMD(parse_leadersetup);
ACMD(parse_logfile);
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
ACMD(parse_characterTable);
  
struct config_commands_type {
  const char *command;
  void (*command_pointer) (char *line);
};


const config_commands_type commands[] = {
  {"localport",         parse_localport},
  {"remoteport",        parse_remoteport},
  {"remotehost",        parse_remotehost},
  {"basefile",          parse_basefile},
  {"roomcolour",        parse_roomcolour},
  {"autocheckexits",    parse_autocheckexits},
  {"autocheckterrain",  parse_autocheckterrain},
  {"movementfailure",   parse_movementfailure},
  {"mapperbriefmode",   parse_mapperbriefmode},
  {"exitspattern",      parse_exitspattern},
  {"terraintype",       parse_terraintype},
  {"leadersetup",       parse_leadersetup},
  {"logfile",           parse_logfile},
  {"debug",             parse_debug},
  {"include",           parse_include},
  {"engine",            parse_engine},
  {"roomnamesimilarityquote",     parse_roomnamequote},
  {"descsimilarityquote",         parse_descquote},
  {"autorefresh",       parse_autorefresh},
  {"automerge",			parse_automerge},
  {"texturesvisibilityrange",      parse_texturesvisibility},
  {"detailsvisibilityrange",       parse_detailsvisibility},
  {"angrylinker",       parse_angrylinker},
  {"characterTable",	parse_characterTable},
  
  {NULL, NULL}
};

void parse_line(char *line);





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
  parse_config(arg);
  current_buffered_reader = old_buffered_reader;
}

ACMD(parse_engine)
{
  char *p;
  char arg[MAX_STR_LEN];
  int i;
  char cause_type, result_type;
  
  
  
  p = skip_spaces(line);                        
  if (!*p) {                                    
    CONFIG_ERROR("missing cause entry");
    return;                                     
  }                                             
  p = one_argument(p, arg, 2);         /* to upper case */

  
  cause_type = -1;
  for (i = 0; event_types[i].name; i++) 
    if ( (event_types[i].group == E_CAUSE) &&               
         (strcmp(arg, event_types[i].name) == 0) ) 
    {
      cause_type = event_types[i].type;
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
  for (i = 0; event_types[i].name; i++) 
    if ( (event_types[i].group == E_RESULT) &&               
         (strcmp(arg, event_types[i].name) == 0) ) 
    {
      result_type = event_types[i].type;
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

ACMD(parse_movementfailure)
{
  char *p;
  char arg[MAX_STR_LEN];
  struct TFailureData *failure;
  
  p = skip_spaces(line);                        
  if (!*p) {               
    CONFIG_ERROR("missing movement failures label");
    return;                                     
  }                                             
  p = one_argument(p, arg, 0); 
  
  failure = new TFailureData;
  
  failure->data = (char *) malloc(strlen(arg)+1);
  strcpy(failure->data, arg);
  
  p = skip_spaces(p);                        
  if (!*p) {                                 
    CONFIG_ERROR("missing pattern");
    free(failure->data);
    free(failure);
    return;                                     
  }                                             
  
  failure->pattern = (char *) malloc(strlen(p)+1);
  strcpy(failure->pattern, p);
  
  failure->type = R_FAIL;
  
  failure->next = failure_data;
  
  failure_data = failure;
}


ACMD(parse_terraintype)
{
  char *p;
  char arg[MAX_STR_LEN];
#ifndef NEW_ENGINE
  struct room_sectors_data *terrain = new room_sectors_data;
#else
  Terrain * terrain = new Terrain();
#endif
  
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
  

  terrain->filename = (char *) malloc(strlen(arg)+1);
  strcpy(terrain->filename, arg);

  p = skip_spaces(p);                        
  if (!*p) {                                    
    CONFIG_ERROR("missing terrain marker");
    free(terrain->filename);
    delete terrain;
    return;                                     
  }                                             
  p = one_argument(p, arg, 1);          /* get as is */ 

  terrain->pattern = arg[0];
  
#ifndef NEW_ENGINE
  terrain->next = room_sectors;
  
  room_sectors = terrain;
#else 
  terrains.insert(make_pair(arg[0], terrain));
  terrainIDs.insert(make_pair(QString::fromAscii(terrain->desc), arg[0]));
#endif
  
  
}


ACMD(parse_autocheckexits)
{
  char *p;
  char arg[MAX_STR_LEN];

  GET_NEXT_ARGUMENT(p, line, arg, 0);

  engine_flags.exits_check = get_input_boolean(arg);
  if (engine_flags.exits_check == -1) {
    CONFIG_ERROR("bad boolean value");
    engine_flags.exits_check = FALSE;
    return;
  }
  
  printf("Exits check is %s.\r\n", ON_OFF(engine_flags.exits_check) );
}


ACMD(parse_automerge)
{
  char *p;
  char arg[MAX_STR_LEN];

  GET_NEXT_ARGUMENT(p, line, arg, 0);
  
  engine_flags.automerge  = get_input_boolean(arg);
  if (engine_flags.automerge == -1) {
    CONFIG_ERROR("bad boolean value");
    engine_flags.automerge = TRUE;
    return;
  }
  
  printf("AutoMerge check is %s.\r\n", ON_OFF(engine_flags.automerge) );
}


ACMD(parse_angrylinker)
{
  char *p;
  char arg[MAX_STR_LEN];

  GET_NEXT_ARGUMENT(p, line, arg, 0);
  
  engine_flags.angrylinker = get_input_boolean(arg);
  if (engine_flags.angrylinker == -1) {
    CONFIG_ERROR("bad boolean value");
    engine_flags.angrylinker = FALSE;
    return;
  }
  
  printf("AngryLinker is %s.\r\n", ON_OFF(engine_flags.angrylinker) );
}

ACMD(parse_autocheckterrain)
{
  char *p;
  char arg[MAX_STR_LEN];

  GET_NEXT_ARGUMENT(p, line, arg, 0);
  
  engine_flags.terrain_check= get_input_boolean(arg);
  if (engine_flags.terrain_check == -1) {
    CONFIG_ERROR("bad boolean value");
    engine_flags.terrain_check = TRUE;
    return;
  }
  
  printf("Terrain check is %s.\r\n", ON_OFF(engine_flags.terrain_check) );
}


ACMD(parse_autorefresh)
{
  char *p;
  char arg[MAX_STR_LEN];

  GET_NEXT_ARGUMENT(p, line, arg, 0);
  
  engine_flags.autorefresh = get_input_boolean(arg);
  if (engine_flags.autorefresh == -1) {
    CONFIG_ERROR("bad boolean value");
    engine_flags.autorefresh = TRUE;
    return;
  }
  
  printf("AutoRefresh is %s.\r\n", ON_OFF(engine_flags.autorefresh) );
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
  
  dispatcher.set_brief_mode(i);
  printf("MBriefMode is %s.\r\n", ON_OFF( dispatcher.get_brief_mode() ) );
}  
  
ACMD(parse_localport)
{
  char *p;
  char arg[MAX_STR_LEN];
  
  GET_NEXT_ARGUMENT(p, line, arg, 0);
  
  local_port = atoi(arg);
}


ACMD(parse_remoteport)
{
  char *p;
  char arg[MAX_STR_LEN];
  
  GET_NEXT_ARGUMENT(p, line, arg, 0);
  
  remote_port = atoi(arg);
}

ACMD(parse_remotehost)
{
  char *p;
  char arg[MAX_STR_LEN];
  
  GET_NEXT_ARGUMENT(p, line, arg, 0);
  
  strcpy(remote_host, arg);
}



ACMD(parse_descquote)
{
  char *p;
  char arg[MAX_STR_LEN];
  
  GET_NEXT_ARGUMENT(p, line, arg, 0);
  
  comparator.set_desc_quote(atoi(arg));
}

ACMD(parse_roomnamequote)
{
  char *p;
  char arg[MAX_STR_LEN];
  
  GET_NEXT_ARGUMENT(p, line, arg, 0);
  
  comparator.set_roomname_quote(atoi(arg));
}

ACMD(parse_texturesvisibility)
{
  char *p;
  char arg[MAX_STR_LEN];
  
  GET_NEXT_ARGUMENT(p, line, arg, 0);
  
  texture_visibilit_range = atoi(arg);
}

ACMD(parse_detailsvisibility)
{
  char *p;
  char arg[MAX_STR_LEN];
  
  GET_NEXT_ARGUMENT(p, line, arg, 0);
  
  details_visibility_range = atoi(arg);
}


ACMD(parse_exitspattern)
{
  char *p;
  char arg[MAX_STR_LEN];
  
  GET_NEXT_ARGUMENT(p, line, arg, 1);   /* get as is */
  
  printf("New exits pattern: \"%s\".\r\n", arg); 
  strcpy(exits_pattern, arg);
}


ACMD(parse_basefile)
{
  char *p;
  char arg[MAX_STR_LEN];
  
  GET_NEXT_ARGUMENT(p, line, arg, 1); /* get as is */
  
  strcpy(base_file, arg);
}

ACMD(parse_logfile)
{
  char *p;
  char arg[MAX_STR_LEN];
  
  GET_NEXT_ARGUMENT(p, line, arg, 1); /* get as is */
  
  strcpy(logfile_name, arg);
}


ACMD(parse_roomcolour)
{
  char *p;
  char arg[MAX_STR_LEN];
  
  GET_NEXT_ARGUMENT(p, line, arg, 1);   /* as is */
  
  strcpy(roomname_start, arg);
  
  GET_NEXT_ARGUMENT(p, p, arg, 1);      /* as is */

  strcpy(roomname_end, arg);
  
  printf("Roomname colour check: %sRoomname colour example%s.\r\n", roomname_start, roomname_end);
}

ACMD(parse_characterTable)
{
	char *p;
	char arg[MAX_STR_LEN];
  
	GET_NEXT_ARGUMENT(p, line, arg, 1);   /* as is */
  
	strcpy(characterTable_file, arg);
	
}

int parse_config(char *filename)
{
  char line[MAX_STR_LEN];
  class buffered_file *buffered;
  
  
  buffered = new class buffered_file;
  
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
    if (strcmp(arg, commands[i].command) == 0 && commands[i].command_pointer !=NULL ) 
    {
      ((*commands[i].command_pointer) (p));
    }
  }
  
  
}
