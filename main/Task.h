/*
 * Task.h
 *
 *  Created on: 30.07.2017
 *      Author: Nicolas
 */

#ifndef MAIN_TASK_H_
#define MAIN_TASK_H_


#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <string>
/**
 * @brief Encapsulate a runnable task.
 *
 * This class is designed to be subclassed with the method:
 *
 * @code{.cpp}
 * void run(void *data) { ... }
 * @endcode
 *
 * For example:
 *
 * @code{.cpp}
 * class CurlTestTask : public Task {
 *    void run(void *data) {
 *       // Do something
 *    }
 * };
 * @endcode
 *
 * implemented.
 */
class Task {
public:
	Task(std::string taskName="Task", uint16_t stackSize=10000);
	virtual ~Task();
	void setStackSize(uint16_t stackSize);
	void start(void* taskData=nullptr);
	void stop();
	/**
	 * @brief Body of the task to execute.
	 *
	 * This function must be implemented in the subclass that represents the actual task to run.
	 * When a task is started by calling start(), this is the code that is executed in the
	 * newly created task.
	 *
	 * @param [in] data The data passed in to the newly started task.
	 */
	virtual void run(void *data) = 0; // Make run pure virtual
	void delay(int ms);

private:
	xTaskHandle m_handle;
	void*       m_taskData;
	static void runTask(void *data);
	std::string m_taskName;
	uint16_t    m_stackSize;
};




#endif /* MAIN_TASK_H_ */
