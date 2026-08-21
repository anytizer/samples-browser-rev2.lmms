#pragma once

#ifndef LMMS_GUI_CUSTOMTABLEWIDGET_H
#define LMMS_GUI_CUSTOMTABLEWIDGET_H

#include <QTableWidget>
#include <QVBoxLayout>
#include <QWidget>
#include <QShortcut>
#include <QKeyEvent>
#include <QMediaPlayer>

namespace lmms::gui
{
class CustomTableWidget : public QTableWidget {
    Q_OBJECT
public:
    CustomTableWidget(QWidget* parent = nullptr) : QTableWidget(parent) {
        this->setFixedSize(800, 640);
    }

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
            if (
                event->key() == Qt::Key_Up || 
                event->key() == Qt::Key_Down || 
                event->key() == Qt::Key_PageUp || 
                event->key() == Qt::Key_PageDown || 

                // non-functional keys at the moment
                event->key() == Qt::Key_Home || 
                event->key() == Qt::Key_End
            ) {
                emit rowActivatedByKeyboard(currentRow);
            }
            else if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
                emit enterPressedOnRow(currentRow);
            }
        }
    }
};

}

#endif