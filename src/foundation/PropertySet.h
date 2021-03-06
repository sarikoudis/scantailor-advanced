// Copyright (C) 2019  Joseph Artsimovich <joseph.artsimovich@gmail.com>, 4lex4 <4lex49@zoho.com>
// Use of this source code is governed by the GNU GPLv3 license that can be found in the LICENSE file.

#ifndef SCANTAILOR_FOUNDATION_PROPERTYSET_H_
#define SCANTAILOR_FOUNDATION_PROPERTYSET_H_

#include <memory>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>

#include "Property.h"

class PropertyFactory;
class QDomDocument;
class QDomElement;
class QString;

class PropertySet {
 public:
  PropertySet() = default;

  /**
   * \brief Makes a deep copy of another property set.
   */
  PropertySet(const PropertySet& other);

  PropertySet(const QDomElement& el, const PropertyFactory& factory);

  virtual ~PropertySet() = default;

  /**
   * \brief Makes a deep copy of another property set.
   */
  PropertySet& operator=(const PropertySet& other);

  void swap(PropertySet& other);

  QDomElement toXml(QDomDocument& doc, const QString& name) const;

  template <typename T>
  using is_property = typename std::enable_if<std::is_base_of<Property, T>::value>::type;

  /**
   * Returns a property stored in this set, if one having a suitable
   * type is found, or returns a null smart pointer otherwise.
   */
  template <typename T, typename = is_property<T>>
  std::shared_ptr<T> locate();

  template <typename T, typename = is_property<T>>
  std::shared_ptr<const T> locate() const;

  /**
   * Returns a property stored in this set, if one having a suitable
   * type is found, or returns a default constructed object otherwise.
   */
  template <typename T, typename = is_property<T>>
  std::shared_ptr<T> locateOrDefault();

  template <typename T, typename = is_property<T>>
  std::shared_ptr<const T> locateOrDefault() const;

  /**
   * Returns a property stored in this set, if one having a suitable
   * type is found.  Otherwise, a default constructed object is put
   * to the set and then returned.
   */
  template <typename T, typename = is_property<T>>
  std::shared_ptr<T> locateOrCreate();

 private:
  using PropertyMap = std::unordered_map<std::type_index, std::shared_ptr<Property>>;

  PropertyMap m_props;
};

template <typename T, typename>
std::shared_ptr<T> PropertySet::locate() {
  auto it(m_props.find(typeid(T)));
  if (it != m_props.end()) {
    return std::static_pointer_cast<T>(it->second);
  } else {
    return nullptr;
  }
}

template <typename T, typename>
std::shared_ptr<const T> PropertySet::locate() const {
  return const_cast<PropertySet*>(this)->locate<T>();
}

template <typename T, typename>
std::shared_ptr<T> PropertySet::locateOrDefault() {
  std::shared_ptr<T> prop = locate<T>();
  if (!prop) {
    return std::make_shared<T>();
  }
  return prop;
}

template <typename T, typename>
std::shared_ptr<const T> PropertySet::locateOrDefault() const {
  return const_cast<PropertySet*>(this)->locateOrDefault<T>();
}

template <typename T, typename>
std::shared_ptr<T> PropertySet::locateOrCreate() {
  std::shared_ptr<T> prop = locate<T>();
  if (!prop) {
    prop = std::make_shared<T>();
    m_props[typeid(T)] = prop;
  }
  return prop;
}

inline void PropertySet::swap(PropertySet& other) {
  m_props.swap(other.m_props);
}

inline void swap(PropertySet& o1, PropertySet& o2) {
  o1.swap(o2);
}

#endif  // ifndef SCANTAILOR_FOUNDATION_PROPERTYSET_H_
