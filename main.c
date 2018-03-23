//Comments with DER should be changed

#include <stdio.h>
#include <unistd.h>     /* for chdir */
#include <stdlib.h>     /* for free */
#include <errno.h>      /* for errno */
#include <sys/types.h>  /* for opendir */
#include <dirent.h>     /* for opendir */
#include <string.h>    /* for strerror() */
#include <memory.h>     /* for strerror() */
#include <sys/stat.h>
#define MAX_PATH 255

enum Action {notdeclared, noaction, user, name, type, print, ls, nouser, path};


DIR *do_file (DIR *pDIR,  char *pPATH);
DIR *do_dir ( char *pPATH/*, char *pFULLpath*/);
int resolve_relpath(char* pPATH,int count);
char *func_check_path(char* pPATH);
enum Action func_check_action(char *pACTION);

int main(int argc, char *argv[]){
    char *pMainPath = NULL;     //Path for the find functions
    enum Action eAction = notdeclared;

    pMainPath = func_check_path(argv[1]);

    resolve_relpath(pMainPath,argc);

     return 0;
}

// ################################################################################
// Functions to check the parameters
// ################################################################################


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

DIR *do_file (DIR *pDIR, char* pPATH){


    //declaration of the variables
    struct stat file;


    if(lstat(pPATH,&file)==-1){
        printf("ERROR");

    }else if(S_ISREG(file.st_mode)){

        printf("\nFile%s",pPATH);


    }else if(S_ISDIR(file.st_mode)){

        printf("\nDirectory:%s",pPATH);
        do_dir(pPATH);

    }


}


DIR *do_dir ( char *pPATH/*, char *pFULLpath*/){
    //open the directory if the name has an other name as "." or ".."
    //relative path: ../name1/name2/name3
    DIR *pDIR = NULL;
    struct dirent *pdirent = NULL;
    int length;

    pDIR=opendir(pPATH);

    if ( pDIR == NULL) {

        printf("Error");
        printf(" %s", strerror(errno)); //print the error message

    } else {

        while((pdirent=readdir(pDIR))!=NULL){

            if(strcmp(pdirent->d_name,".")==0 || strcmp(pdirent->d_name,"..")==0){
                    continue;
            }

            length=strlen(pPATH)+ 2  + strlen(pdirent->d_name);
            char newpath[length];
            strcpy(newpath,"\0");
            strcat(newpath,pPATH);
            strcat(newpath,"/");
            strcat(newpath,pdirent->d_name);
            strcat(newpath,"\0");
            do_file(pDIR,newpath);

        }
        closedir(pDIR);
    }


    return 0;
}


int resolve_relpath(char* pPATH,int count){

    int i=0;


    if((count==1)||((count>1)&&((strcmp(pPATH,"~")==0)||(strcmp(pPATH,".")==0)))){

        do_dir(".");
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

            do_dir(pPATH);

            free(pPATH);
        }

    }else if(count>1 && (pPATH[0]=='/')){

        do_dir(pPATH);

    }else if(count>1 && (pPATH[0]!='/')){

        do_dir(pPATH);
    }
}