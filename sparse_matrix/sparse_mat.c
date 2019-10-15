#include <stdio.h>
#include <stdlib.h>

typedef struct mlnode{
    int row, col, value;
    struct mlnode* nextcol;
    struct mlnode* nextrow;
} node;

node* insert(node* head, int r, int c, int v);
node *above(node *head, int r, int c);
void display(node* head);
node *left(node *head, int r, int c);
node *delete(node* head, int r, int c);
void swapRow(node *head, int row1, int row2);

int main() {
    //defining head node
    node *head = (node *) malloc(sizeof(node));
    head->row = -1;
    head->col = -1;
    head->value = -1;


    int col;
    int row = col = 5;
    node *p = head;

    //making row header of matrix
    for (int i = 0; i < row; i++) {
        //printf("in row loop %d\n", i);
        node* q;
        q = (node*) malloc(sizeof(node));
        p->nextrow = q;
        q->row = i;
        q->col = -1;
        q->value = -1;
        q->nextcol = q;
        p = q;
        //printf("done row loop %d\n", i);
    }
    p->nextrow = head;
    p = head;

    //making column head of matrix
    for (int i = 0; i < col; i++) {
        //printf("in col loop %d\n", i);
        node* q;
        q = (node*) malloc(sizeof(node));
        p->nextcol = q;
        q->row = -1;
        q->col = i;
        q->value = -1;
        q->nextrow = q;
        p = q;
        //printf("in col loop %d\n", i);
    }
    p->nextcol = head;
    int choice = 0;
    //menu starts
    while (choice != 4) {
        printf("Enter a choice\n1. Insert\n2. Delete\n3. Display\n4. Exit\n5. swap rows\n==> ");

        scanf("%d", &choice);
        int r, colm, val;
        int sw1, sw2;
        switch (choice) {
            case 1:
                printf("Enter row value: ");
                scanf("%d", &r);
                printf("Enter column value: ");
                scanf("%d", &colm);
                printf("Enter value to insert: ");
                scanf("%d", &val);
                head = insert(head, r, colm, val);
                break;
            case 2:
                printf("Enter row value: ");
                scanf("%d", &r);
                printf("Enter column value: ");
                scanf("%d", &colm);
                head = delete(head, r, colm);
                break;
            case 3:
                display(head);
                break;
            case 5: printf("Enter row1: ");
                    scanf("%d", &sw1);
                    printf("Enter row2: ");
                    scanf("%d", &sw2);
                    swapRow(head, sw1, sw2);
                break;
            default:
                break;

        }
        if (choice == 4) break;
    }
}

node* insert(node* head, int r, int c, int v){
    node* q, *a, *l;
    q = (node*) malloc(sizeof(node));
    q->row = r;
    q->col = c;
    q->value = v;

    a = above(head, r, c);
    l = left(head, r, c);
    //changing pointers
    q->nextrow = a->nextrow;
    q->nextcol = l->nextcol;
    a->nextrow = q;
    l->nextcol = q;

    return head;
}

node *left(node *head, int r, int c) {
    node* p = head->nextrow, *q;
    //column traversal or horizontal traversal
    while (p->row != r){
        p = p->nextrow;
    }

    //row traversal or vertical
    do{
        q = p;
        p = p->nextcol;
    }while (p->col < c && p->col != -1);

    return q;
}

node *above(node *head, int r, int c) {
    node* p = head, *q;  // did change from node*p = head->nextcol to this bcz
                        // for row swap in empty row case i need above node for header
    while (p->col != c){
        p = p->nextcol;
    }
    do{
        q = p;
        p = p->nextrow;
    }while (p->row < r && p->row != -1);

    return q;
}

node* delete(node* head, int r, int c){
    node* q, *a, *l;
    a = above(head, r, c);
    l = left(head, r, c);

    q = l->nextcol;
    a->nextrow = q->nextrow;
    l->nextcol = q->nextcol;
    printf("%d deleted\n", q->value);
    free(q);

    return head;
}

void display(node* head){
    node* p = head->nextrow, *q;
    while(p != head){
        q = p->nextcol;
        while(q != p){
            printf("row: %d col: %d value: %d \n", q->row, q->col, q->value);
            q = q->nextcol;
        }
        printf("\n");
        p = p->nextrow;
    }
}


void swapRow(node* head, int row1, int row2){
    if(row1 == row2) {printf("Both rows are same!"); return;}
    node* rhead1 = head->nextrow;
    node* rhead2 = head->nextrow;

    //getting row headers of given two rows
    while(rhead1->row != row1 && rhead1->row != -1){
        rhead1 = rhead1->nextrow;
    }
    while (rhead2->row != row2 && rhead2->row != -1){
        rhead2 = rhead2->nextrow;
    }

    node* it1, *it2; // iterator pointer of rows. i.e. this pointer will traverse in rows
    it1 = rhead1->nextcol;
    it2 = rhead2->nextcol;

    while (it1->col != -1 && it2->col != -1){
        if(it1->col == it2->col){
            int temp = it2->value;
            it2->value = it1->value;
            it1->value = temp;
            it1 = it1->nextcol;
            it2 = it2->nextcol;
        }
        else if(it1->col > it2->col){
            head = insert(head, it1->row, it2->col, it2->value);
            node* next = it2->nextcol;
            head = delete(head, it2->row, it2->col);
            it2 = next;
        }
        else{
            head = insert(head, it2->row, it1->col, it1->value);
            node* next = it1->nextcol;
            head = delete(head, it1->row, it1->col);
            it1 = next;
        }
    }
    if(it2->col == -1){
        while (it1->col != -1){
            head = insert(head, it2->row, it1->col, it1->value);
            node* next = it1->nextcol;
            head = delete(head, it1->row, it1->col);
            it1 = next;
        }
    }
    else{
        while(it2->col != -1){
            head = insert(head, it1->row, it2->col, it2->value);
            node* next = it2->nextcol;
            head = delete(head, it2->row, it2->col);
            it2 = next;
        }
    }






}

/*void swapRow(node* head, int row1, int row2){
    if(row1 == row2) printf("Both rows are same!");
    //if(row2 < row1){
    //    int temp = row1;
    //    row1 = row2;
    //    row2 = temp;
    //}
    node* rhead1 = head->nextrow;
    node* rhead2 = head->nextrow;

    while(rhead1->row != row1 && rhead1->row != -1){
        rhead1 = rhead1->nextrow;
    }
    while (rhead2->row != row2 && rhead2->row != -1){
        rhead2 = rhead2->nextrow;
    }

    node* rend1, *rend2;
    node *p = rhead1;
    do{
        rend1 = p;
        p = p->nextcol;
    }while (p == rhead1);
    p = rhead2;
    do{
        rend2 = p;
        p = p->nextcol;
    }while (p == rhead2);

    if(rhead1 == rend1){
        rend2->nextcol = rhead1;
        rhead1->nextcol = rhead2->nextcol;
        rhead2->nextcol = rhead2;
    }
    else if(rhead2 == rend2){
        rend1->nextcol = rhead2;
        rhead2->nextcol = rhead1->nextcol;
        rhead1->nextcol = rhead1;
    }
    else{
        node* r1, *r2;
        r1 = rhead1->nextcol;
        r2 = rhead2->nextcol;
        rhead1->nextcol = r2;
        rhead2->nextcol = r1;
        rend1->nextcol = rhead2;
        rend2->nextcol = rhead1;
    }
}*/
