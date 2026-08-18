#include "mainwindow.h"
#include <QHeaderView>
#include <QFileDialog>
#include <QDir>
#include <QFile>
#include <QDataStream>
#include <QMessageBox>
#include <QtConcurrent/QtConcurrentRun>
#include <QFutureWatcher>
#include <QUrl>
#include <QMediaContent>
#include <algorithm>

static WaveformData processAudioFileOffThread(const QString& filePath, const QString& fileName, int targetColumns) {
    WaveformData result;
    result.fileName = fileName;

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) return result;

    QByteArray header = file.read(44);
    if (header.size() < 44) return result;

    QDataStream stream(header);
    stream.setByteOrder(QDataStream::LittleEndian);

    char chunkId[4], waveId[4];
    stream.readRawData(chunkId, 4);
    quint32 chunkSize;
    stream >> chunkSize;
    stream.readRawData(waveId, 4);

    if (qstrncmp(chunkId, "RIFF", 4) != 0 || qstrncmp(waveId, "WAVE", 4) != 0) return result;

    char fmtId[4];
    quint32 fmtSize;
    quint16 audioFormat, channels, bitsPerSample, blockAlign;
    quint32 sampleRate, byteRate;

    stream.readRawData(fmtId, 4);
    stream >> fmtSize >> audioFormat >> channels >> sampleRate >> byteRate >> blockAlign >> bitsPerSample;

    if (byteRate == 0 || channels == 0) return result;

    qint64 dataSize = file.size() - 44;
    result.durationMs = (dataSize * 1000) / byteRate;

    QByteArray pcmData = file.readAll();
    file.close();

    int bytesPerSample = bitsPerSample / 8;
    if (bytesPerSample == 0) return result;

    int totalFrameSamples = pcmData.size() / bytesPerSample;
    int totalFrames = totalFrameSamples / channels;

    if (totalFrames <= 0) return result;

    int columns = std::max(1, targetColumns);
    result.minMaxPeaks.resize(columns);

    const char* rawPtr = pcmData.constData();
    int totalPcmBytes = pcmData.size();

    for (int col = 0; col < columns; ++col) {
        // Calculate exact frame range for this column
        int startFrame = static_cast<int>((static_cast<double>(col) / columns) * totalFrames);
        int endFrame = static_cast<int>((static_cast<double>(col + 1) / columns) * totalFrames);
        
        // Ensure at least 1 frame is evaluated per column
        if (endFrame <= startFrame) {
            endFrame = std::min(startFrame + 1, totalFrames);
        }

        float minVal = 1.0f;
        float maxVal = -1.0f;
        bool sampleFound = false;

        for (int frame = startFrame; frame < endFrame && frame < totalFrames; ++frame) {
            for (int ch = 0; ch < channels; ++ch) {
                int sampleIdx = (frame * channels) + ch;
                int byteOffset = sampleIdx * bytesPerSample;
                
                if (byteOffset + bytesPerSample > totalPcmBytes) break;

                float sampleNormalized = 0.0f;

                if (bitsPerSample == 16) {
                    int16_t val = *reinterpret_cast<const int16_t*>(rawPtr + byteOffset);
                    sampleNormalized = val / 32768.0f;
                } else if (bitsPerSample == 8) {
                    uint8_t val = *reinterpret_cast<const uint8_t*>(rawPtr + byteOffset);
                    sampleNormalized = (val - 128) / 128.0f;
                } else if (bitsPerSample == 32) {
                    if (audioFormat == 3) {
                        sampleNormalized = *reinterpret_cast<const float*>(rawPtr + byteOffset);
                    } else {
                        int32_t val = *reinterpret_cast<const int32_t*>(rawPtr + byteOffset);
                        sampleNormalized = val / 2147483648.0f;
                    }
                }

                minVal = std::min(minVal, sampleNormalized);
                maxVal = std::max(maxVal, sampleNormalized);
                sampleFound = true;
            }
        }

        if (!sampleFound || minVal > maxVal) {
            minVal = 0.0f;
            maxVal = 0.0f;
        }

        result.minMaxPeaks[col] = qMakePair(minVal, maxVal);
    }

    result.isValid = true;
    return result;
}

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
    lastSelectedFolderPath(QDir::homePath()),
    currentSortColumn(-1),
    currentSortOrder(Qt::AscendingOrder) {
    
    mediaPlayer = new QMediaPlayer(this);
    mediaPlayer->setVolume(100);

    setupUI();
}

