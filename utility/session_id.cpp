
#include "session_id.h"
namespace utility 
{

SessionId::SessionId(const SessionId& other) {
	id_ = other.id_;
}

SessionId SessionId::operator=(const SessionId& other) {
	if (this == &other) {
		return *this;
	}

	id_ = other.id_;

	return *this;
}

bool operator==(const SessionId& rhs, const SessionId& lhs) {
	return (rhs.Id() == lhs.Id());
}

} 
