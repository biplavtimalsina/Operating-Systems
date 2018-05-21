
extern int MAXCOM; // max number of letters to be supported
extern int MAXLIST; // max number of commands to be supported
extern int status;


int takeInput(char* str);
static char* skipwhite(char* s);
int split(char* cmd,char *args[], int i);
void process(char *inputString);
void displayPrompt();