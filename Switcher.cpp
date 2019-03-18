/*
 * Switcher.cpp
 *
 *  Created on: Mar 27, 2018
 *      Author: cheshire
 */
#include "Switcher.h"

unsigned int wall::Switcher::m_currentId{1};

wall::Switcher::Switcher(wall::ImageContainer* images) : m_container{images}
{

}

void wall::Switcher::startSwitching(bool ask_for_interval, int _interval)
{
	if (!m_stop)
		stopSwitching();
	if (m_task.joinable())
			m_task.join();
	int interval;
	if (ask_for_interval)
	{
		std::cout << "Set time interval in minutes: ";
		do
		{
			std::cin >> interval;
			if (std::cin.fail())
			{
				std::cin.ignore(32767, '\n');
				std::cin.clear();
			}
		}
	while(interval <= 0);
	}
	else interval = _interval;

	m_stop = false;
	m_task = std::thread(&wall::Switcher::backgroundTask, this, interval);
	std::cout << "Switching started\n";
}

void wall::Switcher::backgroundTask(int minutes)
{
	std::unique_lock<std::mutex> lock{m_mutex};
	int i{1};
	const std::vector<wall::FileInfo>& tempFiles{m_container->getVector()};
	while (!m_stop)
	{
		if (1 + m_currentId > tempFiles.size())
			m_currentId = 1;
		if (!m_condition.wait_for(lock, std::chrono::minutes(1), [this](){return m_stop;}))
		{
			if (i % minutes == 0)
			{
			m_container->setBackground(++m_currentId);
			}
			else
				std::cout << "*" << std::flush;

			if (i == minutes)
							i = 0;
			++i;

		}
	}
	std::cout <<"Stopped switching.\n";


}

void wall::Switcher::stopSwitching()
{
	std::lock_guard<std::mutex> lock{m_mutex};
	m_stop = true;
	m_condition.notify_one();

}

wall::Switcher& wall::Switcher::operator=(wall::ImageContainer* images)
{
	if (images == m_container)
		return *this;
	m_container = images;
	return *this;
}

void wall::Switcher::nextWallpaper()
{
	std::lock_guard<std::mutex> lock(m_mutex);
	if (1 + m_currentId > m_container->getVector().size())
		m_currentId = 0;
	m_container->setBackground(++m_currentId);
}

void wall::Switcher::previousWallpaper()
{
	std::lock_guard<std::mutex> lock(m_mutex);

	if (m_currentId - 1 == 0)
		m_currentId = m_container->getVector().size() + 1;

	m_container->setBackground(--m_currentId);
}

void wall::Switcher::set_id(int id)
{
 m_currentId = id;
}


