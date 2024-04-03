#include "AXE/AXELink.hpp"

#include <boost/interprocess/creation_tags.hpp>
#include <boost/interprocess/detail/os_file_functions.hpp>
#include <boost/interprocess/shared_memory_object.hpp>

void initLink() {
	using namespace boost::interprocess;
	shared_memory_object shm_obj(create_only, "axe_link_shm", read_write);
}

void breakLink() {
	using namespace boost::interprocess;
	shared_memory_object::remove("axe_link_shm");
}