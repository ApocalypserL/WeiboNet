#ifndef _GRAPH_H_
#define _GRAPH_H_
#include <iostream>
#include <string>
#define INF 1000000
using namespace std;

//图类的前视声明
class Graph;	
//边类的前视声明
class Edge;
//顶点类的前视声明
class Vertex;

//顶点类
class Vertex
{
	friend class Graph;
	private:
		string _id;	//顶点id
		Edge *adj;	//边链表头指针
	public:
		//构造函数
		Vertex() {}
		Vertex(string id) : _id(id), adj(NULL) {}
		//析构函数
		~Vertex() {}
};

//边类
class Edge
{
	friend class Graph;
	private:
		int _tail;	//边终点
		int _weight;	//权值
		Edge *_link;	//下一条边链接指针
	public:
		//构造函数
		Edge() {}
		Edge(int tail, int weight) : _tail(tail), _weight(weight), _link(NULL) {}
		//析构函数
		~Edge() {}
};

//图类，以邻接表存储
//封装，仅用派生类操作
class Graph
{
	private:
		Vertex *NodeTableOut;	//出边表
		Vertex *NodeTableIn;	//入边表
		int _numberVertices;	//当前顶点数
		int _numberEdges;	//当前边数
		int _maxVertices;
		
		//部分以顶点号为参数的函数私有并提供以顶点id为参数的接口
		//查找某id在图中的顶点位置
		int GetVertexPos(const string &id)const
		{
			for(int i = 0; i < _numberVertices; ++i)
			{
				if(NodeTableOut[i]._id == id)return i;
			}
			return -1;
		}

		//插入边，成功返回true，失败返回false
		bool InsertEdge(int v1, int v2, int weight)
		{
			if(v1 >= 0 && v2 >= 0 && v1 < _numberVertices && v2 < _numberVertices && v1 != v2)
			{
				Edge *e = NodeTableOut[v1].adj;
				while(e != NULL && e->_tail != v2)
				{
					e = e->_link;
				}
				if(e != NULL)return false;

				e = new Edge;
				e->_tail = v2;
				e->_weight = weight;
				e->_link = NodeTableOut[v1].adj;
				NodeTableOut[v1].adj = e;
				
				Edge *d = NodeTableIn[v2].adj;
				while(d != NULL && d->_tail != v1)
				{
					d = d->_link;
				}
				if(d != NULL)return false;
				d = new Edge;
				d->_tail = v1;
				d->_weight = weight;
				d->_link = NodeTableIn[v2].adj;
				NodeTableIn[v2].adj = d;

				_numberEdges++;
				return true;
			}
			else return false;
		}
		//删除顶点（及所有与此顶点相关的边），成功返回true，失败返回false
		bool RemoveVertex(int v)
		{
			if(_numberVertices == 0 || v < 0 || v >= _numberVertices)return false;
			Edge *e, *s, *t;
			int i, k;
			while(NodeTableOut[v].adj != NULL)
			{
				//v1的出边表中指向的所有顶点的入边表中删除与v1有关的边
				e = NodeTableOut[v].adj;
				k = e->_tail;
				s = NodeTableIn[k].adj;
				t = NULL;
				while(s != NULL && s->_tail != v)
				{
					t = s;
					s = s->_link;
				}
				if(s != NULL)
				{
					if(t == NULL)NodeTableIn[k].adj = s->_link;
					else t->_link = s->_link;
					delete s;
				}
				NodeTableOut[v].adj = e->_link;
				delete e;
				_numberEdges--;
			}
			while(NodeTableIn[v].adj != NULL)
			{
				//v1的入边表中所有指向v1的顶点的出边表中删除与v1有关的边
				e = NodeTableIn[v].adj;
				k = e->_tail;
				s = NodeTableOut[k].adj;
				t = NULL;
				while(s != NULL && s->_tail != v)
				{
					t = s;
					s = s->_link;
				}
				if(s != NULL)
				{
					if(t == NULL)NodeTableOut[k].adj = s->_link;
					else t->_link = s->_link;
					delete s;
				}
				NodeTableIn[v].adj = e->_link;
				delete e;
			}
			_numberVertices--;
			//用末尾的顶点填补此位置
			NodeTableOut[v]._id = NodeTableOut[_numberVertices]._id;
			e = NodeTableOut[v].adj = NodeTableOut[_numberVertices].adj;
			while(e != NULL)
			{
				s = NodeTableIn[e->_tail].adj;
				while(s != NULL)
				{
					if(s->_tail == _numberVertices)
					{
						s->_tail = v;
						break;
					}
					else s = s->_link;
				}
				e = e->_link;
			}
			NodeTableIn[v]._id = NodeTableIn[_numberVertices]._id;
			e = NodeTableIn[v].adj = NodeTableIn[_numberVertices].adj;
			while(e != NULL)
			{
				s = NodeTableOut[e->_tail].adj;
				while(s != NULL)
				{
					if(s->_tail == _numberVertices)
					{
						s->_tail = v;
						break;
					}
					else s = s->_link;
				}
				e = e->_link;
			}
			return true;
		}
		//删除边，成功返回true，失败返回false
		bool RemoveEdge(int v1, int v2)
		{
			if(v1 != -1 && v2 != -1 && v1 < _numberVertices && v2 < _numberVertices && v1 != v2)
			{
				//v1出边表删除
				Edge *e = NodeTableOut[v1].adj, *pre = NULL;
				while(e != NULL && e->_tail != v2)
				{
					pre = e;
					e = e->_link;
				}
				if(e != NULL)
				{
					if(pre == NULL)NodeTableOut[v1].adj = e->_link;
					else pre->_link = e->_link;
					delete e;
				}
				else return false;
				//v2入边表删除
				e = NodeTableIn[v2].adj;
				pre = NULL;
				while(e->_tail != v1)
				{
					pre = e;
					e = e->_link;
				}
				if(pre == NULL)NodeTableIn[v2].adj = e->_link;
				else pre->_link = e->_link;
				delete e;
				
				return true;
			}
			else return false;
		}
		
