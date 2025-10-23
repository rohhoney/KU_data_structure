#include <stdlib.h> // malloc
#include <stdio.h>

#include "bst.h"

// internal functions (not mandatory)
// used in BST_Insert
static void _insert( NODE *root, NODE *newPtr, int (*compare)(const void *, const void *));

// used in BST_Insert
static NODE *_makeNode( void *dataInPtr);

// used in BST_Destroy
static void _destroy( NODE *root, void (*callback)(void *));

// used in BST_Delete
// return 	pointer to root
static NODE *_delete( NODE *root, void *keyPtr, void **dataOutPtr, int (*compare)(const void *, const void *));

// used in BST_Search
// Retrieve node containing the requested key
// return	address of the node containing the key
//			NULL not found
static NODE *_search( NODE *root, void *keyPtr, int (*compare)(const void *, const void *));

// used in BST_Traverse
static void _traverse( NODE *root, void (*callback)(const void *));

// used in BST_TraverseR
static void _traverseR( NODE *root, void (*callback)(const void *));

// used in printTree
static void _inorder_print( NODE *root, int level, void (*callback)(const void *));

static NODE *_smallest_node( NODE *root);


// internal functions 구현
// used in BST_Insert, 노드가 인수로 들어간다
static void _insert( NODE *root, NODE *newPtr, int (*compare)(const void *, const void *)){
	if(compare(newPtr->dataPtr, root->dataPtr) < 0 ){
		if(root->left == NULL)
			root->left = newPtr;
		else _insert(root->left, newPtr, compare);
	}
	else{
		if(root->right == NULL)
			root->right = newPtr;
		else
			_insert(root->right, newPtr, compare);
	}
}

// used in BST_Insert
static NODE *_makeNode( void *dataInPtr){

	NODE *pNODE;
	pNODE = (NODE*)malloc(sizeof(NODE));

	pNODE -> dataPtr = dataInPtr;
	pNODE -> left = NULL;
	pNODE -> right = NULL;

	return pNODE;
}

// used in BST_Destroy
static void _destroy( NODE *root, void (*callback)(void *)){
	if(root!=NULL){
		_destroy(root->left, callback);
		_destroy(root->right, callback);
		callback(root->dataPtr);
		free(root);
	}


}
//smallest_node, _delete에 사용
static NODE *_smallest_node( NODE *root){
	if(root == NULL)
		return NULL;
	if(root -> left == NULL)
		return root;
	return _smallest_node(root -> left);
}

