#include <iostream>
#include <vector>
#include <boost/filesystem.hpp>
#include <boost/gil/extension/io/jpeg_io.hpp>
#include <boost/gil/extension/io/png_io.hpp>
#include <string>
#include <iomanip>
#include "FileInfo.h"
#include <algorithm>
#include <cstdlib>
#include <ctime>



using namespace boost::filesystem;
using namespace boost::gil;



// Ugly way of setting a background in cinnamon desktop
void setBackground(const std::string &bg)
{
	// gsettings set org.cinnamon.desktop.background picture-uri file:///home/cheshire/Pictures/59464134_p0.jpg
	std::string temp{"gsettings set org.cinnamon.desktop.background picture-uri file://"};
	temp += bg;
	std::system(temp.c_str());
}
// Comparison functions to go along with std::sort
bool compareWidth(const wall::FileInfo &f1, const wall::FileInfo & f2)
{
	return (f1.width() < f2.width());
}
bool compareHeight(const wall::FileInfo &f1, const wall::FileInfo & f2)
{
	return (f1.height() < f2.height());
}
bool compareAspect(const wall::FileInfo &f1, const wall::FileInfo & f2)
{
	return (f1.aspectRatio() < f2.aspectRatio());
}
bool compareName(const wall::FileInfo &f1, const wall::FileInfo & f2)
{
	return (f1.name() < f2.name());
}

void getFiles(const char* dirpath, std::vector<wall::FileInfo> &files)
{
	try
		{			int id{1};
					for (directory_entry& x : directory_iterator(dirpath))
					{
						boost::filesystem::path p1(x.path());
						std::string temp{p1.c_str()};
						if (temp.find(".jpg")!=std::string::npos)
						{
							point2<std::ptrdiff_t> size{jpeg_read_dimensions(temp.c_str())};
							files.push_back(wall::FileInfo(p1.c_str(), size.x, size.y, id++));
						}
						else if (temp.find(".png")!=std::string::npos)
						{
							point2<std::ptrdiff_t> size{png_read_dimensions(temp.c_str())};
							files.push_back(wall::FileInfo(p1.c_str(), size.x, size.y, id++));
						}
			}
		}
		catch(const filesystem_error& ex)
		{
			std::cout << ex.what() << "\n";
		}
}

bool checkFolderPath(const char* path)
{
	if (!exists(path))
	{
		std::cout << "Path doesn't exists.\n";
		return false;
	}
	if (!is_directory(path))
	{
		std::cout << "Path doesn't lead to directory.\n";
		return false;
	}
	return true;
}

void setConditions(const std::vector<wall::FileInfo> &rawFiles, std::vector<wall::FileInfo> &processedFiles)
{
	int minWidth{0}, maxWidth{10000}, minHeight{0}, maxHeight{10000};
	float minAR{0.0f}, maxAR{100.0f};
	char answer;
	do
	{
	if (std::cin.fail())
	{
		std::cin.ignore(25767, '\n');
		std::cin.clear();
	}
	std::cout << "1 - set minimum Width\n";
	std::cout << "2 - set minimum Height\n";
	std::cout << "3 - set maximum Width\n";
	std::cout << "4 - set maximum Height\n";
	std::cout << "5 - set minimum Aspect Ratio\n";
	std::cout << "6 - set maximum Apsect Ratio\n";
	std::cout << "r - run\n";
	std::cout << "b - back (discard changes)\n";

	std::cin >> answer;
	switch (answer)
	{
	case '1':
		std::cout << "enter minimum Width: ";
		std::cin >> minWidth;
		break;
	case '2':
		std::cout << "enter minimum Height: ";
		std::cin >> minHeight;
		break;
	case '3':
		std::cout << "enter maximum Width: ";
		std::cin >> maxWidth;
		break;
	case '4':
		std::cout << "enter maximum Height:";
		std::cin >> maxHeight;
		break;
	case '5':
		std::cout << "enter minimum Aspect Ratio:";
		std::cin >> minAR;
		break;
	case '6':
		std::cout << "enter maximum Aspect Ratio: ";
		std::cin >> maxAR;
		break;
	case 'b':
		return;
	}
	}
	while (answer != 'r');
	processedFiles.clear();
	for (auto &i : rawFiles)
	{
		int width = i.width();
		int height = i.height();
		float AR = i.aspectRatio();
		if (width >= minWidth && width <= maxWidth && height >= minHeight && height <= maxHeight && AR >= minAR && AR <= maxAR)
		{
			processedFiles.push_back(i);
		}
	}
	std::cout << "Conditions applied!\n";

}

