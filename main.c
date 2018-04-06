///
/// @file main.c
///
/// Betriebssysteme Myfind main File
/// Beispiel 1
///
/// @author Ibrahim Milli <ic17b063@technikum-wien.at>
/// @author Dominic Ernstbrunner <ic17b015@technikum-wien.at>
/// @author Florian Harold <ic17b093@technikum-wien.at>
/// @date 2018/04/06
///
/// @version 202
///

/// -------------------------------------------------------------- includes --

#include <stdio.h>
#include <unistd.h>     /* for chdirect */
#include <stdlib.h>     /* for free */
#include <errno.h>      /* for errno */
//#include <sys/types.h>  /* for opendir */
#include <dirent.h>     /* for opendir */
#include <string.h>    /* for strerror() */
#include <memory.h>     /* for strerror() */
#include <sys/stat.h>
#include<fnmatch.h>
#include<pwd.h>
#include<time.h>

/// --------------------------------------------------------------- globals --

enum Action {notdeclared, noaction, user, name, type, print, ls, nouser, path};
enum Error {wrongnumberofarg, wrongarg, tolongarg, notdeclaredarg, nofileorpath, eerrno,wronguser,wrongname};

/// ------------------------------------------------------------- functions --

int do_file (/*DIR *pDIR,*/  char *pPATH,enum Action *action, char* pArrArgument[],char* file_name);
DIR *do_dir ( char *pPATH, enum Action *action,char* pArrArgument[], int path_type);
int resolve_relpath(char* pPATH,int count,enum Action *action,char* pArrArgument[]);
char *func_check_path(char* pPATH);
enum Action func_check_action(char *pACTION);
void func_check_arguments(char *pARGUMENTS[], int COUNTER, enum Action *pArrAction, char *pArrArgument[]);
char *func_print_action(enum Action eAction);
int func_type(char* pARG, struct stat FILE);
int func_nouser(struct stat FILE);
void print_ls(struct stat FILE,char* file_name);
int check_print_user(struct stat FILE,char*arg);
char* getuser(struct stat FILE);
char* getgr(struct stat FILE);
void func_error_expression(enum Error eErrorcode, enum Action eAction, char* arg);

/// MyFind
///
/// This program is similar the same as the find function.
/// You can use following options:
///
/// -user <name>/<uid> <br>
/// -name <pattern> <br>
/// -type [bcdpfls] <br>
/// -print <br>
/// -ls <br>
/// -nouser <br>
/// -path <pattern> <br>
///
/// \param argc the number of arguments
/// \param argv the arguments itselves (including the program name in argv[0])
/// \return always "success"
/// \retval 0 always

int main(int argc, char *argv[]){
    char *pMainPath = NULL;     //Path for the find functions
    enum Action pArrMainAction[argc];
    char *pArrMainArgument[argc];

    func_check_arguments(argv,argc,pArrMainAction,pArrMainArgument);

    pMainPath = func_check_path(argv[1]);

    resolve_relpath(pMainPath,argc,pArrMainAction,pArrMainArgument);

    return 0;
}

/// Function that checks every argument
///
/// This function checks every argument
/// If an argument is wrong the program interrupt with an error handling
///
/// \param pARGUMENTS char array to check
/// \param COUNTER length of the array
/// \param pArrAction array of enum Action that is given back
/// \param pArrArgument array of char of patterns to the Actions that is given back

