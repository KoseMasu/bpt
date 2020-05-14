#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <iostream>
#include <cmath>
#define N 4
#define DNUM 1000000
using namespace std;

typedef struct node {
	bool isLeaf;
	struct node *child[N+1];
	int key[N]; 
	struct node *parent;
    int keynum;
} NODE;

NODE *Root;

bool inp[DNUM+1];
int inpt=DNUM+1;

void insertParent(NODE *left,NODE *right,int insertkey){
    if(left==Root){
        Root = (NODE *)calloc(1, sizeof(NODE));
        Root->parent = NULL;
        Root->isLeaf = false;
        Root->key[0] = insertkey;
        Root->child[0] = left;
        Root->child[1] = right;
        Root->keynum = 1;
        left->parent = Root;
        right->parent = Root;
        return;
    }
    else{
        int key = insertkey;
        NODE *toInsert = left->parent;
        right->parent = left->parent;
        toInsert->child[toInsert->keynum+1] = toInsert->child[toInsert->keynum];
        if(key > toInsert->key[toInsert->keynum-1]){
            toInsert->key[toInsert->keynum] = key;
            toInsert->child[toInsert->keynum+1] = right;
            toInsert->keynum++;
        }
        else{
            for(int i=0;i<toInsert->keynum;i++){
                toInsert->key[toInsert->keynum-i] = toInsert->key[toInsert->keynum-i-1];
                toInsert->child[toInsert->keynum-i+1] = toInsert->child[toInsert->keynum-i];
                if(key > toInsert->key[toInsert->keynum - i]){
                    toInsert->key[toInsert->keynum-i] = key;
                    toInsert->child[toInsert->keynum-i+1] = right;
                    toInsert->keynum++;
                    return;
                }
            }
            toInsert->key[0] = key;
            toInsert->child[1] = right;
            toInsert->keynum++;
            return;
        }
    }
}
void splitnonLeaf(NODE *spnode){
    NODE *New = (NODE *)calloc(1, sizeof(NODE));
    New->keynum = 0;
    New->isLeaf = false;
    int key = spnode->key[(int)ceil(N/2)];
    spnode->key[(int)ceil(N/2)] = 0;
    spnode->keynum--;
    for(int i=(int)ceil(N/2);i<N-1;i++){
        New->key[i-(int)ceil(N/2)] = spnode->key[i+1];
        New->child[i-(int)ceil(N/2)] = spnode->child[i+1];
        New->keynum++;
        spnode->child[i+1]->parent=New;
        spnode->key[i+1]=0;
        spnode->child[i+1]=NULL;
        spnode->keynum--;
    }
    New->child[New->keynum] = spnode->child[N];
    spnode->child[N]->parent=New;
    spnode->child[N] = NULL;
    //spnode->child[spnode->keynum] = New;

    //insertParent
    insertParent(spnode,New,key);
    if(spnode->parent->keynum >= N){
        splitnonLeaf(spnode->parent);
    }
}

void splitLeaf(NODE *leaf){
    NODE *New = (NODE *)calloc(1, sizeof(NODE));
    New->keynum = 0;
    New->isLeaf = true;
    New->parent = leaf->parent;
    for(int i=(int)ceil(N/2);i<N;i++){
        New->key[i-(int)ceil(N/2)] = leaf->key[i];
        New->child[i-(int)ceil(N/2)] = leaf->child[i];
        New->keynum++;
        leaf->key[i]=0;
        leaf->child[i]=NULL;
        leaf->keynum--;
    }
    New->child[New->keynum] = leaf->child[N];
    leaf->child[leaf->keynum] = New;

    //insertParent
    insertParent(leaf,New,New->key[0]);

    //nonleafsplit
    if(leaf->parent->keynum >= N){
        splitnonLeaf(leaf->parent);
    }
}

void insertLeaf(NODE *toInsert,int a,node *data){

    if(toInsert->keynum == 0){
        toInsert->key[0] = a;
        toInsert->child[0] = data;
        toInsert->keynum++;
        return;
    }
    toInsert->child[toInsert->keynum+1] = toInsert->child[toInsert->keynum];
    if(a > toInsert->key[toInsert->keynum-1]){
        toInsert->key[toInsert->keynum] = a;
        toInsert->child[toInsert->keynum] = data;
        toInsert->keynum++;
        return;
    }
    else{
        for(int i=0;i<toInsert->keynum;i++){
            toInsert->key[toInsert->keynum-i] = toInsert->key[toInsert->keynum-i-1];
            toInsert->child[toInsert->keynum-i] = toInsert->child[toInsert->keynum-i-1];
            if(a > toInsert->key[toInsert->keynum - i]){
                toInsert->key[toInsert->keynum-i] = a;
                toInsert->child[toInsert->keynum-i] = data;
                toInsert->keynum++;
                return;
            }
        }
        toInsert->key[0] = a;
        toInsert->child[0] = data;
        toInsert->keynum++;
        return;
    }
}

