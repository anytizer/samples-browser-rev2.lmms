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
        this->setFixedSize(800, 660);
    }

signals:
    void rowActivatedByKeyboard(int row);
    void enterPressedOnRow(int row);
    void escapePressed();

protected:
    void keyPressEvent(QKeyEvent* event) override {
        QTableWidget::keyPressEvent(event);
        int currentRow = this->currentRow();
        int key = event->key();
        if (key == Qt::Key_Escape) {
            emit escapePressed();
            return;
        }
        if (currentRow >= 0) {
            if (
                key == Qt::Key_Up || 
                key == Qt::Key_Down || 
                key == Qt::Key_PageUp || 
                key == Qt::Key_PageDown || 

                // non-functional keys at the moment
                key == Qt::Key_Home || 
                key == Qt::Key_End
            ) {
                emit rowActivatedByKeyboard(currentRow);
            }
            else if (key == Qt::Key_Return || key == Qt::Key_Enter) {
                emit enterPressedOnRow(currentRow);
            }
        }
    }
};

}

#endif
