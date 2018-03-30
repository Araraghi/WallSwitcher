/*
 * Switcher.h
 *
 *  Created on: Mar 26, 2018
 *      Author: cheshire
 */

#ifndef IMAGECONTAINER_H_
#define IMAGECONTAINER_H_
#include "FileInfo.h"
#include <vector>
#include <algorithm>
#include <iterator>
#include <cassert>
#include <cstdlib>
#include <ctime>
#include <exception>
#include <sstream>
#include <boost/filesystem.hpp>
#include <boost/gil/extension/io/jpeg_io.hpp>
#include <boost/gil/extension/io/png_io.hpp>
#include "libCSV.h"

namespace wall
{

class ImageContainer
{
	static const double fraction;
	int getRandom(int min, int max);
	std::vector<wall::FileInfo> m_images;
	void assignID();

public:
	enum sortType
		{
			Width, Height, AspectRatio, Name
		};
	ImageContainer(){}
	ImageContainer& sort(sortType type);
	ImageContainer& randomize();
	bool 			setBackground(int id) const;
	const ImageContainer& print() const;
	ImageContainer& getFiles(const std::string &directory);
	ImageContainer& clear();
	void save_to_file(const char* name, int currentID) const;
	unsigned int load_from_file(const char* name);
	void open_in_viewer(int id);
	const std::vector<wall::FileInfo>& getVector()
		{
			return m_images;
		}
	ImageContainer& push_back(const wall::FileInfo &file)
	{
		m_images.push_back(file);
		return *this;
	}

};



}
#endif /* IMAGECONTAINER_H_ */
