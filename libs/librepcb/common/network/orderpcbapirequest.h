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

#ifndef LIBREPCB_ORDERPCBAPIREQUEST_H
#define LIBREPCB_ORDERPCBAPIREQUEST_H

/*******************************************************************************
 *  Includes
 ******************************************************************************/
#include <QtCore>

/*******************************************************************************
 *  Namespace / Forward Declarations
 ******************************************************************************/
namespace librepcb {

/*******************************************************************************
 *  Class OrderPcbApiRequest
 ******************************************************************************/

/**
 * @brief Order a PCB via a LibrePCB API server
 */
class OrderPcbApiRequest final : public QObject {
  Q_OBJECT

public:
  // Constructors / Destructor
  OrderPcbApiRequest() = delete;
  OrderPcbApiRequest(const OrderPcbApiRequest& other) = delete;
  explicit OrderPcbApiRequest(const QUrl& apiServerUrl,
                              QObject* parent = nullptr) noexcept;
  ~OrderPcbApiRequest() noexcept;

  // General Methods
  void start(const QByteArray& lppz, const QString& boardPath) const noexcept;

  // Operators
  OrderPcbApiRequest& operator=(const OrderPcbApiRequest& rhs) = delete;

signals:
  void progressState(QString state);
  void progressPercent(int percent);
  void succeeded(QUrl redirectUrl);
  void failed(QString errorMsg);

private:  // Methods
  void responseReceived(const QByteArray& data) noexcept;

private:  // Data
  QUrl mApiServerUrl;
};

/*******************************************************************************
 *  End of File
 ******************************************************************************/

}  // namespace librepcb

#endif
