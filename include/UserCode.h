#ifndef USER_CODE_H
#define USER_CODE_H

#include <string>

namespace Shadow {

class UserCode {
public:
	UserCode();
	~UserCode();

	UserCode(const UserCode&) = delete;
	UserCode& operator=(const UserCode&) = delete;

	void reload();
	void unload();

	void* handle;
};

}

#endif // USER_CODE_H
