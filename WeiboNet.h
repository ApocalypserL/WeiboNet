#ifndef _WEIBONET_H_
#define _WEIBONET_H_
#include <algorithm>
#include "Graph.h"
#include "Function.h"
using namespace std;

//微博关系网类，继承于图类，提供外部操作接口
class WeiboNet : public Graph
{
	private:
		
	protected:
	
	public:
		//构造、析构函数
		WeiboNet() {}
		WeiboNet(int size) : Graph(size) {}
		~WeiboNet() {}
		
		//由所给好友文件创建好友关系网
		bool NetCreate(char *friends_table, char *at_table)
		{
			int size, n;
			size = LineCount(friends_table);
			n = MaxOfUsers();
			if(size > n)
			{
				cerr << "Storage space error, some users may not be added" << endl;
				return false;
			}
			//由关注列表赋值，好友的权值为100
			string id1, id2;
			ifstream fin1(friends_table, ios::in);
			if(!fin1)
			{
				cerr << "can not open file" << endl;
			}
			while(fin1 >> id1 >> id2)
			{
				insertUser(id1);
				insertUser(id2);
				insertFriends(id1, id2, 100);
			}
			fin1.close();
			//由@列表赋值，陌生人的@权值为1，否则在原有基础上加1
			ifstream fin2(at_table, ios::in);
			if(!fin2)
			{
				cerr << "can not open file" << endl;
			}
			while(fin2 >> id1 >> id2)
			{
				insertUser(id1);
				insertUser(id2);
				int weight = getWeight(id1, id2);
				if(weight == INF)insertFriends(id1, id2, 1);
				else
				{
					weight++;
					changeWeight(id1, id2, weight);
				}
			} 
			return true;
		}
		
		//查询某用户是否在网内
		bool IsIn(const string &id)const
		{
			return (Graph::IsIn(id));
		}
		//查询网内最大用户数
		int MaxOfUsers()const
		{
			return (Graph::MaxOfVertices());
		}
		//查询当前用户数
		int NumberOfUsers()const
		{
			return (Graph::NumberOfVertices());
		}
		//查询当前好友对数
		int NumberOfFriends()const
		{
			return (Graph::NumberOfEdges());
		}
		//判断关系网是否为空
		bool IsEmpty()const
		{
			return (Graph::IsEmpty());
		}

		//新增一个用户
		bool insertUser(const string &id)
		{
			return (Graph::insertVertex(id));
		}
		//新增一组好友关系
		bool insertFriends(const string &id1, const string &id2, int weight)
		{
			return (Graph::insertEdge(id1, id2, weight));
		}
		//删除一个用户
		bool removeUser(const string &id)
		{
			return (Graph::removeVertex(id));
		}
		//删除一组好友关系
		bool removeFriends(const string &id1, const string &id2)
		{
			return (Graph::removeEdge(id1, id2));
		}

		//查询某对好友的权值
		int getWeight(const string &id1, const string &id2)const
		{
			return (Graph::getWeight(id1, id2));
		}
		//修改某对好友的权值
		int changeWeight(const string &id1, const string &id2, int weight)
		{
			return (Graph::changeWeight(id1, id2, weight));
		}
		
		//DFS
		int DFS(const string &id)const
		{
			int n = NumberOfUsers();
			bool *visited = new bool [n];
			for(int i = 0; i < n; ++i)
			{
				visited[i] = false;
			}
			Graph::DFS(id, visited);
			int count = 0;
			for(int i = 0; i < n; ++i)
			{
				if(visited[i] == true)count++;
			}
			return count;
		}

