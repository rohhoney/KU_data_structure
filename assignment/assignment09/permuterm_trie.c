#include <stdio.h>
#include <stdlib.h>	// malloc
#include <string.h>	// strdup
#include <ctype.h>	// isupper, tolower

#define MAX_DEGREE	27 // 'a' ~ 'z' and EOW
#define EOW			'$' // end of word

// used in the following functions: trieInsert, trieSearch, triePrefixList
#define getIndex(x)		(((x) == EOW) ? MAX_DEGREE-1 : ((x) - 'a'))

// TRIE type definition
typedef struct trieNode {
	int 			index; // -1 (non-word), 0, 1, 2, ...
	struct trieNode	*subtrees[MAX_DEGREE];	//포인터 배열
} TRIE;

////////////////////////////////////////////////////////////////////////////////
// Prototype declarations

/* Allocates dynamic memory for a trie node and returns its address to caller
	return	node pointer
			NULL if overflow
*/
TRIE *trieCreateNode(void);

/* Deletes all data in trie and recycles memory
*/
void trieDestroy( TRIE *root);

/* Inserts new entry into the trie
	return	1 success
			0 failure
*/
// 주의! 엔트리를 중복 삽입하지 않도록 체크해야 함
// 대소문자를 소문자로 통일하여 삽입
// 영문자와 EOW 외 문자를 포함하는 문자열은 삽입하지 않음
int trieInsert( TRIE *root, char *str, int dic_index);

/* Retrieve trie for the requested key
	return	index in dictionary (trie) if key found
			-1 key not found
*/
int trieSearch( TRIE *root, char *str);

/* prints all entries in trie using preorder traversal
*/
void trieList( TRIE *root, char *dic[]);

/* prints all entries starting with str (as prefix) in trie
	ex) "ab" -> "abandoned", "abandoning", "abandonment", "abased", ...
	this function uses trieList function
*/
void triePrefixList( TRIE *root, char *str, char *dic[]);

/* makes permuterms for given str
	ex) "abc" -> "abc$", "bc$a", "c$ab", "$abc"
	return	number of permuterms
*/
int make_permuterms( char *str, char *permuterms[]);

/* recycles memory for permuterms
*/
void clear_permuterms( char *permuterms[], int size);

/* wildcard search
	ex) "ab*", "*ab", "a*b", "*ab*"
	this function uses triePrefixList function
*/
void trieSearchWildcard( TRIE *root, char *str, char *dic[]);

int count_char(const char *str, char ch);

////////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
	TRIE *permute_trie;
	char *dic[100000];

	int ret;
	char str[100];
	FILE *fp;
	char *permuterms[100];
	int num_p; // # of permuterms
	int word_index = 0;
	
	if (argc != 2)
	{
		fprintf( stderr, "Usage: %s FILE\n", argv[0]);
		return 1;
	}
	
	fp = fopen( argv[1], "rt");
	if (fp == NULL)
	{
		fprintf( stderr, "File open error: %s\n", argv[1]);
		return 1;
	}
	
	permute_trie = trieCreateNode(); // trie for permuterm index
	
	while (fscanf( fp, "%s", str) != EOF)
	{	
		num_p = make_permuterms( str, permuterms);
		
		for (int i = 0; i < num_p; i++)
			trieInsert( permute_trie, permuterms[i], word_index);
		
		clear_permuterms( permuterms, num_p);
		
		dic[word_index++] = strdup( str);
	}
	
	fclose( fp);
	
	printf( "\nQuery: ");
	while (fscanf( stdin, "%s", str) != EOF)
	{
		// wildcard search term
		if (strchr( str, '*')) 		//strchr함수는 문자열에서 특정 
		{
			trieSearchWildcard( permute_trie, str, dic); //문자를 찾는 함수이다.
		}
		// keyword search
		else 
		{
			ret = trieSearch( permute_trie, str);
			
			if (ret == -1) printf( "[%s] not found!\n", str);
			else printf( "[%s] found!\n", dic[ret]);
		}
		printf( "\nQuery: ");
	}

	for (int i = 0; i < word_index; i++)
		free( dic[i]);
	
	trieDestroy( permute_trie);
	
	return 0;
}





//////////////////////////////////////////////////////////////////////////////////
//Function Implementation

/* Allocates dynamic memory for a trie node and returns its address to caller
	return	node pointer
			NULL if overflow
*/
TRIE *trieCreateNode(void){
	TRIE *pTrie;
	
	pTrie = (TRIE*)malloc(sizeof(TRIE));
	pTrie -> index = -1;

	for(int i = 0; i < MAX_DEGREE; i++){
		pTrie -> subtrees[i] = NULL;
	} 

	return pTrie;
}

/* Deletes all data in trie and recycles memory
*/
void trieDestroy( TRIE *root){
	if(root == NULL)
		return;
	
	for(int i = 0 ; i < MAX_DEGREE; i++)
		trieDestroy(root->subtrees[i]);
	
	free(root);
	
}

/* Inserts new entry into the trie
	return	1 success
			0 failure
*/
// 주의! 엔트리를 중복 삽입하지 않도록 체크해야 함
// 대소문자를 소문자로 통일하여 삽입
// 영문자와 EOW 외 문자를 포함하는 문자열은 삽입하지 않음
int trieInsert( TRIE *root, char *str, int dic_index){
	TRIE *pLoc;
	int length = strlen(str);

	char letter;
	int index;

	pLoc = root;

	for(int i = 0 ; i < length ; i++){
		letter = tolower(str[i]);
		index = getIndex(letter);
		
		//a-z, '$' 이외의 글자일 때
		if(index < 0 || index >= MAX_DEGREE)
			return 0;

		if(pLoc->subtrees[index] == NULL){
			pLoc -> subtrees[index] = trieCreateNode();
			pLoc = pLoc -> subtrees[index];
		}
		else{

			pLoc = pLoc -> subtrees[index];
		}

		if(i == length -1)
			pLoc -> index = dic_index;
		

	}
	return 1;

}