		//查询某边权值
		int GetWeight(int v1, int v2)const
		{
			if(v1 != -1 && v2 != -1)
			{
				Edge *e = NodeTableOut[v1].adj;
				while(e != NULL && e->_tail != v2)
				{
					e = e->_link;
				}
				if(e != NULL)return e->_weight;
			}
			return INF;
		}
		//修改某边权值，成功返回true，失败返回false
		bool ChangeWeight(int v1, int v2, int weight)
		{
			if(v1 != -1 && v2 != -1)
			{
				Edge *e = NodeTableOut[v1].adj;
				while(e != NULL && e->_tail != v2)
				{
					e = e->_link;
				}
				if(e != NULL) e->_weight = weight;
				else return false;

				e = NodeTableIn[v2].adj;
				while(e != NULL && e->_tail != v1)
				{
					e = e->_link;
				}
				if(e != NULL) e->_weight = weight;
				else return false;
			}
			return true;
		}
		
		//查询顶点v的第一个邻接节点（出边表）
		int GetFirstNeighbor(int v)const
		{
			if(v != -1)
			{
				Edge *e = NodeTableOut[v].adj;
				if(e != NULL)return e->_tail;
			}
			return -1;
		}
		//查询顶点v的第一个邻接节点（入边表）
		int GetFirstNeighborIn(int v)const
		{
			if(v != -1)
			{
				Edge *e = NodeTableIn[v].adj;
				if(e != NULL)return e->_tail;
			}
			return -1;
		}
		//查询顶点v1在邻接节点v2后的下一个邻接节点（出边表）
		int GetNextNeighbor(int v1, int v2)const
		{
			if(v1 != -1)
			{
				Edge *e = NodeTableOut[v1].adj;
				while(e != NULL && e->_tail != v2)
				{
					e = e->_link;
				}
				if(e != NULL && e->_link != NULL)return e->_link->_tail;
			}
			return -1;
		}
		//查询顶点v1在邻接节点v2后的下一个邻接节点（入边表）
		int GetNextNeighborIn(int v1, int v2)const
		{
			if(v1 != -1)
			{
				Edge *e = NodeTableIn[v1].adj;
				while(e != NULL && e->_tail != v2)
				{
					e = e->_link;
				}
				if(e != NULL && e->_link != NULL)return e->_link->_tail;
			}
			return -1;
		}
		//DFS，visited辅助数组标记访问过的点
		void DFS(int v, bool *visited)const
		{
			visited[v] = true;
			int w = GetFirstNeighbor(v);
			while(w != -1)
			{
				if(visited[w] == false)DFS(w, visited);
				w = GetNextNeighbor(v, w);
			}
		}
		//入边表的DFS，仅用于求强连通分量
		void DFSIn(int v, bool *visited)const
		{
			visited[v] = true;
			int w = GetFirstNeighborIn(v);
			while(w != -1)
			{
				if(visited[w] == false)DFSIn(w, visited);
				w = GetNextNeighborIn(v, w);
			}
		}
		//取出与某顶点属于同一强连通分量的所有顶点id
		string *Components(int v)const
		{
			int n = NumberOfVertices();
			bool *visited1 = new bool[n];
			bool *visited2 = new bool[n];
			for(int i = 0; i < n; ++i)
			{
				visited1[i] = false;
				visited2[i] = false;
			}
			DFS(v, visited1);
			DFSIn(v, visited2);
			int v_count = 0;
			for(int i = 0; i < n; ++i)
			{
				if(visited1[i] == true && visited2[i] == true)v_count++;
			}
			string *ids = new string[v_count + 1];
			int j = 0;
			for(int i = 0; i < n; ++i)
			{
				if(visited1[i] == true && visited2[i] == true)
				{
					ids[j] = GetID(i);
					j++;
				}
			}
			ids[v_count] = "end";
			return ids;
		}

