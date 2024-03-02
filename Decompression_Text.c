#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define SIZE 257
#define STRSIZE 50

struct characters
{
    int left; // The left children of the node
    int right; // The right children of the node
    int c1; // The Ascii code of the character, the 256th character represents the final character of the file
    int freq; // The frequency of the character
    char str[STRSIZE]; // The new codeword of the character
    int code; // This code assists the program to build the tree
};

void depth_first_search(struct characters *node, FILE *fo,struct characters *codes);

// Input of the program is the original text
// Generates a codebook of the original text
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

    // Initialize the characters attibutes
    for(i=0;i<SIZE;i++)
    {
        chars[i].freq=0;
        chars[i].c1=i;
        chars[i].left = -1;
        chars[i].right = -1;
    }

    chars[256].freq = 1; // This character indicates the end of the file, which only appears once

    // Count the frequency of characters
    do{
        c=fgetc(fp);
        chars[c].freq++;
    }while(c!=EOF);

    // Sort the characters by their frequencies, from large to small
    for(i=1;i<SIZE;i++)
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

    // Count the numbers of characters that appears in the text
    for(i=0;i<SIZE;i++)
        if(chars[i].freq) appeared_char++;

    // Initialize the tree nodes
    struct characters * tree_node;
    tree_node = (struct characters*)malloc(sizeof(struct characters)*(2*appeared_char-1));

    if (tree_node == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1; // Exit the program with an error code
    }
    
    // This array stores the codes of the tree node so that when we shuffle the tree nodes in its array, we won't loss the code
    struct characters * codes;
    codes = (struct characters*)malloc(sizeof(struct characters)*(2*appeared_char-1));

    if (codes == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1; // Exit the program with an error code
    }

    // Build the tree structure
    for(i=0;i<2*appeared_char-1;i++){
        if (i<appeared_char)
            tree_node[i] = chars[i]; // These are the leaf nodes, which are actual characters
        strcpy(tree_node[i].str,"");
        tree_node[i].code = i; // Give every tree node a code to identify them
        codes[i] = tree_node[i]; 
        tree_node[i].left = tree_node[i].right = -1;
    }

    free(chars);

    for(i=appeared_char-1;i>=1;i--){
        int index = 2*appeared_char-1-i; // The parent node of the two nodes to be merged
        tree_node[index].freq = tree_node[i].freq + tree_node[i-1].freq; // Its frequency is the addition of frquencies of its children
        codes[tree_node[index].code].left = tree_node[i-1].code; 
        codes[tree_node[index].code].right = tree_node[i].code; // Store the parent's left and right children
        // Swap the parent node with its left children in the array
        struct characters temp;
        temp = tree_node[index];
        tree_node[index] = tree_node[i-1];
        tree_node[i-1] = temp;
        j = i-2;
        // Sort the array from index 0 to i, since 0 to i-1 is sorted, just find the right spot for the new node
        while(j>=0 && tree_node[j].freq<tree_node[j+1].freq){
            temp = tree_node[j];
            tree_node[j] = tree_node[j+1];
            tree_node[j+1] = temp;
            j--;
        }
    }

    depth_first_search(&codes[tree_node[0].code],fo,codes);

    // Count the compression rate
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

// Use depth first search to find out the codeword of the characters
void depth_first_search(struct characters *node, FILE *fo,struct characters *codes){

    // If this node is a leaf node, print its character and its codeword to the output file
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
