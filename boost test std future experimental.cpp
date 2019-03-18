#include <iostream>
#include <vector>
#include <boost/filesystem.hpp>
#include <string>
#include "FileInfo.h"
#include "ImageContainer.h"
#include <thread>
#include <chrono>
#include <future>


bool checkFolderPath(const char* path)
{
	using namespace boost::filesystem;
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

void setConditions(wall::ImageContainer &images)
{
	wall::ImageContainer temp{images};
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
	images.clear();
	for (auto &i : temp.getVector())
	{
		int width = i.width();
		int height = i.height();
		float AR = i.aspectRatio();
		if (width >= minWidth && width <= maxWidth && height >= minHeight && height <= maxHeight && AR >= minAR && AR <= maxAR)
		{
			images.push_back(i);
		}
	}
	std::cout << "Conditions applied!\n";

}

void setImage(const wall::ImageContainer& images)
{
	std::string id;
	while(true){
	std::cout << "Enter image id or b to go back:";
	std::cin >> id;
	if (id == "b")
		return;
	if (!images.setBackground(std::stoi(id) - 1))
		std::cout << "Wrong image id!\n";
	}

}

void setOrder(wall::ImageContainer &images)
{
	std::cout << "1 - sort by Name\n";
	std::cout << "2 - sort by Width\n";
	std::cout << "3 - sort by Height\n";
	std::cout << "4 - sort by Aspect Ratio\n";
	std::cout << "b - back\n";
	char answer;
	while(true)
	{
		std::cin >> answer;
		switch (answer)
		{
		case '1':
			images.sort(wall::ImageContainer::Name);
			break;
		case '2':
			images.sort(wall::ImageContainer::Width);
			break;
		case '3':
			images.sort(wall::ImageContainer::Height);
			break;
		case '4':
			images.sort(wall::ImageContainer::AspectRatio);
			break;
		case 'b':
			return;
		}
		std::cout << "Sorted!\n";
	}
}


int main(int argc, char *argv[])
{
	std::thread switching;




	std::vector <wall::FileInfo> rawFiles;
	wall::ImageContainer allImages;
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
	allImages.getFiles(folderPath);															//Loads .png and .jpg files into files vector
	wall::ImageContainer processedImages{allImages};
	wall::FileInfo::setShowAspectRatio();													//Self-explanatory flag
	allImages.print();
	std::promise<int> *promise;
	std::future<int> future;
	char answer;
	while(true)
	{

	std::cout << "1 - Set conditions\n";
	std::cout << "2 - Set image\n";
	std::cout << "3 - Set order by\n";
	std::cout << "4 - Reset conditions\n";
	std::cout << "5 - Start switching\n";
	std::cout << "6 - Stop switching\n";
	std::cout << "q - Quit\n";
	std::cin >> answer;
	if (std::cin.fail())
	{
		std::cin.ignore(32767, '\n');
		std::cin.clear();
	}
	if (answer == 'q')
				break;
	switch (answer)
	{
	case '1':
		setConditions(processedImages);
		break;
	case '2':
		setImage(allImages);
		break;
	case '3':
		setOrder(processedImages);
		break;
	case '4':
		processedImages = allImages;
		break;
	case '5':
		if (switching.joinable())
			switching.join();
		promise = new std::promise<int>;
		future = promise->get_future();
		switching = std::thread(&wall::ImageContainer::startSwitching, &processedImages, 5, std::move(future), processedImages.getCurrent());
		break;
	case '6':
		promise->set_value(1);
		delete promise;
		break;
	}
	if (processedImages.getVector().empty())
		allImages.print();
	else
		processedImages.print();

	}
	if (switching.joinable())
				switching.join();
	processedImages.clear();
	delete promise;
	}
	return 0;
}