/* Retrieve trie for the requested key
	return	index in dictionary (trie) if key found
			-1 key not found
*/
int trieSearch( TRIE *root, char *str){
	if(root == NULL)
		return -1;

	TRIE *pLoc;
	int length = strlen(str);
	char input_str[100];

	strcpy(input_str, str);
	input_str[length] = '$';
	input_str[length + 1] = '\0';

	char letter;
	int index;



	pLoc = root;
	length = strlen(input_str);

	for(int i = 0 ; i < length ; i++){
		letter = tolower(*(input_str + i));
		index = getIndex(letter);
		
		//a-z, '$' 이외의 글자일 때
		if(index < 0 || index > MAX_DEGREE)
			return -1;
		//subtree가 NULL이면
		else if(pLoc->subtrees[index] == NULL){
			return -1;
		}
		else{
			pLoc = pLoc -> subtrees[index];
		}

		if(i == length -1)
			return pLoc -> index;
		

	}

	return -1;
}

/* prints all entries in trie using preorder traversal
*/
void trieList( TRIE *root, char *dic[]){
	if(root == NULL)
		return;

	int index = root -> index;
	if(index != -1)
		printf("%s\n", dic[index]);

	for(int i = 0 ; i < MAX_DEGREE; i++)
		trieList(root->subtrees[i], dic);
		

	
}

/* prints all entries starting with str (as prefix) in trie
	ex) "ab" -> "abandoned", "abandoning", "abandonment", "abased", ...
	this function uses trieList function
*/
void triePrefixList( TRIE *root, char *str, char *dic[]){

	TRIE *pLoc;
	int length = strlen(str);

	char letter;
	int index;

	pLoc = root;

	for(int i = 0 ; i < length ; i++){
		letter = tolower(*(str + i));
		index = getIndex(letter);
		
		//a-z, '$' 이외의 글자일대
		if(index < 0 || index >= MAX_DEGREE)
			return;

		if(pLoc->subtrees[index] == NULL){
			return;
		}
		else{

			pLoc = pLoc -> subtrees[index];
		}
		
	}

	trieList(pLoc, dic);

	
}

/* makes permuterms for given str
	ex) "abc" -> "abc$", "bc$a", "c$ab", "$abc"
	return	number of permuterms
*/
int make_permuterms( char *str, char *permuterms[]){
	char input_str[100];
	int length = strlen(str) + 1; // '$'추가로 + 1

	strcpy(input_str, str);


	input_str[length-1] = '$';
	input_str[length] = '\0';

	for(int i = 0; i < length ; i++){
	
		permuterms[i] = (char*)malloc(sizeof(char) * (length + 1));

		for(int j = 0; j < length ; j++){
			int index;
			index = (i + j <= length - 1) ? i + j : i + j - length; 

			*(permuterms[i] + j) = *(input_str + index);
			*(permuterms[i] + length) = '\0';
		}
	
	}

	return length;
}

/* recycles memory for permuterms
*/
void clear_permuterms( char *permuterms[], int size){
	
	for(int i = 0 ; i < size ; i ++){
		free(permuterms[i]);
	}
}

/* wildcard search
	ex) "ab*", "*ab", "a*b", "*ab*"
	this function uses triePrefixList function
*/
void trieSearchWildcard( TRIE *root, char *str, char *dic[]){
	int length = strlen(str);
	char input_str[100];
	char star[2] = "*";

	if(strcmp(str, star) == 0){
		
		input_str[0] = '$';
		input_str[1] = '\0';
		triePrefixList(root, input_str, dic);
	}
	//"*ab*"
	else if(*str == '*' && *(str + length -1) == '*'){
		if(strlen(str)==2) return; // '**' 예외처리

		strncpy(input_str, str + 1, length-2);
		input_str[length-2] = '\0';

		triePrefixList(root, input_str, dic);
	}
	//"*ab"
	else if(*str == '*'){
		strncpy(input_str, str + 1, length-1);	// 앞에 별빼고 문자 복사
		input_str[length-1] = '$';
		input_str[length] = '\0';

		triePrefixList(root, input_str, dic);
	}
	//"ab*"
	else if(*(str + length -1) == '*'){

		strncpy(input_str + 1, str, length-1);	// 앞에 별빼고 문자 복사
		input_str[0] = '$';
		input_str[length] = '\0';

		triePrefixList(root, input_str, dic);
	}
	//"a*b"
	else{
		int star_index;

		if(count_char(str , '*') != 1)
			return;

		star_index = (int)(strchr(str, '*') - str);
		//b$a
		strncpy(input_str, str + star_index + 1, length - star_index - 1);
		input_str[length - star_index - 1] = '$';
		strncpy(input_str + length - star_index, str, star_index);
		input_str[length] = '\0';
		triePrefixList(root, input_str, dic);
	}
	


}

// 문자열 str에서 문자 ch의 개수를 세는 함수
int count_char( const char *str, char ch) {
    int count = 0;
    while (*str) {
        if (*str == ch) {
            count++;
        }
        str++;
    }
    return count;
}
