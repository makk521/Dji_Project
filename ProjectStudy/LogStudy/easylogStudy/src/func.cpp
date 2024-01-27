#include "func.hpp"

void funLog(){
    LOG(INFO) << "This is an info log in fun";
    LOG(DEBUG) << "This is a debug log in fun";
    LOG(WARNING) << "This is a warning log in fun";
    LOG(ERROR) << "This is an error log in fun";
}
