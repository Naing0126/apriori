#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<iostream>
#include<algorithm>
#include<vector>

FILE *input;
FILE *output;

int min_sup;
int min_sup_num;

int cnt_t = 0;
int cnt_l = 0;
int cnt_l_pre;
int cnt_c = 0;
int cnt_k;

int transaction[1000][30];
int chk[100000];

int L[100000][10];
int C[100000][10];
float sup[100000];

int vertical[30][1000];
int cnt_v[30];

using namespace std;

void init(){
	int i, j;
	for (i = 0; i < 1000; i++){
		for (j = 0; j < 30; j++){
			transaction[i][j] = -1;
		}
	}

	for (i = 0; i < 100000; i++){
		for (j = 0; j < 10; j++){
			L[i][j] = C[i][j] = -1;
		}
		sup[i] = 0;
		chk[i] = 0;
	}

	for (i = 0; i < 30; i++){
		for (j = 0; j < 1000; j++){
			vertical[i][j] = -1;
		}
		cnt_v[i] = 0;
	}
}

void printTransaction(){
	int i, j;
	for (i = 0; i < cnt_t; i++){
		j = 0;
		while (transaction[i][j] >= 0){
			printf("%d ", transaction[i][j]);
			j++;
		}
		printf("\n");
	}
	printf("transaction count is %d\n", cnt_t);
}

void printVertical(){
	int i, j;

	printf("print Vertical array\n");

	for (i = 0; i < 30; i++){
		if (cnt_v[i]>0){
			printf("%d : ", i);
			for (j = 0; j<cnt_v[i]; j++){
				printf("t%d ", vertical[i][j]);
			}
			printf("\n");
		}
	}
}

void findIntersection(vector<int> &target){
	printf("find intersection which include ");
	int i;
	int targetSize = target.size();
	printf("target array size is %d\n", targetSize);
	for (i = 0; i<targetSize - 1; i++){
		printf("%d, ", target[i]);
	}
	printf("%d\n", target[targetSize - 1]);
}

/*
void printChk(){
int i;
printf("printChk\n");
for (i = 0; i < 100000; i++)
printf("%d", chk[i]);
printf("\n");
}
*/

void generateL1(){
	int i;
	printf("generateL1\n");
	//printChk();
	for (i = 0; i < 100000; i++){
		if (chk[i] == 1){
			L[cnt_l][0] = i;
			cnt_l++;
		}
	}
	cnt_l_pre = 0;
}

void printL(int k){
	int i, j;
	printf("printL%d\n", k);
	for (i = 0; i < cnt_l; i++){
		for (j = 0; j < k; j++){
			printf("%d ", L[i][j]);
		}
		printf("\n");
	}
}

int has_infrequent_subset(int cnt_c, int k){
	// C[cnt_c]의 subset들이 L에 전부 있는지 확인. 없는게 있으면 1, 없으면 0
	int ti, li, cki, lki;
	int i;
	int start = 0;
	int hasInfrequent = 0;
	int isSame = 0;

	for (ti = 0; ti < k + 1; ti++){
		hasInfrequent = 0;
		for (li = 0; li < cnt_l; li++){
			cki = lki = 0;
			while (lki <= k){
				if (cki == ti)
					cki++;
				if (C[cnt_c][cki] != L[li][lki]){
					break;
				}
				cki++;
				lki++;
			}
			if (lki > k){
				//same
				isSame = 1;
				break;
			}
		}
		if (isSame == 0){
			return 1;
		}
	}
}

void apriori_gen(int k){
	int i, j;
	int t = 0;
	for (i = 0; i < cnt_l; i++){
		for (j = i; j < cnt_l; j++){
			while (L[i][t] == L[j][t]){
				t++;
			}
			if (t == k){
				// join
				for (t = 0; t < k - 1; t++){
					C[cnt_c][t] = L[i][t];
				}
				if (L[i][k] < L[j][k]){
					C[cnt_c][k] = L[i][k];
					C[cnt_c][k + 1] = L[j][k];
				}
				cnt_c++;
				if (has_infrequent_subset(cnt_c, k)){
					cnt_c--;
				}
			}
			t = 0;
		}
	}

}

void scanning(int cnt_k){
	// counting 후 min_sup보다 큰 경우만 L에 저장
	cnt_l = 0;

	int ci, ti, cki, tki;
	for (ci = 0; ci < cnt_c; ci++){
		for (ti = 0; ti < cnt_t; ti++){
			for (cki = 0; cki < cnt_k; cki++){
				tki = 0;
				while (C[ci][cki] != transaction[ti][tki] && transaction[ti][tki]>-1){
					tki++;
				}
				if (transaction[ti][tki]<0 || tki>30){
					// no exist
					break;
				}
			}
			if (cki>cnt_k){
				// exist
				sup[ci]++;
			}
		}
		if (sup[ci] > min_sup_num){
			// add to L
			int i;
			for (i = 0; i < cnt_k; i++){
				L[cnt_l][i] = C[ci][i];
			}
			cnt_l++;
		}
	}
}

void printCandidates(){
	// cnt_l_pre까지의 L을 출력
	int i, j;
	printf("print Candidates\n");
	for (i = 0; i < cnt_l_pre; i++){
		for (j = 0; j < cnt_k; j++){
			printf("%d ", L[i][j]);
		}
		printf("\n");
	}
}


int main(int argc, char* argv[]){
	int num;
	char c;
	int temp;
	char str[100];
	char *token;

	if (argc < 4){
		printf("check input!\n");
		return 0;
	}

	input = fopen(argv[2], "r");

	init();

	while (!feof(input)){
		// parsing to transaction array
		if (fgets(str, sizeof(str), input)){
			token = strtok(str, "\t");
			int i;
			for (i = 0; token != NULL; i++){
				temp = atoi(token);
				transaction[cnt_t][i] = temp;
				if (chk[temp] == 0)
					chk[temp] = 1;
				vertical[temp][cnt_v[temp]++] = cnt_t;
				token = strtok(NULL, "\t");
			}
			sort(transaction[cnt_t], transaction[cnt_t] + i);
			cnt_t++;
		}
	}

	//	printTransaction();



	//	printVertical();

	min_sup = atoi(argv[1]);
	min_sup_num = min_sup * cnt_t / 100;

	printf("min_sup is %d\n", min_sup_num);

	generateL1();

	printL(1);

	vector<int> target;
	target.push_back(1);
	target.push_back(2);
	target.push_back(4);

	printf("target size is %d\n", target.size());

	findIntersection(target);
	/*
	int k;
	for (k = 2; cnt_l > 0;k++){
	cnt_l_pre = cnt_l;
	apriori_gen(k-1);
	scanning(k);
	}

	cnt_k = k - 1;

	printCandidates();

	*/

	// write to output file
	output = fopen(argv[3], "w");
	fprintf(output, "hello");
	fclose(output);
	return 0;
}