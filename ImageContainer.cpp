/*
 * ImageContainer.cpp
 *
 *  Created on: Mar 26, 2018
 *      Author: cheshire
 */
#include "ImageContainer.h"





const double wall::ImageContainer::fraction{1.0 / (1.0 + RAND_MAX)};

wall::ImageContainer& wall::ImageContainer::getFiles(const std::string &directory)
{
	using namespace boost::filesystem;
	using namespace boost::gil;
	try
			{			int id{1};
						for (directory_entry& x : directory_iterator(directory))
						{
							boost::filesystem::path p1(x.path());
							std::string temp{p1.c_str()};
							if (temp.find(".jpg")!=std::string::npos)
							{
								point2<std::ptrdiff_t> size{jpeg_read_dimensions(temp.c_str())};
								m_images.push_back(wall::FileInfo(p1.c_str(), size.x, size.y, id++));
							}
							else if (temp.find(".png")!=std::string::npos)
							{
								point2<std::ptrdiff_t> size{png_read_dimensions(temp.c_str())};
								m_images.push_back(wall::FileInfo(p1.c_str(), size.x, size.y, id++));
							}
				}
			}
			catch(const filesystem_error& ex)
			{
				std::cout << ex.what() << "\n";
			}
			return *this;

}

wall::ImageContainer& wall::ImageContainer::sort(sortType type)
{
	switch (type)
	{
	case sortType::Width:
		std::sort(m_images.begin(), m_images.end(), [](const wall::FileInfo &f1, const wall::FileInfo &f2)
				{return (f1.width() < f2.width());});
		break;
	case sortType::Height:
		std::sort(m_images.begin(), m_images.end(), [](const wall::FileInfo &f1, const wall::FileInfo &f2)
						{return (f1.height() < f2.height());});
		break;
	case sortType::AspectRatio:
		std::sort(m_images.begin(), m_images.end(), [](const wall::FileInfo &f1, const wall::FileInfo &f2)
								{return (f1.aspectRatio() < f2.aspectRatio());});
		break;
	case sortType::Name:
		std::sort(m_images.begin(), m_images.end(), [](const wall::FileInfo &f1, const wall::FileInfo &f2)
								{return (f1.name() < f2.name());});
		break;
	}
	assignID();

	return *this;
}

int wall::ImageContainer::getRandom(int min, int max)
{
	srand(static_cast<unsigned int>(time(0)));
	return min + static_cast<int>((max - min + 1) * rand() * fraction);
}


wall::ImageContainer& wall::ImageContainer::randomize()
{
	for (auto &i : m_images)
	{
		int random{getRandom(0, m_images.size() - 1)};
		assert(random >= 0 && static_cast<unsigned int>(random) < m_images.size() && "random number bounds");
		std::swap(i, m_images[random]);
	}
	assignID();
	return *this;
}

bool wall::ImageContainer::setBackground(int id) const
{
	std::string bg{m_images[id - 1].path()};
	std::cout << "image id and name: " << m_images[id - 1].name() << " " << id << "\n";
	std::string temp{"gsettings set org.cinnamon.desktop.background picture-uri file://"};
	temp += bg;
	if (boost::filesystem::exists(bg))
		{
		return true;
		}
	else
		return false;

}

const wall::ImageContainer& wall::ImageContainer::print() const
{
	std::cout << "Width Height     AR        ID        Image Name\n";
	for (auto &i : m_images)
	{
		i.print();
	}
	return *this;

}

wall::ImageContainer& wall::ImageContainer::clear()
{

	m_images.clear();
	return *this;
}

void wall::ImageContainer::assignID()
{
	int j{1};
	for (auto &i : m_images)
	{
		i.m_id = j++;
	}
}

void wall::ImageContainer::save_to_file(const char* name, int currentID) const
{

	std::string temp{name};
	temp += "_id";
	cheshire::save_to_file(name, m_images);
	std::vector<int> id{currentID};
	cheshire::save_to_file(temp.c_str(), id);
}

unsigned int wall::ImageContainer::load_from_file(const char* name)
{
	std::fstream stream{name, std::ios::in};
	if (!stream.is_open())
		{
		std::cerr << "File not loaded!";
		return 1;
		}
	m_images.clear();
	unsigned int i{1};
	std::string path;
	int width, height;
	while (stream)
	{
		std::string row;
		std::getline(stream, row);
		std::stringstream ss{row};
		while (ss)
		{
			std::string instance;
			std::getline(ss, path, ',');
			std::string cell;
			std::getline(ss, cell, ',');
			std::stringstream convert;
			convert << cell;
			convert >> width;
			std::getline(ss, cell, ',');
			ss.ignore();
			convert.clear();
			convert << cell;
			convert >> height;


			if (!path.empty() && path != " ")
			{
			m_images.push_back(wall::FileInfo(path , width, height, i++));
			}
		}

	}
	stream.close();
	path.clear();
	path = name;
	path += "_id";
	stream.open(path, std::ios::in);
	std::getline(stream, path, ',');
	std::stringstream convert{path};
	convert >> i;
	return i;
}

void wall::ImageContainer::open_in_viewer(unsigned int id)
{
	std::string temp{"xdg-open " + m_images[id - 1].path()};
	std::system(temp.c_str());
}

wall::FileInfo&			wall::ImageContainer::operator[](unsigned int id)
{
	if (id <= m_images.size())
		return m_images[id - 1];
	else
	{
		std::cerr << "ImageContainer::operator[] Image id is too large";
		return m_images[0];
	}
}

