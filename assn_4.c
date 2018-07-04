#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

typedef struct{
	int n;
	int *key;
	long *child;
}btree_node;

int order;
char filename[100];
btree_node *node;
void add_key(int key_value);
int search(int key_value);
long search_add(int key_value, long prev, btree_node node1);
int cmpfuncs (const void * a, const void * b);
void add_key_to_node(int offset, int key);
void add_key_to_node_new(int offset, int key);
long* split_node(int offset, int key);
long* split_node_between(int offset, long *a);
void search_after_split(long prev, long *a);
int search_into(int key_value, btree_node node1);
long search_parent(long key_value, long prev, btree_node node1);
void print_all();


int main(int argc, char *argv[]){
	order = atoi(argv[2]);
	strcpy(filename, argv[1]);
	char input[100];
	while(1 > 0){
		scanf("%s",input);
		if(strcmp(input, "find") == 0){
			char key[100];
			scanf("%s",key);
			int p = search(atoi(key));
			if( p == 1)	printf("Entry with key=%s exists\n",key);
			else	printf("Entry with key=%s does not exist\n",key);
		}
		else if(strcmp(input, "add") == 0){
			char key[100];
            scanf("%s",key);
			add_key(atoi(key));
		}
		else if(strcmp(input, "print") == 0){
			print_all();
		}
		else if(strcmp(input, "end") == 0){
			break;
		}
	}
	return 0;
}

int cmpfuncs (const void * a, const void * b) {
		int * p = (int *)a;
		int * q = (int *)b;
		if( *p > *q)	return 1;
		else if( *p == *q)	return 1;
		else return 0;
}

void add_key(int key_value){
	FILE *fp;
	long root;
	fp = fopen( filename, "r+b" );
	if ( fp == NULL ) { 
		root = -1;
		fp = fopen( filename, "w+b" );
		fwrite( &root, sizeof( long ), 1, fp );
	} 
	else { 
		fread( &root, sizeof( long ), 1, fp ); 
	}
	if(root == -1){
		int n = 1, i;
		long nn[order];
		for( i = 0; i < order; i++)	nn[i] = -1;
		FILE *file_fp = fopen(filename, "r+b");
		fseek(fp, 0, SEEK_SET);
		fwrite("8", sizeof(long), 1, fp);
		fwrite(&n, sizeof(int), 1, fp);
		fwrite(&key_value, sizeof( int ), order - 1, fp);
		fwrite(nn, sizeof( long ), order, fp);
		fclose(fp);
	}
	else{
		FILE *file_fp = fopen(filename, "r+b");
		fseek(file_fp, 8, SEEK_SET);
		btree_node node1;
		node1.n = 0; 
		node1.key = (int *) calloc( order - 1, sizeof( int ) );
		node1.child = (long *) calloc( order, sizeof( long ) );
		fread(&node1.n, sizeof( int ), 1, file_fp);
		fread(node1.key, sizeof( int ), order - 1, file_fp);
		fread(node1.child, sizeof( long ), order, file_fp);
		fclose(file_fp);
		long offset = search_add(key_value, -3, node1);
		if(offset == -5)	printf("Entry with key=%d already exists\n",key_value);
		else if(offset == -3)	add_key_to_node(8, key_value);
		else if(offset == -10){}
		else	add_key_to_node(offset, key_value);
		fclose(fp);
	}
}

