#pragma once
#include <Engine/Executor/ProcessComponent.hpp>

#include <ossia/editor/mapper/mapper.hpp>
#include <ossia/editor/value/value.hpp>
#include <Mapping/MappingModel.hpp>
#include <QPointer>
#include <State/Address.hpp>
#include <ossia/dataflow/node_process.hpp>
namespace ossia
{
class curve_abstract;
}
namespace Device
{
class DeviceList;
}

namespace Mapping
{
namespace RecreateOnPlay
{
class Component final
    : public ::Engine::Execution::
          ProcessComponent_T<Mapping::ProcessModel, ossia::node_process>
{
  COMPONENT_METADATA("da360b58-9885-4106-be54-8e272ed45dbe")
public:
  Component(
      ::Engine::Execution::IntervalComponent& parentInterval,
      ::Mapping::ProcessModel& element,
      const ::Engine::Execution::Context& ctx,
      const Id<score::Component>& id,
      QObject* parent);

  ~Component();
private:
  void recompute();
  std::shared_ptr<ossia::curve_abstract> rebuildCurve(
      ossia::val_type source,
      ossia::val_type target);

  template <typename T>
  std::shared_ptr<ossia::curve_abstract> on_curveChanged_impl(
      ossia::val_type target);

  template <typename X_T, typename Y_T>
  std::shared_ptr<ossia::curve_abstract> on_curveChanged_impl2();
  ossia::node_ptr m_node;
};

using ComponentFactory
    = ::Engine::Execution::ProcessComponentFactory_T<Component>;
}
}

SCORE_CONCRETE_COMPONENT_FACTORY(
    Engine::Execution::ProcessComponentFactory,
    Mapping::RecreateOnPlay::ComponentFactory)