		//获取某顶点所有的双向邻接节点id
		string *getTails(int v)const
		{
			int t_count = 0;
			
			Edge *e = NodeTableOut[v].adj, *s;
			int k;
			while(e != NULL)
			{
				k = e->_tail;
				s = NodeTableIn[v].adj;
				while(s != NULL && s->_tail != k)
				{
					s = s->_link;
				}
				if(s != NULL)t_count++;
				e = e->_link;
			}
			string *ids = new string[t_count + 1];
			
			int i = 0;
			e = NodeTableOut[v].adj;
			while(e != NULL)
			{
				k = e->_tail;
				s = NodeTableIn[v].adj;
				while(s != NULL && s->_tail != k)
				{
					s = s->_link;
				}
				if(s != NULL)
				{
					ids[i] = GetID(e->_tail);
					i++;
				}
				e = e->_link;
			}
			ids[t_count] = "end";
			return ids;
		}

		//Dijkstra算法，dist数组保存最短路径长度，path数组保存路径
		//此算法最终目的非求最短路径，而是计算关联度，根据每条边分级的权值定义距离
		//距离越小，关联度越大
		void ShortestPath(int v, int *dist, int *path)const
		{
			int n = NumberOfVertices();
			bool *S = new bool[n];
			int i, j, k;
			int w, min;
			for(i = 0; i < n; ++i)
			{	w = GetWeight(v, i);
				if(w != INF)
				{
					if(w >= 100)dist[i] = 1;	//有关注，距离最短
					else dist[i] = 20 - w;	//有@，随@次数缩短距离
				}
				else dist[i] = INF;
				S[i] = false;
				if(i != v && dist[i] < INF)path[i] = v;
				else path[i] = -1;
			}
			S[v] = true;
			dist[v] = 0;
			for(i = 0; i < n - 1; ++i)
			{
				min = INF;
				int u = v;
				for(j = 0; j < n; j++)
				{
					if(S[j] == false && dist[j] < min)
					{
						u = j;
						min = dist[j];
					}
				}
				S[u] = true;
				for(k = 0; k < n; ++k)
				{
					w = GetWeight(u, k);
					if(w != INF)
					{
						if(w >= 100)w = 1;
						else w = 20 - w;
					}
					else w = INF;
					if(S[k] == false && w < INF && dist[u] + w < dist[k])
					{
						dist[k] = dist[u] + w;
						path[k] = u;
					}
				}
			}
		}
		//求两点的关联度距离
		int ShortestPath(int v1, int v2, int *dist, int *path)const
		{
			ShortestPath(v1, dist, path);
			return dist[v2];
		}
	protected:
		//构造、析构函数
		Graph() {}
		Graph(int size)
		{
			_maxVertices = size;
			_numberVertices = 0;
			_numberEdges = 0;
			NodeTableOut = new Vertex[size];
			NodeTableIn = new Vertex[size];
			for(int i = 0; i < _maxVertices; ++i)
			{
				NodeTableOut[i].adj = NULL;
				NodeTableIn[i].adj = NULL;
			}
		}
		~Graph()
		{
			Edge *del;
			for(int i = 0; i < _numberVertices; ++i)
			{
				del = NodeTableOut[i].adj;
				while(del != NULL)
				{
					NodeTableOut[i].adj = del->_link;
					delete del;
					del = NodeTableOut[i].adj;
				}
				del = NodeTableIn[i].adj;
				while(del != NULL)
				{
					NodeTableIn[i].adj = del->_link;
					delete del;
					del = NodeTableIn[i].adj;
				}
			}
			delete[] NodeTableOut;
			delete[] NodeTableIn;
			_maxVertices = 0;
			_numberVertices = 0;
			_numberEdges = 0;
		}
		
