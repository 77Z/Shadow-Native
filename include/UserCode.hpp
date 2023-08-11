#ifndef USER_CODE_HPP
#define USER_CODE_HPP

#include <string>

enum UserCodeReturnStatus_ { UserCodeReturnStatus_Success, UserCodeReturnStatus_Failure };

namespace Shadow::UserCode {

int loadUserCode(void);

}

#endif /* USER_CODE_HPP */