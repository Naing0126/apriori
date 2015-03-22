#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<iostream>
#include<algorithm>
#include<vector>

using namespace std;

FILE *input;
FILE *output;

int min_sup;
int min_sup_num;

int cnt_t = 0;
int cnt_l = 0;
int cnt_l_pre;
int cnt_c = 0;
int cnt_k;

vector<vector<int>> transaction;

typedef struct _tid_List{
	int data;
	vector<int> list;
}tid_list;

vector<tid_list> vertical_type;
int mapDataToVID[100000]; // mapping data to vertical type idx

//int L[100000][10];
vector<vector<vector<int>>> L;
//int C[100000][10];
vector<vector<vector<int>>> C;

float sup[100000];

int vertical[30][1000];
int cnt_v[30];


void init(){
	int i, j;

	for (i = 0; i < 100000; i++){
		mapDataToVID[i] = -1;
		sup[i] = 0;
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
	for (i = 0; i < transaction.size(); i++){
		for (j = 0; j < transaction[i].size(); j++){
			printf("%d ", transaction[i][j]);
		}
		printf("\n");
	}
	printf("transaction count is %d\n", transaction.size());
}

void printVertical(){
	int i, j;

	printf("print Vertical type\n");

	for (i = 0; i < vertical_type.size(); i++){
		printf("%d : {", vertical_type[i].data);
		for (j = 0; j < vertical_type[i].list.size(); j++){
			printf("%d ", vertical_type[i].list[j]);
		}
		printf("}\n");
	}
	printf("vertical type count is %d\n", vertical_type.size());
}


void generateL1(){
	int i;
	printf("generateL1\n");
	vector<vector<int>> L1;
	vector<vector<int>> C1;

	for (i = 0; i < vertical_type.size() ; i++){
		vector<int> temp;
		temp.push_back(vertical_type[i].data);
		C1.push_back(temp);
		if (vertical_type[i].list.size()>=min_sup_num)
			L1.push_back(temp);
	}
	sort(C1.begin(), C1.end());
	C.push_back(C1);

	sort(L1.begin(), L1.end());
	L.push_back(L1);
	
}

void printL(int k){
	int i, j;
	printf("printL%d\n", k);
	for (i = 0; i < L[k-1].size(); i++){
		for (j = 0; j < L[k - 1][i].size(); j++){
			printf("%d ", L[k-1][i][j]);
		}
		printf("\n");
	}
}

void printC(int k){
	int i, j;
	printf("printC%d\n", k);
	for (i = 0; i < C[k - 1].size(); i++){
		for (j = 0; j < C[k - 1][i].size(); j++){
			printf("%d ", C[k - 1][i][j]);
		}
		printf("\n");
	}
}

bool isExist(vector<vector<int>> list, vector<int> target){
	int i, j;
	for (i = 0; i < list.size(); i++){
		int flag = 1;
		for (j = 0; j < target.size(); j++){
			if (list[i][j] != target[j]){
				flag = 0;
				break;
			}
		}
		if (flag == 1)
			return true;
	}
	return false;
}

vector<vector<int>> join(int k){
	// self-joining Lk and make Ck+1 candidates
	printf("self-joining L%d\n", k);

	int i, j;
	int ida = 0,idb = 0;

	printf("L[%d]'s size is %d\n", k, L[k - 1].size());

	vector<vector<int>> temp_c_list;

	for (i = 0; i < L[k-1].size(); i++){
		for (j = i + 1; j < L[k-1].size(); j++){
			int dif_cnt = 0;
			int dif_data;
			int ida = 0;
			int idb = 0;
			int flag = 1; // 1 : enable, 0 : not enable
			while (ida<k && idb<k){
				if (L[k - 1][i][ida] != L[k - 1][j][idb]){
					dif_cnt++;
					if (dif_cnt > 1){
						// when different data cnt is more than 1, join is not enable
						flag = 0;
						break;
					}
					if (L[k - 1][i][ida] < L[k - 1][j][idb]){
						dif_data = L[k - 1][i][ida];
						idb--;
					}
					else{
						dif_data = L[k - 1][j][idb];
						ida--;
					}
				}
				ida++;
				idb++;
			}
			if (flag == 1){
				// join
				int join_id;
				vector<int> temp_c;
				vector<int> target;
				if (ida == k){
					target = L[k - 1][j];
				}
				else{
					target = L[k - 1][i];
				}
				for (join_id = 0; join_id < k; join_id++){
					temp_c.push_back(target[join_id]);
				}
				temp_c.push_back(dif_data);
				sort(temp_c.begin(), temp_c.end());

				if (!isExist(temp_c_list,temp_c))
					temp_c_list.push_back(temp_c);
			}
		}
	}
	return temp_c_list;
}

bool pruning(int k, vector<vector<int>> temp_c_list){
	// pruning Ck candidates
	printf("pruning C%d\n", k);

	if (temp_c_list.size() == 0)
		return false;

	int C_id, i, L_id;
	vector<vector<int>> pruned_c_list;

	for (C_id = 0; C_id < temp_c_list.size(); C_id++){
		int flag = 1;
		for (i = 0; i < k - 1; i++){
			vector<int> temp = temp_c_list[C_id];
			temp.erase(temp.begin()+i);
			for (L_id = 0; L_id < L[k - 2].size(); L_id++){
				if (equal(temp.begin(),temp.end(),L[k-2][L_id].begin())){
					break;
				}
			}
			if (L_id==L[k-2].size()){
				flag = 0;
				break;
			}
		}
		if (flag == 1){
			pruned_c_list.push_back(temp_c_list[C_id]);
		}
	}
	
	if (pruned_c_list.size() > 0){
		C.push_back(pruned_c_list);
		return true;
	}
	else
		return false;
}

bool apriori_gen(int k){
	return pruning(k, join(k - 1));
}

int findIntersection(vector<int> data_set){
	int i;

	vector<int> result = vertical_type[mapDataToVID[data_set[0]]].list;
	
	for (i = 1; i < data_set.size(); i++){
		vector<int> temp(transaction.size());
		vector<int>::iterator it;
		vector<int> target = vertical_type[mapDataToVID[data_set[i]]].list;

		it = set_intersection(result.begin(), result.end(), target.begin(), target.end(),temp.begin());
		temp.resize(it - temp.begin());

		result.clear();

		for (it = temp.begin(); it != temp.end(); ++it){
			result.push_back(*it);
		}
	}

	return result.size();
}

void scanning(int k){
	// Ck의 sup를 구하여 기준을 통과하는 애들만 Lk로 push
	printf("scanning C%d\n",k);
	int i;

	vector<vector<int>> temp_L_list;

	for (i = 0; i < C[k - 1].size(); i++){
		int cnt = findIntersection(C[k - 1][i]);
		int j;
		printf("[");
		for (j = 0; j < C[k - 1][i].size(); j++)
			printf("%d ", C[k - 1][i][j]);
		printf("] = %d\n",cnt);
		if (cnt >= min_sup_num)
			temp_L_list.push_back(C[k - 1][i]);
	}
	L.push_back(temp_L_list);
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
			vector<int> transaction_temp;
			for (i = 0; token != NULL; i++){
				temp = atoi(token);
				transaction_temp.push_back(temp);

				if (mapDataToVID[temp] == -1){
					tid_list temp_list;
					temp_list.data = temp;

					vertical_type.push_back(temp_list);
					mapDataToVID[temp] = vertical_type.size() - 1;
				}

				vertical_type[mapDataToVID[temp]].list.push_back(transaction.size());

				token = strtok(NULL, "\t");
			}
			sort(transaction_temp.begin(), transaction_temp.end());
			transaction.push_back(transaction_temp);
		}
	}

//	printTransaction();

	printVertical();

	min_sup = atoi(argv[1]);
	min_sup_num = min_sup * transaction.size() / 100;

	printf("min_sup_num is %d\n", min_sup_num);

	generateL1();
	
	int k;
	for (k = 2; k < vertical_type.size() ;k++){
		if (apriori_gen(k)){
			printC(k);
			scanning(k);
			printL(k);
		}
		else
			break;
	}

	// write to output file
	output = fopen(argv[3], "w");
	fprintf(output, "hello");
	fclose(output);
	return 0;
}