// used in BST_Delete
// return 	pointer to root
static NODE *_delete( NODE *root, void *keyPtr, void **dataOutPtr, 
						int (*compare)(const void *, const void *)){

	NODE *delete_node;	//삭제할 노드를 가리킬 포인터


	if(root == NULL)
		return NULL;

	if(compare(keyPtr, root->dataPtr) < 0){
		//현재 root의 왼쪽 자식이 지워야 할 노드일 때
		if(root->left == NULL)
			return NULL;
		if(compare(keyPtr,root->left->dataPtr) == 0)
		{
			delete_node = root->left;
			//지워야할 노드에게 오른쪽 자식이 없을때, 그것의 왼쪽 자식을 현 루트의 왼쪽에 연결한다.
			if (delete_node->right == NULL){

				*dataOutPtr = delete_node -> dataPtr;
				root -> left = delete_node -> left;
				return delete_node;
			}
			//지워야할 노드에게 왼쪽 자식이 없을 때, 그것의 오른쪽 자식을 현 루트의 왼쪽에 연결한다.
			else if (delete_node->left == NULL){

				*dataOutPtr = delete_node -> dataPtr;
				root -> left = delete_node -> right;
				return delete_node;
			}
			//지워야할 노드에게 자식이 왼쪽 오른쪽 둘다 있을 때, 지울 노드의 오른쪽 서브트리에서 가장 작은 값과 위치를 바꾸고 지움.
			else{

				NODE *smallest_node;
				*dataOutPtr = delete_node -> dataPtr;
					
				smallest_node = _smallest_node(delete_node->right);
				delete_node -> dataPtr = smallest_node -> dataPtr;
				
				void *temp;
				if(compare(smallest_node->dataPtr, delete_node->right->dataPtr)==0){
					if(smallest_node->right == NULL){
						delete_node->right = NULL;
						return smallest_node;
					}
					else{
						delete_node->right = smallest_node->right;
						return smallest_node;
					}
				}
				else
					return _delete(delete_node -> right, smallest_node -> dataPtr, &temp, compare);


			}
		}
		else
			return _delete(root->left, keyPtr, dataOutPtr, compare);//왼쪽 자식이 지워야할 노드가 아니면 재귀적으로
	}
	//현재 루트의 오른쪽 자식이 지워야할 노드일 때
	else if(compare(keyPtr, root->dataPtr) > 0){
		if(root->right == NULL)
			return NULL;
		//현재 root의 왼쪽 자식이 지워야 할 노드일 때
		if(compare(keyPtr,root->right->dataPtr) == 0)
		{
			delete_node = root->right;
			//지워야할 노드에게 오른쪽 자식이 없을때, 그것의 왼쪽 자식을 현 루트의 왼쪽에 연결한다.
			if (delete_node->right == NULL){

				*dataOutPtr = delete_node -> dataPtr;
				root -> right = delete_node -> left;
				return delete_node;
			}
			//지워야할 노드에게 왼쪽 자식이 없을 때, 그것의 오른쪽 자식을 현 루트의 왼쪽에 연결한다.
			else if (delete_node->left == NULL){

				*dataOutPtr = delete_node -> dataPtr;
				root -> right = delete_node -> right;
				return delete_node;
			}
			//지워야할 노드에게 자식이 왼쪽 오른쪽 둘다 있을 때, 지울 노드의 오른쪽 서브트리에서 가장 작은 값의 데이터 위치를 바꾸고 지움.
			else{

				NODE *smallest_node;
				*dataOutPtr = delete_node -> dataPtr;
					
				smallest_node = _smallest_node(delete_node->right);
				delete_node -> dataPtr = smallest_node -> dataPtr;
				
				/****...근데 smallest_node가 delete_node->right이면 또 예외 처리 해줘야한다...****/
				void *temp;

				if(compare(smallest_node->dataPtr, delete_node->right->dataPtr)==0){
					if(smallest_node->right == NULL){
						delete_node->right = NULL;
						return smallest_node;
					}
					else{
						delete_node->right = smallest_node->right;
						return smallest_node;
					}
				}
				else
					return _delete(delete_node -> right, smallest_node -> dataPtr, &temp, compare);


			}
		}
		else
			return _delete(root->right, keyPtr, dataOutPtr, compare);//오른쪽 자식이 지워야할 노드가 아니면 재귀적으로
	}
	//현재 루트가 지워야할 노드일 때, 딱 처음일때만 해당됨.
	else if(compare(keyPtr, root->dataPtr) == 0){
		return root;
	}


}

// used in BST_Search
// Retrieve node containing the requested key
// return	address of the node containing the key
//			NULL not found
static NODE *_search( NODE *root, void *keyPtr, int (*compare)(const void *, const void *)){
	if (root == NULL)
		return NULL;

	if(compare(keyPtr, root->dataPtr)<0)
		return _search(root->left, keyPtr, compare);
	else if(compare(keyPtr, root->dataPtr)>0)
		return _search(root->right, keyPtr, compare);
	else if(compare(keyPtr, root->dataPtr)==0)
		return root;
}

// used in BST_Traverse
static void _traverse( NODE *root, void (*callback)(const void *)){
	if (root != NULL){
		_traverse(root->left, callback);
		callback(root->dataPtr);
		_traverse(root->right, callback);
	}
}


// used in BST_TraverseR
static void _traverseR( NODE *root, void (*callback)(const void *)){
	if(root != NULL){
		_traverseR(root->right, callback);
		callback(root->dataPtr);
		_traverseR(root->left, callback);
	}
}

// used in printTree
static void _inorder_print( NODE *root, int level, void (*callback)(const void *)){
	if(root != NULL){
		level++;
		_inorder_print(root->right, level, callback);
		
		level--;
		for(int i = 0; i < level; i++)
			printf("\t");
		callback(root->dataPtr);

		level++;
		_inorder_print(root->left, level, callback);
	}
}


