#ifndef MAINFORM_H
#define MAINFORM_H

#include <QWidget>
#include <QGraphicsScene>
#include "orderdispatch.h"

namespace Ui {
class MainForm;
}

class MainForm : public QWidget
{
    Q_OBJECT

public:
    explicit MainForm(QWidget *parent = nullptr);
    ~MainForm() override;

    bool eventFilter(QObject *watched, QEvent *event) override;

    void reset_paint();

private slots:
    void on_btn_start_clicked();

    void on_refresh_result( OrderDispatch*  o);
    void on_bankrupt_result( OrderDispatch*  o);
    void on_revoke_result( OrderDispatch * o);

    void on_btn_end_clicked();

private:
    Ui::MainForm *ui;

    QGraphicsScene scene;

    std::vector<std::vector<QGraphicsRectItem*>> rect_items;
    std::vector<std::vector<QGraphicsTextItem*>> text_items;
    QGraphicsLineItem * line_item;

    OrderDispatch * p_dispatch = nullptr;

    FILE * p_out = nullptr;

    QPoint from_p;
    bool selected = false;
};

#endif // MAINFORM_H
