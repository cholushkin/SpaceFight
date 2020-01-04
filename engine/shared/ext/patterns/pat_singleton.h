#ifndef singleton_h__
#define singleton_h__
#include "core/common/com_types.h"

/*
  ----- StaticSingleton 
  Example:
  DECLARE_STATIC_SINGLETON( UserClassSingleton, IUserClass, UserClass ); // declares type UserClassSingleton by interface and class name 
  DECLARE_STATIC_SINGLETON( MyGameSingleton, Game, Game ); // declares type MyGameSingleton by Game class
*/


template< class T_INTERFACE, class T_USER_CLASS >
class StaticSingleton
{		
public:
	static T_INTERFACE* instance()
	{
		if( !mInstance )
		{	
			mInstance = new T_USER_CLASS();
		}
		return mInstance;
	}

	static void setInstance(T_INTERFACE* inst)
	{
		mInstance = inst;
	}


	static void cleanup()
	{
		if (mInstance)
		{
			delete mInstance;
			mInstance = 0;
		}
	}

	static bool isInstantiated()
	{
		return mInstance != 0;
	}

private:
	static T_INTERFACE*	mInstance;

	// deny copy
	StaticSingleton( const StaticSingleton& );
	StaticSingleton& operator= ( const StaticSingleton& );
};



template< class T_INTERFACE, class T_USER_CLASS >
T_INTERFACE* StaticSingleton<T_INTERFACE, T_USER_CLASS>::mInstance = 0;

#define DECLARE_STATIC_SINGLETON( SingletonNameToDeclare, UserClassInterface, UserClass ) typedef StaticSingleton<UserClassInterface, UserClass> SingletonNameToDeclare;







/*
  ----- PolymorphSingleton
  For example:
  DECLARE_CREATOR_INTERFACE( ICreatorOfBox, IBox); 
  DECLARE_CONCRETE_CREATOR( CreatorBigBox, ICreatorOfBox, BigBox);
  DECLARE_CONCRETE_CREATOR( CreatorSmallBox, ICreatorOfBox, SmallBox);
  DECLARE_POLYMORPH_SINGLETON( BoxSingleton, ICreatorOfBox )
  
  After these declarations you may set creator in run time:
  CreatorBigBox bbc;    // global
  CreatorSmallBox sbc;  // global
  BoxSingleton::instance(); // error: you should set creator first
  BoxSingleton::setCreator( &bbc );
  IBox* bigBox = BoxSingleton::instance();
  BoxSingleton::setCreator( &sbc );
  IBox* smallBox = BoxSingleton::instance();
*/

template<class T_INTERFACE_OF_USER_CLASS>
class ICreator
{
public:
	virtual T_INTERFACE_OF_USER_CLASS* create() = 0;
	virtual ~ICreator() {}
};



template< class T_INTERFACE_OF_USER_CLASS, class T_CREATOR_INTERFACE >
class PolymorphSingleton
{	
public:

	static T_INTERFACE_OF_USER_CLASS* instance()
	{
		if( !m_pInstance )
		{			
			if ( mConcreteCreator == NULL )
			{
				//EALWAYS_ASSERT( "you have to set creator first" );
				return NULL;
			}

			m_pInstance = mConcreteCreator->create();
		}

		return m_pInstance;
	}


	static void cleanup()
	{
		if (m_pInstance)
		{
			delete m_pInstance;
			m_pInstance = 0;
		}
	}


	static bool isInstantiated()
	{
		return m_pInstance != NULL;
	}


	static void setCreator( T_CREATOR_INTERFACE* creInt)
	{
		cleanup();
		mConcreteCreator = creInt;
	}

private:
	static T_CREATOR_INTERFACE*					mConcreteCreator;
	static T_INTERFACE_OF_USER_CLASS*			m_pInstance;
};

template< class T_INTERFACE_OF_USER_CLASS, class T_CREATOR_INTERFACE >
T_CREATOR_INTERFACE* PolymorphSingleton<T_INTERFACE_OF_USER_CLASS, T_CREATOR_INTERFACE>::mConcreteCreator = NULL;


template< class T_INTERFACE_OF_USER_CLASS, class T_CREATOR_INTERFACE >
T_INTERFACE_OF_USER_CLASS* PolymorphSingleton<T_INTERFACE_OF_USER_CLASS, T_CREATOR_INTERFACE>::m_pInstance = NULL;

// ===== declarators for polymorph singleton
#define DECLARE_CREATOR_INTERFACE( CreatorInterfaceName, UserClassInterfaceName )\
typedef ICreator<UserClassInterfaceName> CreatorInterfaceName


#define DECLARE_CONCRETE_CREATOR( ConcreteCreatorName, CreatorInterfaceName, ConcreteUserClassName, UserClassInterfaceName)\
class ConcreteCreatorName : public CreatorInterfaceName\
{\
public:\
	virtual UserClassInterfaceName* create(){return new ConcreteUserClassName();}\
}

#define DECLARE_POLYMORPH_SINGLETON( SingletonName, UserClassInterfaceName, CreatorInterfaceName )\
typedef PolymorphSingleton<UserClassInterfaceName, CreatorInterfaceName> SingletonName


#endif // singleton_h__
