#define E_NOEXIT        0
#define E_NORMAL        1
#define E_OPENDOOR      2
#define E_CLOSEDDOOR    3
#define E_PORTAL        4




int compare_exits(CRoom *p, int exits[]);
void parse_exits(const char *exits_line, int exits[]);
void do_exits(const char *exits_line);

