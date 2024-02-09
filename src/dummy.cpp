#include "dummy.hpp"
#include "Debug/TaskIndicator.hpp"
#include <chrono>
#include <thread>

static void workMethod() {

	int t = Shadow::TaskIndicator::addTask("Running task ends soon?");

	std::this_thread::sleep_for(std::chrono::seconds(5));

	Shadow::TaskIndicator::endTask(t);

}

void Shadow::dummy() {

	std::thread th(workMethod);

	th.detach();

}