/*
 * Switcher.h
 *
 *  Created on: Mar 27, 2018
 *      Author: cheshire
 */

#ifndef SWITCHER_H_
#define SWITCHER_H_
#include <thread>
#include <iostream>
#include <mutex>
#include <condition_variable>
#include "ImageContainer.h"
#include <chrono>

namespace wall {

class Switcher
{
	static unsigned int m_currentId;
	std::mutex m_mutex;
	wall::ImageContainer* m_container;
	std::condition_variable m_condition;
	std::thread m_task;
	bool m_stop{true};
public:
	Switcher(wall::ImageContainer* images);
	void backgroundTask(int seconds);
	void startSwitching();
	void stopSwitching();
	void resetPosition(){m_currentId = 1;}
	void nextWallpaper();
	void previousWallpaper();
	int get_current_id() const {return m_currentId;};
	static void set_id(int id);
	Switcher& operator=(wall::ImageContainer* images);
	~Switcher()
	{
		if (m_task.joinable())
			m_task.join();
	}
};



}
#endif /* SWITCHER_H_ */