void setImage(const std::vector<wall::FileInfo> &rawFiles)
{
	std::string id;
	while(true){
	std::cout << "Enter image id or b to go back:";
	std::cin >> id;
	if (id == "b")
		return;
	id = rawFiles[std::stoi(id) - 1].path();
	if (exists(id))
	{
		setBackground(id);
	}
	else
		std::cout << "Wrong image id!\n";
	}

}

void setOrder(std::vector<wall::FileInfo> &processedFiles)
{
	std::cout << "1 - sort by Name\n";
	std::cout << "2 - sort by Width\n";
	std::cout << "3 - sort by Height\n";
	std::cout << "4 - sort by Aspect Ratio\n";
	std::cout << "b - back\n";
	char answer;
	using method = bool(*)(const wall::FileInfo&, const wall::FileInfo&);
	method ptr{compareName};
	while(true)
	{
		std::cin >> answer;
		switch (answer)
		{
		case '1':
			ptr = compareName;
			break;
		case '2':
			ptr = compareWidth;
			break;
		case '3':
			ptr = compareHeight;
			break;
		case '4':
			ptr = compareAspect;
			break;
		case 'b':
			return;
		}
		std::sort(processedFiles.begin(), processedFiles.end(), ptr);
		std::cout << "Sorted!\n";
	}
}

void columnDesc()
{
	std::cout << "Width Height     AR        ID        Image Name\n";
}
int main(int argc, char *argv[])
{
	srand(static_cast<unsigned int>(time(0)));
	std::vector <wall::FileInfo> rawFiles;
	std::cout << "Welcome to cinnanom mint wallpaper changer.\n";
	std::string folderPath;																	//Declare vector that will contain info about the images
	while(true) {																			//Main loop
	if (argv[1]) 																			//If argument not present enter path manually via console
		folderPath = argv[1];
	else
	{

	do																						//Enter path manually
	{
		std::cout << "Enter a path to a directory to load pictures from or q for exit:\n";
		std::cin >> folderPath;
		if (folderPath == "q")
			return 20;
	}
	while (!checkFolderPath(folderPath.c_str()));											//Checks whether path was entered correctly
	}
	getFiles(folderPath.c_str(), rawFiles);													//Loads .png and .jpg files into files vector
	std::vector <wall::FileInfo> processedFiles;

	processedFiles = rawFiles;
	wall::FileInfo::setShowAspectRatio();													//Self-explanatory flag
	//std::sort(files.begin(), files.end(), compareName);
	columnDesc();																			//Column info
	for (auto &i : rawFiles)																//Prints images in folder
	{
		i.print();
	}
	char answer;
	while(true)
	{
	std::cout << "1 - Set conditions\n";
	std::cout << "2 - Set image\n";
	std::cout << "3 - Set order by\n";
	std::cout << "4 - Start switching";
	std::cout << "q - Quit\n";
	std::cin >> answer;
	if (answer == 'q')
				return 10;
	switch (answer)
	{
	case '1':
		if (processedFiles.empty())
			setConditions(rawFiles, processedFiles);
		else
		{
			std::vector<wall::FileInfo> temp{processedFiles};
			setConditions(temp, processedFiles);
		}
		break;
	case '2':
		setImage(rawFiles);
		break;
	case '3':
		if (processedFiles.empty())
			processedFiles = rawFiles;
		setOrder(processedFiles);
		break;
	case '4':
		if (processedFiles.empty())
			processedFiles = rawFiles;
		break;

	}
	columnDesc();
	for (auto &i : processedFiles)
			{
				i.print();
			}
	}

	processedFiles.clear();
	}
	return 0;
}
