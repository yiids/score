#pragma once
#include <QtGlobal>
#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
#include <QByteArray>
#include <QHashFunctions>
#include <QString>

#include <functional>

#include <ossia-config.hpp>

// TODO merge String.hpp here
namespace std
{
template <>
struct hash<QString>
{
  std::size_t operator()(const QString& path) const { return qHash(path); }
};
template <>
struct hash<QByteArray>
{
  std::size_t operator()(const QByteArray& id) const { return qHash(id); }
};
}
#endif
