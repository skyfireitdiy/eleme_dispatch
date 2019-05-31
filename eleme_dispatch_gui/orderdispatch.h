#ifndef ORDERDISPATCH_H
#define ORDERDISPATCH_H


#include <vector>
#include <string>
#include <algorithm>
#include <atomic>
#include <mutex>
#include <QObject>

#define AT(x) QString::fromUtf8(x).toLocal8Bit()


constexpr int NO_ORDER = 2000;
constexpr int START_X = 8;
constexpr int START_Y = 9;
constexpr int START_MONEY = 1000;
constexpr int DIE_TIME = 60;
constexpr int WARN_TIME = 30;

constexpr int RIDER_PRICE = 300;

constexpr int EARN_MONEY = 10;
constexpr int LOST_MONEY = 50;

constexpr int CELL_TIME = 1;


// 订单结构

typedef struct Order
{
    int id = 0;			// 编号
    int time = DIE_TIME;		// 剩余时间
    int from_x=0;		// 商家x
    int from_y=0;		// 商家y
    int to_x=0;		// 顾客x
    int to_y=0;		// 顾客y
    int status=1;		// 当前状态（1. 待取 2. 待送 3. 完成）
} Order;

using OrderList = std::vector<Order>;

// 路径信息

typedef struct PathData {
    int dis;	   // 距离
    int next_x;	   // 下一地点的x坐标
    int next_y;	   // 下一地点的y坐标
    int timeout=0;   // 是否会超时
    int over=0;		// 是否会破产
} PathData;



// 骑手结构

typedef struct Rider
{
    int x;				 // 骑手x
    int y;				 // 骑手y
    OrderList orders;	 // 负责的订单

    int last_path_flag;		// 下一个目的地是否有效
    PathData last_path;		// 下一个目的地信息

    OrderList stop_merchant_order;	// 停靠商家訂單
    OrderList stop_customer_order;	// 停靠客戶訂單
}Rider;


using RiderList = std::vector<Rider>;



// 距离信息

typedef struct DistanceData
{
    int dis;		// 距离
    int left_x;		// 目的地相邻的第一个点的x坐标
    int left_y;		// 目的地相邻的第一个点的y坐标
    int mid_x;
    int mid_y;
    int right_x;	// 目的地相邻的第二个点的x坐标
    int right_y;	// 目的地相邻的第二个点的y坐标
} DistanceData;



// 订单调度所需要的结构

class OrderDispatch  : public QObject{
    Q_OBJECT
private:
    int money;					 // 账户余额
    RiderList riders;			 // 骑手列表
    OrderList finished_orders;	 // 已完成列表
    int time;					 // 当前时间
    int order_count;			 // 已接受订单数量
    FILE *p_out;				 // 输出文件

    std::vector<Order> dequed_order;
    std::mutex mu_deque;
    std::atomic<int> global_id{0};
    std::atomic<bool> finished{false};

public:

    static DistanceData distance(int x1, int y1, int x2, int y2);

    struct PathData shortest_go(Rider rider);

    OrderDispatch();

    Rider* add_rider();

    void add_order(const Order o);

    void output();

    void display();

    int run();

    void start_dispatch(FILE * p_out);

    void add_order_to_deque(Order o);

signals:
    void refresh_result( OrderDispatch*  o);
    void bankrupt_result( OrderDispatch*  o);
    void revoke_result( OrderDispatch*  o);

    friend class MainForm;

};




#endif // ORDERDISPATCH_H
