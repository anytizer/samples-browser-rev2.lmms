#include <QWidget>
#include <QSizePolicy>
#include <QString>
#include <QPushButton>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QPen>
#include <QColor>
#include <QPainter>

#include <algorithm>
#include <cmath>

#include "WaveformWidget.h"

namespace lmms::gui
{

WaveformWidget::WaveformWidget(QWidget* parent)
{
    // Create Search/Filter Input
    txtSearchFilter = new QLineEdit(this);
    txtSearchFilter->setPlaceholderText("Filter files...");
    txtSearchFilter->setClearButtonEnabled(true);
    txtSearchFilter->setStyleSheet(
        "QLineEdit {"
        "    background-color: #2a2a2a;"
        "    color: #ffffff;"
        "    border: 1px solid #444444;"
        "    border-radius: 4px;"
        "    padding: 4px 8px;"
        "    font-size: 11px;"
        "    min-width: 140px;"
        "}"
        "QLineEdit:focus {"
        "    border: 1px solid #87CEEB;"
        "}"
    );

    // Create action buttons
    btnOpenFolder = new QPushButton("Open Folder", this);
    btnPlaySound = new QPushButton("Play Sound", this);
    btnImportSample = new QPushButton("Import Sample", this);

    // Sky blue styling
    QString buttonStyle = 
        "QPushButton {"
        "    background-color: #87CEEB;"
        "    color: #000000;"
        "    border: none;"
        "    border-radius: 4px;"
        "    padding: 6px 12px;"
        "    font-weight: bold;"
        "    font-size: 11px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #00BFFF;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #4682B4;"
        "    color: #ffffff;"
        "}";

    btnOpenFolder->setStyleSheet(buttonStyle);
    btnPlaySound->setStyleSheet(buttonStyle);
    btnImportSample->setStyleSheet(buttonStyle);

    // Overlay layout at bottom-right (Input added before Open Folder)
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->setContentsMargins(10, 10, 15, 12);
    buttonLayout->setSpacing(8);
    buttonLayout->addStretch();
    buttonLayout->addWidget(txtSearchFilter);
    buttonLayout->addWidget(btnOpenFolder);
    buttonLayout->addWidget(btnPlaySound);
    buttonLayout->addWidget(btnImportSample);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addStretch();
    mainLayout->addLayout(buttonLayout);

    // Signal connections
    connect(txtSearchFilter, &QLineEdit::textChanged, this, &WaveformWidget::filterTextChanged);
    connect(btnOpenFolder, &QPushButton::clicked, this, &WaveformWidget::openFolderRequested);
    connect(btnPlaySound, &QPushButton::clicked, this, &WaveformWidget::playSoundRequested);
    connect(btnImportSample, &QPushButton::clicked, this, [this]() {
        if (m_importCallback && m_data.isValid) {
            m_importCallback(m_data.fileName);
        }
    });
}

void WaveformWidget::setImportCallback(std::function<void(const QString&)> callback) {
    m_importCallback = callback;
}

void WaveformWidget::setWaveformData(const WaveformData& data) {
    m_data = data;
    update();
}

void WaveformWidget::clear() {
    m_data = WaveformData();
    txtSearchFilter->clear();
    update();
}

void WaveformWidget::mouseDoubleClickEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        emit doubleClicked();
    }
    QWidget::mouseDoubleClickEvent(event);
}

void WaveformWidget::paintEvent(QPaintEvent* /*event*/) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    int w = width();
    int h = height();
    int centerY = h / 2;

    painter.fillRect(rect(), QColor("#1e1e1e"));

    if (!m_data.isValid) {
        painter.setPen(QColor("#888888"));
        painter.setFont(QFont("Arial", 10));
        painter.drawText(rect(), Qt::AlignCenter, "Double-click here or click 'Open Folder' to select directory");
        return;
    }

    painter.setPen(QPen(QColor("#444444"), 1, Qt::DashLine));
    painter.drawLine(0, centerY, w, centerY);

    if (m_data.durationMs > 0) {
        qint64 tickIntervalMs = (m_data.durationMs < 5000) ? 100 : 1000;
        painter.setPen(QPen(QColor(255, 255, 255, 60), 1, Qt::DotLine));

        for (qint64 ms = tickIntervalMs; ms < m_data.durationMs; ms += tickIntervalMs) {
            int xPos = static_cast<int>((static_cast<double>(ms) / m_data.durationMs) * w);
            if (xPos > 0 && xPos < w) {
                painter.drawLine(xPos, 0, xPos, h);
            }
        }
    }

    const auto& peaks = m_data.minMaxPeaks;
    if (!peaks.isEmpty()) {
        float maxPeakVal = 0.0f;
        for (const auto& pair : peaks) {
            maxPeakVal = std::max(maxPeakVal, std::abs(pair.first));
            maxPeakVal = std::max(maxPeakVal, std::abs(pair.second));
        }

        float availableHalfHeight = (h / 2.0f) - 6.0f;
        float gainScale = 1.0f;
        if (maxPeakVal > 0.0001f && maxPeakVal < 0.50f) {
            gainScale = 0.50f / maxPeakVal;
        }

        float effectiveMaxHeight = availableHalfHeight * gainScale;

        painter.setPen(QPen(QColor("#4da6ff"), 1));
        double xStep = static_cast<double>(w) / peaks.size();

        for (int i = 0; i < peaks.size(); ++i) {
            int x = static_cast<int>(i * xStep);
            int yMin = centerY - static_cast<int>(peaks[i].second * effectiveMaxHeight);
            int yMax = centerY - static_cast<int>(peaks[i].first * effectiveMaxHeight);

            if (yMin == yMax) yMax = yMin + 1;
            painter.drawLine(x, yMin, x, yMax);
        }
    }

    // painter.setPen(QColor("#00ffcc"));
    // painter.setFont(QFont("Arial", 9, QFont::Bold));
    // painter.drawText(15, 20, QString("Waveform: %1").arg(m_data.fileName));
}

}