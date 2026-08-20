#pragma once

#ifndef LMMS_GUI_WAFEFORM_DATA_H
#define LMMS_GUI_WAFEFORM_DATA_H

#include <QVector>
#include <QPair>
#include <QString>

namespace lmms::gui
{
    struct WaveformData {
        QVector<QPair<float, float>> minMaxPeaks;
        qint64 durationMs = 0;
        QString fileName;
        bool isValid = false;
    };
}

#endif