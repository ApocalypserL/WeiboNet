#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include "WeiboNet.h"
#include "Function.h"
using namespace std;

int main(int argc, char **argv)
{
	char friends_table_origin[] = "newfriends.txt";
	char friends_table_new[] = "friends_table.txt";
	char at_table_origin[] = "at.txt";
	char at_table_new[] = "at_table.txt";

	bool result = true;

	cout << "Loading Data Files..." << endl;
	result = (friendsPretreat(friends_table_origin, friends_table_new) && atPretreat(at_table_origin, at_table_new));
	
	if(result)cout << "Done" << endl;
	else
	{
		cerr << "Error!" << endl;
		return -1;
	}

	cout << "Creating Data Net..." << endl;
	
	int lines = LineCount(friends_table_new);
	WeiboNet wn(lines);
	result = wn.NetCreate(friends_table_new, at_table_new);
	if(result)cout << "Done" << endl;
	else
	{
		cerr << "Error!" << endl;
		return -1;
	}

	cout << "微博数据网v1.0" << endl;
	cout << "当前用户数" << wn.NumberOfUsers() << endl;

	menuprint();
	int choice;
	while(cin >> choice)
	{
		switch(choice)
		{
			case 1:
				{
					string id;
					cout << "请输入用户ID" << endl;
					cin >> id;
					wn.getFriends(id);
					cout << "----------" << endl;
					break;
				}
			case 2:
				{
					string id;
					cout << "请输入用户id" << endl;
					cin >> id;
					WeiboNet *friendsNet = wn.subNet(id);
					if(friendsNet == NULL)break;
					cout << "圈内有用户 " << friendsNet->NumberOfUsers() << " 人" << endl;
	
					cout << "1.查看与你联系最频繁的人" << endl;
					cout << "2.查看圈内联系最频繁的人" << endl;
					cout << "0.返回" << endl;
					int c;
					while(cin >> c)
					{
						switch(c)
						{
							case 1:
								{
									friendsNet->mostClose(id);
									cout << "----------" << endl;
									break;
								}
							case 2:
								{
									friendsNet->mostCloseFriends();
									cout << "----------" << endl;
									break;
								}
							case 0:
								{
									break;
								}
							default:
								{
									cout << "Please enter the right number" << endl;
									cout << "----------" << endl;
									break;
								}
						}
						if(c == 0)break;
						else
						{
							cout << "1.查看与你联系最频繁的人" << endl;
							cout << "2.查看圈内联系最频繁的人" << endl;
							cout << "0.返回" << endl;
						}
					}
					if(friendsNet != NULL)delete friendsNet;
					friendsNet = NULL;
					cout << "----------" << endl;
					break;
				}
			case 3:
				{
					string id1, id2;
					cout << "请输入两个用户ID，以空格隔开" << endl;
					cin >> id1 >> id2;
					int distance = wn.getDistance(id1, id2);
					if(distance <= 0)cout << "没有关联！" << endl;
					else cout << "用户" << id1 << "和用户" << id2 << "的关联度为" << distance << endl;
					cout << "----------" << endl;
					break;
				}
			case 4:
				{
					string id;
					cout << "请输入用户id" << endl;
					cin >> id;
					wn.mostCloseFriend(id);
					cout << "----------" << endl;
					break;
				}
			case 0:
				{
					cout << "Goodbye" << endl;
					break;
				}
			default:
				{
					cout << "Please enter the right number" << endl;
					cout << "----------" << endl;
					break;
				}
		}
		if(choice == 0)break;
		else
		{
			menuprint();	
		}
	}

	remove(friends_table_new);
	remove(at_table_new);
	return 0;
}