void func_check_arguments(char *pARGUMENTS[], int COUNTER, enum Action *pArrAction, char *pArrArgument[]){
    enum Action etAction = notdeclared;
    int i = 1;
    int j = 0;
    char bad_chars[]="!@%~|";
    unsigned int  z;
    //1) Check if the first argument is the path or a option

    if(func_check_action(pARGUMENTS[i]) == notdeclared){   //if it is not declared --> check if it is a path

        i++;
    }
    for(;i<=COUNTER;i++, j++){
        etAction = func_check_action(pARGUMENTS[i]);
        pArrAction[j] = etAction; //Action Array
        //Check if there is an argument for the specified options
        if(etAction == user || etAction == name || etAction == type || etAction == path){
            if(pARGUMENTS[++i]==NULL) {
                func_error_expression(wrongnumberofarg, etAction, "");
            }
            else{
                if(etAction == type && strlen(pARGUMENTS[i])>1) {
                    func_error_expression(tolongarg, etAction, "");
                }
                if(etAction == name ){
                    for(z=0;z<strlen(bad_chars);++z){
                        if(strchr(pARGUMENTS[i],bad_chars[z])!=NULL){
                            func_error_expression(wrongname,name,pARGUMENTS[i]);
                        }
                    }
                }
                pArrArgument[j] = pARGUMENTS[i]; //Argument Array
            }
        }
        else if(etAction == notdeclared){
            func_error_expression(notdeclaredarg,etAction,pARGUMENTS[i]);
        }
    }
}

/// Function that checks the path
///
/// This function checks the path and returns them.
/// If the path is an action, the working directory is returned.
/// If the path is also one, give it back
/// If the path is not able to open -> error handling
///
/// \param pPATH path to check
/// \return ".": if the path is an action
/// \return "path": if the path is correct

char *func_check_path(char *pPATH){
    //declaration
    //pt --- pointer temporary

    unsigned int  i;

    int length=strlen(pPATH)+1;
    char temp[length];
    int j=0;


   for(i=0;i<strlen(pPATH);i++) {
        if (pPATH[i] == 92 || pPATH[i] == '\0') {

            continue;
        }
     temp[j++] = pPATH[i];
    }
    temp[j]='\0';
    strcpy(pPATH,temp);

    return pPATH;
}

/// Function that checks the action
///
/// Returns an enum Action addicted to the Actionstring
///
/// \param pAction Actionstring to check
/// \return enum Action

enum Action func_check_action(char *pACTION){
    if(pACTION == NULL)
        return noaction;
    else if(strcmp(pACTION,"-user")==0)
        return user;
    else if(strcmp(pACTION,"-name")==0)
        return name;
    else if(strcmp(pACTION,"-type")==0)
        return type;
    else if(strcmp(pACTION,"-print")==0)
        return print;
    else if(strcmp(pACTION,"-ls")==0)
        return ls;
    else if(strcmp(pACTION,"-nouser")==0)
        return nouser;
    else if(strcmp(pACTION,"-path")==0)
        return path;
    else
        return notdeclared;
}

/// Function that give back the action as string
///
/// \param Action which should be converted to a string
/// \return characterstring of the Action

char *func_print_action(enum Action eAction){
    switch(eAction){
        case notdeclared: return "not declared";
        case noaction: return "no action";
        case user: return "-user";
        case name: return "-name";
        case type: return "-type";
        case print: return "-print";
        case ls: return "-ls";
        case nouser: return "-nouser";
        case path: return "-path";
        default: return "FUNC_PRINT_ACTION: INVALID OPERATION!"; // |DEr| close program with an error?
    }
}

/// Function checks if the file has the right type
///
/// This function checks if the type of the file is equal with pARG
///
/// \param pARG type to be conform with the file
/// \param FILE which should be checked
///
/// \retval 1 correct type
/// \retval 0 incorrect type

int func_type(char* pARG, struct stat FILE){
    int t_return = 0;
    if (S_ISREG(FILE.st_mode) && !strcmp(pARG,"f")) //regular file
        t_return = 1;
    else if (S_ISDIR(FILE.st_mode) && !strcmp(pARG,"d")) //directory
        t_return = 1;
    else if (S_ISCHR(FILE.st_mode) && !strcmp(pARG,"c")) //character device
        t_return = 1;
    else if (S_ISBLK(FILE.st_mode) && !strcmp(pARG,"b")) //block device
        t_return = 1;
    else if (S_ISFIFO(FILE.st_mode) && !strcmp(pARG,"p")) //FIFO (named pipe)
        t_return = 1;
    else if (S_ISLNK(FILE.st_mode) && !strcmp(pARG,"l")) //symbolic link
        t_return = 1;
    else if (S_ISSOCK(FILE.st_mode) && !strcmp(pARG,"s")) //socket
        t_return = 1;
    return t_return;
}

