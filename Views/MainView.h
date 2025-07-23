#pragma once

#include <QtWidgets/QMainWindow>
#include <QSortFilterProxyModel>
#include <QJsonObject>
#include "../Models/ProcessDataModel.h"
#include "ui_MainView.h"

class MainView : public QMainWindow
{
	Q_OBJECT

public:
	MainView(QJsonObject data, QWidget* parent = Q_NULLPTR);
	void UpdateAllProcessesTable(QList<Process> processes);
	void UpdateActiveWindowsTable(QList<Process> processes);

	void setActiveWindowLabel(QString title);
	void setActiveWindowTimeSpent(QString time);
	void setTotalTimeSpent(QString time);
	void setKeyboardIdleTime(QString time);
	void setMouseIdleTime(QString time);

private slots:
	void WindowsTableSelectionChanged(const QItemSelection&, const QItemSelection&);
	void closeEvent(QCloseEvent* event);

signals:
	void NotifyWindowBlockStatusChanged(Process& p);
	void NotifyApplicationClosing();

private:

	Ui::MainViewClass ui;

	QSortFilterProxyModel processProxy;
	ProcessDataModel processModel;

	QSortFilterProxyModel windowProxy;
	ProcessDataModel windowModel;
};
