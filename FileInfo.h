/*
 * FileInfo.h
 *
 *  Created on: Mar 21, 2018
 *      Author: cheshire
 */

#ifndef FILEINFO_H_
#define FILEINFO_H_

#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>

namespace wall
{
class FileInfo
{
	std::string m_name;
	std::string m_path;
	int m_width, m_height;
	static bool m_ar;
	float m_aspectRatio;


public:
	int m_id;
	FileInfo(const std::string& path, int width, int height, int id);
	const std::string& name() 	const {return m_name;}
	const std::string& path() 	const {return m_path;}
	const int 		   width() 	const {return m_width;}
	const int 		   height() const {return m_height;}
	void 		   	   print()  const;
	static void		   setShowAspectRatio() {m_ar = !m_ar;}
	const float 	   aspectRatio() const {return m_aspectRatio;}
	const int		   id()		const {return m_id;};
	friend std::fstream& operator<<(std::fstream& out, const wall::FileInfo& file);

};

}



#endif /* FILEINFO_H_ */
