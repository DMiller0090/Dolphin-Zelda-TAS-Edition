// Copyright 2026 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <array>

#include <QDialog>

#include "Core/HW/WiimoteEmu/DesiredWiimoteState.h"
#include "Core/Movie.h"

class QCheckBox;
class QGroupBox;
class QLabel;
class QObject;
class QPushButton;
class QEvent;
class QPoint;
class QRubberBand;
class QShowEvent;
class QSpinBox;
class QStackedWidget;
class QTableView;
class QTimer;
class QHideEvent;
class DTMEditorModel;

class DTMEditorDialog final : public QDialog
{
  Q_OBJECT

public:
  explicit DTMEditorDialog(QWidget* parent = nullptr);

  bool HasMovieLoaded() const;
  void PromptOpen();

private:
  enum class EditorMovieKind
  {
    None,
    GC,
    Wii,
  };

  void CreateWidgets();
  void Refresh();
  bool RefreshGCRuntimeMovie();
  bool RefreshWiiRuntimeMovie();
  void CopySelectedInputs();
  bool PasteCopiedInputs(int start_row);
  void PopulateEditor();
  void PopulateGCEditor(int row, int controller);
  void PopulateWiiEditor(int row);
  void ApplyEditorChanges();
  void SetEditorEnabled(bool enabled);
  bool LoadFile(const QString& path);
  bool SaveFile(const QString& path);
  void UpdateStatusLabel();
  void UpdateHotkeyFocusState();
  std::vector<int> GetSelectedRows() const;
  bool MoveSelectedInputs(const std::vector<int>& selected_rows, int dest_start);
  void SelectRows(const std::vector<int>& rows, int current_row);
  void RefreshEditedRows(const std::vector<int>& rows);
  bool HasCopiedInputs() const;
  bool HasCompatibleCopiedInputs() const;
  void ShowTableContextMenu(const QPoint& pos);
  void ClearPendingDrag();
  void BeginPendingDrag();
  void FinishPendingDrag(bool apply_drag);
  void UpdateDragPreview();
  int GetPrimaryDataColumn() const;
  void ApplyGCEditorChange(Movie::ControllerState* state, const QObject* source) const;
  void ApplyWiiEditorChange(Movie::WiiRuntimeInputRow* row, const QObject* source) const;
  Movie::ControllerState BuildGCStateFromEditor() const;
  WiimoteEmu::SerializedWiimoteState BuildWiiSerializedStateFromEditor() const;
  bool eventFilter(QObject* watched, QEvent* event) override;
  void changeEvent(QEvent* event) override;
  void showEvent(QShowEvent* event) override;
  void hideEvent(QHideEvent* event) override;

  DTMEditorModel* m_model = nullptr;
  QTableView* m_table = nullptr;
  QLabel* m_status_label = nullptr;
  QGroupBox* m_editor_box = nullptr;
  QStackedWidget* m_editor_stack = nullptr;
  QPushButton* m_open_button = nullptr;
  QPushButton* m_save_button = nullptr;
  QRubberBand* m_drag_preview = nullptr;
  QTimer* m_refresh_timer = nullptr;
  QTimer* m_drag_hold_timer = nullptr;

  QCheckBox* m_connected = nullptr;
  QCheckBox* m_start = nullptr;
  QCheckBox* m_a = nullptr;
  QCheckBox* m_b = nullptr;
  QCheckBox* m_x = nullptr;
  QCheckBox* m_y = nullptr;
  QCheckBox* m_z = nullptr;
  QCheckBox* m_l = nullptr;
  QCheckBox* m_r = nullptr;
  QCheckBox* m_up = nullptr;
  QCheckBox* m_down = nullptr;
  QCheckBox* m_left = nullptr;
  QCheckBox* m_right = nullptr;
  QCheckBox* m_disc = nullptr;
  QCheckBox* m_reset = nullptr;
  QCheckBox* m_get_origin = nullptr;
  QSpinBox* m_trigger_l = nullptr;
  QSpinBox* m_trigger_r = nullptr;
  QSpinBox* m_stick_x = nullptr;
  QSpinBox* m_stick_y = nullptr;
  QSpinBox* m_cstick_x = nullptr;
  QSpinBox* m_cstick_y = nullptr;

  QLabel* m_wii_extension_label = nullptr;
  QLabel* m_wii_motion_plus_label = nullptr;
  QCheckBox* m_wii_reset = nullptr;
  QCheckBox* m_wii_a = nullptr;
  QCheckBox* m_wii_b = nullptr;
  QCheckBox* m_wii_one = nullptr;
  QCheckBox* m_wii_two = nullptr;
  QCheckBox* m_wii_plus = nullptr;
  QCheckBox* m_wii_minus = nullptr;
  QCheckBox* m_wii_home = nullptr;
  QCheckBox* m_wii_up = nullptr;
  QCheckBox* m_wii_down = nullptr;
  QCheckBox* m_wii_left = nullptr;
  QCheckBox* m_wii_right = nullptr;
  QCheckBox* m_wii_c = nullptr;
  QCheckBox* m_wii_z = nullptr;
  QCheckBox* m_wii_gyro_slow_x = nullptr;
  QCheckBox* m_wii_gyro_slow_y = nullptr;
  QCheckBox* m_wii_gyro_slow_z = nullptr;
  QSpinBox* m_wii_accel_x = nullptr;
  QSpinBox* m_wii_accel_y = nullptr;
  QSpinBox* m_wii_accel_z = nullptr;
  QSpinBox* m_wii_battery = nullptr;
  std::array<QCheckBox*, 4> m_wii_ir_visible{};
  std::array<QSpinBox*, 4> m_wii_ir_x{};
  std::array<QSpinBox*, 4> m_wii_ir_y{};
  std::array<QSpinBox*, 4> m_wii_ir_size{};
  QSpinBox* m_wii_gyro_x = nullptr;
  QSpinBox* m_wii_gyro_y = nullptr;
  QSpinBox* m_wii_gyro_z = nullptr;
  QSpinBox* m_wii_nunchuk_x = nullptr;
  QSpinBox* m_wii_nunchuk_y = nullptr;
  QSpinBox* m_wii_nunchuk_accel_x = nullptr;
  QSpinBox* m_wii_nunchuk_accel_y = nullptr;
  QSpinBox* m_wii_nunchuk_accel_z = nullptr;

  QString m_file_path;
  Movie::DTMHeader m_header{};
  EditorMovieKind m_file_movie_kind = EditorMovieKind::None;
  EditorMovieKind m_runtime_movie_kind = EditorMovieKind::None;
  bool m_using_runtime_movie = false;
  int m_last_runtime_row = -1;
  u64 m_last_runtime_frame = 0;
  u64 m_last_runtime_data_generation = 0;
  bool m_has_last_runtime_frame = false;
  bool m_dirty = false;
  bool m_updating_editor = false;
  bool m_refresh_in_progress = false;
  bool m_drag_pending = false;
  bool m_drag_active = false;
  std::vector<int> m_drag_rows;
  int m_drag_press_row = -1;
  int m_drag_hover_row = -1;
  EditorMovieKind m_copied_movie_kind = EditorMovieKind::None;
  std::vector<std::array<Movie::ControllerState, 4>> m_copied_gc_rows;
  std::vector<Movie::WiiRuntimeInputRow> m_copied_wii_rows;

  Movie::WiiRuntimeInputRow m_current_wii_row{};
  WiimoteEmu::DesiredWiimoteState m_current_wii_state{};
  bool m_current_wii_state_valid = false;
};
