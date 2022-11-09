#pragma once
#ifndef THRAEDMANAGER_H
#define THRAEDMANAGER_H

#include <mutex>
#include <thread>
#include <functional>

class ThreadManager
{
public:
	static void CreateThread( std::function<void()> thread );
	static void WaitForAllThreads();
	static void Lock();
	static void Unlock();
private:
	static std::mutex m_mutex;
	static std::vector<std::thread*> m_pThreads;
};

#endif