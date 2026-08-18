#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QWidget>
#include <QShortcut>
#include <QKeyEvent>
#include <QMediaPlayer>
#include "waveformwidget.h"

class CustomTableWidget : public QTableWidget {
    Q_OBJECT
public:
    CustomTableWidget(QWidget* parent = nullptr) : QTableWidget(parent) {}

signals:
    void rowActivatedByKeyboard(int row);
    void enterPressedOnRow(int row);
    void escapePressed();

protected:
    void keyPressEvent(QKeyEvent* event) override {
        QTableWidget::keyPressEvent(event);
        int currentRow = this->currentRow();
        if (event->key() == Qt::Key_Escape) {
            emit escapePressed();
            return;
        }
        if (currentRow >= 0) {
            if (event->key() == Qt::Key_Up || event->key() == Qt::Key_Down) {
                emit rowActivatedByKeyboard(currentRow);
            }
            else if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
                emit enterPressedOnRow(currentRow);
            }
        }
    }
};

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow() = default;

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

#endif // MAINWINDOW_H