#include "NetworkThread.h"
#include "../Utils/Utils.h"


NetworkThread::NetworkThread() {
	stopped = false;
}

NetworkThread::~NetworkThread() {
}

void NetworkThread::feedUpdatedData(QJsonObject _json, QJsonObject userData)
{
	mutex.lock();
	this->userData = userData;
	this->json = _json;
	mutex.unlock();
}

void NetworkThread::stop()
{
	mutex.lock();
	stopped = true;
	mutex.unlock();
}

void NetworkThread::startLoggingInProcess(QString username, QString password)
{
	//bypass login for testing purposes
	QJsonObject sessionJson;
	sessionJson["id"] = "12345"; //example ID
	sessionJson["token"] = "abcdefg1234567890"; //example token

	emit SessionCreated(sessionJson);
	return;

	QNetworkRequest request(QUrl("http://91.92.136.227/remoteassistant/login"));
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
	QNetworkAccessManager manager;

	QUrlQuery postData;
	postData.addQueryItem("username", username);
	postData.addQueryItem("password", password);

	QNetworkReply* reply = manager.post(request, postData.toString().toUtf8());
	while (!reply->isFinished())
		qApp->processEvents();

	QByteArray bytes = reply->readAll();
	//QString str = QString::fromUtf8(bytes.data(), bytes.size());
	int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

	reply->deleteLater();

	switch (statusCode)
	{
	case 0:
		emit LoginFailed("Request timed out.");
		break;
	case 404:
		emit LoginFailed("Server is currently in maintenance mode.");
		break;
	case 401:
		emit LoginFailed("Username & password does not match.");
		break;
	case 200:
	{
		auto jsonisedData = Utils::getInstance().bytearrayToJson(bytes);
		//auto tokne = jsonisedData["token"].toString().toStdString();
		startSession(jsonisedData);
		break;
	}
	}

}

void NetworkThread::startSession(QJsonObject json)
{
	//QString urls("http://91.92.136.227/remoteassistant/user/" + QString(json["id"].toString()) + "/session/start");
	QNetworkRequest request(QUrl("http://91.92.136.227/remoteassistant/user/" + QString(json["id"].toString()) + "/session/start"));
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
	QString authText = "Bearer " + QString(json["token"].toString());
	request.setRawHeader("Authorization", authText.toUtf8());
	QNetworkAccessManager manager;

	QUrlQuery postData;
	//postData.addQueryItem("token", json["token"].toString());

	QNetworkReply* reply = manager.post(request, postData.toString().toUtf8());
	while (!reply->isFinished())
		qApp->processEvents();

	QByteArray bytes = reply->readAll();
	//QString session_id = QString::fromUtf8(bytes.data(), bytes.size());
	int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
	if (statusCode == 200)
	{
		emit SessionCreated(json);
	}
	else
		emit LoginFailed("Session could not be started.");
	reply->deleteLater();
}

void NetworkThread::endSession(QJsonObject _userData)
{
	//QString urls("91.92.136.227/remoteassistant/user/" + QString(json["id"].toString()) + "/session/start");
	QNetworkRequest request(QUrl("http://91.92.136.227/remoteassistant/user/" + QString(_userData["id"].toString()) + "/session/end"));
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
	QString authText = "Bearer " + QString(_userData["token"].toString());
	request.setRawHeader("Authorization", authText.toUtf8());
	QNetworkAccessManager manager;

	QUrlQuery postData;
	//postData.addQueryItem("token", json["token"].toString());

	QNetworkReply* reply = manager.post(request, postData.toString().toUtf8());
	while (!reply->isFinished())
		qApp->processEvents();

	QByteArray bytes = reply->readAll();
	//QString session_id = QString::fromUtf8(bytes.data(), bytes.size());
	int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
	if (statusCode == 200)
	{
		emit SessionCreated(json);
	}
	else
		emit LoginFailed("Session could not be started.");
	reply->deleteLater();
}

void NetworkThread::process()
{
	while (!isStopped())
	{
		if (!json.empty())
		{
			QString id = userData["id"].toString();

			QNetworkRequest request(QUrl("http://91.92.136.227/remoteassistant/user/" + QString(userData["id"].toString()) + "/session/load"));
			//request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
			request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
			QString authText = "Bearer " + QString(userData["token"].toString());
			request.setRawHeader("Authorization", authText.toUtf8());

			QNetworkAccessManager manager;
			auto byteArray = QJsonDocument(json).toJson();
			QNetworkReply* reply = manager.post(request, byteArray);

			while (!reply->isFinished())
				qApp->processEvents();

			QByteArray bytes = reply->readAll();
			QString ret_value = QString::fromUtf8(bytes.data(), bytes.size());
			int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
			if (statusCode == 200)
			{

			}
			QJsonDocument return_json = QJsonDocument::fromJson(bytes);
			reply->deleteLater();
		}
		std::this_thread::sleep_for(std::chrono::seconds(5));
		emit RequestSent();
	}

}

bool NetworkThread::isStopped()
{
	bool stopped;
	mutex.lock();
	stopped = this->stopped;
	mutex.unlock();
	return stopped;
}
