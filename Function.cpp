#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
using namespace std;

//统计数据行数
int LineCount(char *filename)
{
	ifstream fin(filename, ios::in);
	if(!fin)
	{
		cerr << "can not open file" << endl;
		return -1;
	}
	char c;
	int count = 0;
	while(fin.get(c))
	{
		if(c == '\n')count++;
	}
	fin.close();
	return count;
}
//数据预处理
//关注列表整理
bool friendsPretreat(char *friends_table_origin, char *friends_table_new)
{
	int numberLine = LineCount(friends_table_origin);
	ifstream fin(friends_table_origin, ios::in);
	if(!fin)
	{
		cerr << "can not open file" << endl;
		return false;
	}
	ofstream fout(friends_table_new, ios::out);
	if(!fout)
	{
		cerr << "can not pretreat data" << endl;
		return false;
	}
	int numberData = 0;
	bool flag = 0;
	string id1, id2;
	string *id1g = new string[numberLine];
	string *id2g = new string[numberLine];
	while(fin >> id1 >> id2)
	{
		flag = 0;
		if(id1 == id2) continue;
		for(int i = 0; i <= numberData; ++i)
		{
			if(id1 == id1g[i] && id2 == id2g[i])
			{
				flag = 1;
				break;
			}
		}
		if(!flag)
		{
			id1g[numberData] = id1;
			id2g[numberData] = id2;
			fout << id1 << " " << id2 << endl;
			numberData++;
		}
		else continue;
	}
	delete[] id1g;
	delete[] id2g;
	fin.close();
	fout.close();
	return true;
}
//at列表整理
bool atPretreat(char *at_table_origin, char *at_table_new)
{
	ifstream fin(at_table_origin, ios::in);
	if(!fin)
	{
		cerr << "can not open file" << endl;
		return false;
	}
	ofstream fout(at_table_new, ios::out);
	if(!fout)
	{
		cerr << "can not pretreat data" << endl;
		return false;
	}
	string ids;
	string id1(9, 0);
	string id2(9, 0);
	while(fin >> ids)
	{
		for(int i = 0; i < 9; ++i)
		{
			id1[i] = ids[i];
			id2[i] = ids[i + 10];
		}
		if(id1 == id2)continue;
		else fout << id1 << " " << id2 << endl;
	}
	fin.close();
	fout.close();
	return true;
}

//菜单
void menuprint()
{
	cout << "请选择你需要的功能" << endl;
	cout << "1.查询用户好友列表" << endl;
	cout << "2.查询用户朋友圈" << endl;
	cout << "3.查询用户间关联度" << endl;
	cout << "4.查询与用户关联度最高的5位用户" << endl;
	cout << "0.退出" << endl;
}