/// Function checks if the file has an user
///
/// Checks if no user corresponds to file's user id
///
/// \param FILE that is checked
///
/// \retval 1 no user found
/// \retval 0 has an user

int func_nouser(struct stat FILE){
    int t_return = 0;
    if(!getpwuid(FILE.st_uid)){
        t_return = 1;
    }
    return t_return;
}

/// Function to check the files
///
/// This function checks every argument. Everyone of them had to be correct to print the file.
/// Is one argument not conform to the file jump to the next one.
///
/// \param pPATH Current file path
/// \param action An array with the actions, they should be checked
/// \param pArrArgument An array with the additions to the actions
/// \param file name Current name of the file
///
/// \return every time NULL

int do_file (/*DIR *pDIR, */char* pPATH, enum Action *action, char* pArrArgument[],char* file_name){

    //declaration of the variables
    struct stat file;
    int i=0;
    int check=0;
    int ls_help=0;

    if (lstat(pPATH, &file) == -1) {
        func_error_expression(eerrno, notdeclared, pPATH);
    }
    while(action[i]!=notdeclared){
        if (action[i] == type) {
            if (func_type(pArrArgument[i], file)) {
                printf("\n%s", pPATH);
            }else{
               func_error_expression(wrongarg,type,pArrArgument[i]);
            }
        } else if (action[i] == nouser) {
            if (func_nouser(file)) {
                printf("\n---------------------------------\n%s", pPATH);
            }
        } else if (action[i] == name) {
            if (!(fnmatch(pArrArgument[i], file_name, FNM_PATHNAME))) {
                check++;
            }
        } else if ((action[i] == print) || (action[i] == noaction)) {
            check++;
        } else if (action[i] == user) {
            if (check_print_user(file, pArrArgument[i])) {
                check++;
            }
        } else if (action[i] == ls) {
            ls_help=1;
            check++;
        } else if (action[i] == path) {
            if ((strcmp(pPATH, pArrArgument[i]) == 0)) {
                check++;
            }
        }
    i++;
    }
    if((check==i) && (ls_help==0)){
        printf("\n%s", pPATH);
    }else if((check==i) && (ls_help==1)){
        print_ls(file, pPATH);
    }
    check=0;
    i=0;
    ls_help=0;
    if (S_ISDIR(file.st_mode)) {
        do_dir(pPATH, action, pArrArgument,1);
    }
return 0;
}

/// Function to open a dictionary
///
/// This function opens the different dictionary (exeption: "." and "..") and
/// generates the relative path for the next functions
///
/// \param pPATH Current file path
/// \param action An array with the actions, they should be checked
/// \param pArrArgument An array with the additions to the actions
///
/// \return every time NULL

DIR *do_dir ( char *pPATH, enum Action *action,char* pArrArgument[],int path_type) {
    //open the directory if the name has an other name as "." or ".."
    //relative path: ../name1/name2/name3
    DIR *pDIR = NULL;
    struct dirent *pdirent = NULL;
    long int length=strlen(pPATH);
    unsigned int i=0;
    char* help;
    char dat_name[length];

    pDIR = opendir(pPATH);

    if (pDIR == NULL) {
        if(path_type==0){
            i=strlen(pPATH);
            help=strrchr(pPATH,'/');
            strcpy(dat_name,help);
            help=&dat_name[1];
            dat_name[0]='\0';
            do{

                pPATH[i]='\0';
                i--;
            }while(pPATH[i]!='/');
            pPATH[i]='\0';
            pDIR=opendir(pPATH);
            while ((pdirent = readdir(pDIR)) != NULL) {

                if (strcmp(pdirent->d_name, ".") == 0 || strcmp(pdirent->d_name, "..") == 0) {
                    continue;
                }

                if(!(fnmatch(help,pdirent->d_name,FNM_PATHNAME))) {
                    do_file(pdirent->d_name, action, pArrArgument, pdirent->d_name);
                }
            }
        }else if(path_type==1){
            pDIR=opendir(".");
            while ((pdirent = readdir(pDIR)) != NULL) {

                if (strcmp(pdirent->d_name, ".") == 0 || strcmp(pdirent->d_name, "..") == 0) {
                    continue;
                }

                if(!(fnmatch(pPATH,pdirent->d_name,FNM_PATHNAME))) {
                    do_file(pdirent->d_name, action, pArrArgument, pdirent->d_name);
                }
            }
        }
    } else {
        while ((pdirent = readdir(pDIR)) != NULL) {

            if (strcmp(pdirent->d_name, ".") == 0 || strcmp(pdirent->d_name, "..") == 0) {
                continue;
            }

            length = strlen(pPATH) + 2 + strlen(pdirent->d_name);
            char newpath[length];
            strcpy(newpath, "\0");
            strcat(newpath, pPATH);
            strcat(newpath, "/");
            strcat(newpath, pdirent->d_name);
            strcat(newpath, "\0");

            do_file(/*pDIR,*/ newpath, action, pArrArgument, pdirent->d_name);
        }
    closedir(pDIR);
    }
    return NULL;
}

