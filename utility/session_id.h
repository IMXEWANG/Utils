//////////////////////////////////////////////////////////////////////////////////
/// session_id.h
/// @version: 1.0.0
/// @author: Eric
/// @date: 2016/10/21
//////////////////////////////////////////////////////////////////////////////////
#ifndef FUANDA_DSCORE_SESSION_ID_H_
#define FUANDA_DSCORE_SESSION_ID_H_

#include "Global.h"

namespace utility 
{

// Session identify, map table constructor session id collection
class COMMON_API SessionId
{
public:
	// Default constructor
	SessionId() 
	{
	}

	// Copy constructor
	SessionId(const SessionId& other);

	explicit SessionId(int id) : id_(id) 
	{
	}

	// Assginment operator
	SessionId operator=(const SessionId& other);

	// Session id
	int Id() const { return id_; }
	void Id(int value) { id_ = value; }

	// Opeator < for map search
	bool operator < (const SessionId& other) const {
		if (id_ < other.Id()) {
			return true;
		}

		return false;
	}

private:
	// Session object's socket handle value
	int id_;
};

COMMON_API bool operator==(const SessionId& rhs, const SessionId& lhs);

}
#endif
