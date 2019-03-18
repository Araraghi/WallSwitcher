#ifndef LIBCSV_H
#define LIBCSV_H

#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>

namespace cheshire {
template <template<typename, typename> class Container,
		  typename Value, typename Allocator = std::allocator<Value>>
void save_to_file(const char* file, const Container<Value, Allocator>& data)
{
	std::fstream stream{file, std::ios::out};
		int newLine{1};
		for (auto &i : data)
		{
			stream << i << ", ";
			if (newLine++ % 10 == 0)
				stream << "\n";
		}
}


template <template<typename, typename> class Container,
		  typename Value, typename Allocator = std::allocator<Value>>
void load_from_file(const char* file, Container<Value, Allocator>* data)
{
	std::fstream stream{file, std::ios::in};
	if (!stream.is_open())
		{std::cerr << "Couldn't load a file!";
		return;
		}
	Value temp;



	while (stream)

	{
		std::string row;
		std::getline(stream, row);
		std::stringstream ss{row};
		while (ss)
		{
			std::string cell;
			std::getline(ss, cell, ',');
			std::stringstream convert;
			convert << cell;
			convert >> temp;
			if (!cell.empty() && cell != " ")
			data->push_back(temp);
		}

	}
}

}
#endif