/// Function to resolve the relative path
///
/// If an directory is closed, this function resolves the last part of the relative path
///
/// \param pPATH Current file path
/// \param count number of arguments
/// \param action An array with the actions, they should be checked
/// \param pArrArgument An array with the additions to the actions
///
/// \retval 0

int resolve_relpath(char* pPATH,int count,enum Action *action,char* pArrArgument[]){
    long int i=0;
    long int length=strlen(pPATH)+1;

    if((count==1)||((count>1)&&((strcmp(pPATH,"~")==0)||(strcmp(pPATH,".")==0)))){
        do_dir(".",action,pArrArgument,1);

    }else if(count>1 && ((strcmp(pPATH,".."))==0)) {

        if ((pPATH = (getcwd(NULL, 0))) == NULL) {
            func_error_expression(eerrno, notdeclared, pPATH);

        } else {
            i=strlen(pPATH);
            do{
                pPATH[i]='\0';
                i--;
            }while(pPATH[i]!='/');

            pPATH[i]='\0';
            do_dir(pPATH,action,pArrArgument,0);
            free(pPATH);
        }

    }else if(count>1 && (pPATH[0]=='/')){
        do_dir(pPATH,action,pArrArgument,0);

    }else if(count>1 && (pPATH[0]!='/')){
        length = strlen(pPATH) + 2;
        char newpath[length];
        strcpy(newpath, "\0");
        strcat(newpath,pPATH);

        do_dir(newpath,action,pArrArgument,1);
    }
    return 0;
}

/// Function to print in ls format
///
/// Reads the properties of the file and print it in ls format
///
/// \param FILE Current file which should be printed
/// \param file_name Name of the current file

void print_ls(struct stat FILE,char* file_name){

    char buffer[80];
    struct tm* tm;

    printf("\t%ld",FILE.st_ino);
    printf("\t%ld",FILE.st_blocks);
    printf((S_ISDIR(FILE.st_mode))? "\td":"\t-");
    printf( (FILE.st_mode & S_IRUSR) ? "r" : "-");
    printf( (FILE.st_mode & S_IWUSR) ? "w" : "-");
    printf( (FILE.st_mode & S_IXUSR) ? "x" : "-");
    printf( (FILE.st_mode & S_IRGRP) ? "r" : "-");
    printf( (FILE.st_mode & S_IWGRP) ? "w" : "-");
    printf( (FILE.st_mode & S_IXGRP) ? "x" : "-");
    printf( (FILE.st_mode & S_IROTH) ? "r" : "-");
    printf( (FILE.st_mode & S_IWOTH) ? "w" : "-");
    printf( (FILE.st_mode & S_IXOTH) ? "x" : "-");
    printf("\t%x",FILE.st_nlink);
    printf("\t%10s",getuser(FILE));
    printf("\t%10s",getgr(FILE));
    printf("\t%10ld\t",FILE.st_size);
    tm=localtime(&FILE.st_mtime);
    strftime(buffer,80,"%Y %B %d %H : %M\t",tm);
    printf("\t%40s\t%s\n",buffer,file_name);
}