NODE * serch(NODE *looking,int a){
    if(looking->isLeaf==true){
        return looking;
    }
    for(int i=0;i<looking->keynum;i++){
        if(a<looking->key[i]){
            return serch(looking->child[i] , a);
        }
    }
    return serch(looking->child[looking->keynum] , a);
}

void insert(int a){
    //nodeをserch
    NODE *toInsert = serch(Root,a);

    for(int i=0;i<toInsert->keynum;i++){
        if(toInsert->key[i]==a){
            cout<<a<<" already exist."<<endl;
            return;
        }
    }
    
    //insert
    insertLeaf(toInsert,a,NULL);

    //split
    if(toInsert->keynum >= N){
        splitLeaf(toInsert);
    }
    
}
void deleteKey(NODE *looking,int a,node *data){
    int i;
    for(i=0;i<looking->keynum;i++){
        if(looking->key[i] == a)break;
    }
    for(;i<looking->keynum;i++){
        looking->key[i]=looking->key[i+1];
    }
    for(i=0;i<looking->keynum;i++){
        if(looking->child[i] == data)break;
    }
    for(;i<looking->keynum+1;i++){
        looking->child[i]=looking->child[i+1];
    }
    looking->keynum--;
    if(Root==looking&&looking->keynum==0){
        if(looking->isLeaf==true){
            return;
        }
        Root = looking->child[0];
        Root->parent = NULL;
        if(Root->isLeaf==false){
            for(i=0;i<Root->keynum;i++){
                Root->child[i]->parent=Root;
            }
        }
        free(looking);
        return;
    }
    else if((ceil((N-1)/2)>=looking->keynum && looking->isLeaf==true) || (ceil(N/2)>looking->keynum+1 && looking->isLeaf==false)){
        if(Root==looking&&looking->isLeaf==true){
            return;
        }
        NODE *brother;
        int keyInd;
        bool reverse=false;
        for(i=0;i<looking->parent->keynum+1;i++){
            if(looking->parent->child[i]==looking){
                if(i!=0){
                    brother = looking->parent->child[i-1];
                    keyInd = i-1;
                    break;
                }
                else{
                    brother = looking->pare\nt->child[i+1];
                    keyInd = i;
                    reverse = true;
                    break;
                }
            }
        }
        //合体
        if(looking->keynum + brother->keynum < N && (looking->isLeaf==true || brother->keynum+1<N)){
            if(reverse==true){
                brother = looking;
                looking = brother->parent->child[keyInd+1];
            }
            if(brother->isLeaf==false){
                brother->key[brother->keynum]=brother->parent->key[keyInd];
                brother->keynum++;
            }
            for(i=0;i<looking->keynum;i++){
                brother->key[brother->keynum+i]=looking->key[i];
                brother->child[brother->keynum+i]=looking->child[i];
            }
            brother->child[brother->keynum+i]=looking->child[i];
            brother->keynum+=i;
            if(brother->isLeaf==false){
                for(i=0;i<=brother->keynum;i++){
                    brother->child[i]->parent=brother;
                }
            }
            deleteKey(looking->parent,looking->parent->key[keyInd],looking);
            free(looking);
            return;
        }
        //再分配
        else{
            if(reverse==false){//brotherは前
                looking->child[looking->keynum+1]=looking->child[looking->keynum];
                for(i=looking->keynum;i>0;i--){
                    looking->child[i]=looking->child[i-1];
                    looking->key[i]=looking->key[i-1];
                }
                if(looking->isLeaf==false){
                    looking->child[0]=brother->child[brother->keynum];
                    looking->key[0]=looking->parent->key[keyInd];
                    looking->child[0]->parent=looking;
                    looking->keynum++;
                    looking->parent->key[keyInd]=brother->key[brother->keynum-1];
                    brother->key[brother->keynum-1]=0;
                    brother->child[brother->keynum]=NULL;
                    brother->keynum--;
                }
                else{
                    looking->child[0]=brother->child[brother->keynum-1];
                    looking->key[0]=brother->key[brother->keynum-1];
                    looking->keynum++;
                    looking->parent->key[keyInd]=brother->key[brother->keynum-1];
                    brother->key[brother->keynum-1]=0;
                    brother->child[brother->keynum-1]=brother->child[brother->keynum];
                    brother->keynum--;
                }
            }
            else{//brotherは後ろ
                if(looking->isLeaf==false){
                    looking->key[looking->keynum]=looking->parent->key[keyInd];
                    looking->child[looking->keynum+1]=brother->child[0];
                    looking->keynum++;
                    looking->child[looking->keynum]->parent=looking;
                    looking->parent->key[keyInd]=brother->key[0];
                }
                else{
                    looking->key[looking->keynum]=brother->key[0];
                    looking->child[looking->keynum+1]=looking->child[looking->keynum];
                    looking->child[looking->keynum]=brother->child[0];
                    looking->keynum++;
                    looking->parent->key[keyInd]=brother->key[1];
                }
                for(i=1;i<brother->keynum;i++){
                    brother->key[i-1]=brother->key[i];
                    brother->child[i-1]=brother->child[i];
                }
                brother->child[i-1]=brother->child[i];
                brother->keynum--;
            }
        }
    }
}

