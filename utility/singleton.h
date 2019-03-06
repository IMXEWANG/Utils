#ifndef UTILITY_singleton_H
#define UTILITY_singleton_H
#include <stdio.h>
#include "nocopyable.h"

namespace utility {
    template <typename T>
    class singleton : public nocopyable
    {
    public:
        static T* create_instance()
        {
            if (m_instance == NULL) {
                m_instance = new T();
            }
            return m_instance;
        }

        static T * get_instance()
        {
            if (m_instance == NULL) {
                printf("m_instance is NULL\n");
            }
            return m_instance;
        }

        static void destory_instance()
        {
            if (m_instance != NULL) {
                delete m_instance;
                m_instance = NULL;
            }
        }

    private:
        static T *m_instance;
    };

    template<typename T>
    T* singleton<T>::m_instance = NULL;
}
#endif

