// This is an open source non-commercial project. Dear PVS-Studio, please check
// it. PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "AutomationModel.hpp"

#include <State/Address.hpp>

#include <Process/Dataflow/MinMaxFloatPort.hpp>
#include <Process/Dataflow/Port.hpp>
#include <Process/Dataflow/PortSerialization.hpp>

#include <Curve/CurveModel.hpp>

#include <score/serialization/DataStreamVisitor.hpp>
#include <score/serialization/JSONValueVisitor.hpp>
#include <score/serialization/JSONVisitor.hpp>
#include <score/serialization/VisitorCommon.hpp>

#include <ossia/network/dataspace/dataspace_visitors.hpp>

template <>
void DataStreamReader::read(const Automation::ProcessModel& autom)
{
  m_stream << *autom.outlet;
  readFrom(autom.curve());

  m_stream << autom.tween();

  insertDelimiter();
}

template <>
void DataStreamWriter::write(Automation::ProcessModel& autom)
{
  autom.outlet = Process::load_value_outlet(*this, &autom);

  autom.setCurve(new Curve::Model{*this, &autom});

  bool tw;

  m_stream >> tw;

  autom.setTween(tw);

  checkDelimiter();
}

template <>
void JSONReader::read(const Automation::ProcessModel& autom)
{
  obj["Outlet"] = *autom.outlet;
  obj["Curve"] = autom.curve();
  obj["Tween"] = autom.tween();
}

template <>
void JSONWriter::write(Automation::ProcessModel& autom)
{
  if(auto outl = obj.tryGet("Outlet"))
  {
    JSONWriter writer{*outl};
    autom.outlet = Process::load_value_outlet(writer, &autom);
  }
  else
  {
    autom.outlet
        = std::make_unique<Process::MinMaxFloatOutlet>(Id<Process::Port>(0), &autom);
  }

  JSONObject::Deserializer curve_deser{obj["Curve"]};
  autom.setCurve(new Curve::Model{curve_deser, &autom});

  if(auto tw = obj.tryGet("Tween"))
    autom.setTween(tw->toBool());
}
