#include "mainform.h"
#include "ui_mainform.h"
#include <QDebug>
#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>
#include <thread>
#include <QMessageBox>


MainForm::MainForm(QWidget *parent) : QWidget(parent),
                                      ui(new Ui::MainForm),
                                      rect_items(17),
                                      text_items(17)
{
    ui->setupUi(this);

    qRegisterMetaType<OrderDispatch*>("OrderDispatch*");

    ui->btn_end->setEnabled(false);
    ui->g_view->setScene(&scene);

    scene.installEventFilter(this);

    auto rect = ui->g_view->geometry();
    qDebug()<<rect;
    auto cell_width = rect.width() / 17;
    auto cell_height = rect.height() / 17;



    for(int i=0;i<17;++i)
    {
        for(int j=0;j<17;++j)
        {
            rect_items[i].push_back(scene.addRect(QRect(i*cell_width, j*cell_height, cell_width, cell_height)));
            auto p_txt = scene.addText("");
            text_items[i].push_back(p_txt);
            p_txt->setPos(i*cell_width, j*cell_height);
        }
    }
    line_item = scene.addLine(0,0,0,0, QPen(QBrush(Qt::red), 2));

    reset_paint();
}

MainForm::~MainForm()
{
    delete ui;
}

bool MainForm::eventFilter(QObject *watched, QEvent *event)
{
    if(p_dispatch && !p_dispatch->finished)
    {
        if(watched == &scene)
        {
            if(event->type() == QEvent::GraphicsSceneMousePress)
            {
                auto cell_width = (ui->g_view->width() + ui->g_view->width()/17) / 9;
                auto cell_heigth = (ui->g_view->height() + ui->g_view->height() / 17) / 9;
                int sel_r = 0;
                int sel_c = 0;
                auto pos = QCursor::pos();
                pos = ui->g_view->mapFromGlobal(pos);
                for(int i=0;i<9;++i)
                {
                    if(cell_width*i-cell_width/2<pos.x() && cell_width*i+cell_width/2>=pos.x())
                    {
                        sel_c = i*2;
                        break;
                    }
                }
                for(int i=0;i<9;++i)
                {
                    if(cell_heigth*i-cell_heigth/2<pos.y() && cell_heigth*i+cell_heigth/2>=pos.y())
                    {
                        sel_r = i*2;
                        break;
                    }
                }
                from_p.setX(sel_c);
                from_p.setY(sel_r);
                selected = true;
                return  true;
            }
            else if(event->type() == QEvent::GraphicsSceneMouseMove)
            {
                if(selected)
                {
                    auto cell_width = (ui->g_view->width() + ui->g_view->width()/17) / 9;
                    auto cell_heigth = (ui->g_view->height() + ui->g_view->height() / 17) / 9;
                    int sel_r = 0;
                    int sel_c = 0;
                    auto pos = QCursor::pos();
                    pos = ui->g_view->mapFromGlobal(pos);
                    for(int i=0;i<9;++i)
                    {
                        if(cell_width*i-cell_width/2<pos.x() && cell_width*i+cell_width/2>=pos.x())
                        {
                            sel_c = i*2;
                            break;
                        }
                    }
                    for(int i=0;i<9;++i)
                    {
                        if(cell_heigth*i-cell_heigth/2<pos.y() && cell_heigth*i+cell_heigth/2>=pos.y())
                        {
                            sel_r = i*2;
                            break;
                        }
                    }
                    auto cell_little_width = cell_width / 2;
                    auto cell_little_heigth = cell_heigth / 2;

                    line_item->setLine(from_p.x() * cell_little_width + cell_little_width / 2,
                                       from_p.y() * cell_little_heigth + cell_little_heigth / 2,
                                       sel_c * cell_little_width + cell_little_width / 2,
                                       sel_r * cell_little_heigth + cell_little_heigth / 2);
                    line_item->show();
                    return  true;
                }
            }
            else if(event->type() == QEvent::GraphicsSceneMouseRelease)
            {
                if(selected)
                {
                    auto cell_width = (ui->g_view->width() + ui->g_view->width()/17) / 9;
                    auto cell_heigth = (ui->g_view->height() + ui->g_view->height() / 17) / 9;
                    int sel_r = 0;
                    int sel_c = 0;
                    auto pos = QCursor::pos();
                    pos = ui->g_view->mapFromGlobal(pos);
                    for(int i=0;i<9;++i)
                    {
                        if(cell_width*i-cell_width/2<pos.x() && cell_width*i+cell_width/2>=pos.x())
                        {
                            sel_c = i*2;
                            break;
                        }
                    }
                    for(int i=0;i<9;++i)
                    {
                        if(cell_heigth*i-cell_heigth/2<pos.y() && cell_heigth*i+cell_heigth/2>=pos.y())
                        {
                            sel_r = i*2;
                            break;
                        }
                    }
                    Order o;
                    o.from_x = from_p.x();
                    o.from_y = from_p.y();
                    o.to_x = sel_c;
                    o.to_y = sel_r;
                    p_dispatch->add_order_to_deque(o);
                    line_item->hide();
                    selected = false;
                    return true;
                }
            }
        }
    }
    return QWidget::eventFilter(watched, event);
}

