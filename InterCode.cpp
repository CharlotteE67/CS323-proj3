
#include "InterCode.hpp"

#include <utility>

InterCode::InterCode() = default;

InterCode::InterCode(int type, string x, string y, string z): interCodeType(type), x(std::move(x)), y(std::move(y)), z(std::move(z)){
}
