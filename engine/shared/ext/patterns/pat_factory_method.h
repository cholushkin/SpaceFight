#ifndef pat_factory_method_h__
#define pat_factory_method_h__

#include <map>

NAMESPACE_BEGIN(pat)

template< class AbstractProduct, typename IdentifierType, typename ProductCreator = AbstractProduct* (*)() >
class FactoryMethod
{
public:
  bool RegisterType(const IdentifierType& id, ProductCreator creator)
  {
    return m_associations.insert(
      typename IdToProductCreatorMap::value_type(id, creator)).second != 0;
  }

  bool UnregisterType(const IdentifierType& id)
  {
    return m_associations.erase(id) != 0;
  }

  bool UnregisterAllTypes()
  {
    if(GetRegisteredCount() == 0)
      return false;
    m_associations.clear();
    return true;
  }

  AbstractProduct* CreateObjectDefault(const IdentifierType& id) const 
  {
    typename IdToProductCreatorMap::const_iterator i = m_associations.find(id);
    if (i != m_associations.end())
    {
      return (i->second)();
    }
    EALWAYS_ASSERT("creator not found");
    return NULL; // error handler may be here
  }

  ProductCreator GetCreator(const IdentifierType& id) const 
  {
    typename IdToProductCreatorMap::const_iterator i = m_associations.find(id);
    if (i != m_associations.end())
    {
      return (i->second);
    }
    return NULL;
  }

  u32 GetRegisteredCount() const
  {
    return m_associations.size();
  }

private:
  typedef std::map<IdentifierType, ProductCreator> IdToProductCreatorMap;
  IdToProductCreatorMap m_associations;
};

NAMESPACE_END(pat)
#endif // pat_factory_method_h__
