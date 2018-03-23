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


DIR *do_file (DIR *pDIR,  char *pPATH);
DIR *do_dir ( char *pPATH/*, char *pFULLpath*/);
int resolve_relpath(char* pPATH,int count);


int main(int argc, char *argv[]){


    resolve_relpath(argv[1],argc);

     return 0;
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