		//主函数内使用函数
		//生成某个用户所在的好友圈，在强连通分量的基础上，根据关联度距离（见下）的定义限制一定的范围
		//将以输入用户为中心的范围内用户创建一个子圈
		WeiboNet *subNet(const string &id)const
		{
			if(!IsIn(id))
			{
				cerr << "用户 " << id << " 不存在！" << endl;
				return NULL;
			}
			int count = 0;
			string *ids = Graph::Components(id);	//获得强连通分量
			while(ids[count] != "end")
			{
				count++;
			}
			string newuser;
			WeiboNet *subNet = new WeiboNet(count);
			for(int i = 0; i < count; ++i)
			{
				newuser = ids[i];
				subNet->insertUser(newuser);	//用户添加进子网
			}
			string user1, user2;
			for(int i = 0; i < count; ++i)
			{
				for(int j = 0; j < count; ++j)
				{
					if(i == j)continue;
					else
					{
						user1 = ids[i];
						user2 = ids[j];
						int weight1 = getWeight(user1, user2);
						int weight2 = getWeight(user2, user1);
						//连接所有原关系网中存在的好友关系
						if(weight1 < 100 || weight1 == INF || weight2 < 100 || weight2 == INF)continue;
						else 
						{
							subNet->insertFriends(user1, user2, weight1);
							subNet->insertFriends(user2, user1, weight2);
						}
					}
				}
			}
			int n = subNet->NumberOfUsers();
			int *dist = new int[n];
			int *path = new int[n];
			//去除离中心用户范围过大的用户
			subNet->Graph::ShortestPath(id, dist, path);
			for(int i = 0; i < n; ++i)
			{
				if(dist[i] > 3)
				{
					subNet->removeUser(ids[i]);
				}
			}
			delete[] dist;
			delete[] path;	
			delete[] ids;
			return subNet;
		}
		//查询与用户联系最频繁的人
		//以相互之间@的次数为基础，若两个用户没有联系（既不互关也不@），则不参与统计
		void mostClose(const string &id)const
		{
			int n = NumberOfUsers();
			int *times = new int[n];
			string id2;
			int weight1 = 0, weight2 = 0;
			for(int i = 0; i < n; ++i)
			{
				id2 = Graph::GetID(i);
				weight1 = getWeight(id, id2);
				weight2 = getWeight(id2, id);
				if(weight1 == INF && weight2 == INF)times[i] = -1;	//没有直接联系
				else if(weight1 == INF || weight2 == INF)	//非互相关注
				{
					if((weight1 + weight2) >= (INF + 100))times[i] = weight1 + weight2 - INF - 100; //单方面关注+没有@
					else times[i] = weight1 + weight2 - INF;	//都无关注但有@
				}
				else
				{
					if(weight1 + weight2 >= 200)times[i] = weight1 + weight2 - 200;	//互相关注
					else times[i] = weight1 + weight2 - 100;	//单方面关注+有@
				}
			}
			int max = -1;
			int maxid = 0;
			for(int i = 0; i < n; ++i)
			{
				if(times[i] > max)
				{
					max = times[i];
					maxid = i;
				}
			}
			id2 = Graph::GetID(maxid);
			cout << "与用户 " << id << " 联系最频繁的是用户 " << id2 << endl;
			cout << "你们一共交流了 " << times[maxid] << " 次" << endl;
			delete[] times;
		}

