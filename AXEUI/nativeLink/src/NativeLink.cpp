#include "node/v8-array-buffer.h"
#include "node/v8-typed-array.h"
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

static bool linkConnected = false;
// static boost::interprocess::shared_memory_object* gblShm;

static boost::interprocess::shared_memory_object sharedMem(
	boost::interprocess::open_or_create,
	"axe_link_shm",
	boost::interprocess::read_write
);

void foo(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();

	std::cout << "Test" << std::endl;

	args.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, "foobar!!!!").ToLocalChecked());
}

/// returns false on failure, true on success
void connectLink(const v8::FunctionCallbackInfo<v8::Value>& args) {
	using namespace boost::interprocess;

	v8::Isolate* isolate = args.GetIsolate();

	if (linkConnected) {
		args.GetReturnValue().Set(v8::Boolean::New(isolate, false));
		return;
	}

	linkConnected = true;

	// shared_memory_object shm(open_only, "axe_link_shm", read_write);

	// gblShm = &shm;

	// shm.truncate(114584);

	// mapped_region region(shm, read_write);

	// std::memset(region.get_address(), 1, region.get_size());

	// char* mem = static_cast<char*>(region.get_address());

	args.GetReturnValue().Set(v8::Boolean::New(isolate, true));
}

void sendFrame(const v8::FunctionCallbackInfo<v8::Value>& args) {
	using namespace boost::interprocess;
	v8::Isolate* isolate = args.GetIsolate();

	v8::Local<v8::Uint8Array> nodeBuffer = args[0].As<v8::Uint8Array>();

	// v8::Local<v8::ArrayBuffer> bufferObj = args[0].As<v8::ArrayBuffer>();
	std::cout << "Got frame of size " << (offset_t)nodeBuffer->ByteLength() << std::endl;
	nodeBuffer->Buffer()->Data();

	// std::cout << bufferObj->IsArrayBuffer() << std::endl;

	sharedMem.truncate(nodeBuffer->ByteLength());

	mapped_region region(sharedMem, read_write);

	std::memcpy(region.get_address(), nodeBuffer->Buffer()->Data(), nodeBuffer->ByteLength());
}

// void CreateSHMLink(const v8::FunctionCallbackInfo<v8::Value>& args) {
// 	Shadow::AXEUI::SHMLink::NewInstance(args);
// }

void Init(v8::Local<v8::Object> exports) {
	NODE_SET_METHOD(exports, "connectLink", connectLink);
	NODE_SET_METHOD(exports, "sendFrame", sendFrame);
	NODE_SET_METHOD(exports, "foo", foo);

	// Shadow::AXEUI::SHMLink::Init(exports->GetIsolate());
	// NODE_SET_METHOD(exports, "SHMLink", CreateSHMLink);
}

NODE_MODULE(NODE_GYP_MODULE_NAME, Init);