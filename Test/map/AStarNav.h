#pragma once

#include <vector>
#include <map>
#include <queue>
// ��ά��ͼA*�㷨ʵ��

const int OBLIQUE = 14;  // б���ƶ�Ȩ��Ϊ14
const int STEP = 10;
struct Point
{
	Point(int id, int reachable);
	~Point();

	void CalcF() { m_F = m_G + m_H; }
	bool IsReachable()
	{
		if (m_reachable == 0) return true;
		return false;
	}

	bool operator>(const Point* p) const 
	{
		return m_F > p->m_F;  // С����
	}

	bool operator>(const Point& p) const
	{
		return m_F > p.m_F;  // С����
	}

	int m_F{ 0 };  // F = G + H
	int m_G{ 0 };  // G ��ʾ����� A �ƶ���������ָ��������ƶ��ķ� (����б�����ƶ�)
	int m_H{ 0 };  // H ��ʾ��ָ���ķ����ƶ����յ� B ��Ԥ�ƺķ� (H �кܶ���㷽��, ���������趨ֻ�������������ƶ�)
	int m_id; // ��ά����ת����һά�����λ�ñ�� 0, 1, 2,....
	int m_reachable;  // 0 �ɴ� 1 ���ɴ�
	Point* m_parent{nullptr};  // ���游�ڵ�
	int m_close{0};  // 0 δ����closelist 1 ����closelist
	int m_x{0};
	int m_y{0};
};


//--------------
//|0 1 3 4 5
//|6 7 8 9 10
//|11 12 13 14 15
//|....
typedef std::vector<Point*> point_vec_type;
typedef std::priority_queue<Point, point_vec_type> point_queue_type;
class AStarNav
{
public:
	AStarNav();
	~AStarNav();

	bool FindPath(int start_id, int end_id, std::vector<Point*>& find_path);
	bool FindPath(int start_id, int end_id);
	bool CanReach(int start_id, int end_id);

	bool LoadMap(const char* path);
	void GetAroundPoints(int point_id, point_vec_type& point_set);
	int GetPointId(int i, int j);
private:
	void GetPos(int& i, int& j, int point_id);
	bool StopSearch(int target_id);
	Point* GetPoint(int point_id);
	Point* GetMinFInOpenList();
	bool IsInCloseList(Point* point);
	bool IsInOpenList(Point* point);
	int CalcG(const Point* start, const Point* point);
	int CalcH(const Point* end, const Point* point);
private:
	point_vec_type m_close_list;  // �������б�
	point_vec_type m_open_list;   // ������б�
	point_queue_type m_open_queue; // ������Сֵ�ĸ����ṹ
	std::map<int, Point*> m_points; // ��ͼ���Ը���
	int m_map_width{ 0 };
	int m_map_hight{ 0 };
};
