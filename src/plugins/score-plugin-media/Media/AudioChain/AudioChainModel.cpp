#include <Media/AudioChain/AudioChainModel.hpp>
#include <Process/Dataflow/Port.hpp>
#include <Process/Process.hpp>
#include <Process/ProcessList.hpp>

#include <score/plugins/SerializableHelpers.hpp>
#include <score/model/EntitySerialization.hpp>
#include <Effect/EffectFactory.hpp>
#include <wobjectimpl.h>

W_OBJECT_IMPL(Media::AudioChain::ProcessModel)

namespace Media::AudioChain
{
ProcessModel::ProcessModel(
    const TimeVal& duration,
    const Id<Process::ProcessModel>& id,
    QObject* parent)
    : ChainProcess{duration, id, Metadata<ObjectKey_k, ProcessModel>::get(), parent}
    , inlet{Process::make_audio_inlet(Id<Process::Port>(0), this)}
    , outlet{Process::make_audio_outlet(Id<Process::Port>(0), this)}
{
  metadata().setInstanceName(*this);

  outlet->setPropagate(true);
  init();
}

ProcessModel::~ProcessModel()
{

}

}


template <>
void DataStreamReader::read(const Media::AudioChain::ProcessModel& proc)
{
  m_stream << *proc.inlet << *proc.outlet;

  int32_t n = (int)proc.effects().size();
  m_stream << n;
  for (auto& eff : proc.effects())
  {
    readFrom(eff);
  }
  insertDelimiter();
}

template <>
void DataStreamWriter::write(Media::AudioChain::ProcessModel& proc)
{
  proc.inlet = Process::load_audio_inlet(*this, &proc);
  proc.outlet = Process::load_audio_outlet(*this, &proc);

  int32_t n = 0;
  m_stream >> n;

  auto& fxs = components.interfaces<Process::ProcessFactoryList>();
  for (int i = 0; i < n; i++)
  {
    auto fx = deserialize_interface(fxs, *this, &proc);
    if (fx)
      proc.insertEffect(fx, i);
    else
      SCORE_TODO;
  }

  checkDelimiter();
}

template <>
void JSONObjectReader::read(const Media::AudioChain::ProcessModel& proc)
{
  obj["Inlet"] = toJsonObject(*proc.inlet);
  obj["Outlet"] = toJsonObject(*proc.outlet);
  obj["Effects"] = toJsonArray(proc.effects());
}

template <>
void JSONObjectWriter::write(Media::AudioChain::ProcessModel& proc)
{
  {
    JSONObjectWriter writer{obj["Inlet"].toObject()};
    proc.inlet = Process::load_audio_inlet(writer, &proc);
  }
  {
    JSONObjectWriter writer{obj["Outlet"].toObject()};
    proc.outlet = Process::load_audio_outlet(writer, &proc);
  }

  const QJsonArray fx_array = obj["Effects"].toArray();
  auto& fxs = components.interfaces<Process::ProcessFactoryList>();

  for (const auto& json_vref : fx_array)
  {
    JSONObject::Deserializer deserializer{json_vref.toObject()};
    auto fx = deserialize_interface(fxs, deserializer, &proc);
    if (fx)
    {
      proc.m_effects.add(fx);

      proc.effectsChanged();
    }
    else
      SCORE_TODO;
  }
}
