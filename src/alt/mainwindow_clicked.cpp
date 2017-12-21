/*
 *    Copyright (C) 2017 AltEdit Authors
 *
 *    This file is part of AltEdit project
 *
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "mainwindow.h"
#include "basicedit.h"
#include "bufmgr.h"
#include "highlighter.h"
#include "find.h"
#include "replace.h"
#include "grep.h"
#include "jump.h"
#include "global.h"

void MainWindow::clicked_filebar(bool right) {
	FLOG();
	(void) right;

	// get full path for the file
	QString fname = filebar_->getLineText();
	fdebug("clicked filebar %s\n", fname.toUtf8().data());
	openFile(fname);
	functionList();
}

// line is 1 based
void MainWindow::clicked_info(bool right) {
	FLOG();
	(void) right;

	active().text_->setFocus();

	assert(infotext_);
	QString txt = infotext_->getLineText();
	fdebug("clicked_info: %s\n", txt.toUtf8().data());

	// parse text line
	char *str = txt.toUtf8().data();
	if (*str == ' ' || *str == '\t')
		return;
	char *ptr = strchr(str, '\n');
	if (ptr)
		*ptr = '\0';
	ptr = strchr(str, '\t');
	if (ptr)
		*ptr = '\0';

	// the line number
	char *nstr = strchr(str, ':');
	if (!nstr)
		return;
	*nstr = '\0';
	if (*(++nstr) == '\0')
		return;

	// after ptr
	//  - make output
	//         :11	void MainWindow::clicked_filebar(int line, int col) {
	// - search (F5)
	//         :99		int line = atoi(nstr);
	// grep
	//         /home/netblue/work/alt/src/alt/mainwindow_clicked.cpp:83
	ptr = nstr;
	while (*ptr != '\0') {
		if (!isdigit(*ptr)) {
			*ptr = '\0';
			break;
		}
		ptr++;
	}

	// check file and line number
	int line = atoi(nstr);
	if (line < 0)
		return;

	if (right) {
		QString out = open_tagged_file(str);
		infotext_->setPlainText(out);
		infotext_->setLine(line);
	}
	else {
		QString file = QString(str);
		if (!file.isEmpty()) {
			// try to access the file
			if (access(file.toUtf8().data(), R_OK))
				return;
		}
		else {
			// the first line could be the file name
			file = infotext_->getFirstLineText();
			file.chop(1); // remove '\n"

			if (access(file.toUtf8().data(), R_OK))
				file = QString("");
		}

		fdebug("clicked_info: processing #%s# %d\n", file.toUtf8().data(), line);
		openFile(file);
		active().text_->setLine(line);
	}
}

void MainWindow::clicked0(bool right) {
	FLOG();
	(void) right;
	bool dostatus = (active_ != 0);
	active_ = 0;
	if (dostatus) {
		setTitle();
		updateBufmgrActions();
	}
	if (!active().bufmgr_->active()->file_.isEmpty())
		filebar_->setDirForFile(active().bufmgr_->active()->file_);
	QTimer::singleShot(50, this, SLOT(statusDelay()));
}

void MainWindow::clicked1(bool right) {
	FLOG();
	(void) right;
	bool dostatus = (active_ != 1);
	active_ = 1;
	if (dostatus) {
		setTitle();
		updateBufmgrActions();
	}
	if (!active().bufmgr_->active()->file_.isEmpty())
		filebar_->setDirForFile(active().bufmgr_->active()->file_);
	QTimer::singleShot(50, this, SLOT(statusDelay()));
}

void MainWindow::clicked2(bool right) {
	FLOG();
	(void) right;
	bool dostatus = (active_ != 2);
	active_ = 2;
	if (dostatus) {
		setTitle();
		updateBufmgrActions();
	}
	if (!active().bufmgr_->active()->file_.isEmpty())
		filebar_->setDirForFile(active().bufmgr_->active()->file_);
	QTimer::singleShot(50, this, SLOT(statusDelay()));
}

void MainWindow::clicked3(bool right) {
	FLOG();
	(void) right;

	bool dostatus = (active_ != 3);
	active_ = 3;
	if (dostatus) {
		setTitle();
		updateBufmgrActions();
	}
	if (!active().bufmgr_->active()->file_.isEmpty())
		filebar_->setDirForFile(active().bufmgr_->active()->file_);
	QTimer::singleShot(50, this, SLOT(statusDelay()));
}


void MainWindow::clicked_bufmgr(QAction *action) {
	FLOG();
	fdebug("clicked_bufmgr: %s\n", action->text().toUtf8().data());
	saveBufmgr();

	// goto next buffer
	if (action == nextbufferAct) {
		active().bufmgr_->next();
		goto restore;
	}

	// switch buffer
	if (active().bufmgr_->set(action->text()))
		goto restore;

	if (!active().bufmgr_->active()->file_.isEmpty())
		filebar_->setDirForFile(active().bufmgr_->active()->file_);

	// do nothing
	return;

restore:
	// restore editor window
	restoreBufmgr();

	if (!active().bufmgr_->active()->file_.isEmpty())
		filebar_->setDirForFile(active().bufmgr_->active()->file_);

	// set window title
	setTitle();
}

void MainWindow::textModified0() {
	FLOG();
	BufMgrData *data = panel[0].bufmgr_->active();
	if (!data->modified_) {
		data->modified_ = true;
		updateBufmgrActions();
		setTitle();
	}
	QTimer::singleShot(50, this, SLOT(statusDelay()));
}

void MainWindow::textModified1() {
	FLOG();
	BufMgrData *data = panel[1].bufmgr_->active();
	if (!data->modified_) {
		data->modified_ = true;
		updateBufmgrActions();
		setTitle();
	}
	QTimer::singleShot(50, this, SLOT(statusDelay()));
}

void MainWindow::textModified2() {
	FLOG();
	BufMgrData *data = panel[2].bufmgr_->active();
	if (!data->modified_) {
		data->modified_ = true;
		updateBufmgrActions();
		setTitle();
	}
	QTimer::singleShot(50, this, SLOT(statusDelay()));
}

void MainWindow::textModified3() {
	FLOG();
	BufMgrData *data = panel[3].bufmgr_->active();
	if (!data->modified_) {
		data->modified_ = true;
		updateBufmgrActions();
		setTitle();
	}
	QTimer::singleShot(50, this, SLOT(statusDelay()));
}

void MainWindow::cursorMoved() {
	FLOG();
	QTimer::singleShot(50, this, SLOT(statusDelay()));
}

void MainWindow::statusDelay() {
	int line = active().text_->getLineNumber();
	int col = active().text_->getColumnNumber();
	status_position_->setText(QString("%1:%2").arg(line).arg(col));

	setStatusModified(panel[0].bufmgr_->active()->modified_);
}

void MainWindow::selectionChanged() {
	FLOG();
	int start = active().text_->selectionStart();
	int end = active().text_->selectionEnd();
	int delta = end - start;
	if (delta)
		status_selection_->setText(QString("%1").arg(end - start));
	else
		status_selection_->setText(QString(""));
}

void MainWindow::recording(bool status) {
	FLOG();

	if (status)
		status_recording_->setText("Recording");
	else
		status_recording_->setText("");
}


void MainWindow::accumulating(bool status) {
	FLOG();

	if (status)
		status_accumulating_->setText("Accumulating");
	else
		status_accumulating_->setText("");
}

void MainWindow::setStatusModified(bool status) {
	FLOG();

	if (status)
		status_modified_->setText("Modified");
	else
		status_modified_->setText("");
}