void MainWindow::setupUI() {
    setFixedSize(800, 600);
    setWindowTitle("Samples Scanner (rev2)");

    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    waveformWidget = new WaveformWidget(this);

    connect(waveformWidget, &WaveformWidget::doubleClicked, this, &MainWindow::openSamplesFolderAndScan);
    connect(waveformWidget, &WaveformWidget::openFolderRequested, this, &MainWindow::openSamplesFolderAndScan);
    connect(waveformWidget, &WaveformWidget::playSoundRequested, this, &MainWindow::playSelectedSample);

    waveformWidget->setImportCallback([this](const QString& fileName) {
        QMessageBox::information(this, "Sample Imported", QString("Successfully imported sample file:\n%1").arg(fileName));
    });

    tableWidget = new CustomTableWidget(this);
    tableWidget->setColumnCount(6);

    baseHeaders = QStringList{ "File Name", "File Size (KB)", "Sample Rate", "Channels", "Bits/Sample", "Duration (ms)" };
    tableWidget->setHorizontalHeaderLabels(baseHeaders);

    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableWidget->setAlternatingRowColors(true);
    tableWidget->verticalHeader()->setVisible(false);
    tableWidget->setSortingEnabled(false);

    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    tableWidget->setStyleSheet(
        "QTableWidget {"
        "    background-color: #121212;"
        "    alternate-background-color: #1a1a1a;"
        "    color: #e0e0e0;"
        "    gridline-color: #2c2c2c;"
        "    selection-background-color: #0d47a1;"
        "    selection-color: #ffffff;"
        "    border: none;"
        "}"
        "QHeaderView::section {"
        "    background-color: #1f1f1f;"
        "    color: #ffffff;"
        "    padding: 6px;"
        "    border: 1px solid #2c2c2c;"
        "    font-weight: bold;"
        "}"
        "QTableView::item {"
        "    padding: 5px;"
        "}"
    );

    connect(tableWidget, &QTableWidget::cellClicked, this, &MainWindow::onRowClicked);
    connect(tableWidget, &QTableWidget::cellDoubleClicked, this, &MainWindow::onRowDoubleClicked);
    connect(tableWidget, &CustomTableWidget::rowActivatedByKeyboard, this, &MainWindow::onRowSelected);
    connect(tableWidget, &CustomTableWidget::enterPressedOnRow, this, &MainWindow::onEnterPressed);
    connect(tableWidget, &CustomTableWidget::escapePressed, this, &QWidget::close);
    connect(tableWidget->horizontalHeader(), &QHeaderView::sectionClicked, this, &MainWindow::handleHeaderClicked);

    f5Shortcut = new QShortcut(QKeySequence(Qt::Key_F5), this);
    connect(f5Shortcut, &QShortcut::activated, this, &MainWindow::openSamplesFolderAndScan);

    connect(waveformWidget, &WaveformWidget::filterTextChanged, this, [this](const QString& filterText) {
        for (int i = 0; i < tableWidget->rowCount(); ++i) {
            QTableWidgetItem* item = tableWidget->item(i, 0);
            if (item) {
                bool matches = item->text().contains(filterText, Qt::CaseInsensitive);
                tableWidget->setRowHidden(i, !matches);
            }
        }
    });

    mainLayout->addWidget(waveformWidget, 2);
    mainLayout->addWidget(tableWidget, 4);

    setCentralWidget(centralWidget);
}

void MainWindow::openSamplesFolderAndScan() {
    QString folderPath = QFileDialog::getExistingDirectory(this, "Select Samples Folder", lastSelectedFolderPath);
    if (!folderPath.isEmpty()) {
        lastSelectedFolderPath = folderPath;
        currentSelectedFilePath.clear();
        currentSortColumn = -1;
        currentSortOrder = Qt::AscendingOrder;
        tableWidget->setHorizontalHeaderLabels(baseHeaders);
        waveformWidget->clear();
        scanWavDirectory(folderPath);
    }
}

void MainWindow::playSelectedSample() {
    if (!currentSelectedFilePath.isEmpty()) {
        mediaPlayer->setMedia(QMediaContent(QUrl::fromLocalFile(currentSelectedFilePath)));
        mediaPlayer->play();
    }
}

bool MainWindow::parseWavHeader(const QString& filePath, qint64& fileSizeKB, quint32& sampleRate, quint16& channels, quint16& bitsPerSample, qint64& durationMs) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) return false;

    fileSizeKB = file.size() / 1024;

    QByteArray header = file.read(44);
    if (header.size() < 44) return false;

    QDataStream stream(header);
    stream.setByteOrder(QDataStream::LittleEndian);

    char chunkId[4], waveId[4], fmtId[4];
    stream.readRawData(chunkId, 4);
    quint32 chunkSize;
    stream >> chunkSize;
    stream.readRawData(waveId, 4);

    if (qstrncmp(chunkId, "RIFF", 4) != 0 || qstrncmp(waveId, "WAVE", 4) != 0) {
        return false;
    }

    quint32 fmtSize;
    quint16 audioFormat;
    quint32 byteRate;
    quint16 blockAlign;

    stream.readRawData(fmtId, 4);
    stream >> fmtSize >> audioFormat >> channels >> sampleRate >> byteRate >> blockAlign >> bitsPerSample;

    if (byteRate > 0) {
        qint64 dataSize = file.size() - 44;
        durationMs = (dataSize * 1000) / byteRate;
    } else {
        durationMs = 0;
    }

    file.close();
    return true;
}

