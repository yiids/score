// This is an open source non-commercial project. Dear PVS-Studio, please check
// it. PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "OSCQueryDevice.hpp"

#include <Device/Protocol/DeviceSettings.hpp>
#include <Explorer/DeviceList.hpp>
#include <Explorer/DeviceLogging.hpp>
#include <Protocols/OSCQuery/OSCQuerySpecificSettings.hpp>

#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/generic/generic_parameter.hpp>
#include <ossia/network/oscquery/oscquery_mirror.hpp>
#include <ossia/network/rate_limiting_protocol.hpp>

#include <QString>
#include <QTimer>
#include <QVariant>

#include <wobjectimpl.h>

#include <memory>
W_OBJECT_IMPL(Protocols::OSCQueryDevice)

namespace Protocols
{
OSCQueryDevice::OSCQueryDevice(const Device::DeviceSettings& settings)
    : OwningDeviceInterface{settings}
{
  m_capas.canRefreshTree = true;
  m_capas.canRenameNode = false;
  m_capas.canSetProperties = false;

  connect(
      this, &OSCQueryDevice::sig_command, this, &OSCQueryDevice::slot_command,
      Qt::QueuedConnection);
  connect(
        this, &OSCQueryDevice::sig_disconnect, this, [=] {
    // TODO how to notify the GUI ?
    m_connected = false;
  },
      Qt::QueuedConnection);
}

bool OSCQueryDevice::connected() const
{
  return m_dev && m_connected;
}

void OSCQueryDevice::disconnect()
{
  if (m_mirror)
  {
    m_mirror->set_disconnect_callback([=] {});
    m_mirror->set_fail_callback([=] {});
    m_mirror  = nullptr;
  }

  OwningDeviceInterface::disconnect();
}

bool OSCQueryDevice::reconnect()
{
  const auto& cur_settings = settings();
  const auto& stgs = cur_settings.deviceSpecificSettings.value<OSCQuerySpecificSettings>();

  if(m_dev && m_mirror && m_oldSettings == cur_settings)
  {
    // TODO update settings
    try {
      m_mirror->reconnect();
      m_connected = true;
    }
    catch (std::exception& e)
    {
      qDebug() << "Could not connect: " << e.what();
      m_connected = false;
    }
    catch (...)
    {
      // TODO save the reason of the non-connection.
      m_connected = false;
    }
    m_mirror->reconnect();
    m_connected = true;
    return connected();
  }

  disconnect();

  try
  {
    std::unique_ptr<ossia::net::protocol_base> ossia_settings
        = std::make_unique<ossia::oscquery::oscquery_mirror_protocol>(
            stgs.host.toStdString());

    auto& p = static_cast<ossia::oscquery::oscquery_mirror_protocol&>(*ossia_settings);
    m_mirror = &p;

    if(stgs.rate)
    {
      ossia_settings = std::make_unique<ossia::net::rate_limiting_protocol>(
            std::chrono::milliseconds{*stgs.rate},
            std::move(ossia_settings));
    }
    // run the commands in the Qt event loop
    // FIXME they should be disabled upon manual disconnection

    m_dev = std::make_unique<ossia::net::generic_device>(
        std::move(ossia_settings), settings().name.toStdString());

    deviceChanged(nullptr, m_dev.get());

    p.set_command_callback([=] { sig_command(); });
    p.set_disconnect_callback([=] { sig_disconnect(); });
    p.set_fail_callback([=] { sig_disconnect(); });

    setLogging_impl(Device::get_cur_logging(isLogging()));

    enableCallbacks();
    m_connected = true;
  }
  catch (std::exception& e)
  {
    qDebug() << "Could not connect: " << e.what();
    m_connected = false;
  }
  catch (...)
  {
    // TODO save the reason of the non-connection.
    m_connected = false;
  }

  return connected();
}

void OSCQueryDevice::recreate(const Device::Node& n)
{
  for (auto& child : n)
  {
    addNode(child);
  }
}

void OSCQueryDevice::slot_command()
{
  if (m_mirror)
  {
    m_mirror->run_commands();
  }
}
}
