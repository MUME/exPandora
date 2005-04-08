/* config file reader and parcer - header file */

#define BUFFER_SIZE 4096
#define MAX_STR_LEN     400

#define LOWER(c)   (((c)>='A'  && (c) <= 'Z') ? ((c)+('a'-'A')) : (c))
#define UPPER(c)   (((c)>='a'  && (c) <= 'z') ? ((c)+('A'-'a')) : (c) )

#define ACMD(name)  void Configuration::name(char *line)

#define CONFIG_ERROR(message)				\
  printf("-!!- %s:%i: Error : %s.\r\n",			\
	 buffered->get_filename(),	\
	 buffered->get_linenum(),	\
	 message); 



#define GET_NEXT_ARGUMENT(p, line, argument, mode)	\
  p = skip_spaces(line);				\
  if (!*p) {						\
    CONFIG_ERROR("missing argument")			\
      return;						\
  }							\
  p = one_argument(p, arg, mode); 


class Configuration;
struct config_commands_type {
  const char *command;
  void (Configuration::*command_pointer) (char *line);
};


class buffered_file {

 public:
  buffered_file();    

  int open(char *fname);
  int readln(char *line, int maxlen);
  int get_linenum();

  void close() { fclose(f); };
  int eof();
  char *get_filename();

 private:
  FILE *f; 

  char filename[MAX_STR_LEN];
  char buffered_buffer[BUFFER_SIZE];
  int buffered_amount;
  int buffered_pos; /* current position in buffer */

  int line_number;

};



class Configuration {

 public:
  Configuration();
  int parse_config(char *filename);
  // possibility to pass the parsed things needed
 private:
  int details_visibility_range;
  int texture_visibilit_range;
  int desc_quote;
  int local_port;
  int remote_port;
  char *  characterTable_file;
  char * base_file;
  char * remote_host;
  buffered_file * buffered;
  char * skip_spaces(char * str);
  void parse_line(char *line);
  char * one_argument(char *argument, char *first_arg, int mode);

  ACMD(parse_localport);
  ACMD(parse_remoteport);
  ACMD(parse_remotehost);
  ACMD(parse_basefile);
  ACMD(parse_terraintype);
  ACMD(parse_include);
  ACMD(parse_descquote);
  ACMD(parse_texturesvisibility);
  ACMD(parse_detailsvisibility);
  ACMD(parse_characterTable);
  
  static const config_commands_type commands[];
};






const config_commands_type Configuration::commands[] = {
  {"localport",         &Configuration::parse_localport},
  {"remoteport",        &Configuration::parse_remoteport},
  {"remotehost",        &Configuration::parse_remotehost},
  {"basefile",          &Configuration::parse_basefile},
  {"terraintype",       &Configuration::parse_terraintype},
  {"include",           &Configuration::parse_include},
  {"descsimilarityquote",         &Configuration::parse_descquote},
  {"texturesvisibilityrange",      &Configuration::parse_texturesvisibility},
  {"detailsvisibilityrange",       &Configuration::parse_detailsvisibility},
  {"characterTable",	&Configuration::parse_characterTable},
  {NULL, NULL}
};



#ifdef DMALLOC
#include <dmalloc.h>
#endif
