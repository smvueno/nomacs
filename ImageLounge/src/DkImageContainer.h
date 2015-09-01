/*******************************************************************************************************
 DkImageContainer.h
 Created on:	21.02.2014
 
 nomacs is a fast and small image viewer with the capability of synchronizing multiple instances
 
 Copyright (C) 2011-2014 Markus Diem <markus@nomacs.org>
 Copyright (C) 2011-2014 Stefan Fiel <stefan@nomacs.org>
 Copyright (C) 2011-2014 Florian Kleber <florian@nomacs.org>

 This file is part of nomacs.

 nomacs is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 nomacs is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.

 *******************************************************************************************************/

#pragma once

#pragma warning(push, 0)	// no warnings from includes - begin
#include <QFutureWatcher>
#include <QTimer>
#include <QSharedPointer>
#pragma warning(pop)		// no warnings from includes - end

#pragma warning(disable: 4251)	// TODO: remove

#ifndef DllExport
#ifdef DK_DLL_EXPORT
#define DllExport Q_DECL_EXPORT
#elif DK_DLL_IMPORT
#define DllExport Q_DECL_IMPORT
#else
#define DllExport
#endif
#endif

#include "DkThumbs.h"

namespace nmc {

// nomacs defines
class DkBasicLoader;
class DkMetaDataT;
class DkZipContainer;
class FileDownloader;

class DllExport DkImageContainer {

public:
	enum {
		loading_canceled = -3,
		loading = -2,
		exists_not = -1,
		not_loaded,
		loaded,
	};

	DkImageContainer(const QString& filePath);
	virtual ~DkImageContainer();
	bool operator==(const DkImageContainer& ric) const;
	bool operator< (const DkImageContainer& o) const;
	bool operator<= (const DkImageContainer& o) const;
	bool operator> (const DkImageContainer& o) const;
	bool operator>= (const DkImageContainer& o) const;

	QImage image();

	bool hasImage() const;
	int getLoadState() const;
	QFileInfo fileInfo() const;
	QString filePath() const;
	QString dirPath() const;
	QString fileName() const;
	bool isFromZip();
	bool isEdited() const;
	bool isSelected() const;
	void setEdited(bool edited);
	QString getTitleAttribute() const;
	float getMemoryUsage() const;
	float getFileSize() const;
	virtual QSharedPointer<DkBasicLoader> getLoader();
	virtual QSharedPointer<DkMetaDataT> getMetaData();
	virtual QSharedPointer<DkThumbNailT> getThumb();
	virtual QSharedPointer<QByteArray> getFileBuffer();
#ifdef WITH_QUAZIP
	QSharedPointer<DkZipContainer> getZipData();
#endif
#ifdef WIN32
	std::wstring getFileNameWStr() const;
#endif

	bool exists();
	bool setPageIdx(int skipIdx);

	QSharedPointer<QByteArray> loadFileToBuffer(const QString& filePath);
	bool loadImage();
	void setImage(const QImage& img);
	void setImage(const QImage& img, const QString& filePath);
	bool saveImage(const QString& filePath, const QImage saveImg, int compression = -1);
	bool saveImage(const QString& filePath, int compression = -1);
	void saveMetaData();
	virtual void clear();

protected:
	QSharedPointer<DkBasicLoader> loadImageIntern(const QString& filePath, QSharedPointer<DkBasicLoader> loader, const QSharedPointer<QByteArray> fileBuffer);
	void saveMetaDataIntern(const QString& filePath, QSharedPointer<DkBasicLoader> loader, QSharedPointer<QByteArray> fileBuffer = QSharedPointer<QByteArray>());
	QString saveImageIntern(const QString& filePath, QSharedPointer<DkBasicLoader> loader, QImage saveImg, int compression);
	void setFilePath(const QString& filePath);
	void init();

	QSharedPointer<QByteArray> mFileBuffer;
	QSharedPointer<DkBasicLoader> mLoader;
	QSharedPointer<DkThumbNailT> mThumb;

	int mLoadState	= not_loaded;
	bool mEdited	= false;
	bool mSelected	= false;

	QFileInfo mFileInfo;

#ifdef WITH_QUAZIP	
	QSharedPointer<DkZipContainer> mZipData;
#endif
#ifdef WIN32
	std::wstring mFileNameStr;	// speeds up sorting of filenames on windows
#endif

private:
	QString mFilePath;

};

bool imageContainerLessThan(const DkImageContainer& l, const DkImageContainer& r);
bool imageContainerLessThanPtr(const QSharedPointer<DkImageContainer> l, const QSharedPointer<DkImageContainer> r);

class DllExport DkImageContainerT : public QObject, public DkImageContainer {
	Q_OBJECT

public:
	DkImageContainerT(const QString& filePath);
	virtual ~DkImageContainerT();

	void fetchFile();
	void cancel();
	void clear();
	void receiveUpdates(QObject* obj, bool connectSignals = true);
	void downloadFile(const QUrl& url);

	bool loadImageThreaded(bool force = false);
	bool saveImageThreaded(const QString& filePath, const QImage saveImg, int compression = -1);
	bool saveImageThreaded(const QString& filePath, int compression = -1);
	void saveMetaDataThreaded();
	bool isFileDownloaded() const;

	virtual QSharedPointer<DkBasicLoader> getLoader();
	virtual QSharedPointer<DkThumbNailT> getThumb();

signals:
	void fileLoadedSignal(bool loaded = true) const;
	void fileSavedSignal(const QString& fileInfo, bool saved = true) const;
	void showInfoSignal(const QString& msg, int time = 3000, int position = 0) const;
	void errorDialogSignal(const QString& msg) const;
	void thumbLoadedSignal(bool loaded = true) const;

public slots:
	void checkForFileUpdates(); 

protected slots:
	void bufferLoaded();
	void imageLoaded();
	void savingFinished();
	void loadingFinished();
	void fileDownloaded();

protected:
	void fetchImage();
	
	QSharedPointer<QByteArray> loadFileToBuffer(const QString& filePath);
	QSharedPointer<DkBasicLoader> loadImageIntern(const QString& filePath, QSharedPointer<DkBasicLoader> loader, const QSharedPointer<QByteArray> fileBuffer);
	QString saveImageIntern(const QString& filePath, QSharedPointer<DkBasicLoader> loader, QImage saveImg, int compression);
	void saveMetaDataIntern(const QString& filePath, QSharedPointer<DkBasicLoader> loader, QSharedPointer<QByteArray> fileBuffer);
	
	QFutureWatcher<QSharedPointer<QByteArray> > mBufferWatcher;
	QFutureWatcher<QSharedPointer<DkBasicLoader> > mImageWatcher;
	QFutureWatcher<QString> mSaveImageWatcher;
	QFutureWatcher<bool> mSaveMetaDataWatcher;

	QSharedPointer<FileDownloader> mFileDownloader;

	bool mFetchingImage = false;
	bool mFetchingBuffer = false;
	bool mWaitForUpdate = false;
	bool mDownloaded = false;

	QTimer mFileUpdateTimer;
};

};
