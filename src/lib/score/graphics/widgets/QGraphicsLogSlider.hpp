#pragma once
#include <score/graphics/widgets/Constants.hpp>
#include <score/graphics/widgets/QGraphicsSliderBase.hpp>

#include <QGraphicsItem>
#include <QObject>

#include <score_lib_base_export.h>

#include <verdigris>

namespace score
{
class SCORE_LIB_BASE_EXPORT QGraphicsLogSlider final
    : public QObject
    , public QGraphicsSliderBase<QGraphicsLogSlider>
{
  W_OBJECT(QGraphicsLogSlider)
  Q_INTERFACES(QGraphicsItem)
  friend struct DefaultGraphicsSliderImpl;
  friend struct QGraphicsSliderBase<QGraphicsLogSlider>;

  double m_value{};
  double m_execValue{};

public:
  double min{}, max{};

private:
  bool m_grab{};
  bool m_hasExec{};

public:
  QGraphicsLogSlider(QGraphicsItem* parent);

  void setRange(double min, double max);
  void setValue(double v);
  double value() const;
  void setExecutionValue(double v);
  void resetExecution();

  double map(double v) const noexcept;
  double unmap(double v) const noexcept;

  bool moving = false;

public:
  void sliderMoved() E_SIGNAL(SCORE_LIB_BASE_EXPORT, sliderMoved)
  void sliderReleased() E_SIGNAL(SCORE_LIB_BASE_EXPORT, sliderReleased)

private:
  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
  void contextMenuEvent(QGraphicsSceneContextMenuEvent* event) override;
  void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
      override;
};
}
