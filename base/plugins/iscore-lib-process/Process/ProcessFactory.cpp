#include "ProcessFactory.hpp"
#include "StateProcessFactoryList.hpp"
#include "ProcessList.hpp"
#include <iscore/tools/ModelPathSerialization.hpp>
#include <Process/Dummy/DummyLayerModel.hpp>
#include <Process/Dummy/DummyLayerPresenter.hpp>
#include <Process/Dummy/DummyLayerView.hpp>
#include <Process/LayerModelPanelProxy.hpp>
#include <Process/Process.hpp>

namespace Process
{
ProcessModelFactory::~ProcessModelFactory() = default;
LayerFactory::~LayerFactory() = default;
ProcessFactoryList::~ProcessFactoryList() = default;
LayerFactoryList::~LayerFactoryList() = default;
StateProcessList::~StateProcessList() = default;


LayerModel* LayerFactory::make(
        Process::ProcessModel& proc,
        const Id<LayerModel>& viewModelId,
        const QByteArray& constructionData,
        QObject* parent)
{
    auto lm = makeLayer_impl(proc, viewModelId, constructionData, parent);
    proc.addLayer(lm);

    return lm;
}


LayerModel*LayerFactory::load(
        const VisitorVariant& v,
        QObject* parent)
{
    switch(v.identifier)
    {
        case DataStream::type():
        {
            auto& des = static_cast<DataStream::Deserializer&>(v.visitor);

            ObjectPath process;
            des.m_stream >> process;
            auto& proc = process.find<Process::ProcessModel>();

            // Note : we pass a reference to the stream,
            // so it is already increased past the reading of the path.
            auto lm = loadLayer_impl(proc, v, parent);
            proc.addLayer(lm);

            return lm;
        }
        case JSONObject::type():
        {
            auto& des = static_cast<JSONObject::Deserializer&>(v.visitor);
            auto proc = fromJsonObject<ObjectPath>(des.m_obj["SharedProcess"]);

            if(auto p = proc.try_find<Process::ProcessModel>())
            {
                auto& proc = *p;
                auto lm = loadLayer_impl(proc, v, parent);
                proc.addLayer(lm);

                return lm;
            }
            return nullptr;
        }
        default:
            return nullptr;
    }
}

LayerModel*LayerFactory::cloneLayer(
        Process::ProcessModel& proc,
        const Id<LayerModel>& newId,
        const LayerModel& source,
        QObject* parent)
{
    auto lm = cloneLayer_impl(proc, newId, source, parent);
    proc.addLayer(lm);

    return lm;
}



QByteArray LayerFactory::makeLayerConstructionData(
        const ProcessModel&) const
{
    return { };
}

QByteArray LayerFactory::makeStaticLayerConstructionData() const
{
    return { };
}

LayerPresenter*LayerFactory::makeLayerPresenter(
        const LayerModel& lm,
        LayerView* v,
        const ProcessPresenterContext& context,
        QObject* parent)
{
    return new Dummy::DummyLayerPresenter{lm, static_cast<Dummy::DummyLayerView*>(v), context, parent};
}

LayerView*LayerFactory::makeLayerView(
        const LayerModel& view,
        QGraphicsItem* parent)
{
    return new Dummy::DummyLayerView{parent};
}

LayerModelPanelProxy* LayerFactory::makePanel(const LayerModel& layer, QObject* parent)
{
    return new Process::GraphicsViewLayerModelPanelProxy{layer, parent};
}

bool LayerFactory::matches(const ProcessModel& p) const
{
    return false;
}

LayerModel*LayerFactory::makeLayer_impl(
        ProcessModel& p,
        const Id<LayerModel>& viewModelId,
        const QByteArray& constructionData,
        QObject* parent)
{
    return new Dummy::Layer{p, viewModelId, parent};
}

LayerModel*LayerFactory::loadLayer_impl(
        ProcessModel& p ,
        const VisitorVariant& vis,
        QObject* parent)
{
    return deserialize_dyn(vis, [&] (auto&& deserializer)
    {
        auto autom = new Dummy::Layer{
                     deserializer, p, parent};

        return autom;
    });
}

LayerModel*LayerFactory::cloneLayer_impl(
        ProcessModel& p,
        const Id<LayerModel>& newId,
        const LayerModel& source,
        QObject* parent)
{
    return new Dummy::Layer{
        safe_cast<const Dummy::Layer&>(source),
                p,
                newId,
                parent};
}

}
