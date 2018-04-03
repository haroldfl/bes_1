// Check comments with |DEr|

#include <stdio.h>
#include <unistd.h>     /* for chdir */
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


DIR *do_file (DIR *pDIR,  char *pPATH,int action, char* arg,char* file_name);
DIR *do_dir ( char *pPATH, int action,char* arg);
int resolve_relpath(char* pPATH,int count,int action,char* arg);
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
int main(int argc, char *argv[]){
    char *pMainPath = NULL;     //Path for the find functions
    enum Action pArrMainAction[argc];
    char *pArrMainArgument[argc];


    func_check_arguments(argv,argc,pArrMainAction,pArrMainArgument);

    pMainPath = func_check_path(argv[1]);

//    resolve_relpath(pMainPath,argc,action,argv[3]);
    resolve_relpath(pMainPath,argc,user,argv[3]);

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
    //1) Check if the first argument is the path or a option
    etAction = func_check_action(pARGUMENTS[i]);
    if (etAction == notdeclared){   //if it is not declared --> check if it is a path
        func_check_path(pARGUMENTS[i]);
        i++;
    }
    for(i;i<COUNTER;i++, j++){
        etAction = func_check_action(pARGUMENTS[i]);
        pArrAction[j] = etAction; //Action Array
        //Check if there is an argument for the specified options
        if(etAction == user || etAction == name || etAction == type || etAction == path){
            if(pARGUMENTS[++i]==NULL){
                printf("Missing argument to '%s'",func_print_action(etAction));
                EXIT_FAILURE;
            }
            else{
                pArrArgument[j] = pARGUMENTS[i]; //Argument Array
            }
        }
        else if(etAction == notdeclared || etAction == noaction){
            printf("Unknown predicate '%s'",pARGUMENTS[i]);
            EXIT_FAILURE;
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
    enum Action tAction = 0;

    //program

    tAction = func_check_action(pPATH);

    if(tAction == notdeclared){
        ptDIR=opendir(pPATH);
        if ( ptDIR == NULL) {
            printf("'%s': %s", pPATH, strerror(errno)); //print the error message
            EXIT_FAILURE;
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

DIR *do_file (DIR *pDIR, char* pPATH, int action, char* arg,char* file_name){

    //declaration of the variables
    struct stat file;


        if (lstat(pPATH, &file) == -1) {
            printf("ERROR");
        }

        if (action == type){
            if(func_type(arg,file)){
                printf("\n%s", pPATH);
            }
        } else if (action == nouser){
            if(func_nouser(file)){
                printf("\n---------------------------------\n%s", pPATH);
            }
        } else if(action==name){
                if(!(fnmatch(arg, file_name, FNM_PATHNAME))) {
                    printf("\n%s", pPATH);
                }
        }else if((action==print)||(action==notdeclared)){
               printf("\n%s", pPATH);
        } else if(action==user) {
               if(arg!=NULL&&check_print_user(file,arg)){
                   printf("\n%s", pPATH);
               }


            }else if(action==ls){
               print_ls(file,pPATH);

            }else if(action==path){
               //printf("%s_______%s\n",pPATH,arg);
               if((strcmp(pPATH,arg)==0)){
                   printf("\n%s", pPATH);
               }
           }

        if (S_ISDIR(file.st_mode)) {


            do_dir(pPATH, action, arg);
        }


}


DIR *do_dir ( char *pPATH, int action,char* arg) {
    //open the directory if the name has an other name as "." or ".."
    //relative path: ../name1/name2/name3
    DIR *pDIR = NULL;
    struct dirent *pdirent = NULL;
    int length;



    pDIR = opendir(pPATH);

    if (pDIR == NULL) {

        printf("Error");
        printf(" %s", strerror(errno)); //print the error message

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




            do_file(pDIR, newpath, action, arg, pdirent->d_name);


        }

        closedir(pDIR);
        }

    return 0;
}


int resolve_relpath(char* pPATH,int count,int action,char* arg){

    int i=0;


    if((count==1)||((count>1)&&((strcmp(pPATH,"~")==0)||(strcmp(pPATH,".")==0)))){

        do_dir(".",action,arg);
        //free(pPATH);
     // }

    }else if(count>1 && ((strcmp(pPATH,".."))==0)) {

        if ((pPATH = (getcwd(NULL, 0))) == NULL) {

            perror("getcwd error");

        } else {

            i=strlen(pPATH);

            do{

                pPATH[i]='\0';
                    i--;

            }while(pPATH[i]!='/');

            pPATH[i]='\0';

            do_dir(pPATH,action,arg);

            free(pPATH);
        }

    }else if(count>1 && (pPATH[0]=='/')){

        do_dir(pPATH,action,arg);

    }else if(count>1 && (pPATH[0]!='/')){

        do_dir(pPATH,action,arg);
    }
}
void print_ls(struct stat FILE,char* file_name){

    char buffer[80];

    struct tm* tm;


    printf("\t%d",FILE.st_ino);
    printf("\t%d",FILE.st_blocks);
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
    printf("\t%d",FILE.st_nlink);
    printf("\t%10s",getuser(FILE));
    printf("\t%10s",getgr(FILE));
    printf("\t%10d\t",FILE.st_size);
    tm=localtime(&FILE.st_mtime);
    strftime(buffer,80,"%Y %B %d %H : %M\t",tm);
    printf("\t%40s\t%s\n",buffer,file_name);


}
int check_print_user(struct stat FILE,char*arg){
    struct passwd *stuser=NULL;
    struct passwd *iduser=NULL;
    char* ptr;
    long t=0;

    int length=strlen(arg);

    t=strtol(arg,&ptr,10);

    if(((stuser=getpwnam(arg))==NULL)&&((stuser=getpwuid(t))==NULL)){         //save user information in struct passwd user
        printf("ERROR");
        return 0;
    } else {
        if(FILE.st_uid==stuser->pw_uid){

            return 1;
        }
    }
}
char* getuser(struct stat FILE){

    struct passwd *stuser=NULL;

    if(stuser=getpwuid(FILE.st_uid)){
     return stuser->pw_name;
    }else
        return NULL;

}
char* getgr(struct stat FILE){

    struct passwd *stuser=NULL;

    if(stuser=getpwuid(FILE.st_gid)){
        return stuser->pw_name;
    }else
        return NULL;

}
