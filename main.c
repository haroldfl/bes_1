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
char *expand_path (char *pPATH, char *expand);
int resolve_relpath(char* pPATH,int count);


int main(int argc, char *argv[]){


    resolve_relpath(argv[1],argc);
    //do_dir(argv[1]);
    /*if(argc==1||((argc>1)&&(strcmp(argv[1],".")==0))) {

        if ((pPATH_full = (getcwd(NULL, 0))) == NULL) {
            perror("getcwd error");
        } else {
            do_dir(pPATH_full);
            free(pPATH_full);
        }
    }else
        if(validate_path(argv[1])){

        }
    */
     return 0;
}

DIR *do_file (DIR *pDIR, char* pPATH){

    int i = 0;
    //declaration of the variables
    struct stat file;


    //printf("\n\n Uebergabe PFAD (do_file): %s", pPATH);



        if(lstat(pPATH,&file)==-1){
            printf("ERROR");
        }
        else if(S_ISREG(file.st_mode)){
            printf("\nFile%s",pPATH);


        }else if(S_ISDIR(file.st_mode)){

            //pPATH=expand_path(pPATH,"/");
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

    //pPATH = expand_path(pPATH, "/");

    //if(strcmp(".",pPATH)!=0 && strcmp("..",pPATH)!=0) {
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
                //printf("\npPATH:%s",pPATH);
                //printf("\n%s",newpath);
                do_file(pDIR,newpath);




            }
            closedir(pDIR);
        }


    return 0;
}





char *expand_path (char *pPATH, char *expand){
    int pathlength = 0;
    int expandlength = 0;
    char *pnewPATH = NULL;
    char test[256];
    int i = 0;
    int j = 0;
    while(pPATH[pathlength]!='\0'){
        pathlength++;
    }
    while(expand[expandlength]!='\0'){
        expandlength++;
    }

    pnewPATH = malloc((pathlength + expandlength + 1) * sizeof(char) );

    //write the actual path in the variable
    for(i=0; i<pathlength; i++){
        pnewPATH[i] = pPATH[i];
    }
    j=0;
    for(i; j<expandlength; i++, j++){
        pnewPATH[i] = expand[j];
    }
    pnewPATH[i]='\0';
    return pnewPATH;
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
            //printf("%s",pPATH);
            do{
                pPATH[i]='\0';
                i--;
            }while(pPATH[i]!='/');
            pPATH[i]='\0';

            //printf("%s",pPATH);
            do_dir(pPATH);
            free(pPATH);
        }
    }else if(count>1 && (pPATH[0]=='/')){
        do_dir(pPATH);
    }else if(count>1 && (pPATH[0]!='/')){
        do_dir(pPATH);
    }
}