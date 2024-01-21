#ifndef LMMS_I_MEMORY_MANAGER_H
#define LMMS_I_MEMORY_MANAGER_H

#include <cstddef>

namespace lmms
{

class IMemoryManager
{
public:
    virtual ~IMemoryManager() = default;
    static IMemoryManager* Instance();

    virtual void * do_alloc( size_t size ) = 0;
    virtual void do_free( void * ptr ) = 0;
};


#define I_MM_OPERATORS								\
public: 											\
static void * operator new ( size_t size )		\
{													\
	return IMemoryManager::Instance()->do_alloc( size );			\
}													\
static void * operator new[] ( size_t size )		\
{													\
	return IMemoryManager::Instance()->do_alloc( size );			\
}													\
static void operator delete ( void * ptr )		\
{													\
	IMemoryManager::Instance()->do_free( ptr );					\
}													\
static void operator delete[] ( void * ptr )	\
{													\
	IMemoryManager::Instance()->do_free( ptr );					\
}

}

#endif // LMMS_I_MEMORY_MANAGER_H