////////////////////////////////////////////////////////////////////////////////
// bst.h, TREE가 인수로 들어간다.

/* Allocates dynamic memory for a tree head node and returns its address to caller
	return	head node pointer
			NULL if overflow
*/
TREE *BST_Create( int (*compare)(const void *, const void *)){

	TREE *pTree;
	
	pTree = (TREE*)malloc(sizeof(TREE));

	pTree -> count = 0;
	pTree -> root = NULL;
	pTree -> compare = compare;

	return pTree;
}

/* Deletes all data in tree and recycles memory
*/
void BST_Destroy( TREE *pTree, void (*callback)(void *)){
	_destroy(pTree->root, callback);
	free(pTree);
}

/* Inserts new data into the tree
	return	1 success
			0 overflow
*/
int BST_Insert( TREE *pTree, void *dataInPtr){
	
	NODE *pNODE;
	pNODE = _makeNode(dataInPtr);

	if(pTree->root == NULL)
		pTree->root = pNODE;
	else
		_insert(pTree->root, pNODE, pTree->compare);

	pTree -> count++;
	
	return 1;

}

/* Deletes a node with keyPtr from the tree
	return	address of data of the node containing the key
			NULL not found
*/
void *BST_Delete( TREE *pTree, void *keyPtr){
	
	void *ptr;
	NODE *pNODE;	
	pNODE = _delete(pTree->root, keyPtr, &ptr, pTree->compare);
	
	if (pNODE ==NULL)
		return NULL;
	//만약 지워야하는게 메인트리의 루트라면
	if(pTree->compare(pNODE->dataPtr, pTree->root->dataPtr) == 0){

		ptr = pNODE -> dataPtr;
		//지워야할 노드에게 오른쪽 자식이 없을때, 그것의 왼쪽 자식을 현 루트의 왼쪽에 연결한다.
		if (pNODE->right == NULL){
			pTree -> root = pNODE -> left;	//tree구조체의 루트가 가르키는게 바뀐다.
		}
		//지워야할 노드에게 왼쪽 자식이 없을 때, 그것의 오른쪽 자식을 현 루트의 왼쪽에 연결한다.
		else if (pNODE->left == NULL){
			pTree -> root = pNODE -> right;
		}
		//지워야할 노드에게 자식이 왼쪽 오른쪽 둘다 있을 때, 지울 노드의 오른쪽 서브트리에서 가장 작은 값과 위치를 바꾸고 지움.
		else{
				NODE *smallest_node;
				void *temp;

				smallest_node = _smallest_node(pNODE->right);
				pNODE -> dataPtr = smallest_node -> dataPtr;

				if(pTree->compare(smallest_node->dataPtr, pNODE->right->dataPtr)==0){
					if(smallest_node->right == NULL){
						pNODE->right = NULL;
						pNODE = smallest_node;
					}
					else{
						pNODE->right = smallest_node->right;
						pNODE = smallest_node;
					}
				}
				else
					pNODE = _delete(pNODE -> right, smallest_node -> dataPtr, &temp, pTree->compare);


		}


	}

		free(pNODE);
		pTree -> count--;
	


	return ptr;

}

/* Retrieve tree for the node containing the requested key (keyPtr)
	return	address of data of the node containing the key
			NULL not found
*/
void *BST_Search( TREE *pTree, void *keyPtr){
	NODE *pNODE;

	pNODE = _search(pTree->root, keyPtr, pTree->compare);
	if(pNODE == NULL)
		return NULL;
	return pNODE -> dataPtr;
}

/* prints tree using inorder traversal
*/
void BST_Traverse( TREE *pTree, void (*callback)(const void *)){
	_traverse(pTree->root, callback);
}

/* prints tree using right-to-left inorder traversal
*/
void BST_TraverseR( TREE *pTree, void (*callback)(const void *)){
	_traverseR(pTree->root, callback);
}

/* Print tree using right-to-left inorder traversal with level
*/
void printTree( TREE *pTree, void (*callback)(const void *)){

	_inorder_print(pTree->root, 0, callback);
}

/*
	returns number of nodes in tree
*/
int BST_Count( TREE *pTree){
	return pTree->count;
}


