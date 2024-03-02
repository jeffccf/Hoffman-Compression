#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#define SIZE 257
#define LEN 50

int ascii_converter(char * str);

    struct characters
{
   int c;
   char str[LEN];
};

// Input of the program are the generated codebook and the original text
// Generates a compressed text
int main(int argc, char *argv[])
{
    struct characters chars[SIZE];
    int i=0;
    FILE *fp1,*fp2,*fo;
    char cur[LEN];
    int curchar = 0;
    char last = '\n';
    int c = 0;
    bool check = false;
    char buffer[LEN+8];

    fp1=fopen(argv[1],"rb"); // The original text
    fp2=fopen(argv[2],"rb"); // The codebook
    fo=fopen(argv[3],"wb"); // The output file

    for(i=0;i<SIZE;i++){
        chars[i].c=i;
        strcpy(chars[i].str,"");
    }
    strcpy(cur,"");
    i = 0;
    
    while(c!=EOF){
        c = fgetc(fp2);
        if (check){
            if (c == '\r'){
                cur[i] = '\0';
                if(curchar>=0 && curchar<=9)
                    curchar += '0';
                strcpy(chars[curchar].str,cur);
                strcpy(cur,"");
                last = '\n';
                check = false;
                i = 0;
                curchar = 0;
                c = fgetc(fp2);
            } else{
                cur[i++] = c;
            }
        } else if (c == ' '){
            if(last == '\n'){
                curchar = ' ';
            } else {
                check = true;
            }
        } else if (last == 92){
            if(c=='n')
                curchar = '\n';
            else if(c=='r')
                curchar = '\r';
        } else if (c>='0' && c<='9'){
            curchar = curchar*10+c-'0';
        } else {
            curchar = c;
        }
        last = c;
    }
    int cnt = 0;
    do{
        c = fgetc(fp1);
        strcat(buffer,chars[c].str);
        while(strlen(buffer)>=8){
            fprintf(fo,"%c",ascii_converter(buffer));
            cnt++;
            strcpy(buffer,buffer+8);
        }
    }while(c!=EOF);
    
    strcat(buffer,chars[256].str);

    while(strlen(buffer)>=8){
        fprintf(fo,"%c",ascii_converter(buffer));
        strcpy(buffer,buffer+8);
        cnt++;
    }

    c = 0;
    if(strlen(buffer)){
        for(int i=0;i<8;i++){
            c = c*2;
            if(buffer[i]=='1') c+=1;
        }
        cnt++;
    }
    fprintf(fo,"%c",c);

    fclose(fp1);
    fclose(fo);
    fclose(fp2);
    
    return 0;
}

int ascii_converter(char * str){
    if(strlen(str)<8) return ' ';
    int res = 0;
    for(int i=0;i<8;i++){
        res = res*2+(str[i]-'0');
    }
    return res;
}
