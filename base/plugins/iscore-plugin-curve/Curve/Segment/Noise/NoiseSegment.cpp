#include <QPoint>
#include <iscore/serialization/VisitorCommon.hpp>
#include <vector>

#include "NoiseSegment.hpp"
#include <Curve/Palette/CurvePoint.hpp>

#include <iscore/model/Identifier.hpp>

#include <ossia/editor/curve/curve_segment/linear.hpp>
#include <random>

namespace Curve
{
template<typename T>
struct Noise
{
    auto operator()(double val, T start, T end) {
      return
          (end - start) * (double(std::rand()) / RAND_MAX);
    }
};

NoiseSegment::NoiseSegment(
    const NoiseSegment& other,
    const IdentifiedObject::id_type& id,
    QObject* parent)
    : SegmentModel{other.start(), other.end(), id, parent}
{
}

void NoiseSegment::on_startChanged()
{
  emit dataChanged();
}

void NoiseSegment::on_endChanged()
{
  emit dataChanged();
}

const constexpr std::size_t rand_N = 35;
auto array_gen()
{
  std::array<double, rand_N> arr{};
  int32_t seed = 1; // chosen by fair d20 dice roll.
  for(std::size_t i = 0; i < rand_N; i++)
  {
    // Modified from Intel fastrand :
    // https://software.intel.com/en-us/articles/fast-random-number-generator-on-the-intel-pentiumr-4-processor
    seed = (214013 * seed + 2532011);
    arr[i] = ((seed >> 16) & 0x7FFF) / 32768.;
  }
  return arr;
}
void NoiseSegment::updateData(int numInterp) const
{
  static const auto noise{array_gen()};

  if (!m_valid)
  {
    m_data.resize(rand_N);

    double start_x = start().x();
    double end_x = end().x();
    for (std::size_t j = 0; j <= (rand_N - 1); j++)
    {
      QPointF& pt = m_data[j];
      pt.setX(start_x + (double(j) / (rand_N - 1)) * (end_x - start_x));
      pt.setY(double(noise[j]));
    }
  }
}

double NoiseSegment::valueAt(double x) const
{
  return start().y()
         + (end().y() - start().y()) * (x - start().x())
               / (end().x() - start().x());
}

QVariant NoiseSegment::toSegmentSpecificData() const
{
  return QVariant::fromValue(data_type{});
}

std::function<float(double, float, float)>
NoiseSegment::makeFloatFunction() const
{
  return Noise<float>{};
}

std::function<int(double, int, int)> NoiseSegment::makeIntFunction() const
{
  return Noise<int>{};
}

std::function<bool(double, bool, bool)> NoiseSegment::makeBoolFunction() const
{
  return Noise<bool>{};
}
}