long search_add(int key_value, long prev, btree_node node1){
	long offset = 0;
	int s = 0;
	while(s < (node1.n) && s < (order - 1)){
		if(key_value == node1.key[s])	return -5;
		else if(key_value < node1.key[s])	break;
		else s++;
	}
	if(node1.child[s] != -1){
		FILE *file_fp = fopen(filename, "r+b");
		fseek(file_fp, node1.child[s], SEEK_SET);
		btree_node node2;
		node2.n = 0; 
		node2.key = (int *) calloc( order - 1, sizeof( int ) );
		node2.child = (long *) calloc( order, sizeof( long ) );
		fread(&node2.n, sizeof( int ), 1, file_fp);
		fread(node2.key, sizeof( int ), order - 1, file_fp);
		fread(node2.child, sizeof( long ), order, file_fp);
		fclose(file_fp);
		return search_add(key_value, node1.child[s], node2);
	}
	else{
		if(node1.n == (order - 1)){
			long *a;
			if(prev == -3){
				a = split_node(8, key_value);
				FILE *file_fp = fopen(filename, "r+b");
				fseek(file_fp, 8, SEEK_SET);
				btree_node node2;
				node2.n = 1; 
				node2.key = (int *) calloc( order - 1, sizeof( int ) );
				node2.child = (long *) calloc( order, sizeof( long ) );
				int s = 0;
				node2.key[0] = a[0];
				node2.child[0] = a[1];
				node2.child[1] = a[2];
				for( s = 2; s < order; s++)	node2.child[s] = -1;
				fwrite(&node2.n, sizeof(int), 1, file_fp);
				fwrite(node2.key, sizeof( int ), order - 1, file_fp);
				fwrite(node2.child, sizeof( long ), order, file_fp);
				fclose(file_fp);
				return -10;
			}
			else{
				a = split_node(prev, key_value);
				search_after_split(prev, a);
				return -10;
			}
		}
		return prev;
	}
}

void search_after_split(long prev, long *a){
	FILE *file_fp = fopen(filename, "r+b");
	fseek(file_fp, 8, SEEK_SET);
	btree_node node1;
	node1.n = 0; 
	node1.key = (int *) calloc( order - 1, sizeof( int ) );
	node1.child = (long *) calloc( order, sizeof( long ) );
	fread(&node1.n, sizeof( int ), 1, file_fp);
	fread(node1.key, sizeof( int ), order - 1, file_fp);
	fread(node1.child, sizeof( long ), order, file_fp);
	fclose(file_fp);
	long offset = search_parent(prev, -3, node1);
	if(offset == -3) offset = 8;
	FILE *file_fp1 = fopen(filename, "r+b");
	fseek(file_fp1, offset, SEEK_SET);
	btree_node node12;
	node12.n = 0; 
	node12.key = (int *) calloc( order - 1, sizeof( int ) );
	node12.child = (long *) calloc( order, sizeof( long ) );
	fread(&node12.n, sizeof( int ), 1, file_fp1);
	fread(node12.key, sizeof( int ), order - 1, file_fp1);
	fread(node12.child, sizeof( long ), order, file_fp1);
	fclose(file_fp1);
	if(node12.n < (order - 1)){
		int s = 0;
		while(s < (node12.n)){
			if(a[0] < node12.key[s]) break;
			else s++;
		}
		int t = s + 1;
		node12.n += 1;
		int temp = node12.key[s];
		node12.key[s] = a[0];
		long temp_child = node12.child[s + 1];
		node12.child[s] = a[1];
		node12.child[s + 1] = a[2];
		s++;
		t++;
		int temp12;
		long temp_child1;
		while(s < node12.n){
			temp12 = node12.key[s];
			node12.key[s] = temp;
			temp = temp12;
			s++;
		}
		while(t < node12.n + 1){
			temp_child1 = node12.child[t];
			node12.child[t] = temp_child;
			temp_child = temp_child1;
			t++;
		}
		FILE *file_fp11 = fopen(filename, "r+b");
		fseek(file_fp11, offset, SEEK_SET);
		fwrite(&node12.n, sizeof(int), 1, file_fp11);
		fwrite(node12.key, sizeof( int ), order - 1, file_fp11);
		fwrite(node12.child, sizeof( long ), order, file_fp11);
		fclose(file_fp11);
	}
	else{
		if(offset == 8){
			long *aa = split_node_between(offset, a);
			FILE *file_fp2 = fopen(filename, "r+b");
			fseek(file_fp2, 8, SEEK_SET);
			btree_node node13;
			node13.n = 0; 
			node13.key = (int *) calloc( order - 1, sizeof( int ) );
			node13.child = (long *) calloc( order, sizeof( long ) );
			node13.n = 1;
			node13.key[0] = aa[0];
			node13.child[0] = aa[1];
			node13.child[1] = aa[2];
			int r = 0;
			for(r = 2; r < order; r++)	node13.child[r] = -1;
			fseek(file_fp2, offset, SEEK_SET);
			fwrite(&node13.n, sizeof(int), 1, file_fp2);
			fwrite(node13.key, sizeof( int ), order - 1, file_fp2);
			fwrite(node13.child, sizeof( long ), order, file_fp2);
			fclose(file_fp2);
		}
		else{
			int next_offset = search_parent(offset, -3, node1);
			long *aa = split_node_between(offset, a);
			search_after_split(offset, aa);
		}
	}
}

