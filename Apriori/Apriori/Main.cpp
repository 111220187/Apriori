// by Shi Jianming
/*
�����ھ򣺹�����������㷨Apriori��ʵ��
*/

#define _CRT_SECURE_NO_WARNINGS
#define HOME

#include <iostream>
#include <cstdio>
#include <vector>
#include <string>
#include <cmath>
#include <map>
#include <locale>
using namespace std;
const double eps = 1e-8;
const int MaxColNum = 100;

int rowNum, columnNum; // ����������
double supportMin, confidenceMin; // ��С֧�ֶȣ� ��С���Ŷ�
int supporNum; // ��С֧����
int total;
int Case;

vector<vector<int> > dataBase; // ����ԭʼ���ݼ�
vector<string> columnName; // ����ÿһ�е���Ŀ��

// ���ݼ�
struct itemset
{
	vector<int> item; // ���񣨰���0�������
	int cnt; // ������ִ���
	int id; // ����Ψһ��ʶ
	itemset()
	{
		cnt = 0;
		id = -1;
	}
};

vector<itemset> preL; // Ƶ��(k-1)-�
vector<itemset> C; // ��ѡ(k)-�
vector<itemset> L; // Ƶ��(k)-�

map<int, itemset> forL; // Ϊ����Ƶ��(k)-�

int C1[MaxColNum]; // ��¼C1


/****************************************************/
/*
Hash����
Hash������ h(p) = p mod k
ʱ�临�Ӷȣ�O(k)
*/

struct hashTrie
{
	hashTrie *next[MaxColNum]; // Hash����̽ڵ�
	vector<itemset> C; // ��ѡ(k)-�
	hashTrie()
	{
		fill(next, next + MaxColNum, nullptr);
	}
};
// ����Hash��
void CrehashTrie(hashTrie *root, vector<int> branch)
{
	hashTrie *p = root;
	for (auto i = 0; i < branch.size(); ++i)
	{
		int pos = branch[i] % branch.size();
		if (nullptr == p->next[pos])
		{
			p->next[pos] = new hashTrie;
		}
		p = p->next[pos];
	}
	itemset itsetTmp;
	itsetTmp.item = branch;
	itsetTmp.id = (total++);
	p->C.push_back(itsetTmp);
}
// ����branch��ֵ���ж��Ƿ������Hash����ƥ��ɹ�������¼��Hash����ƥ��ɹ��Ĵ���������Ƶ����
bool FindhashTrie(hashTrie *root, vector<int> branch)
{
	hashTrie *p = root;
	for (auto i = 0; i < branch.size(); ++i)
	{
		int pos = branch[i] % branch.size();
		if (nullptr == p->next[pos])
		{
			return false;
		}
		p = p->next[pos];
	}
	for (auto &tmp : p->C)
	{
		auto i = 0;
		for (; i != tmp.item.size(); ++i)
		{
			if (tmp.item[i] != branch[i])
			{
				break;
			}
		}
		if (i == tmp.item.size())
		{

			++(tmp.cnt);
			if (tmp.cnt >= (supporNum))
			{
				if (forL.find(tmp.id) != forL.end())
				{
					++(forL[tmp.id].cnt);
				}else
				{
					forL.insert({tmp.id, tmp});
				}
			}
			return true;
		}
	}
	return false;
}
// ����Hash��
void DelhashTrie(hashTrie *T, int len)
{
	for (int i = 0; i < len; ++i)
	{
		if (T->next[i] != nullptr)
		{
			DelhashTrie(T->next[i], len);
		}
	}
	if (!T->C.empty())
	{
		T->C.clear();
	}
	delete[] T->next;
	total = 0;
}

/****************************************************/



/****************************************************/
/*
�Ӽ���{0,1,2,3..,(N-1)} ���ҳ����д�СΪk���Ӽ�, �������ֵ�������
*/
vector<vector<int>> combine;
int arr[MaxColNum];
int visit[MaxColNum];
int combineN, combineK;
// ��ʼ��dfs(0, 0)
void dfs(int d, int pos)
{
	if (d == combineK)
	{
		vector<int> tmp;
		for (int i = 0; i < combineK; ++i)
		{
			tmp.push_back(arr[i]);
		}
		combine.push_back(tmp);
		return;
	}
	for (int i = pos; i < combineN; ++i)
	{
		if (!visit[i])
		{
			visit[i] = true;
			arr[d] = i;
			dfs(d + 1, i + 1);
			visit[i] = false;
		}
	}
}
/****************************************************/

// ��ȡԭʼ���ݼ�
void Input()
{
	cin >> rowNum >> columnNum;
	supporNum = ceil(supportMin*(rowNum - 1));
	string rowFirst;
	for (auto i = 0; i < rowNum; ++i)
	{
		cin >> rowFirst;
		vector<int> dataRow;
		int valueTmp;
		// ȥ���������ݵĵ�һ��
		for (auto j = 0; j < (columnNum - 1); ++j)
		{
			if (i != 0)
			{
				cin >> valueTmp;
				if (valueTmp) {
					++C1[j];
					dataRow.push_back(j);
				}
			}
			else
			{
				string colNameTmp;
				cin >> colNameTmp; 
				columnName.push_back(colNameTmp);
			}
		}
		if (i != 0) dataBase.push_back(dataRow);
	}
}