void MainWindow::scanWavDirectory(const QString& folderPath) {
    QDir dir(folderPath);
    QStringList wavFiles = dir.entryList(QStringList() << "*.wav", QDir::Files);

    tableWidget->setRowCount(0);
    tableWidget->setRowCount(wavFiles.size());

    for (int i = 0; i < wavFiles.size(); ++i) {
        QString fullPath = dir.absoluteFilePath(wavFiles[i]);
        qint64 sizeKB = 0;
        quint32 rate = 0;
        quint16 chans = 0, bits = 0;
        qint64 duration = 0;

        if (parseWavHeader(fullPath, sizeKB, rate, chans, bits, duration)) {
            tableWidget->setItem(i, 0, new QTableWidgetItem(wavFiles[i]));

            auto* sizeItem = new QTableWidgetItem();
            sizeItem->setData(Qt::DisplayRole, sizeKB);
            tableWidget->setItem(i, 1, sizeItem);

            auto* rateItem = new QTableWidgetItem();
            rateItem->setData(Qt::DisplayRole, rate);
            tableWidget->setItem(i, 2, rateItem);

            auto* chansItem = new QTableWidgetItem();
            chansItem->setData(Qt::DisplayRole, chans);
            tableWidget->setItem(i, 3, chansItem);

            auto* bitsItem = new QTableWidgetItem();
            bitsItem->setData(Qt::DisplayRole, bits);
            tableWidget->setItem(i, 4, bitsItem);

            auto* durationItem = new QTableWidgetItem();
            durationItem->setData(Qt::DisplayRole, duration);
            tableWidget->setItem(i, 5, durationItem);
        }
    }

    // Automatically select and render waveform for the first row if valid files were loaded
    if (tableWidget->rowCount() > 0) {
        tableWidget->selectRow(0);
        onRowSelected(0);
    }
}

void MainWindow::handleHeaderClicked(int logicalIndex) {
    if (currentSortColumn == logicalIndex) {
        currentSortOrder = (currentSortOrder == Qt::AscendingOrder) ? Qt::DescendingOrder : Qt::AscendingOrder;
    } else {
        currentSortColumn = logicalIndex;
        currentSortOrder = Qt::AscendingOrder;
    }

    tableWidget->sortItems(currentSortColumn, currentSortOrder);

    QStringList updatedHeaders = baseHeaders;
    for (int i = 0; i < updatedHeaders.size(); ++i) {
        if (i == currentSortColumn) {
            QString arrow = (currentSortOrder == Qt::AscendingOrder) ? " ▲" : " ▼";
            updatedHeaders[i] += arrow;
        }
    }
    tableWidget->setHorizontalHeaderLabels(updatedHeaders);
}

void MainWindow::onRowClicked(int row, int /*column*/) {
    onRowSelected(row);
}

void MainWindow::onRowDoubleClicked(int row, int /*column*/) {
    QTableWidgetItem* item = tableWidget->item(row, 0);
    if (!item || lastSelectedFolderPath.isEmpty()) return;

    QString fullPath = QDir(lastSelectedFolderPath).filePath(item->text());

    QMessageBox::information(this, "Sample Alert", QString("Selected Sample Full Path:\n%1").arg(fullPath));
}

void MainWindow::onRowSelected(int row) {
    if (mediaPlayer) {
        mediaPlayer->stop();
    }

    QTableWidgetItem* item = tableWidget->item(row, 0);
    if (!item || lastSelectedFolderPath.isEmpty()) return;

    QString fileName = item->text();
    currentSelectedFilePath = QDir(lastSelectedFolderPath).filePath(fileName);

    int targetCanvasWidth = waveformWidget->width();

    QFuture<WaveformData> future = QtConcurrent::run([filePath = currentSelectedFilePath, fileName, targetCanvasWidth]() {
        return processAudioFileOffThread(filePath, fileName, targetCanvasWidth);
    });

    QFutureWatcher<WaveformData>* watcher = new QFutureWatcher<WaveformData>(this);
    connect(watcher, &QFutureWatcher<WaveformData>::finished, this, [this, watcher]() {
        waveformWidget->setWaveformData(watcher->result());
        watcher->deleteLater();
    });

    watcher->setFuture(future);
}

void MainWindow::onEnterPressed(int row) {
    QTableWidgetItem* item = tableWidget->item(row, 0);
    if (!item) return;
    onRowDoubleClicked(row, 0);
}