long search_parent(long key_value, long prev, btree_node node1){
	long offset = 0;
	int s = 0;
	if(key_value <= -3) return -3;
	while(s < (node1.n + 1) && s < (order)){
		if(key_value == node1.child[s]){
			offset = prev;
			break;
		}
		else{
			if(node1.child[s] != -1 && offset == 0){
				FILE *file_fp = fopen(filename, "r+b");
				fseek(file_fp, node1.child[s], SEEK_SET);
				btree_node node2;
				node2.n = 0; 
				node2.key = (int *) calloc( order - 1, sizeof( int ) );
				node2.child = (long *) calloc( order, sizeof( long ) );
				fread(&node2.n, sizeof( int ), 1, file_fp);
				fread(node2.key, sizeof( int ), order - 1, file_fp);
				fread(node2.child, sizeof( long ), order, file_fp);
				fclose(file_fp);
				offset = search_parent(key_value, node1.child[s], node2);
			}
			s++;
		}
	}
	return offset;
}

long* split_node_between(int offset, long *a){
	long *back = (long *) calloc(3, sizeof(int));
	FILE *file_fp = fopen(filename, "r+b");
	fseek(file_fp, offset, SEEK_SET);
	btree_node node1;
	node1.n = 0; 
	node1.key = (int *) calloc( order - 1, sizeof( int ) );
	node1.child = (long *) calloc( order, sizeof( long ) );
	fread(&node1.n, sizeof( int ), 1, file_fp);
	fread(node1.key, sizeof( int ), order - 1, file_fp);
	fread(node1.child, sizeof( long ), order, file_fp);
	int s = 0;
	int key_arr[node1.n + 1];
	long child_arr[node1.n + 2];
	while(s < (node1.n)){
		if(a[0] < node1.key[s]) break;
		else s++;
	}
	int t = 0;
	while(t < s){
		key_arr[t] = node1.key[t];
		child_arr[t] = node1.child[t];
		t++;
	}
	key_arr[s] = a[0];
	child_arr[s] = a[1];
	child_arr[s + 1] = a[2];
	t++;
	int u = t;
	u++;
	while(t < node1.n + 1){
		key_arr[t] = node1.key[t - 1];
		t++;
	}
	while(u < node1.n + 2){
		child_arr[u] = node1.child[u - 1];
		u++;
	}
	int mid = (node1.n + 1)/2;
	btree_node node2, node3;
	node2.n = 0; 
	node2.key = (int *) calloc( order - 1, sizeof( int ) );
	node2.child = (long *) calloc( order, sizeof( long ) );
	node3.n = 0; 
	node3.key = (int *) calloc( order - 1, sizeof( int ) );
	node3.child = (long *) calloc( order, sizeof( long ) );
	int ss = 0, tt = 0;
	for(ss = 0; ss < mid; ss++){
		node2.n += 1;
		node2.key[ss] = key_arr[ss];
		node2.child[ss] = child_arr[ss];
	}
	node2.child[ss] = child_arr[ss];
	int count = 0;
	for(tt = mid +1; tt < order; tt++) node2.child[tt] = -1;
	for(ss = mid + 1; ss < order; ss++){
		node3.n += 1;
		node3.key[ss - (mid + 1)] = key_arr[ss];
		node3.child[ss - (mid + 1)] = child_arr[ss];
		count++;
	}
	node3.child[ss - (mid + 1)] = child_arr[ss];
	for(tt = count + 1; tt < order; tt++) node3.child[tt] = -1;
	back[0] = key_arr[mid];
	fseek(file_fp, 0L, SEEK_END);
	back[1] = ftell(file_fp);
	fwrite(&node2.n, sizeof(int), 1, file_fp);
	fwrite(node2.key, sizeof( int ), order - 1, file_fp);
	fwrite(node2.child, sizeof( long ), order, file_fp);
	back[2] = ftell(file_fp);
	fwrite(&node3.n, sizeof(int), 1, file_fp);
	fwrite(node3.key, sizeof( int ), order - 1, file_fp);
	fwrite(node3.child, sizeof( long ), order, file_fp);
	fclose(file_fp);
	return back;
}

