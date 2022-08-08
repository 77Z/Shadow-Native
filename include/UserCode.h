#ifndef USER_CODE_H
#define USER_CODE_H

#include <string>

enum UserCodeReturnStatus {
	USER_CODE_SUCCESS,
	USER_CODE_FAILURE
};

namespace Shadow {
	namespace UserCode {
		int loadUserCode(std::string libraryFile);
	}
}

#endif // USER_CODE_H