void MainForm::reset_paint()
{
    for(int i=0;i<17;++i)
    {
        for(int j=0;j<17;++j)
        {
            rect_items[i][j]->setPen(QPen(QColor(0,0,0,0)));
            text_items[i][j]->setPlainText("");
            if(i%2==0&&j%2==0)
            {
                rect_items[i][j]->setBrush(QBrush(Qt::yellow));
            }
            else
            {
                rect_items[i][j]->setBrush(QBrush(Qt::gray));
            }
        }
    }
    line_item->hide();
}

void MainForm::on_btn_start_clicked()
{
    delete p_dispatch;
    ui->btn_start->setEnabled(false);
    ui->btn_end->setEnabled(true);
    p_out = fopen("output.txt", "w");
    std::thread([=]{
        p_dispatch = new OrderDispatch();
        connect(p_dispatch, &OrderDispatch::refresh_result, this, &MainForm::on_refresh_result,Qt::QueuedConnection);
        connect(p_dispatch, &OrderDispatch::bankrupt_result, this, &MainForm::on_bankrupt_result,Qt::QueuedConnection);
        connect(p_dispatch, &OrderDispatch::revoke_result, this, &MainForm::on_revoke_result,Qt::QueuedConnection);
        p_dispatch->start_dispatch(p_out);
    }).detach();
}

void MainForm::on_refresh_result(OrderDispatch * o)
{
    reset_paint();
    {
        std::unique_lock<std::mutex> lck(o->mu_deque);
        QString str;
        int timeout_count = 0;
        for (auto order: o->finished_orders)
        {
            if (order.time < DIE_TIME - WARN_TIME)
            {
                ++timeout_count;
            }
        }
        str += QString("时间:%1\n钱:%2\n接单数:%3\n完成数:%4\n超时数:%5\n").arg(o->time).arg(o->money).arg(o->order_count).arg(o->finished_orders.size()).arg(timeout_count);

        for (size_t i = 0; i < o->riders.size(); ++i)
        {
            const Rider rider = o->riders[i];
            str += QString("骑手%1位置：%2,%3\n未完成订单数量：%4\n").arg(i).arg(rider.x).arg(rider.y).arg(rider.orders.size());
//            if (rider.last_path_flag)
//            {
//                str += QString("准备前往：%1,%2, 距离：%3\n").arg(rider.last_path.next_x).arg(rider.last_path.next_y).arg(rider.last_path.dis);
//            }
        }
        ui->txt_status->setText(str);


        for(int i = 0;i<o->riders.size();++i)
        {
            auto rider = o->riders[i];
            rect_items[rider.x][rider.y]->setBrush(Qt::white);
            text_items[rider.x][rider.y]->setPlainText("骑" + QString::number(i));
            for(auto order: rider.orders)
            {
                if(order.status == 1)
                {
                    text_items[order.from_x][order.from_y]->setPlainText("商" + QString::number(order.id));
                    rect_items[order.from_x][order.from_y]->setBrush(Qt::green);
                }
                rect_items[order.to_x][order.to_y]->setBrush(Qt::red);
                text_items[order.to_x][order.to_y]->setPlainText("客" + QString::number(order.id));
            }
        }
    }
}

void MainForm::on_bankrupt_result(OrderDispatch * o)
{
    QMessageBox::information(this, "提示", "已破产");
}

void MainForm::on_revoke_result(OrderDispatch * o)
{
    QMessageBox::information(this, "提示", "已吊销");
}

void MainForm::on_btn_end_clicked()
{
    p_dispatch->finished = true;
    ui->btn_start->setEnabled(true);
    ui->btn_end->setEnabled(false);
    fclose(p_out);
}
