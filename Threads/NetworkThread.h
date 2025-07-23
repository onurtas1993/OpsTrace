#pragma once

#include "../Models/Process.h"
#include <QObject>
#include <QMutex>

#include <QtNetwork>

class NetworkThread : public QObject {
	Q_OBJECT
private:
	QJsonObject json;
	QJsonObject userData;
	QMutex mutex;
	bool stopped;
	bool isStopped();
public:
	NetworkThread();
	~NetworkThread();
	void startSession(QJsonObject token);
	void feedUpdatedData(QJsonObject json, QJsonObject userData);
public slots:
	void endSession(QJsonObject _userData);
	void startLoggingInProcess(QString username, QString password);
	void process();
	void stop();
signals:
	void SessionCreated(QJsonObject info);
	void LoginFailed(QString msg);
	void SessionTerminated();
	void exited(int ospid, int code);
	void RequestSent();
};