		//查询圈内联系最频繁的用户
		//子过程，寻找每个用户联系最频繁的用户
		void mostCloseFriends(const string &id, int &timem, string &idm)
		{
			int n = NumberOfUsers();
			int *times = new int[n];
			string id2;
			int weight1 = 0, weight2 = 0;
			for(int i = 0; i < n; ++i)
			{
				id2 = Graph::GetID(i);
				weight1 = getWeight(id, id2);
				weight2 = getWeight(id2, id);
				if(weight1 == INF || weight2 == INF)times[i] = -1;
				else times[i] = weight1 + weight2 - 200;
			}
			int max = -1;
			int maxid = 0;
			for(int i = 0; i < n; ++i)
			{
				if(times[i] > max)
				{
					max = times[i];
					maxid = i;
				}
			}
			timem = times[maxid];
			idm = Graph::GetID(maxid);
		}
		//主过程，排序所有频繁对，输出最高者
		void mostCloseFriends()
		{
			int n = NumberOfUsers();
			int *times = new int[n];
			string *ids = new string[n];
			string id;
			for(int i = 0; i < n; ++i)
			{
				id = Graph::GetID(i);
				mostCloseFriends(id, times[i], ids[i]);
			}
			int iter = 3;
			int max = 0;
			int maxid = 0;
			string *idau = new string[6];
			bool flag = false;
			int j = 0;
			//输出前三位
			while(iter--)
			{
				max = -1;
				for(int i = 0; i < n; ++i)
				{
					if(times[i] > max)
					{
						max = times[i];
						maxid = i;
					}
				}
				id = Graph::GetID(maxid);
				for(int i = 0; i < 10; ++i)
				{
					if(id == idau[i])
					{
						flag = true;
						break;
					}
				}
				//去除重复统计，如A与B联系最频繁则B与A联系也最频繁
				if(!flag)
				{
					idau[j] = id;
					idau[j + 1] = ids[maxid];
					cout << "用户 " << id << "与用户 " << ids[maxid] << " 交流了 " << times[maxid] << " 次" << endl;
					j += 2;
					n--;
					ids[maxid] = ids[n];
					times[maxid] = times[n];
				}
				else 
				{
					n--;
					flag = false;
					ids[maxid] = ids[n];
					times[maxid] = times[n];
					iter++;
				}
			}
			delete[] times;
			delete[] ids;
			delete[] idau;
		}
		//查询某用户所有的好友，即与其相互关注的用户
		void getFriends(const string &id)const
		{
			if(!IsIn(id))
			{
				cerr << "查无此用户！" << endl;
				return;
			}
			string *ids = Graph::getTails(id);
			
			cout << "用户" << id << "的好友有:" << endl;
			int i = 0;
			while(ids[i] != "end")
			{
				if((getWeight(id, ids[i]) < 100) || (getWeight(ids[i], id) < 100))	//非好友
				{
					i++;
					continue;
				}
				cout << ids[i] << " ";
				//统计@次数
				cout << "你@Ta " << getWeight(id, ids[i]) - 100 << "次" << " ";
				cout << "Ta@你 " << getWeight(ids[i], id) - 100 << "次" << endl;
				i++;
			}
			
			delete[] ids;
		}
		//查询两个用户之间的关联度，由关联度关系（见Graph.h）确定
		//设定基础值为90（即关注的人），随两用户之间间隔的用户数递减
		int getDistance(const string &id1, const string &id2)const
		{
			if(!IsIn(id1))
			{
				cerr << "用户 " << id1 << " 不存在！" << endl;
				return 0;
			}
			if(!IsIn(id2))
			{
				cerr << "用户 " << id2 << " 不存在！" << endl;
				return 0;
			}
			int n = NumberOfUsers();
			int *dist = new int[n];
			int *path = new int[n];
			int distance = 92 - 2 * Graph::ShortestPath(id1, id2, dist, path);
			delete[] dist;
			delete[] path;
			return distance;
		}
		//查询与某用户关联度最高的用户
		void mostCloseFriend(const string &id)const
		{
			if(!IsIn(id))
			{
				cerr << "查无此用户！" << endl;
				return;
			}
			int n = NumberOfUsers();
			int *dist = new int[n];
			int *path = new int[n];
			Graph::ShortestPath(id, dist, path);

			string *ids = new string[n];
			int *distance = new int[n];
			string idi;
			for(int i = 0; i < n; ++i)
			{
				idi = Graph::GetID(i);
				if(idi == id)continue;
				else 
				{	
					ids[i] = idi;
					distance[i] = 92 - 2 * dist[i];
				}
			}
			
			int iter = 5;
			int max = 0;
			int maxid = 0;
			//输出最高的5个用户
			while(iter--)
			{
				max = 0;
				for(int i = 0; i < n; ++i)
				{
					if(distance[i] > max)
					{
						max = distance[i];
						maxid = i;
					}
				}
				cout << "与用户 " << ids[maxid] << " 关联度 " << distance[maxid] << endl;
				n--;
				ids[maxid] = ids[n];
				distance[maxid] = distance[n];
			}
			delete[] ids;
			delete[] distance;
		}
};

#endif
