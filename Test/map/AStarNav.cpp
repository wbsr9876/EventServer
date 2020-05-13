#include <fstream>
#include <string>
#include <algorithm>
#include "AStarNav.h"
#include "lexical_cast.hpp"
#include "StringUtil.h"


Point::Point(int id, int reachable):m_id(id),m_reachable(reachable)
{
}

Point::~Point()
{
}

AStarNav::AStarNav()
{

}

AStarNav::~AStarNav()
{
	for (auto it : m_points)
	{
		delete it.second;
		it.second = nullptr;
	}
	m_close_list.clear();
	m_open_list.clear();
	m_points.clear();
}

bool AStarNav::LoadMap(const char* path)
{
	std::ifstream ifs(path);
	if (!ifs) return false;

	std::string line;
	int point_id = 0;
	while (std::getline(ifs, line))
	{
		std::vector<std::string>  vec_nodes;
		StringUtil::SplitCpp(line, ",", vec_nodes);
		if (m_map_width == 0)
		{
			m_map_width = (int)vec_nodes.size();
		}
		for (auto& it : vec_nodes)
		{
			int k = lexical_cast<int>(it);
			Point* point = new Point(point_id, k);
			point->m_x = point_id % m_map_width;
			point->m_y = m_map_hight;
			m_points.emplace(point_id, point);
			point_id++;
		}
		m_map_hight++;
	}
	ifs.close();
	return true;
}

void AStarNav::GetPos(int& i, int& j, int point_id)
{
	i = point_id % m_map_width;
	j = point_id / m_map_hight;
}

int AStarNav::GetPointId(int i, int j)
{
	int point_id = j * m_map_width + i;
	return point_id;
}

void AStarNav::GetAroundPoints(int point_id, point_vec_type& point_vec)
{
	int i = 0, j = 0;
	GetPos(i, j, point_id);
	Point* point = nullptr;

	for(int x = i-1; x<=i+1; x++)
		for (int y = j - 1; y <= j + 1; y++)
		{
			if (x >= m_map_width || x < 0) continue;
			if (y >= m_map_hight || y < 0) continue;
			Point* point = GetPoint(GetPointId(x, y));
			if (point && point->IsReachable() && point->m_id != point_id)
			{
				point_vec.emplace_back(point);
			}
		}
}


bool AStarNav::FindPath(int start_id, int end_id, std::vector<Point*>& find_path)
{
	Point* start = GetPoint(start_id);
	Point* end = GetPoint(end_id);
	if (start == nullptr || end == nullptr) 
		return false;
	m_open_list.emplace_back(start);
	while(m_open_list.size()>0)
	{
		Point* tmp_start = GetMinFInOpenList();
		m_close_list.emplace_back(tmp_start);
		//auto it = std::find_if(m_open_list.begin(), m_open_list.end(), [tmp_start](const Point* elem) {
		//	if (elem == tmp_start)
		//	{
		//		return true;
		//	}
		//	return false;
		//	});

		m_open_list.erase(it);

		points_type around_points;
		GetAroundPoints(tmp_start->m_id, around_points);
		for (auto point : around_points)
		{
			if (!point->IsReachable() || IsInCloseList(point))
				continue;
			if (!IsInOpenList(point))
			{
				point->m_parent = tmp_start;
				point->m_G = CalcG(tmp_start, point);
				point->m_H = CalcH(end, point);
				m_open_list.emplace_back(point);
			}
			else
			{
				int G = CalcG(tmp_start, point);
				if (G < point->m_G)
				{
					point->m_parent = tmp_start;
					point->m_G = G;
					point->CalcF();
				}
			}
		}
		if (IsInOpenList(end))
		{
			Point* point = end;
			while (point->m_parent)
			{
				find_path.emplace_back(point);
				point = point->m_parent;
			}
			find_path.emplace_back(start);  // ·�������˿�ʼ����λ��
			return true;
		}
	} 
	return false;
}

bool AStarNav::FindPath(int start_id, int end_id)
{
	Point* start = GetPoint(start_id);
	Point* end = GetPoint(end_id);
	if (start == nullptr || end == nullptr)
		return false;
	m_open_list.emplace_back(start);
	while (m_open_list.size() > 0)
	{
		Point* tmp_start = m_open_list.top();
		m_close_list.emplace_back(tmp_start);
		auto it = std::find_if(m_open_list.begin(), m_open_list.end(), [tmp_start](const Point* elem) {
			if (elem == tmp_start)
			{
				return true;
			}
			return false;
			});
		m_open_list.pop();

		point_vec_type around_points;
		GetAroundPoints(tmp_start->m_id, around_points);
		for (auto point : around_points)
		{
			if (!point->IsReachable() || IsInCloseList(point))
				continue;
			if (!IsInOpenList(point))
			{
				point->m_parent = tmp_start;
				point->m_G = CalcG(tmp_start, point);
				point->m_H = CalcH(end, point);
				m_open_list.emplace_back(point);
			}
			else
			{
				int G = CalcG(tmp_start, point);
				if (G < point->m_G)
				{
					point->m_parent = tmp_start;
					point->m_G = G;
					point->CalcF();
				}
			}
		}
		if (IsInOpenList(end))
		{
			return true;
		}
	}
	return false;

}

bool AStarNav::CanReach(int start_id, int end_id)
{
	return FindPath(start_id, end_id);
}

bool comp(const Point* a, const Point* b)
{
	return a->m_F < b->m_F;
}

Point* AStarNav::GetMinFInOpenList()
{
	//std::stable_sort(m_open_list.begin(), m_open_list.end(), comp);
	//auto it = m_open_list.begin();
	//if (it != m_open_list.end())
	//{
	//	return *it;
	//}
	return nullptr;
}

// Ŀ����Ѿ��� "�����б�", ��ʱ��·�����ҵ�
bool AStarNav::StopSearch(int target_id)
{
	auto it = std::find_if(m_open_list.begin(), m_open_list.end(), [target_id](Point* point) {
		if (point->m_id == target_id)
		{
			return true;
		}
		return false;
		});
	if (it != m_open_list.end())
	{
		return true;
	}
	return false;
}


Point* AStarNav::GetPoint(int point_id)
{
	auto it = m_points.find(point_id);
	if (it == m_points.end())
	{
		return nullptr;
	}
	return it->second;
}

bool AStarNav::IsInCloseList(Point* point)
{
	auto it = std::find(m_close_list.begin(), m_close_list.end(), point);
	if (it == m_close_list.end())
		return false;
	return true;
}

bool AStarNav::IsInOpenList(Point* point)
{
	auto it = std::find(m_open_list.begin(), m_open_list.end(), point);
	if (it == m_open_list.end())
		return false;
	return true;
}

int AStarNav::CalcG(const Point* start, const Point* point)
{ 
	int G = (abs(point->m_x - start->m_x) + abs(point->m_y - start->m_y)) == 2 ? STEP : OBLIQUE;
	int parentG = point->m_parent != nullptr ? point->m_parent->m_G : 0;
	return G + parentG;
}

int AStarNav::CalcH(const Point* end, const Point* point)
{
	int step = abs(point->m_x - end->m_x) + abs(point->m_y - end->m_y);
	return step * STEP;
}