long* split_node(int offset, int key){
	long *back = (long *) calloc(3, sizeof(int));
	FILE *file_fp = fopen(filename, "r+b");
	fseek(file_fp, offset, SEEK_SET);
	btree_node node1;
	node1.n = 0; 
	node1.key = (int *) calloc( order - 1, sizeof( int ) );
	node1.child = (long *) calloc( order, sizeof( long ) );
	fread(&node1.n, sizeof( int ), 1, file_fp);
	fread(node1.key, sizeof( int ), order - 1, file_fp);
	fread(node1.child, sizeof( long ), order, file_fp);
	int a[node1.n + 1], i;
	for( i = 0; i < node1.n; i++)	a[i] = node1.key[i];
	a[node1.n] = key;
	qsort(a, node1.n + 1, sizeof(int), cmpfuncs);
	int mid = (node1.n + 1)/2;
	btree_node node2, node3;
	node2.n = 0; 
	node2.key = (int *) calloc( order - 1, sizeof( int ) );
	node2.child = (long *) calloc( order, sizeof( long ) );
	node3.n = 0; 
	node3.key = (int *) calloc( order - 1, sizeof( int ) );
	node3.child = (long *) calloc( order, sizeof( long ) );
	int s = 0, t = 0;
	for(s = 0; s < mid; s++){
		node2.n += 1;
		node2.key[s] = a[s];
	}
	for(t = 0; t < order; t++) node2.child[t] = -1;
	for(s = mid + 1; s < order; s++){
		node3.n += 1;
		node3.key[s - (mid + 1)] = a[s];
	}
	for(t = 0; t < order; t++) node3.child[t] = -1;
	back[0] = a[mid];
	fseek(file_fp, 0L, SEEK_END);
	back[1] = ftell(file_fp);
	fwrite(&node2.n, sizeof(int), 1, file_fp);
	fwrite(node2.key, sizeof( int ), order - 1, file_fp);
	fwrite(node2.child, sizeof( long ), order, file_fp);
	back[2] = ftell(file_fp);
	fwrite(&node3.n, sizeof(int), 1, file_fp);
	fwrite(node3.key, sizeof( int ), order - 1, file_fp);
	fwrite(node3.child, sizeof( long ), order, file_fp);
	fclose(file_fp);
	return back;
}

void add_key_to_node(int offset, int key){
	int key_value = key;
	FILE *file_fp = fopen(filename, "r+b");
	fseek(file_fp, offset, SEEK_SET);
	btree_node node1;
	node1.n = 0; 
	node1.key = (int *) calloc( order - 1, sizeof( int ) );
	node1.child = (long *) calloc( order, sizeof( long ) );
	fread(&node1.n, sizeof( int ), 1, file_fp);
	fseek(file_fp, offset, SEEK_SET);
	node1.n += 1;
	fwrite(&node1.n, sizeof(int), 1, file_fp);
	fseek(file_fp, offset + sizeof(int) * node1.n, SEEK_SET);
	fwrite(&key_value, sizeof( int ), 1, file_fp);
	fseek(file_fp, offset + sizeof(int), SEEK_SET);
	fread(node1.key, sizeof( int ), order - 1, file_fp);
	fread(node1.child, sizeof( long ), order, file_fp);
	qsort(node1.key, node1.n, sizeof(int), cmpfuncs);
	fseek(file_fp, offset + sizeof(int), SEEK_SET);
	fwrite(node1.key, sizeof( int ), node1.n, file_fp);
	fclose(file_fp);
}	

