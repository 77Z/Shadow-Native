#include <boost/interprocess/creation_tags.hpp>
#include <boost/interprocess/detail/os_file_functions.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include <cstring>
#include <iostream>
#include <node/node.h>
#include <node/v8-function-callback.h>
#include <node/v8-isolate.h>
#include <node/v8-local-handle.h>
#include <node/v8-object.h>
#include <node/v8-primitive.h>
#include <node/v8-value.h>

void foobar(const v8::FunctionCallbackInfo<v8::Value>& args) {

	v8::Isolate* isolate = args.GetIsolate();

	std::cout << "Test" << std::endl;

	// args.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, "Return Value"));
}

void connectLink(const v8::FunctionCallbackInfo<v8::Value>& args) {
	using namespace boost::interprocess;

	v8::Isolate* isolate = args.GetIsolate();

	shared_memory_object shm(open_only, "axe_link_shm", read_write);

	shm.truncate(1000);

	mapped_region region(shm, read_write);

	std::memset(region.get_address(), 1, region.get_size());

	char* mem = static_cast<char*>(region.get_address());

}

void Init(v8::Local<v8::Object> exports) {
	NODE_SET_METHOD(exports, "connectLink", connectLink);
	NODE_SET_METHOD(exports, "foobar", foobar);
}

NODE_MODULE(NODE_GYP_MODULE_NAME, Init);