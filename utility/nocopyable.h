#ifndef UTILITY_NOCOPYABLE_H
#define UTILITY_NOCOPYABLE_H

namespace utility {
    class nocopyable
    {
    protected:
        nocopyable(){};
        virtual ~nocopyable(){};
        nocopyable(const nocopyable&);
        nocopyable& operator = (const nocopyable&);
	};
}
#endif