// ��ȡƵ��1-�
void Ini()
{
	for (auto i = 0; i < (columnNum - 1); ++i)
	{
		if (C1[i] >= supporNum)
		{
			itemset itemsetTmp;
			itemsetTmp.item.push_back(i);
			itemsetTmp.cnt = C1[i];
			preL.push_back(itemsetTmp);
		}
	}
}


// ö���������񣨼�ԭʼ���ݣ�������k-�������֧�ֶ�
void getItemsK(hashTrie *root, int k)
{
	vector<int> branch;
//	int bbb = 0;
	for (auto tmp : dataBase)
	{
//		cout << bbb++ << " : " << endl;
		if (tmp.size() < k) continue;

		combineN = tmp.size();
		combineK = k;
		dfs(0, 0);

		for (int i = 0; i < combine.size(); ++i)
		{
			for (int j = 0; j < combine[i].size(); ++j)
			{
				branch.push_back(tmp[combine[i][j]]);
			}
			/***********************/
			/*
			ƥ���ѡk-�������֧����
			*/
			FindhashTrie(root, branch);
//			if (FindhashTrie(root, branch))
//			{
//				for (auto aaa = 0; aaa < branch.size(); ++aaa)
//				{
//					cout << branch[aaa] << " ";
//				}
//				cout << endl;
//			}
//			/***********************/
			branch.clear();
		}
		combine.clear();
//		cout << endl;
	}
	
}

// �ж����ɵĺ�ѡ(k)-���ĳ��(k-1)-���Ӽ��Ƿ�ΪƵ���
bool isInfrequentSubset(itemset c)
{
	hashTrie *root = new hashTrie;
	int k = c.item.size() - 1;
	for (auto tmp : preL)
	{
		CrehashTrie(root, tmp.item);
	}
	vector<int> branch;

	combineN = c.item.size();
	combineK = k;
	dfs(0, 0);

	for (int i = 0; i < combine.size(); ++i)
	{
		for (int j = 0; j < combine[i].size(); ++j)
		{
			branch.push_back(c.item[combine[i][j]]);
		}

		/***********************/
		/*
		�ж����ɵ�((k-1)-���Ӽ��Ƿ�ΪƵ���ġ�
		*/
		if (!FindhashTrie(root, branch))
		{
			combine.clear();
			DelhashTrie(root, k);
			return true;
		}
		/***********************/
		branch.clear();
	}
	combine.clear();
	DelhashTrie(root, k);
	return false;
}

// ������ѡ(k)-�
void apriori_gen(int k)
{
	for (auto L1 = 0; L1 < preL.size(); ++L1)
	{
		for (auto L2 = L1 + 1; L2 < preL.size(); ++L2)
		{
		    auto judge = true;
			for (auto i = 0; i < (k - 1); ++i)
			{
				if (preL[L1].item[i] != preL[L2].item[i])
				{
					judge = false;
				}
			}
			if (!judge) continue;
			itemset itemsetTmp;
			for (auto i = 0; i < (k - 1); ++i)
			{
				itemsetTmp.item.push_back(preL[L1].item[i]);
			}
			itemsetTmp.item.push_back(preL[L1].item[k - 1]);
			itemsetTmp.item.push_back(preL[L2].item[k - 1]);
			if (isInfrequentSubset(itemsetTmp)) {
				continue;
			}
			C.push_back(itemsetTmp);
		}
	}
}
// Apriori�㷨ʵ�֣��������������
void Apriori()
{
	for (auto k = 2; !preL.empty(); ++k)
	{
		hashTrie *root = new hashTrie;
		apriori_gen(k - 1); // �����ѡ(k)-�;
		for (auto i = 0; i < C.size(); ++i)
		{
			CrehashTrie(root, C[i].item);
		}
		C.clear();
		getItemsK(root, k);
		DelhashTrie(root, k);
		for (auto tmp : forL)
		{
			L.push_back(tmp.second);
		}
		forL.clear();
		if (L.empty())
		{
			break;
		}
		for (auto fromTmp : L)
		{
			for (auto toTmp : preL)
			{
				auto i = 0;
				for (; i < toTmp.item.size(); ++i)
				{
					if (toTmp.item[i] != fromTmp.item[i])
					{
						break;
					}
				}
				if (i == toTmp.item.size())
				{
//					double aaa = (1.0*fromTmp.cnt) / (1.0*toTmp.cnt);
//					double bbb = (1.0*fromTmp.cnt) / (1.0*toTmp.cnt) - confidenceMin;
					if ((1.0*fromTmp.cnt)/(1.0*toTmp.cnt) - confidenceMin >= 0.0)
					{
						cout << "Case " << Case++ << " : " << endl;
						for (auto j = 0; j < toTmp.item.size(); ++j)
						{
							cout << columnName[toTmp.item[j]];
							if (j != toTmp.item.size() - 1)
							{
								cout << ",";
							}
						}
						cout << " => " << columnName[fromTmp.item[toTmp.item.size()]] << endl;
					}
				}
			}
		}
		preL.clear();
		preL = L;
		L.clear();
	}
}

int main()
{
#ifdef HOME
	freopen("in", "r", stdin);
	freopen("out", "w", stdout);
#endif
	cin >> supportMin >> confidenceMin;
	Case = 0;
	total = 0;
	Input();
	Ini();
	Apriori();

#ifdef HOME
	cerr << "Time elapsed: " << clock() / CLOCKS_PER_SEC << " ms" << endl;
#endif
	return 0;
}