void deletion(int a){
    NODE *looking = serch(Root,a);
    for(int i=0;i<looking->keynum;i++){
        if(looking->key[i]==a){
            deleteKey(looking,a,NULL);
            return;
        }
    }
    cout<<a<<" does not exist."<<endl;
    return;
}

int input(){
    // int a=rand()%30+1;

    // while(inp[a]==true){
    //     a++;
    //     if(a==DNUM){
    //         a=1;
    //     }
    // }

    // inp[a]=true;
    int a;
    cin>>a;
    return a;
}

void printTree(NODE *looking){

    if(looking->isLeaf==true){
        cout<<"[";
        for(int i=0;i<looking->keynum;i++){
            cout<<looking->key[i]<<",";
        }
        cout<<"]";
        return;
    }
    else{
        cout<<"[";
        for(int i=0;i<=looking->keynum;i++){
            printTree(looking->child[i]);
            if(i!=looking->keynum)cout<<looking->key[i];
        }
        cout<<"]";
    }
}
bool keyserch(int a){
    NODE *looking = serch(Root,a);
    for(int i=0;i<looking->keynum;i++){
        if(looking->key[i]==a)return true;
    }
    return false;
}
void swap(int *a,int *b){
    int c=*a;
    *a=*b;
    *b=c;
}

int main(){
    Root = (NODE *)calloc(1, sizeof(NODE));
    Root->isLeaf = true;
    Root->parent = NULL;
    Root->keynum = 0;
    
    int count=10;


    /*100万件のデータを挿入して削除*/
    /*
    int num = 100;
    int list[num];
    for(int i=0;i<num;i++){
        list[i]=i+1;
    }
    for(int i=0;i<num*100;i++){
        swap(&list[rand()%num],&list[rand()%num]);
    }
    // for(int i=0;i<num;i++){
    //     cout<<list[i]<<" ";
    // }
    // cout<<endl;
    for(int i=0;i<num;i++){
        insert(list[i]);
    }
    cout<<endl;
    for(int i=0;i<num;i++){
        if(!keyserch(list[i])){
            cout<<list[i]<<"losted";
        }
    }
    cout<<"all exist"<<endl;
    //printTree(Root);
    //cout<<endl;
    for(int i=0;i<num*100;i++){
        swap(&list[rand()%num],&list[rand()%num]);
    }
    int rmnum=num/2;
    for(int i=0;i<rmnum;i++){
        deletion(list[i]);
        // printTree(Root);
        // cout<<endl;
    }
    //printTree(Root);
    cout<<endl<<"all rm"<<endl;
    for(int i=0;i<num*100;i++){
        swap(&list[rand()%rmnum],&list[rand()%rmnum]);
    }
    for(int i=0;i<rmnum;i++){
        insert(list[i]);
    }
    for(int i=0;i<num;i++){
        if(!keyserch(list[i])){
            cout<<list[i]<<"losted";
        }
    }
    printTree(Root);
    cout<<"all exist"<<endl;
    return 0;*/
    


    int i=0;
    //int x=0,y=0;
    while(1){
        int a = rand()%DNUM+1;
        if(a>DNUM){
            cout<<"ERR";
            break;;
        }
        cout<<"in "<<a<<endl;
        // if(a==12){
        //     x++;
        // }
        
        insert(a);
        //printTree(Root);
        a = rand()%DNUM+1;
        if(a>DNUM){
            cout<<"ERR";
            break;;
        }
        cout<<"del "<<a<<endl;
        // if(a==12){
        //     y++;
        // }
        // cout<<"x="<<x<<",y="<<y<<endl;
        // if(x==26428&&y==26092){
        //     cout<<x;
        // }
        //printTree(Root);
        
        deletion(a);
        //printTree(Root);
        
    }
    //printTree(Root);
    // int in;
    // while(1){
    //     cin>>in;
    //     if(keyserch(in)==true)cout<<"true";
    //     else cout<<"false";
    // }
    // for(int i=1;i<=DNUM;i++){
    //     if(keyserch(i)==false)cout<<"false\n";
    // }
    //printTree(Root);
    cout<<"fin\n";
    

    return 0;
}