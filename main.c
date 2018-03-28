// Check comments with |DEr|

#include <stdio.h>
#include <unistd.h>     /* for chdir */
#include <stdlib.h>     /* for free */
#include <errno.h>      /* for errno */
#include <sys/types.h>  /* for opendir */
#include <dirent.h>     /* for opendir */
#include <string.h>    /* for strerror() */
#include <memory.h>     /* for strerror() */
#include <sys/stat.h>
#include<fnmatch.h>
#include<pwd.h>

enum Action {notdeclared, noaction, user, name, type, print, ls, nouser, path};


DIR *do_file (DIR *pDIR,  char *pPATH,int action, char* arg,int output);
DIR *do_dir ( char *pPATH, int action,char* arg);
int resolve_relpath(char* pPATH,int count,int action,char* arg);
char *func_check_path(char* pPATH);
enum Action func_check_action(char *pACTION);
void func_check_arguments(char *pARGUMENTS[], int COUNTER, enum Action *pArrAction, char *pArrArgument[]);
char *func_print_action(enum Action eAction);

int main(int argc, char *argv[]){
    char *pMainPath = NULL;     //Path for the find functions
    enum Action pArrMainAction[argc];
    char *pArrMainArgument[argc];
    int action=6;

    func_check_arguments(argv,argc,pArrMainAction,pArrMainArgument);

    pMainPath = func_check_path(argv[1]);

    resolve_relpath(pMainPath,argc,action,argv[3]);

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

DIR *do_file (DIR *pDIR, char* pPATH, int action, char* arg,int output){


    //declaration of the variables
    struct stat file;
    struct passwd *user=NULL;

        if (lstat(pPATH, &file) == -1) {
            printf("ERROR");
        }

        if (S_ISREG(file.st_mode)) {
            if((output==1&&action==1)||(action==7)||(action==0)) {
                printf("\nFile%s", pPATH);
            }else if(action==3) {
                if((user=getpwnam(arg))==NULL){         //save user information in struct passwd user
                    printf("ERROR");
                } else {
                    if(file.st_uid==user->pw_uid){
                        printf("\nUFile%s", pPATH);
                    }
                }

            }else if(action==6){
                printf("%d",file.st_mode);
                printf("-");
                printf( (file.st_mode & S_IRUSR) ? "r" : "-");
                printf( (file.st_mode & S_IWUSR) ? "w" : "-");
                printf( (file.st_mode & S_IXUSR) ? "x" : "-");
                printf( (file.st_mode & S_IRGRP) ? "r" : "-");
                printf( (file.st_mode & S_IWGRP) ? "w" : "-");
                printf( (file.st_mode & S_IXGRP) ? "x" : "-");
                printf( (file.st_mode & S_IROTH) ? "r" : "-");
                printf( (file.st_mode & S_IWOTH) ? "w" : "-");
                printf( (file.st_mode & S_IXOTH) ? "x" : "-");

            }

        } else if (S_ISDIR(file.st_mode)) {
            if((output==1&&action==1)||(action==7)||(action==0)) {
                printf("\nDirectory:%s", pPATH);
            }else if(action==3) {

                if((user=getpwnam(arg))==NULL){
                    printf("ERROR");
                } else {
                    if(file.st_uid==user->pw_uid){
                        printf("\nUDirectory:%s", pPATH);
                    }
                }
            }

            do_dir(pPATH, action, arg);
        }


}


DIR *do_dir ( char *pPATH, int action,char* arg) {
    //open the directory if the name has an other name as "." or ".."
    //relative path: ../name1/name2/name3
    DIR *pDIR = NULL;
    struct dirent *pdirent = NULL;
    int length;
    int output = 0;


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
                    if (!(fnmatch(arg, pdirent->d_name, FNM_PATHNAME))) {
                        output = 1;
                    }



            do_file(pDIR, newpath, action, arg, output);
            output=0;
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