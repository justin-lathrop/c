// combination of preorder, postorder and inorder traversals can be used to create a BT
// in BST we can get all values in ascending order using inorder traversal
// preorder traversal can duplicate the tree
// postorder traversal can delete the tree





#include <stdio.h>
#include <stdlib.h>


struct node{
    struct node* left;
    int data;
    struct node* right;
};

struct node* createNode(int data){
    struct node* newNode = malloc(sizeof(struct node));
    newNode -> data = data;
    newNode -> left = NULL;
    newNode -> right = NULL;
    return newNode;
}







// USING RECURSION



// preorder traversal
//  root node -> left subtree -> right subtree
void preorder(struct node* root){
    if(root==NULL){
        return;
    }
    printf("%d ", root->data);
    preorder(root->left);
    preorder(root->right);
}

// inorder traversal
// left -> root -> right
void inorder(struct node* root){
    if(root==NULL){
        return;
    }
    inorder(root->left);
    printf("%d ", root->data);
    inorder(root->right);
}

// postorder traversal
// left -> right -> root
void postorder(struct node* root){
    if(root==NULL){
        return;
    }
    postorder(root->left);
    postorder(root->right);
    printf("%d ", root->data);
}





// USING STACKS (without recursion)


// we push the present one in
// then pop it and check for left and right
// push right first then left
// then pop one out and check for left and right
// and so on

// we push and pop addresses and NOT values

// looks like recursion but it is a loop till stack is empty


#define N 100
struct node* stack[N];   // global declaration
int top = -1;

void push(struct node* ptr){
    if(top == N-1){
        printf("stack overflow\n");
    }
    else{
        top++;
        stack[top] = ptr;
    }
}

struct node* pop(){
    if(top == -1){
        printf("stack underflow");
        exit(1);
    }
    struct node* popped;
    popped = stack[top];
    top--;
    return popped;
    
}

int isEmpty(){
    if(top == -1){
        return 1;
    }
    else{
        return 0;
    }
}

void preorderAlt(struct node* root){
    struct node *ptr = root;
    if(ptr == NULL){
        printf("Empty binary tree\n");
        exit(1);
    }
    push(ptr);
    while(!isEmpty()){
        ptr = pop();
        printf("%d ", ptr->data);
        if(ptr->right != NULL){
            push(ptr->right);
        }
        if(ptr->left != NULL){
            push(ptr->left);
        }
    }
}

void inorderAlt(struct node* root){
    struct node *ptr = root;
    while (1){
        while (ptr->left != NULL){
            push(ptr);
            ptr = ptr->left;
        }
        while (ptr->right == NULL){
            printf("%d ", ptr->data);
            if (isEmpty()){
                return;
            }
            ptr = pop();
        }
        printf("%d ", ptr->data);
        ptr = ptr->right;
    }
}

void postorderAlt(struct node* root){
    struct node *ptr = root;
    struct node *temp = root;
    while (1){
        while (ptr->left != NULL){
            push(ptr);
            ptr = ptr->left;
        }
        while (ptr->right == NULL || ptr->right == temp){
            printf("%d ", ptr->data);
            temp = ptr;
            if (isEmpty()){
                return;
            }
            ptr = pop();
        }
        push(ptr);
        ptr = ptr->right;
    }
}





// for level order traversal we use QUEUE
struct node* queue[N];
int front = -1, rear = -1;

int isFullQueue() {
    return rear == N - 1;
}

int isEmptyQueue() {
    return front == -1;
}

void enqueue(struct node* data) {
    if (isFullQueue()) {
        printf("Queue is full. Cannot enqueue.\n");
        return;
    }
    if (front == -1) {
        front = 0;
    }
    queue[++rear] = data;
}

struct node* dequeue() {
    if (isEmptyQueue()) {
        printf("Queue is empty. Cannot dequeue.\n");
        return NULL;
    }
    struct node* removedNode = queue[front];
    front++;
    if (front > rear) {
        front = rear = -1;  // Reset front and rear when the queue is empty
    }
    return removedNode;
}

void levelOrder(struct node* root){
    struct node *ptr = root;
    if(ptr == NULL){
        printf("Empty tree");
        return;
    }
    enqueue(ptr);
    while(!isEmptyQueue()){
        ptr = dequeue();
        printf("%d ", ptr->data);
        if(ptr->left != NULL){
            enqueue(ptr->left);
        }
        if(ptr->right != NULL){
            enqueue(ptr->right);
        }
    }
    
}






int main(){
    struct node* root = NULL;
    root = createNode(1);
    root->left = createNode(2);
    root->right = createNode(3);
    root->left->left = createNode(4);
    root->left->right = createNode(5);
    root->right->right = createNode(6);
    root->left->right->left = createNode(7);
    root->left->right->right = createNode(8);
    root->right->right->left = createNode(9);

    printf("Preorder traversal gives: \n");
    preorder(root);
    printf("\n");

    printf("Inorder traversal gives: \n");
    inorder(root);
    printf("\n");

    printf("Postorder traversal gives: \n");
    postorder(root);
    printf("\n");

    printf("Preorder alt traversal gives: \n");
    preorderAlt(root);
    printf("\n");

    printf("Inorder alt traversal gives: \n");
    inorderAlt(root);
    printf("\n");

    printf("Postorder alt traversal gives: \n");
    postorderAlt(root);
    printf("\n");

    printf("Level order traversal gives: \n");
    levelOrder(root);
    printf("\n");
}