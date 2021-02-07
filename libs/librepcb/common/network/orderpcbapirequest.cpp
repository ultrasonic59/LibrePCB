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

/*******************************************************************************
 *  Includes
 ******************************************************************************/
#include "orderpcbapirequest.h"

#include "network/networkrequest.h"

#include <QtCore>

/*******************************************************************************
 *  Namespace
 ******************************************************************************/
namespace librepcb {

/*******************************************************************************
 *  Constructors / Destructor
 ******************************************************************************/

OrderPcbApiRequest::OrderPcbApiRequest(const QUrl& apiServerUrl,
                                       QObject* parent) noexcept
  : QObject(parent), mApiServerUrl(apiServerUrl) {
}

OrderPcbApiRequest::~OrderPcbApiRequest() noexcept {
}

/*******************************************************************************
 *  General Methods
 ******************************************************************************/

void OrderPcbApiRequest::start(const QByteArray& lppz,
                               const QString& boardPath) const noexcept {
  // Build JSON object to be uploaded.
  QJsonObject obj;
  obj.insert("project", QString(lppz.toBase64()));
  obj.insert("board", boardPath.isEmpty() ? nullptr : boardPath);
  QByteArray data = QJsonDocument(obj).toJson();

  // Upload data to API server.
  QUrl url = QUrl(mApiServerUrl.toString() % "/api/v1/order");
  NetworkRequest* request = new NetworkRequest(url, data);
  request->setHeaderField("Content-Type", "application/json");
  request->setHeaderField("Content-Length",
                          QString::number(data.size()).toUtf8());
  request->setHeaderField("Accept", "application/json;charset=UTF-8");
  request->setHeaderField("Accept-Charset", "UTF-8");
  connect(request, &NetworkRequest::progressState, this,
          &OrderPcbApiRequest::progressState);
  connect(request, &NetworkRequest::progressPercent, this,
          &OrderPcbApiRequest::progressPercent, Qt::QueuedConnection);
  connect(request, &NetworkRequest::errored, this, &OrderPcbApiRequest::failed,
          Qt::QueuedConnection);
  connect(request, &NetworkRequest::dataReceived, this,
          &OrderPcbApiRequest::responseReceived, Qt::QueuedConnection);
  connect(this, &OrderPcbApiRequest::destroyed, request, &NetworkRequest::abort,
          Qt::QueuedConnection);
  request->start();
}

/*******************************************************************************
 *  Private Methods
 ******************************************************************************/

void OrderPcbApiRequest::responseReceived(const QByteArray& data) noexcept {
  QJsonDocument doc = QJsonDocument::fromJson(data);
  if (doc.isNull() || doc.isEmpty() || (!doc.isObject())) {
    emit failed("Received JSON object is not valid.");
    return;
  }
  QString message = doc.object().value("message").toString();
  if (!message.isEmpty()) {
    qWarning() << "Server returned message:" << message;
  }
  QString redirectUrlStr = doc.object().value("redirect_url").toString();
  if (redirectUrlStr.isEmpty()) {
    emit failed(tr("The server rejected the request: %1").arg(message));
    return;
  }
  QUrl redirectUrl = QUrl(redirectUrlStr);
  if (!redirectUrl.isValid()) {
    qWarning() << "Received invalid redirect URL from server:"
               << redirectUrlStr;
    emit failed("Received an invalid redirect URL.");
    return;
  }
  qDebug() << "Received redirect URL from server:" << redirectUrlStr;
  emit succeeded(redirectUrl);
}

/*******************************************************************************
 *  End of File
 ******************************************************************************/

}  // namespace librepcb
