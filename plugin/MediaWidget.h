#pragma once

#ifndef LMMS_GUI_MEDIAWIDGET_H
#define LMMS_GUI_MEDIAWIDGET_H

#include <QTableWidget>
#include <QVBoxLayout>
#include <QWidget>
#include <QShortcut>
#include <QKeyEvent>
#include <QMediaPlayer>
#include "WaveformWidget.h"
#include "CustomTableWidget.h"

namespace lmms::gui
{


class MediaWidget : public QWidget {
    Q_OBJECT

public:
    MediaWidget(QWidget* parent = nullptr);
    ~MediaWidget() = default;

private slots:
    void onRowClicked(int row, int column);
    void onRowDoubleClicked(int row, int column);
    void onRowSelected(int row);
    void onEnterPressed(int row);
    void openSamplesFolderAndScan();
    void playSelectedSample();
    void handleHeaderClicked(int logicalIndex);

private:
    void setupUI();
    void scanWavDirectory(const QString& folderPath);
    bool parseWavHeader(const QString& filePath, qint64& fileSizeKB, quint32& sampleRate, quint16& channels, quint16& bitsPerSample, qint64& durationMs);

    WaveformWidget* waveformWidget;
    CustomTableWidget* tableWidget;
    QShortcut* f5Shortcut;
    QMediaPlayer* mediaPlayer;
    QString lastSelectedFolderPath;
    QString currentSelectedFilePath;
    QStringList baseHeaders;
    int currentSortColumn;
    Qt::SortOrder currentSortOrder;
};

#endif // LMMS_GUI_MEDIAWIDGET_H

}