		//查询某顶点的id
		string GetID(int v)const
		{
			return (v >= 0 && v < _numberVertices)? NodeTableOut[v]._id : NULL;
		}
		//查询某id是否在图内
		bool IsIn(const string &id)const
		{
			for(int i = 0; i < _numberVertices; ++i)
			{
				if(id == NodeTableOut[i]._id)return true;
			}
			return false;
		}
		//查询最大顶点数
		int MaxOfVertices()const
		{
			return _maxVertices;
		}
		//查询当前顶点数
		int NumberOfVertices()const
		{
			return _numberVertices;
		}
		//查询当前边数
		int NumberOfEdges()const
		{
			return _numberEdges;
		}
		//判断图是否为空
		bool IsEmpty()const
		{
			return (_numberVertices == 0 || _numberEdges == 0);
		}

		//插入顶点，成功返回true，失败返回false
		bool insertVertex(const string &id)
		{
			if(_numberVertices == _maxVertices)return false;
			for(int i = 0; i < _numberVertices; ++i)
			{
				if((id == NodeTableOut[i]._id) || (id == NodeTableIn[i]._id))return false;
			}
			NodeTableOut[_numberVertices]._id = id;
			NodeTableIn[_numberVertices]._id = id;
			_numberVertices++;
			return true;
		}
		//插入边（以顶点id操作）
		bool insertEdge(const string &id1, const string &id2, int weight)
		{
			int v1, v2;
			v1 = GetVertexPos(id1);
			v2 = GetVertexPos(id2);
			return (InsertEdge(v1, v2, weight));
		}	
		//删除顶点（以顶点id操作）
		bool removeVertex(const string &id)
		{
			int v;
			v = GetVertexPos(id);
			return (RemoveVertex(v));
		}
		//删除边（以顶点id操作）
		bool removeEdge(const string &id1, const string &id2)
		{
			int v1, v2;
			v1 = GetVertexPos(id1);
			v2 = GetVertexPos(id2);
			return (RemoveEdge(v1, v2));
		}
		
		//查询某边权值（以顶点id操作）
		int getWeight(const string &id1, const string &id2)const
		{
			int v1, v2;
			v1 = GetVertexPos(id1);
			v2 = GetVertexPos(id2);
			return (GetWeight(v1, v2));
		}
		//修改某边权值（以顶点id操作）
		bool changeWeight(const string &id1, const string &id2, int weight)
		{
			int v1, v2;
			v1 = GetVertexPos(id1);
			v2 = GetVertexPos(id2);
			return (ChangeWeight(v1, v2, weight));
		}

		
		//DFS（以顶点id操作）
		void DFS(const string &id, bool *visited)const
		{
			int v = GetVertexPos(id);
			DFS(v, visited);
		}
		
		//标记与某顶点属于同一连通分量的所有顶点（以顶点id操作）
		string *Components(const string &id)const
		{
			int v = GetVertexPos(id);
			return (Components(v));
		}

		//获取某顶点所有的双向邻接节点id（以顶点id操作）
		string *getTails(const string &id)const
		{
			int v = GetVertexPos(id);
			return (getTails(v));
		}
		
		//Dijkstra求关联度距离（以顶点id操作）
		void ShortestPath(const string &id, int *dist, int *path)const
		{
			int v = GetVertexPos(id);
			ShortestPath(v, dist, path);
		}
		//求两点之间关联度距离（以顶点id操作）
		int ShortestPath(const string &id1, const string &id2, int *dist, int *path)const
		{
			int v1 = GetVertexPos(id1);
			int v2 = GetVertexPos(id2);
			return (ShortestPath(v1, v2, dist, path));
		}	
	public:

};

#endif
