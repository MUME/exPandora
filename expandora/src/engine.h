


extern QMutex analyzer_mutex;
extern char    engine_config[];


void engine();                          /* main entry point */
void engine_init();
int engine_parse_command_line(char cause, char result, char *line);

int check_roomdesc();
void angrylinker(struct Troom *r);
