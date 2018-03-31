#include <iostream>
#include <boost/filesystem.hpp>
#include "FileInfo.h"
#include "ImageContainer.h"
#include "Switcher.h"

void menu()
{
			std::cout << "1 - Set conditions\n";
			std::cout << "2 - Set image\n";
			std::cout << "3 - Set order by\n";
			std::cout << "4 - Reset conditions\n";
			std::cout << "5 - Randomize\n";
			std::cout << "6 - Start switching\n";
			std::cout << "7 - Stop switching\n";
			std::cout << "8 - Next wallpaper\n";
			std::cout << "9 - Previous wallpaper\n";
			std::cout << "o - Open image in viewer\n";
			std::cout << "f - Add current image to Favourites\n";
			std::cout << "s - Show favourite images\n";
			std::cout << "q - Quit\n";
}

void showFavourites(wall::ImageContainer& container)
{
	if (container.empty())
	{
		std::cout << "No favourite images! Nothing to see here.\n";
		return;
	}
	container.assignID();
	container.print();
	char answer;
	std::cout << "1 - Delete from favourites\n";
	std::cout << "2 - Clear favourites\n";
	std::cout << "b - Go back\n";
	while (true)
	{
		std::cin >> answer;
		if (std::cin.fail())
		{
			std::cin.ignore(32767, '\n');
			std::cin.clear();
		}
		switch (answer)
		{
		case '1':
			std::cout << "Enter Id of image to remove from favourites: ";
			unsigned int id;
			std::cin >> id;
			if (id >= 1 && id <= container.size())
				container.erase(id);
			break;
		case '2':
			container.clear();
			break;
		case 'b':
			menu();
			return;
			break;
		default:
			std::cout << "Wrong input!\n";
		}
	}
}

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
	//Simply make temp copy of container, clear orginal container, apply conditions on temp and push_back to orginal
	wall::ImageContainer temp{images};
	int minWidth{0}, maxWidth{10000}, minHeight{0}, maxHeight{10000};
	float minAR{0.0f}, maxAR{100.0f};
	char answer;
	std::cout << "1 - set minimum Width\n";
	std::cout << "2 - set minimum Height\n";
	std::cout << "3 - set maximum Width\n";
	std::cout << "4 - set maximum Height\n";
	std::cout << "5 - set minimum Aspect Ratio\n";
	std::cout << "6 - set maximum Apsect Ratio\n";
	std::cout << "r - run\n";
	std::cout << "b - back (discard changes)\n";
	do
	{
	std::cin >> answer;
	if (std::cin.fail())
		{
			std::cin.ignore(25767, '\n');
			std::cin.clear();
		}
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
	images.assignID();
	std::cout << "Conditions applied!\n";

}

void setImage(const wall::ImageContainer& images)
{
	std::string id;
	while(true)
	{
	std::cout << "Enter image id or b to go back:";
	std::cin >> id;
	if (id == "b")
		return;
	unsigned intID = std::stoi(id);
	if (intID > 0 && (intID <= images.size()))
	{
		images.setBackground(intID - 1);
	}
	else
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
	if (std::cin.fail())
			{
				std::cin.ignore(32767, '\n');
				std::cin.clear();
			}
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
	wall::ImageContainer allImages;
	std::cout << "Welcome to cinnanom mint wallpaper changer.\n";
	std::string folderPath;																	//Declare vector that will contain info about the images
	bool running{true};
	while(running)
	{																						//Main loop
		if (argv[1]) 																		//If argument not present enter path manually via console
			folderPath = argv[1];
		else
		{

			do																				//Enter path manually
			{
				std::cout << "Enter a path to a directory to load pictures from or q for exit:\n";
				std::cin >> folderPath;
				if (folderPath == "q")
					return 20;
			}
			while (!checkFolderPath(folderPath.c_str()));									//Checks whether path was entered correctly
	}

	allImages.getFiles(folderPath);															//Loads .png and .jpg files ImageContainer
	wall::ImageContainer processedImages{allImages};										//copy of ImageContainer in case user wishes to reset filters
	wall::ImageContainer favourites;
	wall::FileInfo::setShowAspectRatio();													//Self-explanatory flag
	wall::Switcher::set_id(processedImages.load_from_file("wallpapers"));
	favourites.load_from_file("favourites");
	processedImages.print();

	char answer;
	wall::Switcher switcher{&processedImages};												//switcher is an object that manages switching task in background
	menu();
	while(true)
	{


		std::cin >> answer;
		if (std::cin.fail())
		{
			std::cin.ignore(32767, '\n');
			std::cin.clear();
		}
		if (answer == 'q')
			{
				switcher.stopSwitching();
				running = false;
				break;
			}
		switch (answer)
		{
		case '1':
			setConditions(processedImages);
			processedImages.print();
			switcher.resetPosition();
			menu();
			break;
		case '2':
			setImage(allImages);
			break;
		case '3':
			setOrder(processedImages);
			processedImages.print();
			switcher.resetPosition();
			menu();
			break;
		case '4':
			processedImages = allImages;
			break;
		case '5':
			processedImages.randomize();
			switcher.resetPosition();
			break;
		case '6':
			switcher = &processedImages;
			switcher.startSwitching();
			break;
		case '7':
			switcher.stopSwitching();
			break;
		case '8':
			switcher.nextWallpaper();
			break;
		case '9':
			switcher.previousWallpaper();
			break;
		case 'o':
			processedImages.open_in_viewer(switcher.get_current_id());
			break;
		case 'f':
			favourites.push_back(processedImages[switcher.get_current_id()]);
			break;
		case 's':
			showFavourites(favourites);
			break;
		default:
			std::cout << "Wrong input!\n";
		}
	}
		processedImages.save_to_file("wallpapers", switcher.get_current_id());
		favourites.save_to_file("favourites");
	}

	return 0;
}
