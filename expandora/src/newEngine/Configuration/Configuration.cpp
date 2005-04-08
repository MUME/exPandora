/* config file parser and reader */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <qmutex.h>
#include <qstring.h>

#include "Terrain.h"
#include "Configuration.h"

Configuration::Configuration() {

}

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








ACMD(parse_include)
{
  char *p;
  char arg[MAX_STR_LEN];
  class buffered_file *old_buffered_reader;

  
  GET_NEXT_ARGUMENT(p, line, arg, 0);
  
  old_buffered_reader = buffered;
  parse_config(arg);
  buffered = old_buffered_reader;
}








ACMD(parse_terraintype)
{
  char *p;
  char arg[MAX_STR_LEN];

  Terrain * terrain = new Terrain();

  
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
  
  terrains.insert(make_pair(arg[0], terrain));
  terrainIDs.insert(make_pair(QString::fromAscii(terrain->desc), arg[0]));  
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
  
  desc_quote = (atoi(arg));
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




ACMD(parse_basefile)
{
  char *p;
  char arg[MAX_STR_LEN];
  
  GET_NEXT_ARGUMENT(p, line, arg, 1); /* get as is */
  
  strcpy(base_file, arg);
}



ACMD(parse_characterTable)
{
  char *p;
  char arg[MAX_STR_LEN];
  
  GET_NEXT_ARGUMENT(p, line, arg, 1);   /* as is */
  
  characterTable_file = arg;
	
}

int Configuration::parse_config(char *filename)
{
  char line[MAX_STR_LEN];
  
  
  buffered = new buffered_file;
  
  printf("Loading config file %s\r\n", filename);

  
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

void Configuration::parse_line(char *line)
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
	(this->*(commands[i].command_pointer)) (p);
      }
  }
  
  
}


char * Configuration::skip_spaces(char *str)
{
  while (isspace(*str) && (*str != 0))
    str++;
  return str;
}



/* mode 0 - lower all chars in argument */
/* mode 1 - do not lower all chars in argument */
/* mode 2 - upper all chars in argument */
char * Configuration::one_argument(char *argument, char *first_arg, int mode)
{

  if (!argument) {
    *first_arg = '\0';
    return (NULL);
  }

  while (*argument && !isspace(*argument)) {
    if (mode == 0) {
      *(first_arg++) = LOWER(*argument);
    } else if (mode == 1) {
      *(first_arg++) = *argument;
    } else if (mode == 2) {
      *(first_arg++) = UPPER(*argument);
    }
    argument++;
  }
  
  *(first_arg) = '\0';

  return (argument);
}
