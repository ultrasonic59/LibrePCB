/*
 * LibrePCB - Professional EDA for everyone!
 * Copyright (C) 2013 LibrePCB Developers, see AUTHORS.md for contributors.
 * https://librepcb.org/
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef LIBREPCB_PROJECT_EDITOR_ORDERPCBDIALOG_H
#define LIBREPCB_PROJECT_EDITOR_ORDERPCBDIALOG_H

/*******************************************************************************
 *  Includes
 ******************************************************************************/
#include <QtCore>
#include <QtWidgets>

/*******************************************************************************
 *  Namespace / Forward Declarations
 ******************************************************************************/
namespace librepcb {

namespace workspace {
class Workspace;
}

class OrderPcbApiRequest;

namespace project {

class Project;
class Board;

namespace editor {

namespace Ui {
class OrderPcbDialog;
}

/*******************************************************************************
 *  Class OrderPcbDialog
 ******************************************************************************/

/**
 * @brief The OrderPcbDialog class
 */
class OrderPcbDialog final : public QDialog {
  Q_OBJECT

public:
  // Constructors / Destructor
  OrderPcbDialog() = delete;
  OrderPcbDialog(const OrderPcbDialog& other) = delete;
  explicit OrderPcbDialog(workspace::Workspace& workspace, Project& project,
                          const Board* board = nullptr,
                          QWidget* parent = nullptr) noexcept;
  ~OrderPcbDialog() noexcept;

  // Operator Overloads
  OrderPcbDialog& operator=(const OrderPcbDialog& rhs) = delete;

private:  // Methods
  void uploadButtonClicked() noexcept;
  void startUpload() noexcept;
  void uploadProgressPercent(int percent) noexcept;
  void uploadSucceeded(const QUrl& redirectUrl) noexcept;
  void uploadFailed(const QString& errorMsg) noexcept;

private:  // Data
  Project& mProject;
  const Board* mBoard;
  QUrl mServerUrl;
  QScopedPointer<Ui::OrderPcbDialog> mUi;
  QScopedPointer<OrderPcbApiRequest> mRequest;
};

/*******************************************************************************
 *  End of File
 ******************************************************************************/

}  // namespace editor
}  // namespace project
}  // namespace librepcb

#endif  // LIBREPCB_PROJECT_EDITOR_ORDERPCBDIALOG_H
