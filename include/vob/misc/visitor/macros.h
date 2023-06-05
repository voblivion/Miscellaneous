#pragma once

#include "name_value_pair.h"

#include <type_traits>


#define VOB_MISVI_ACCEPT(Type) \
template <typename TVisitor, typename TValue> \
requires std::is_same_v<std::remove_cvref_t<TValue>, Type> \
bool accept(TVisitor& a_visitor, TValue& a_value)


#define VOB_MISVI_NVP(name, attribute) \
a_visitor.visit(vob::misvi::nvp(name, a_value.m_##attribute))
