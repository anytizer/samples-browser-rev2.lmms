#pragma once

#ifndef WAVEFORMWIDGET_H
#define WAVEFORMWIDGET_H

#include <QWidget>
#include <QVector>
#include <QPair>
#include <QPushButton>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <functional>

#include "WaveformData.h"

namespace lmms::gui
{

class WaveformWidget: public QWidget {
    Q_OBJECT
public:
    explicit WaveformWidget(QWidget* parent = nullptr);
    void setWaveformData(const WaveformData& data);
    void clear();

    void setImportCallback(std::function<void(const QString&)> callback);

signals:
    void doubleClicked();
    void openFolderRequested();
    void playSoundRequested();
    void filterTextChanged(const QString& text);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;

private:
    WaveformData m_data;
    QLineEdit* txtSearchFilter;
    QPushButton* btnOpenFolder;
    QPushButton* btnPlaySound;
    QPushButton* btnImportSample;
    std::function<void(const QString&)> m_importCallback;
};

}

#endif // WAVEFORMWIDGET_H