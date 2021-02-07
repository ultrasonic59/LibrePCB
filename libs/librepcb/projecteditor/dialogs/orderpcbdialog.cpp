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
#include "orderpcbdialog.h"

#include "librepcb/project/boards/board.h"
#include "ui_orderpcbdialog.h"

#include <librepcb/common/application.h>
#include <librepcb/common/fileio/transactionaldirectory.h>
#include <librepcb/common/fileio/transactionalfilesystem.h>
#include <librepcb/common/network/orderpcbapirequest.h>
#include <librepcb/project/project.h>
#include <librepcb/workspace/settings/workspacesettings.h>
#include <librepcb/workspace/workspace.h>

#include <QtCore>
#include <QtWidgets>

/*******************************************************************************
 *  Namespace
 ******************************************************************************/
namespace librepcb {
namespace project {
namespace editor {

/*******************************************************************************
 *  Constructors / Destructor
 ******************************************************************************/

OrderPcbDialog::OrderPcbDialog(workspace::Workspace& workspace,
                               Project& project, const Board* board,
                               QWidget* parent) noexcept
  : QDialog(parent),
    mProject(project),
    mBoard(board),
    mServerUrl(),
    mUi(new Ui::OrderPcbDialog) {
  mUi->setupUi(this);
  mUi->progressBar->hide();

  // Use the first API server URL configured in the workspace settings.
  const QList<QUrl>& urls = workspace.getSettings().repositoryUrls.get();
  QString text = "<p>" %
      tr("This tool helps you to quickly and easily get your designed PCB "
         "manufactured. The project directory will be uploaded to an API "
         "server (*.lppz export). Afterwards you can continue the order "
         "process in the web browser.") %
      "</p>";
  if (urls.count() > 0) {
    mServerUrl = urls.first();
    QUrl infoUrl(mServerUrl.toString() % "/api");
    QString hyperlink = QString("<a href=\"%1\">%2</a>")
                            .arg(infoUrl.toString(), infoUrl.toDisplayString());
    text += "<p>" %
        tr("For more information about how the uploaded project gets "
           "processed, see %1.")
            .arg(hyperlink) %
        "</p>";
  } else {
    text += "<p><strong><span style=\"color:#ff0000;\">" %
        tr("This feature is not available because there is no API server "
           "configured in your workspace settings!") %
        "</style></strong></p>";
    mUi->cbxOpenBrowser->setEnabled(false);
    mUi->btnUpload->setEnabled(false);
  }
  mUi->lblDocumentation->setText("<html><head/><body>" % text %
                                 "</body></html>");

  connect(mUi->btnUpload, &QPushButton::clicked, this,
          &OrderPcbDialog::uploadButtonClicked);
}

OrderPcbDialog::~OrderPcbDialog() noexcept {
}

/*******************************************************************************
 *  Private Methods
 ******************************************************************************/

void OrderPcbDialog::uploadButtonClicked() noexcept {
  // Lock UI during work.
  mUi->btnUpload->hide();
  mUi->progressBar->setValue(0);
  mUi->progressBar->show();
  mUi->lblStatus->setText(tr("Exporting project..."));

  // To get the UI updated immediately, delay the upload slightly.
#if (QT_VERSION >= QT_VERSION_CHECK(5, 4, 0))
  QTimer::singleShot(5, this, &OrderPcbDialog::startUpload);
#else
  QTimer::singleShot(5, this, SLOT(startUpload()));
#endif
}

void OrderPcbDialog::startUpload() noexcept {
  try {
    // Generate *.lppz.
    qDebug() << "Export project to *.lppz for ordering PCBs...";
    if (qApp->isFileFormatStable()) {
      // Note that usually we save the project to the transactional file system
      // (but not to the disk!) before exporting the *.lppz since the user
      // probably expects that the current state of the project gets exported.
      // However, if the file format is unstable (i.e. on development builds),
      // this would lead in a *.lppz of an unstable file format, which cannot
      // be ordered (the server will not support an unstable file format).
      // Therefore we don't save the project on development builds.
      mProject.save();  // can throw
    }
    QByteArray lppz =
        mProject.getDirectory().getFileSystem()->exportToZip();  // can throw
    mUi->progressBar->setValue(10);

    // Start uploading project.
    qDebug() << "Upload *.lppz to API server...";
    mUi->lblStatus->setText(tr("Uploading project..."));
    mRequest.reset(new OrderPcbApiRequest(mServerUrl));
    connect(mRequest.data(), &OrderPcbApiRequest::progressState, mUi->lblStatus,
            &QLabel::setText);
    connect(mRequest.data(), &OrderPcbApiRequest::progressPercent, this,
            &OrderPcbDialog::uploadProgressPercent);
    connect(mRequest.data(), &OrderPcbApiRequest::succeeded, this,
            &OrderPcbDialog::uploadSucceeded);
    connect(mRequest.data(), &OrderPcbApiRequest::failed, this,
            &OrderPcbDialog::uploadFailed);
    mRequest->start(lppz, mBoard ? mBoard->getRelativePath() : QString());
  } catch (const Exception& e) {
    uploadFailed(e.getMsg());
  }
}

void OrderPcbDialog::uploadProgressPercent(int percent) noexcept {
  mUi->progressBar->setValue(10 + ((percent * 8) / 10));
}

void OrderPcbDialog::uploadSucceeded(const QUrl& redirectUrl) noexcept {
  qDebug() << "Successfully uploaded *.lppz to API server:"
           << redirectUrl.toDisplayString();

  mUi->progressBar->hide();
  mUi->btnUpload->show();
  mUi->lblStatus->setText(tr("Success! Open %1 to continue.")
                              .arg(QString("<a href=\"%1\">%2</a>")
                                       .arg(redirectUrl.toString(),
                                            redirectUrl.toDisplayString())));
  if (mUi->cbxOpenBrowser->isChecked() &&
      QDesktopServices::openUrl(redirectUrl)) {
    // The web browser might need a few seconds to open. Let's keep the dialog
    // open during this time - if the dialog closes immediately but no browser
    // is visible yet, it looks like the feature does not work.
#if (QT_VERSION >= QT_VERSION_CHECK(5, 4, 0))
    QTimer::singleShot(5000, this, &OrderPcbDialog::accept);
#else
    QTimer::singleShot(5000, this, SLOT(accept()));
#endif
  }
}

void OrderPcbDialog::uploadFailed(const QString& errorMsg) noexcept {
  qDebug() << "Failed to upload *.lppz to API server:" << errorMsg;

  mUi->progressBar->hide();
  mUi->btnUpload->show();
  mUi->lblStatus->setText(tr("Error: %1").arg(errorMsg));
}

/*******************************************************************************
 *  End of File
 ******************************************************************************/

}  // namespace editor
}  // namespace project
}  // namespace librepcb
