// Copyright 2019 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include "DolphinQt/TAS/TASCheckBox.h"

#include <QMouseEvent>
#include <QStylePainter>
#include <QStyleOptionButton>

#include "Core/Config/MainSettings.h"
#include "Core/Movie.h"
#include "Core/System.h"
#include "DolphinQt/QtUtils/QueueOnObject.h"
#include "DolphinQt/TAS/TASInputWindow.h"

TASCheckBox::TASCheckBox(const QString& text, TASInputWindow* parent)
    : QCheckBox(text, parent), m_parent(parent)
{
  setTristate(true);

#if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)
  connect(this, &TASCheckBox::checkStateChanged, this, &TASCheckBox::OnUIValueChanged);
#else
  connect(this, &TASCheckBox::stateChanged, this, &TASCheckBox::OnUIValueChanged);
#endif
}

bool TASCheckBox::GetValue() const
{
  Qt::CheckState check_state = static_cast<Qt::CheckState>(m_state.GetValue());

  if (check_state == Qt::PartiallyChecked)
  {
    const bool active = IsTurboActive();
    if (Config::Get(Config::MAIN_MOVIE_TURBO_VISUALIZER))
      QueueVisualTurboUpdate(active, !m_visual_turbo_enabled_seen.exchange(true));
    else
      m_visual_turbo_enabled_seen.store(false, std::memory_order_relaxed);
    return active;
  }

  return check_state != Qt::Unchecked;
}

void TASCheckBox::OnControllerValueChanged(bool new_value)
{
  if (m_state.OnControllerValueChanged(new_value ? Qt::Checked : Qt::Unchecked))
    QueueOnObject(this, &TASCheckBox::ApplyControllerValueChange);
}

void TASCheckBox::mousePressEvent(QMouseEvent* event)
{
  if (event->button() != Qt::RightButton)
  {
    setChecked(!isChecked());
    return;
  }

  if (checkState() == Qt::PartiallyChecked)
  {
    m_last_sampled_turbo_active.store(false, std::memory_order_relaxed);
    m_visual_turbo_enabled_seen.store(false, std::memory_order_relaxed);
    m_visual_turbo_active = false;
    setCheckState(Qt::Unchecked);
    return;
  }

  m_frame_turbo_started = Core::System::GetInstance().GetMovie().GetCurrentFrame();
  m_turbo_press_frames = m_parent->GetTurboPressFrames();
  m_turbo_total_frames = m_turbo_press_frames + m_parent->GetTurboReleaseFrames();
  m_last_sampled_turbo_active.store(true, std::memory_order_relaxed);
  m_visual_turbo_enabled_seen.store(Config::Get(Config::MAIN_MOVIE_TURBO_VISUALIZER),
                                    std::memory_order_relaxed);
  m_visual_turbo_active = true;
  setCheckState(Qt::PartiallyChecked);
  update();
}

void TASCheckBox::paintEvent(QPaintEvent* event)
{
  if (checkState() != Qt::PartiallyChecked ||
      !Config::Get(Config::MAIN_MOVIE_TURBO_VISUALIZER))
  {
    QCheckBox::paintEvent(event);
    return;
  }

  QStyleOptionButton option;
  initStyleOption(&option);
  option.state &= ~QStyle::State_NoChange;
  if (m_visual_turbo_active)
    option.state |= QStyle::State_On;
  else
    option.state |= QStyle::State_Off;

  QStylePainter painter(this);
  painter.drawControl(QStyle::CE_CheckBox, option);
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)
void TASCheckBox::OnUIValueChanged(Qt::CheckState new_value)
#else
void TASCheckBox::OnUIValueChanged(int new_value)
#endif
{
  m_state.OnUIValueChanged(new_value);
}

void TASCheckBox::ApplyControllerValueChange()
{
  const QSignalBlocker blocker(this);
  setCheckState(static_cast<Qt::CheckState>(m_state.ApplyControllerValueChange()));
}

bool TASCheckBox::IsTurboActive() const
{
  if (m_turbo_total_frames <= 0)
    return false;

  const u64 frames_elapsed =
      Core::System::GetInstance().GetMovie().GetCurrentFrame() - m_frame_turbo_started;
  return static_cast<int>(frames_elapsed % m_turbo_total_frames) < m_turbo_press_frames;
}

void TASCheckBox::QueueVisualTurboUpdate(bool active, bool force) const
{
  if (!force && m_last_sampled_turbo_active.exchange(active, std::memory_order_relaxed) == active)
    return;

  m_last_sampled_turbo_active.store(active, std::memory_order_relaxed);
  TASCheckBox* widget = const_cast<TASCheckBox*>(this);
  QueueOnObject(widget, [widget, active] {
    widget->m_visual_turbo_active = active;
    widget->repaint();
  });
}