/// Function for the user action
///
/// Checks the file is conform with the user name in the arg
///
/// \param FILE Current file which should be checked about the user name
/// \param file_name searched user

int check_print_user(struct stat FILE,char*arg){
    struct passwd *stuser=NULL;
    char* ptr;
    long t=0;

    t=strtol(arg,&ptr,10);

    if(((getpwnam(arg))!=NULL)) {
        stuser=(getpwnam(arg));
        if(FILE.st_uid==stuser->pw_uid){
            return 1;
        }

    }else if((getpwuid(t))!=NULL){         //save user information in struct passwd user
        stuser=(getpwuid(t));
        if(FILE.st_uid==stuser->pw_uid){
            return 1;
        }

    }else
    func_error_expression(wronguser,user,arg);
    return 0;
}

/// Function to get the user name of a file
///
/// Returns the username of a file
///
/// \param FILE Current file which should be checked about the user name
/// \return If there is a user -> user, else NULL

char* getuser(struct stat FILE){

    struct passwd *stuser=NULL;

    if(getpwuid(FILE.st_uid)!=NULL){
        stuser=getpwuid(FILE.st_uid);
        return stuser->pw_name;
    }else
        return NULL;

}

/// Function to get the group name of a file
///
/// Returns the groupname of a file
///
/// \param FILE Current file which should be checked about the group name
/// \return If there is a group -> group, else NULL

char* getgr(struct stat FILE){

    struct passwd *stuser=NULL;

    if(getpwuid(FILE.st_gid)!=NULL){
        stuser=getpwuid(FILE.st_gid);
        return stuser->pw_name;
    }else
        return NULL;

}

/// Function that print the errormessage and interrupt
///
/// There are different ways of the errorhandling
/// <br><br>ErrCode = wrongnumberofarg
/// <br> Output format: "Missing argument to '(eAction)'"
/// <br> ErrCode = wrongarg
/// <br> Output format: "Unknown argument to '(eAction)': (arg)"
/// <br> ErrCode = tolongarg
/// <br> Output format: "Arguments to '(eAction)' should contain only one letter"
/// <br> ErrCode = wrongname
/// <br> Output format: "Wrong name argument to action (eAction): '(arg)'"
/// <br> ErrCode = notdeclaredarg
/// <br> Output format: "Unknown predicate '(arg)'"
/// <br> ErrCode = nofileorpath
/// <br> Output format: "'(arg)': No such file or directory"
/// <br> ErrCode = eerrno
/// <br> Output format: "'(arg)': (errno)\n"
/// <br> ErrCode = wronguser
/// <br> Output format: "'(arg)' is not the name of a known user"
///
/// \param eErrorcode Current code for the detected erro
/// \param eAction Current active Action
/// \param arg Argument that should also be printed (example: PATH)

void func_error_expression(enum Error eErrorcode, enum Action eAction, char* arg){
    if((eAction == name || eAction == type || eAction == user || eAction == path) && eErrorcode == wrongnumberofarg){
        fprintf(stderr, "Missing argument to '%s'",func_print_action(eAction));
    }
    else if(eAction == type && eErrorcode == wrongarg){
        fprintf(stderr, "Unknown argument to '%s': %s",func_print_action(eAction), arg);
    }
    else if(eAction == type && eErrorcode == tolongarg){
        fprintf(stderr, "Arguments to '%s' should contain only one letter",func_print_action(eAction));
    }else if(eAction == name && eErrorcode==wrongname){
        fprintf(stderr, "Wrong name argument to action %s '%s'",func_print_action(eAction),arg);
    }
    else if(eErrorcode == notdeclaredarg){
        fprintf(stderr, "Unknown predicate '%s'",arg);
    }
    else if(eErrorcode == nofileorpath){
        fprintf(stderr, "'%s': No such file or directory",arg);
    }
    else if (eErrorcode == eerrno){
        fprintf(stderr, "'%s': %s\n",arg, strerror(errno));
        return;
    }else if(eErrorcode==wronguser && eAction==user){
        fprintf(stderr,"'%s' is not the name of a known user",arg);
    }
    exit(EXIT_FAILURE);
}
