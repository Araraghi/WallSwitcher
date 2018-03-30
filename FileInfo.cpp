/*
 * FileInfo.cpp
 *
 *  Created on: Mar 21, 2018
 *      Author: cheshire
 */

#include "FileInfo.h"
#include <stdexcept>

bool wall::FileInfo::m_ar = false;


wall::FileInfo::FileInfo(const std::string& path, int width = 0, int height = 0, int id = -1)
	: m_path{path}, m_width{width}, m_height{height}, m_aspectRatio{static_cast<float>(width) / height}, m_id{id}
{

	int last{static_cast<int>(m_path.rfind('/'))};
	if (m_path.rfind('/') != std::string::npos)
	m_name.assign(path, last + 1, std::string::npos);
	else
		throw std::runtime_error("FileInfo constructor can't get name of the file");
}

void wall::FileInfo::print() const
{
	std::cout << std::left << std::setw(4) << m_width;
	std::cout << " x " << std::setw(10) << m_height;
	if (m_ar)
		std::cout << std::setw(10) << std::setprecision(2) << m_aspectRatio;
	std::cout << std::setw(10) << m_id;
	std::cout << m_name << "\n";

}
namespace wall {
std::fstream& operator<<(std::fstream& out, const wall::FileInfo& file)
	{
		out << file.m_path << ",";
		out	<< file.m_width<< ",";
		out	<< file.m_height;
			return out;
	}

}



