#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#define LEN 50
#define SIZE 257

    struct characters
{
   int c;
   char str[LEN];
   struct characters * right;
   struct characters * left;
};

struct characters * build_trie(struct characters * node,struct characters * cur, int index, int str_len);
void dfs(struct characters * root);
struct characters * new_node();
char * char_to_bin(char c);

// Input of the program are the generated codebook and the compressed text
// Recovers the original text
int main(int argc, char *argv[])
{
    struct characters chars[SIZE];
    FILE *fp1,*fp2,*fo;
    int i;
    fp1=fopen(argv[1],"rb"); // The compressed text
    fp2=fopen(argv[2],"rb"); // The codebook
    fo=fopen(argv[3],"wb"); // The output file
    char last = '\n';
    int c;
    char cur[LEN],buffer[9];
    int curchar = 0;
    bool check = false;
    

    for(i=0;i<SIZE;i++){
        chars[i].c=i;
        strcpy(chars[i].str,"");
        chars[i].right = NULL;
        chars[i].left = NULL;
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
    struct characters * root;
    root = new_node();
    for(i=0;i<SIZE;i++){
        if(strlen(chars[i].str)>0){
            root = build_trie(root,chars+i,0,strlen(chars[i].str));
        }
    }
    struct characters * node;
    node = root;
    do{
        c = fgetc(fp1);
        strcpy(buffer,char_to_bin(c));
        for(int i=0;i<8;i++){
            if(buffer[i] == '1'){
                node = node->left;
            } else {
                node = node->right;
            }
            if(node->c!=257){
                if(node->c == 256){
                    fclose(fp1);
                    fclose(fp2);
                    fclose(fo);
                    return 0;
                }
                fprintf(fo,"%c",node->c);
                node = root;
            }
        }
    } while (c!=EOF);
    fclose(fp1);
    fclose(fp2);
    fclose(fo);
    return 0;
}

struct characters * build_trie(struct characters * node, struct characters * cur, int index, int str_len){

    if(index == str_len){
        return cur;
    }

    if(!node){
        node = new_node();
    }

    if(cur->str[index]=='0'){ //go to right
        node->right = build_trie(node->right,cur,index+1,str_len);
    } else { //go to left
        node->left = build_trie(node->left,cur,index+1,str_len);
    }

    return node;
}

struct characters * new_node(){
    struct characters * newnode = (struct characters *)malloc(sizeof(struct characters));
    newnode->c = 257;
    newnode->left = NULL;
    newnode->right = NULL;
    strcpy(newnode->str,"");
    return newnode;
}

void dfs(struct characters * root){
    if(!root) return ;

    if((!root->right && root->left) || (root->right && !root->left)) {
        if(!root->left) printf("left\n");
        else printf("right\n");
        printf("Wrong! %d %s\n",root->c,root->str); 
    }
 
    if(!root->right && !root->left){
        printf("%d %s\n",root->c,root->str);
        return ;
    }
    dfs(root->left);
    dfs(root->right);
    return;
}

char * char_to_bin(char c){
    char * tmp = (char*)malloc(9 * sizeof(char));
    for(int i=7;i>=0;i--){
        tmp[i] = (c&1)+'0';
        c >>= 1;
    }
    tmp[8] = '\0';
    return tmp;
}
