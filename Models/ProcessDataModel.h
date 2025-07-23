#pragma once 

#include "../Models/Process.h"
#include "../Utils/Utils.h"
#include <QList>
#include <QtWidgets>

class ProcessDataModel : public QAbstractTableModel
{
	Q_OBJECT
private:

	QList<Process> m_data;
	QPixmap getCorrectPicture(const Process& process) const
	{
		QPixmap pixmap;
		if (process.isBlocked())
			pixmap.load(":/MainView/Resources/ic_unchecked.png");
		else
			pixmap.load(":/MainView/Resources/ic_checked.png");
		int w = 18;
		int h = 18;
		pixmap = pixmap.scaled(w, h, Qt::KeepAspectRatioByExpanding);
		return pixmap;
	}
public:
	ProcessDataModel(QObject* parent = {}) : QAbstractTableModel{ parent } {}
	int rowCount(const QModelIndex&) const override { return m_data.count(); }
	int columnCount(const QModelIndex&) const override { return 3; }
	QVariant data(const QModelIndex& index, int role) const override {
		if (role == Qt::DecorationRole)
		{
			if (index.column() == 2)
			{
				const auto& process = m_data[index.row()];
				return getCorrectPicture(process);
			}
		}
		if (role != Qt::DisplayRole && role != Qt::EditRole) return {};
		const auto& process = m_data[index.row()];
		switch (index.column()) {
		case 0: return QString::fromStdString(process.name());
		case 1:
		{
			int hour = process.uptime() / 3600;
			int minute = (process.uptime() % 3600) / 60;
			int second = process.uptime() % 60;

			return QString::fromStdString(Utils::getInstance().timeFormatter(hour, minute, second));
		}
		default: return {};
		};
	}
	QVariant headerData(int section, Qt::Orientation orientation, int role) const override {
		if (orientation != Qt::Horizontal || role != Qt::DisplayRole) return {};
		switch (section) {
		case 0: return "Process Name";
		case 1: return "Uptime (in secs)";
		case 2: return "Data Share Status";
		default: return {};
		}
	}
	void append(const Process& process)
	{
		beginInsertRows({}, m_data.count(), m_data.count());
		m_data.append(process);
		endInsertRows();
	}

	void clear()
	{
		if (m_data.count() == 0) return;
		beginRemoveRows({}, 0, m_data.count() - 1);
		endRemoveRows();
		m_data.clear();
	}

	void updateTableDataSet(QList<Process> processes)
	{
		clear();
		for (auto& process : processes)
			append(process);
	}

	void updateTableDataSetFAST(QList<Process> processes)
	{
		if (m_data.size() != processes.size())
			updateTableDataSet(processes);
		else
		{
			int size = processes.size();
			for (int i = 0; i < size; i++)
			{
				m_data[i].setUptime(processes[i].uptime());
				m_data[i].setBlockStatus(processes[i].isBlocked());
			}
		}

	}
};
