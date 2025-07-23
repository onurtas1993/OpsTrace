#include "MainView.h" 
#include <QCloseEvent>

MainView::MainView(QJsonObject data, QWidget* parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	processProxy.setSourceModel(&processModel);
	processProxy.setFilterKeyColumn(0);
	ui.processTableView->setModel(&processProxy);
	ui.processTableView->setColumnWidth(0, 180);
	ui.processTableView->setColumnWidth(1, 96);
	connect(ui.searchLineEdit, SIGNAL(textChanged(QString)), &processProxy, SLOT(setFilterFixedString(QString)));

	windowProxy.setSourceModel(&windowModel);
	windowProxy.setFilterKeyColumn(0);
	ui.activeWindowTableView->setModel(&windowProxy);
	ui.activeWindowTableView->setColumnWidth(0, 260);
	ui.activeWindowTableView->setColumnWidth(1, 96);
	connect(ui.activeSearchLineEdit, SIGNAL(textChanged(QString)), &windowProxy, SLOT(setFilterFixedString(QString)));

	connect(
		ui.activeWindowTableView->selectionModel(),
		SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
		this,
		SLOT(WindowsTableSelectionChanged(const QItemSelection&, const QItemSelection&))
	);
	connect(qApp, SIGNAL(QCoreApplication::aboutToQuit()), this, SLOT(MainView::closeEvent(QCloseEvent*)));

	ui.firstNameLabel->setText(data["name"].toString() + " (" + data["id"].toString() + ")");
	ui.titleLabel->setText(data["title"].toString());
}

void MainView::UpdateAllProcessesTable(QList<Process> processes)
{
	processModel.updateTableDataSetFAST(processes);
	ui.processTableView->viewport()->update();
}

void MainView::UpdateActiveWindowsTable(QList<Process> processes)
{
	windowModel.updateTableDataSetFAST(processes);
	ui.activeWindowTableView->viewport()->update();
}

void MainView::setActiveWindowLabel(QString title)
{
	ui.ActiveWindowTitleLabel->setText(title);
}

void MainView::setActiveWindowTimeSpent(QString time)
{
	ui.activeTimeLabel->setText(time);
}

void MainView::setTotalTimeSpent(QString time)
{
	ui.totalTimeLabel->setText(time);
}

void MainView::setKeyboardIdleTime(QString time)
{
	ui.keyboardIdleValueLabel->setText(time);
}

void MainView::setMouseIdleTime(QString time)
{
	ui.mouseIdleLabel->setText(time);
}

void MainView::WindowsTableSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected)
{
	QModelIndexList indexes = selected.indexes();
	if (indexes.count() == 0) return;
	auto name = windowProxy.data(indexes[0]).toString().toStdString();

	Process p(name);
	emit NotifyWindowBlockStatusChanged(p);
}

void MainView::closeEvent(QCloseEvent* event)
{

	QMessageBox::StandardButton resBtn = QMessageBox::question(this, "Ending session?",
		tr("Are you sure?\n"),
		QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
		QMessageBox::Yes);
	if (resBtn != QMessageBox::Yes) {
		event->ignore();
	}
	else {
		//in order to end the session with the server
		emit NotifyApplicationClosing();
		event->accept();
	}
}
