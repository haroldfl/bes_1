// Check comments with |DEr|

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

enum Action {notdeclared, noaction, user, name, type, print, ls, nouser, path};
enum Error {wrongnumberofarg, wrongarg, tolongarg, notdeclaredarg, nofileorpath, eerrno,wronguser,wrongname};


DIR *do_file (/*DIR *pDIR,*/  char *pPATH,enum Action *action, char* pArrArgument[],char* file_name);
DIR *do_dir ( char *pPATH, enum Action *action,char* pArrArgument[]);
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
int check_valid_action(enum Action action);

int main(int argc, char *argv[]){
    char *pMainPath = NULL;     //Path for the find functions
    enum Action pArrMainAction[argc];
    char *pArrMainArgument[argc];


    func_check_arguments(argv,argc,pArrMainAction,pArrMainArgument);

    pMainPath = func_check_path(argv[1]);


//    resolve_relpath(pMainPath,argc,action,argv[3]);
    resolve_relpath(pMainPath,argc,pArrMainAction,pArrMainArgument);

     return 0;
}

// ################################################################################
// Functions to check the parameters
// ################################################################################

// --- func_check_arguments
// checks the different actions and the arguments of this
// writes the actions and the arguments in the arrays of them

void func_check_arguments(char *pARGUMENTS[], int COUNTER, enum Action *pArrAction, char *pArrArgument[]){
    enum Action etAction = notdeclared;
    int i = 1;
    int j = 0;
    char bad_chars[]="!@%~|";
    unsigned int  z;
    //1) Check if the first argument is the path or a option
    etAction = func_check_action(pARGUMENTS[i]);
    if (etAction == notdeclared){   //if it is not declared --> check if it is a path
        func_check_path(pARGUMENTS[i]);
        i++;
    }
    for(;i<COUNTER;i++, j++){
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
        else if(etAction == notdeclared || etAction == noaction){
            func_error_expression(notdeclaredarg,etAction,pARGUMENTS[i]);
        }
    }
}

// --- func_check_path
// if the first element is a parameter or NULL return "." as path
// otherwise return the first element as path

char *func_check_path(char *pPATH){
    //declaration
    //pt --- pointer temporary
    DIR *ptDIR = NULL;
    enum Action tAction;


    //program

    tAction = func_check_action(pPATH);

    if(tAction == notdeclared){

        ptDIR=opendir(pPATH);


        if ( ptDIR == NULL) {


            func_error_expression(nofileorpath,tAction,pPATH);
        }
        return pPATH;
    }
    else
        return ".";
}

// --- func_check_action

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

// Function returns a string addicted to the action
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

// -type
// return: 1 <--> correct type, 0 <--> incorrect type
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
    //|DEr| D ... door is missing. Error exprasion on linux find function
    return t_return;
}

// -nouser
// return: 1 <--> correct (nouser), 0 <--> incorrect (user)
int func_nouser(struct stat FILE){
    int t_return = 0;
    struct passwd *tpPWD=NULL;
    tpPWD=getpwuid(FILE.st_uid);
    if(tpPWD==NULL){
        t_return = 1;
    }
    //|DEr| errno checken
    return t_return;
}

DIR *do_file (/*DIR *pDIR, */char* pPATH, enum Action *action, char* pArrArgument[],char* file_name){

    //declaration of the variables
    struct stat file;
    int i=0;
    int check=0;
    int ls_help=0;


/*
    while(action[i]!=NULL){
        printf("%s\n",func_print_action(action[i]));
        i++;
    }
    i=0;
    while(pArrArgument[i]==NULL)i++;
        printf("%d %s\n",i,pArrArgument[i]);
  */



        if (lstat(pPATH, &file) == -1) {
            func_error_expression(eerrno, notdeclared, pPATH);
        }

     while(check_valid_action(action[i])){
        //printf("%s\n",func_print_action(action[i]));

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
                //printf("\n%s", pPATH);
                check++;
            }
        } else if ((action[i] == print) || (action[i] == notdeclared)) {
            //printf("\n%s", pPATH);
            check++;
        } else if (action[i] == user) {
            if (check_print_user(file, pArrArgument[i])) {
                check++;
            }


        } else if (action[i] == ls) {
            ls_help=1;
            check++;

        } else if (action[i] == path) {
            //printf("%s_______%s\n",pPATH,pArrArgument[i]);
            if ((strcmp(pPATH, pArrArgument[i]) == 0)) {
                check++;
                //printf("\n%s", pPATH);
            }
        }
    i++;
    }
    if((check==i) && (ls_help==0)){
        printf("\n%s", pPATH);
    }else if((check==i) && (ls_help==1)){
        //printf("\n%s", pPATH);
        print_ls(file, pPATH);
    }
    check=0;
    i=0;
    ls_help=0;
    
        if (S_ISDIR(file.st_mode)) {


            do_dir(pPATH, action, pArrArgument);
        }

return NULL;
}


DIR *do_dir ( char *pPATH, enum Action *action,char* pArrArgument[]) {
    //open the directory if the name has an other name as "." or ".."
    //relative path: ../name1/name2/name3
    DIR *pDIR = NULL;
    struct dirent *pdirent = NULL;
    long int length;



    pDIR = opendir(pPATH);

    if (pDIR == NULL) {
        func_error_expression(eerrno, notdeclared, pPATH);

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
                                                    //-name




            do_file(/*pDIR,*/ newpath, action, pArrArgument, pdirent->d_name);


        }

        closedir(pDIR);
        }

    return NULL;
}


int resolve_relpath(char* pPATH,int count,enum Action *action,char* pArrArgument[]){

    long int i=0;


    if((count==1)||((count>1)&&((strcmp(pPATH,"~")==0)||(strcmp(pPATH,".")==0)))){

        do_dir(".",action,pArrArgument);
        //free(pPATH);
     // }

    }else if(count>1 && ((strcmp(pPATH,".."))==0)) {

        if ((pPATH = (getcwd(NULL, 0))) == NULL) {
            //perror("getcwd error");
            func_error_expression(eerrno, notdeclared, pPATH);

        } else {

            i=strlen(pPATH);

            do{

                pPATH[i]='\0';
                    i--;

            }while(pPATH[i]!='/');

            pPATH[i]='\0';

            do_dir(pPATH,action,pArrArgument);

            free(pPATH);
        }

    }else if(count>1 && (pPATH[0]=='/')){

        do_dir(pPATH,action,pArrArgument);

    }else if(count>1 && (pPATH[0]!='/')){

        do_dir(pPATH,action,pArrArgument);
    }
    return 0;
}
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
char* getuser(struct stat FILE){

    struct passwd *stuser=NULL;

    if(getpwuid(FILE.st_uid)!=NULL){
        stuser=getpwuid(FILE.st_uid);
        return stuser->pw_name;
    }else
        return NULL;

}
char* getgr(struct stat FILE){

    struct passwd *stuser=NULL;

    if(getpwuid(FILE.st_gid)!=NULL){
        stuser=getpwuid(FILE.st_gid);
        return stuser->pw_name;
    }else
        return NULL;

}

//Error Expression
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
int check_valid_action(enum Action action){



    if((action!=notdeclared)&&(action!=user)&&(action!=name)&&(action!=type)&&(action!=print)&&(action!=ls)&&(action!=nouser)&&(action!=path)){
        return 0;
    }else {
        return 1;
    }
    }