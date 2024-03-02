#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define SIZE 257
#define STRSIZE 50

struct characters
{
    int left;
    int right;
    int c1;
    int freq;
    char str[STRSIZE];
    int code;
};

void depth_first_search(struct characters *node, FILE *fo,struct characters *codes);

int main(int argc, char *argv[])
{
    struct characters *chars,tmp;
    int i, j, appeared_char = 0, c;
    FILE *fp, *fo;

    fp=fopen(argv[1],"rb");
    fo=fopen(argv[2],"wb");

    chars = (struct characters*)malloc(sizeof(struct characters)*SIZE);
    if (chars == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1; // Exit the program with an error code
    }

    for(i=0;i<SIZE;i++)
    {
        chars[i].freq=0;
        chars[i].c1=i;
        chars[i].left = -1;
        chars[i].right = -1;
    }

    chars[256].freq = 1;

    do{
        c=fgetc(fp);
        chars[c].freq++;// count the frequency of characters
    }while(c!=EOF);

    for(i=1;i<SIZE;i++)// sort the frequency of characters
    {
        for(j=0;j<i;j++)
        {
            if(chars[i].freq>chars[j].freq)
            {
                tmp=chars[i];
                chars[i]=chars[j];
                chars[j]=tmp;
            }
        }
    }

    for(i=0;i<SIZE;i++)
        if(chars[i].freq) appeared_char++;

    struct characters * tree_node;
    tree_node = (struct characters*)malloc(sizeof(struct characters)*(2*appeared_char-1));

    if (tree_node == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1; // Exit the program with an error code
    }
    
    struct characters * codes;
    codes = (struct characters*)malloc(sizeof(struct characters)*(2*appeared_char-1));

    if (codes == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1; // Exit the program with an error code
    }

    for(i=0;i<2*appeared_char-1;i++){
        if (i<appeared_char)
            tree_node[i] = chars[i];
        strcpy(tree_node[i].str,"");
        tree_node[i].code = i;
        codes[i] = tree_node[i];
        tree_node[i].left = tree_node[i].right = -1;
    }

    free(chars);

    for(i=appeared_char-1;i>=1;i--){
        int index = 2*appeared_char-1-i;
        tree_node[index].freq = tree_node[i].freq + tree_node[i-1].freq;
        codes[tree_node[index].code].left = tree_node[i-1].code;
        codes[tree_node[index].code].right = tree_node[i].code;
        struct characters temp;
        temp = tree_node[index];
        tree_node[index] = tree_node[i-1];
        tree_node[i-1] = temp;
        j = i-2;
        while(j>=0 && tree_node[j].freq<tree_node[j+1].freq){
            temp = tree_node[j];
            tree_node[j] = tree_node[j+1];
            tree_node[j+1] = temp;
            j--;
        }
    }

    depth_first_search(&codes[tree_node[0].code],fo,codes);

    int total = 0, len = 0;
    for(i=0;i<appeared_char;i++){
        total += codes[i].freq;
        len += strlen(codes[i].str)*codes[i].freq;
    }

    printf("Average bit for a character is %f\n",(float)len/total);
    printf("Compression rate is %f\n",(float)len/total/8);

    free(codes);
    free(tree_node);

    fclose(fp);
    fclose(fo);

    return 0;
}

void depth_first_search(struct characters *node, FILE *fo,struct characters *codes){

    if(node->left==-1){

        if(node->c1 == 10){
            fprintf(fo,"\\n ");
        } else if (node->c1 == 13){
            fprintf(fo,"\\r ");
        } else if (node->c1 <= 127){
            fprintf(fo,"%c ",node->c1);
        } else {
            fprintf(fo,"%d ",node->c1);
        }
        fprintf(fo,"%s\r\n",node->str);
        return;

    }
    char tmp[STRSIZE];
    strcpy(tmp,node->str);
    strcpy(codes[node->left].str,strcat(tmp,"1"));
    strcpy(tmp,node->str);
    strcpy(codes[node->right].str,strcat(tmp,"0"));
    depth_first_search(&codes[node->left],fo,codes);
    depth_first_search(&codes[node->right],fo,codes);

    return;
}
