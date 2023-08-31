#include <stdio.h>
#include <stdlib.h>
#include "string.h"
#define BIT8 8
#define CODE_SIZE 18
#define uc unsigned char
typedef struct node {
    unsigned char symb;
    unsigned char isSymb;
    unsigned int freq;
    unsigned char code[CODE_SIZE];
    int level;
    struct node * left, *right, *next;
}NODE;
unsigned char symbcode[500][25]={0};
void add_to_list(NODE** head, NODE ** value,int phase)
{
    while (*head)
    {
        if ((*&(*head))->freq<(*(&(*value)))->freq)
            head = &(*head)->next;
        else
            break;
    }
    NODE* new_elem = (NODE*)malloc(sizeof(NODE));
    new_elem->freq=(*(&(*value)))->freq;
    new_elem->symb=(*(&(*value)))->symb;
    new_elem->isSymb=(*(&(*value)))->isSymb;
    new_elem->level=((*(&(*value)))->level)+1;
    new_elem->next = *head;
    new_elem->left=(*(&(*value)))->left;
    new_elem->right=(*(&(*value)))->right;
    if (phase==1){
        new_elem->right=NULL;
        new_elem->left=NULL;
    }
    *head = new_elem;
}

NODE * MakeNodeFromNode(NODE * left, NODE * right){
    NODE * res = (NODE*)malloc(sizeof(NODE));
    res->freq = left->freq + right->freq;
    res->isSymb = 0;
    res->symb = 0;
    res->left = left;
    res->right = right;
    res->next = 0;
    return res;
}
typedef union bit2char {
    char symb;
    struct bit {
        unsigned b1 : 1;
        unsigned b2 : 1;
        unsigned b3 : 1;
        unsigned b4 : 1;
        unsigned b5 : 1;
        unsigned b6 : 1;
        unsigned b7 : 1;
        unsigned b8 : 1;
    }mbit;
}BIT2CHAR;
NODE * MakeTreeFromList(NODE * head){
    while (head && head->next){
        NODE * left = head;
        NODE * right = head->next;
        NODE * buffer = (NODE*)malloc(sizeof(NODE));
        buffer=MakeNodeFromNode(left, right);
        add_to_list(&(head->next->next), &buffer,2);
        head = head->next->next;
    }
    return head;
}
void print_list(NODE const* head)
{
    while (head){
        printf("%c %d\n",head->symb,head->freq);
        head=head->next;
    }
}
void print_tree(NODE const* head,int p)
{
    if (head==NULL){
        return;
    }
    print_tree(head->left,p+1);
    if (head->isSymb){
        printf("%c %d %d %s\n",head->symb,head->freq,p,head->code);
    }
    print_tree(head->right,p+1);
}
void make_code(NODE* head,int p){
    if (head==NULL){
        return;
    }
    if (head->isSymb==1){
        head->code[p]='\0';
        for (int i=0;i<=p;i++){
            symbcode[head->symb][i]=head->code[i];
        }
        return;
    }
    for (int i=0;i<p;i++){
        head->left->code[i]=head->code[i];
        head->right->code[i]=head->code[i];
    }
    if (head->left!=NULL){
        head->left->code[p]='0';
        make_code(head->left,p+1);
    }

    if (head->right!=NULL){
        head->right->code[p]='1';
        make_code(head->right,p+1);
    }

}
int main() {
    int freq[500]={0};
    char from[35]={0};
    scanf("%s",from);
    FILE *fr = fopen(from, "rb");
    if (!fr){
        printf("NOT OPEN\n");
        return 0;
    }
    fseek(fr, 0L, SEEK_END);
    long length = ftell(fr);
    fseek(fr, 0, SEEK_SET);
    for (int i = 0; i < length; ++i){
        freq[(unsigned char) fgetc(fr)]++;
    }

    NODE * head=NULL;

    //FILE * buff =fopen("buffer.txt","wb+");
    unsigned char* buff=(unsigned char*) calloc(length*8,sizeof (unsigned char));
    int bufflen=0;
    for (int i=0;i<=255;i++){
        if (freq[i]!=0){
            NODE * n = (NODE*)malloc(sizeof(NODE));
            n->symb=i;
            n->isSymb=1;
            n->freq=freq[i];
            n->level=0;
            add_to_list(&head,&n,1);
        }
    }
    //print_list(head);
    //printf("\n");
    head=MakeTreeFromList(head);
    if (head->isSymb==1){

        NODE* new_elem = (NODE*)malloc(sizeof(NODE));
        new_elem->isSymb=1;
        new_elem->symb=head->symb;
        new_elem->left=NULL;
        new_elem->right=NULL;
        new_elem->freq=head->freq;
        head->left=new_elem;
        head->isSymb=0;

    }
    make_code(head,0);
    //print_tree(head,0);

    fseek(fr, 0, SEEK_SET);
    BIT2CHAR symb;
    for (int i=0;i<length;i++){
        unsigned char g=fgetc(fr);
        //printf("%d",strlen((char*)symbcode[g]));
        for (int j=0;j< strlen((char*)symbcode[g]);j++){
            buff[bufflen]=symbcode[g][j];
            bufflen++;
        }

    }
    buff[bufflen]='\0';
    //printf("%d %s",bufflen,buff);
    fclose(fr);
    //сжатие по коду
    FILE * to =fopen(from,"wb+");
    //FILE *buff2 = fopen("buffer.txt", "rb");
//    if (!buff2){
//        printf("NOT OPEN\n");
//        return 0;
//    }
//    fseek(buff2, 0L, SEEK_END);
//    long lengthbuff = ftell(buff2);
//    fseek(buff2, 0, SEEK_SET);
    int number=0;
    int curbuflen=0;
    int k=1;//позиция во временном буфере
    for (int i=0;i<bufflen;i++){
        switch (k) {
            case 1:
                symb.mbit.b1=(int) (buff[curbuflen]-'0');
                k++;
                curbuflen++;
                break;
            case 2:
                symb.mbit.b2=(int) (buff[curbuflen]-'0');
                k++;
                curbuflen++;
                break;
            case 3:
                symb.mbit.b3=(int) (buff[curbuflen]-'0');
                curbuflen++;
                k++;
                break;
            case 4:
                symb.mbit.b4=(int) (buff[curbuflen]-'0');
                curbuflen++;
                k++;
                break;
            case 5:
                symb.mbit.b5=(int) (buff[curbuflen]-'0');
                curbuflen++;
                k++;
                break;
            case 6:
                symb.mbit.b6=(int) (buff[curbuflen]-'0');
                curbuflen++;
                k++;
                break;
            case 7:
                symb.mbit.b7=(int) (buff[curbuflen]-'0');
                curbuflen++;
                k++;
                break;
            case 8:
                symb.mbit.b8=(int) (buff[curbuflen]-'0');
                curbuflen++;
                k=1;
                number++;
                fputc(symb.symb,to);
                break;
        }
    }
    if (k!=1){
        fputc(symb.symb,to);
        number++;
    }
    fclose(to);
    int gdk;
    printf("bip\n");
    scanf("%d",&gdk);
    //а теперь распаковка
    FILE * rasp = fopen(from,"rb");
    fseek(rasp, 0L, SEEK_END);
    long lengthrasp = ftell(rasp);
    fseek(rasp, 0, SEEK_SET);
    NODE * cur_elem =head;
    uc * result = (uc*)malloc((length+5) * sizeof(uc));
    int resultlen=0;
    for (int i=0;i<lengthrasp;i++){
        symb.symb= (uc)fgetc(rasp);
        switch (symb.mbit.b1) {
            case 0:
                cur_elem=cur_elem->left;
                break;
            case 1:
                cur_elem=cur_elem->right;
                break;
        }
        if (cur_elem==NULL){
            break;
        }
        if (cur_elem->isSymb==1 && resultlen<length){
            result[resultlen]=cur_elem->symb;
            resultlen++;
            cur_elem=head;
        }
        switch (symb.mbit.b2) {
            case 0:
                cur_elem=cur_elem->left;
                break;
            case 1:
                cur_elem=cur_elem->right;
                break;
        }
        if (cur_elem==NULL){
            break;
        }
        if (cur_elem->isSymb==1 && resultlen<length){
            result[resultlen]=cur_elem->symb;
            resultlen++;
            cur_elem=head;
        }
        switch (symb.mbit.b3) {
            case 0:
                cur_elem=cur_elem->left;
                break;
            case 1:
                cur_elem=cur_elem->right;
                break;
        }
        if (cur_elem==NULL){
            break;
        }
        if (cur_elem->isSymb==1 && resultlen<length){
            result[resultlen]=cur_elem->symb;
            resultlen++;
            cur_elem=head;
        }
        switch (symb.mbit.b4) {
            case 0:
                cur_elem=cur_elem->left;
                break;
            case 1:
                cur_elem=cur_elem->right;
                break;
        }
        if (cur_elem==NULL){
            break;
        }
        if (cur_elem->isSymb==1 && resultlen<length){
            result[resultlen]=cur_elem->symb;
            resultlen++;
            cur_elem=head;
        }
        switch (symb.mbit.b5) {
            case 0:
                cur_elem=cur_elem->left;
                break;
            case 1:
                cur_elem=cur_elem->right;
                break;
        }
        if (cur_elem==NULL){
            break;
        }
        if (cur_elem->isSymb==1 && resultlen<length){
            result[resultlen]=cur_elem->symb;
            resultlen++;
            cur_elem=head;
        }
        switch (symb.mbit.b6) {
            case 0:
                cur_elem=cur_elem->left;
                break;
            case 1:
                cur_elem=cur_elem->right;
                break;
        }
        if (cur_elem==NULL){
            break;
        }
        if (cur_elem->isSymb==1 && resultlen<length){
            result[resultlen]=cur_elem->symb;
            resultlen++;
            cur_elem=head;
        }
        switch (symb.mbit.b7) {
            case 0:
                cur_elem=cur_elem->left;
                break;
            case 1:
                cur_elem=cur_elem->right;
                break;
        }
        if (cur_elem==NULL){
            break;
        }
        if (cur_elem->isSymb==1 && resultlen<length){
            result[resultlen]=cur_elem->symb;
            resultlen++;
            cur_elem=head;
        }
        switch (symb.mbit.b8) {
            case 0:
                cur_elem=cur_elem->left;
                break;
            case 1:
                cur_elem=cur_elem->right;
                break;
        }
        if (cur_elem==NULL){
            break;
        }
        if (cur_elem->isSymb==1 && resultlen<length){
            result[resultlen]=cur_elem->symb;
            resultlen++;
            cur_elem=head;
        }
    }
    fclose(rasp);
    //printf("dfg %ld %ld %d %d abvgd\n",length,lengthrasp,resultlen,number);

    result[resultlen]='\0';
    //printf("\n%s\n",result);
    FILE * to2 =fopen(from,"wb+");
    fseek(to2, 0, SEEK_SET);
    for (int i=0;i<resultlen;i++){
        fputc(result[i],to2);
    }
    fclose(to2);
    return 0;
}