void add_key_to_node_new(int offset, int key){
	FILE *file_fp = fopen(filename, "r+b");
	fseek(file_fp, offset, SEEK_SET);
	btree_node node1;
	node1.n = 0; 
	node1.key = (int *) calloc( order - 1, sizeof( int ) );
	node1.child = (long *) calloc( order, sizeof( long ) );
	fwrite("1", sizeof(int), 1, file_fp);
	fseek(file_fp, offset + sizeof(int), SEEK_SET);
	fwrite(&key, sizeof( int ), order-1, file_fp);
	fwrite("", sizeof( long ), order, file_fp);
	fclose(file_fp);
}

int search(int key_value){
	FILE *file_fp1 = fopen(filename, "r+b");
	if(file_fp1 == NULL)	return -1;
	fseek(file_fp1, 8, SEEK_SET);
	btree_node node1;
	node1.n = 0; 
	node1.key = (int *) calloc( order - 1, sizeof( int ) );
	node1.child = (long *) calloc( order, sizeof( long ) );
	fread(&node1.n, sizeof( int ), 1, file_fp1);
	fread(node1.key, sizeof( int ), order - 1, file_fp1);
	fread(node1.child, sizeof( long ), order, file_fp1);
	fclose(file_fp1);
	return search_into(key_value, node1);
}

int search_into(int key_value, btree_node node1){
	long offset = 0;
	int s = 0;
	while(s < (node1.n) && s < (order - 1)){
		if(key_value == node1.key[s])	return 1;
		else if(key_value < node1.key[s])	break;
		else s++;
	}
	if(node1.child[s] != -1){
		FILE *file_fp = fopen(filename, "r+b");
		fseek(file_fp, node1.child[s], SEEK_SET);
		btree_node node2;
		node2.n = 0; 
		node2.key = (int *) calloc( order - 1, sizeof( int ) );
		node2.child = (long *) calloc( order, sizeof( long ) );
		fread(&node2.n, sizeof( int ), 1, file_fp);
		fread(node2.key, sizeof( int ), order - 1, file_fp);
		fread(node2.child, sizeof( long ), order, file_fp);
		fclose(file_fp);
		return search_into(key_value, node2);
	}
	else	return -1;
}

void print_all(){
	long *print_offset = (long *) calloc(10000, sizeof(int));
	int i = 0, j = 0;
	print_offset[i++] = -1;
	print_offset[i++] = 8;
	while(j < i){
		if(print_offset[j] < 0){
			if(j + 1 == i)	break;
			if(abs(print_offset[j]) == 1)	printf(" %d:",abs(print_offset[j++]));
			else	printf(" \n %d:",abs(print_offset[j++]));
			print_offset[i++] = print_offset[j - 1] - 1;
		}
			FILE *file_fp = fopen(filename, "r+b");
			fseek(file_fp, print_offset[j++], SEEK_SET);
			btree_node node1;
			node1.n = 0; 
			node1.key = (int *) calloc( order - 1, sizeof( int ) );
			node1.child = (long *) calloc( order, sizeof( long ) );
			fread(&node1.n, sizeof( int ), 1, file_fp);
			fread(node1.key, sizeof( int ), order - 1, file_fp);
			fread(node1.child, sizeof( long ), order, file_fp);
			int k = 0;
			printf(" ");
			for(k = 0; k < node1.n + 1; k++)	if(node1.child[k] != -1)	print_offset[i++] = node1.child[k];
			for(k = 0; k < node1.n ; k++){
				if(k == node1.n - 1)	printf("%d",node1.key[k]);
				else	printf("%d,",node1.key[k]);
			}
			fclose(file_fp);
	}
	printf(" \n");
}
