#include "orderdispatch.h"
#include <QString>

DistanceData OrderDispatch::distance(int x1, int y1, int x2, int y2)
{
    struct DistanceData ret;

    ret.dis = (abs(x1 - x2) + abs(y1 - y2)) / 2;
    // 如果点1在点2左上方
    if (x1 < x2 && y1 < y2) {
        ret.left_x = x2 - 1;
        ret.left_y = y2;
        ret.right_x = x2;
        ret.right_y = y2 - 1;
        ret.mid_x = ret.left_x;
        ret.mid_y = ret.left_y;
    }
    // 如果点1在点2左下方
    else if (x1 < x2 && y1 > y2) {
        ret.left_x = x2 - 1;
        ret.left_y = y2;
        ret.right_x = x2;
        ret.right_y = y2 + 1;
        ret.mid_x = ret.left_x;
        ret.mid_y = ret.left_y;
    }
    // 如果点1在点2右上方
    else if (x1 > x2 && y1 < y2) {
        ret.left_x = x2;
        ret.left_y = y2 - 1;
        ret.right_x = x2 + 1;
        ret.right_y = y2;
        ret.mid_x = ret.left_x;
        ret.mid_y = ret.left_y;
    }
    // 如果点1在点2右下方
    else if(x1 > x2 && y1 > y2)
    {
        ret.left_x = x2;
        ret.left_y = y2 + 1;
        ret.right_x = x2 + 1;
        ret.right_y = y2;
        ret.mid_x = ret.left_x;
        ret.mid_y = ret.left_y;
    }
    else if(x1 == x2 && y1 < y2)
    {
        ret.left_x = x2 == 0? x2+1: x2-1;
        ret.left_y = y2;
        ret.right_x = x2 == 16? x2-1:x2+1;
        ret.right_y = y2;
        ret.mid_x = x2;
        ret.mid_y = y2-1;
    }
    else if(x1 == x2 && y1 > y2)
    {
        ret.left_x = x2 == 0? x2+1: x2-1;
        ret.left_y = y2;
        ret.right_x = x2 == 16? x2-1:x2+1;
        ret.right_y = y2;
        ret.mid_x = x2;
        ret.mid_y = y2+1;
    }
    else if(x1 < x2 && y1 == y2)
    {
        ret.left_y = y2 == 0? y2+1: y2-1;
        ret.left_x = x2;
        ret.right_y = y2 == 16? y2-1:y2+1;
        ret.right_x = x2;
        ret.mid_y = y2;
        ret.mid_x = x2+1;
    }
    else if(x1 > x2 && y1 == y2)
    {
        ret.left_y = y2 == 0? y2+1: y2-1;
        ret.left_x = x2;
        ret.right_y = y2 == 16? y2-1:y2+1;
        ret.right_x = x2;
        ret.mid_y = y2;
        ret.mid_x = x2-1;
    }
    return ret;
}

PathData OrderDispatch::shortest_go(Rider rider)
{
    struct PathData result;
    result.dis = NO_ORDER;
    result.next_x = 0;
    result.next_y = 0;
    result.timeout = 0;
    result.over = 0;
    for (auto & rider:rider.orders)
    {
        // 记录已超时的订单
        if (rider.time < DIE_TIME - WARN_TIME)
        {
            result.timeout = 1;
        }
        // 如果有订单超时破产，直接返回，说明此路径走不通
        if (rider.time < 0)
        {
            result.over = 1;
            return result;
        }
    }
    for (size_t i = 0; i < rider.orders.size(); ++i)
    {
        struct DistanceData dist;
        struct PathData left_path;
        struct PathData right_path;
        struct PathData mid_path;
        int order_status = rider.orders[ i].status;
        // 订单未取
        if (order_status == 1)
        {
            // 计算到所有点的距离
            dist = distance(rider.x, rider.y, rider.orders[ i].from_x, rider.orders[ i].from_y);
            // 假设到第一个点，计算出left_path，递归计算后面的路径
            struct Rider left_rider = rider;
            left_rider.x = dist.left_x;
            left_rider.y = dist.left_y;
            left_rider.orders[i].status = 2;
            for (auto& order: left_rider.orders)
            {
                order.time -= dist.dis;
            }
            left_path = shortest_go(left_rider);


            struct Rider mid_rider = rider;
            mid_rider.x = dist.mid_x;
            mid_rider.y = dist.mid_y;
            mid_rider.orders[i].status = 2;
            for (auto& order: mid_rider.orders)
            {
                order.time -= dist.dis;
            }
            mid_path = shortest_go(mid_rider);
            // 假设到第二个点，计算出right_path
            struct Rider right_rider = rider;
            right_rider.x = dist.right_x;
            right_rider.y = dist.right_y;
            right_rider.orders[i].status = 2;
            for (auto& order: right_rider.orders)
            {
                order.time -= dist.dis;
            }
            right_path = shortest_go(right_rider);
        }
        // 订单未送，和上面的计算方式一样
        else if (order_status == 2)
        {
            dist = distance(rider.x, rider.y, rider.orders[ i].to_x, rider.orders[ i].to_y);
            struct Rider left_rider = rider;
            left_rider.x = dist.left_x;
            left_rider.y = dist.left_y;
            left_rider.orders[i].status = 3;
            for (auto& order: left_rider.orders)
            {
                order.time -= dist.dis;
            }

            left_path = shortest_go(left_rider);

            struct Rider mid_rider = rider;
            mid_rider.x = dist.mid_x;
            mid_rider.y = dist.mid_y;
            mid_rider.orders[i].status = 3;
            for (auto& order: mid_rider.orders)
            {
                order.time -= dist.dis;
            }
            mid_path = shortest_go(mid_rider);
            struct Rider right_rider = rider;
            right_rider.x = dist.right_x;
            right_rider.y = dist.right_y;
            right_rider.orders[i].status = 3;
            for (auto& order: right_rider.orders)
            {
                order.time -= dist.dis;
            }
            right_path = shortest_go(right_rider);
        }
        if (order_status == 1 || order_status == 2)
        {
            // 不管向左还是向右都超时了，就返回超时
            if(left_path.over && right_path.over && mid_path.over)
            {
                return left_path;
            }

            // 三条路径都没超时，比较那个短就返回哪个
            else
            {
                if (result.dis > left_path.dis + dist.dis)
                {
                    result.dis = left_path.dis + dist.dis;
                    result.next_x = dist.left_x;
                    result.next_y = dist.left_y;
                    result.timeout = left_path.timeout;
                    result.over = left_path.over;
                }

                if (result.dis > right_path.dis + dist.dis)
                {
                    result.dis = right_path.dis + dist.dis;
                    result.next_x = dist.right_x;
                    result.next_y = dist.right_y;
                    result.timeout = right_path.timeout;
                    result.over = right_path.over;
                }

                if (result.dis > mid_path.dis + dist.dis)
                {
                    result.dis = mid_path.dis + dist.dis;
                    result.next_x = dist.mid_x;
                    result.next_y = dist.mid_y;
                    result.timeout = mid_path.timeout;
                    result.over = mid_path.over;
                }


            }
        }
    }
    // 如果没有未取或者未送的订单，就返回0，去往初始x, y
    if (result.dis == NO_ORDER)
    {
        result.dis = 0;
        result.next_x = START_X;
        result.next_y = START_Y;
    }
    return result;
}

OrderDispatch::OrderDispatch(FILE *p_in, FILE *p_out)
{
    time = 0;
    money = START_MONEY;
    order_count = 0;
    this->p_in = p_in;
    this->p_out = p_out;
    add_rider();
}

Rider *OrderDispatch::add_rider()
{
    // 金钱减少
    money -= RIDER_PRICE;
    struct Rider r;
    // 骑手初始位置
    r.x = START_X;
    r.y = START_Y;
    r.last_path.next_x = r.x;
    r.last_path.next_y = r.y;
    r.last_path.over = 0;
    r.last_path.timeout = 0;
    r.last_path.dis = 0;
    r.last_path_flag = 0;
    riders.push_back(r);
    return &riders[riders.size() - 1];
}

void OrderDispatch::add_order(const Order o)
{
    struct Rider * ch_rider = nullptr;
    int cost = INT_MAX;
    struct PathData path;
    path.timeout = 0;
    for (auto& rider: riders)
    {
        struct PathData bf_path;
        // 计算未添加该订单时，该骑手送完所需的路程
        if (rider.last_path_flag)
        {
            bf_path = rider.last_path;
        }
        else
        {
            bf_path = shortest_go(rider);
            rider.last_path = bf_path;
            rider.last_path_flag = 1;
            if (bf_path.over)
            {
                continue;
            }
        }
        Rider rider_cpy = rider;
        rider_cpy.orders = rider.orders;
        rider_cpy.orders.push_back(o);
        // 假设加给该骑手，所需路程
        const struct PathData af_path = shortest_go(rider_cpy);
        // 计算增加后比增加前多的路程
        const int c = af_path.dis - bf_path.dis;

        // 找出最小的那个骑手
        if (c < cost)
        {
            path = af_path;
            cost = c;
            ch_rider = &rider;
        }
    }
    // 如果最小的那个骑手都超时了，就买骑手
    if (ch_rider == nullptr || path.timeout == 1)
    {
        if (money >= RIDER_PRICE)
        {
            ch_rider = add_rider();
        }
        // 如果没骑手可以送，就给第一个……（反正都要破产了）
        else if(ch_rider == nullptr)
        {
            ch_rider = &riders[0];
        }
    }
    ch_rider->orders.push_back(o);
    ch_rider->last_path_flag = 0;
    ++order_count;
}


void OrderDispatch::display()
{
    fprintf(stdout, AT("时间：%d\n"), time);
    fprintf(stdout, AT("钱：%d\n"), money);
    fprintf(stdout, AT("接单数：%d\n"), order_count);
    fprintf(stdout, AT("完成数：%d; 结单："), finished_orders.size());

    for (auto & rider: riders)
    {
        for (auto order: rider.stop_customer_order)
        {
            fprintf(stdout, "%d ", order.id);
        }
    }

    fprintf(stdout, "\n");

    int timeout_count = 0;
    for (auto order: finished_orders)
    {
        if (order.time < DIE_TIME - WARN_TIME)
        {
            ++timeout_count;
        }
    }
    fprintf(stdout, AT("超时数：%d; 罚单："), timeout_count);

    for (auto rider:riders)
    {
        for (auto order: rider.stop_customer_order)
        {
            if (order.time < DIE_TIME - WARN_TIME)
            {
                fprintf(stdout, "%d ", order.id);
            }
        }
    }

    fprintf(stdout, "\n");

    for (size_t i = 0; i < riders.size(); ++i)
    {
        const Rider rider = riders[i];
        fprintf(stdout, AT("骑手%d位置：%d,%d; 停靠："), i, rider.x, rider.y);

        for (auto order: rider.stop_merchant_order)
        {
            fprintf(stdout, AT("餐馆：%d %d;"), order.from_x, order.from_y);
        }
        for (auto order: rider.stop_customer_order)
        {
            fprintf(stdout, AT("食客：%d %d;"), order.to_x, order.to_y);
        }
        fprintf(stdout, "\n");
    }
}

void OrderDispatch::output()
{
    fprintf(p_out, AT("时间：%d\n"), time);
    fprintf(p_out, AT("钱：%d\n"), money);
    fprintf(p_out, AT("接单数：%d\n"), order_count);
    fprintf(p_out, AT("完成数：%d\n"/*"; 结单："*/), finished_orders.size());

//    for (auto & rider: riders)
//    {
//        for (auto order: rider.stop_customer_order)
//        {
//            fprintf(p_out, "%d ", order.id);
//        }
//    }

//    fprintf(p_out, "\n");

    int timeout_count = 0;
    for (auto order: finished_orders)
    {
        if (order.time < DIE_TIME - WARN_TIME)
        {
            ++timeout_count;
        }
    }
    fprintf(p_out, AT("超时数：%d\n" /*"; 罚单："*/), timeout_count);

//    for (auto rider:riders)
//    {
//        for (auto order: rider.stop_customer_order)
//        {
//            if (order.time < DIE_TIME - WARN_TIME)
//            {
//                fprintf(p_out, "%d ", order.id);
//            }
//        }
//    }

//    fprintf(p_out, "\n");

    for (size_t i = 0; i < riders.size(); ++i)
    {
        const Rider rider = riders[i];
        fprintf(p_out, AT("骑手%d位置：%d,%d\n"/*"; 停靠："*/), i, rider.x, rider.y);

//        for (auto order: rider.stop_merchant_order)
//        {
//            fprintf(p_out, AT("餐馆：%d %d;"), order.from_x, order.from_y);
//        }
//        for (auto order: rider.stop_customer_order)
//        {
//            fprintf(p_out, AT("食客：%d %d;"), order.to_x, order.to_y);
//        }
//        fprintf(p_out, "\n");
    }
    display();
}

int OrderDispatch::run()
{
    time += CELL_TIME;
    // _sleep(CELL_TIME * 1000);
    for (auto &rider: riders)
    {
        rider.stop_customer_order.clear();
        rider.stop_merchant_order.clear();
        struct PathData path;
        // 计算一下最优方案的下一个坐标
        if (rider.last_path_flag)
        {
            path = rider.last_path;
            if(rider.last_path.dis>0)
                --rider.last_path.dis;
        }
        else
        {
            path = shortest_go(rider);
            if(path.over)
            {
                return 1;
            }
            rider.last_path = path;
            if(rider.last_path.dis>0)
                --rider.last_path.dis;
            rider.last_path_flag = 1;
        }
        // 朝那个坐标走两步
        if (abs(rider.x - path.next_x) < 2 && rider.y < path.next_y && rider.x%2==1)
            rider.y += 2;
        else if ( abs(rider.x - path.next_x) < 2 && rider.y > path.next_y && rider.x%2==1)
            rider.y -= 2;
        else if (rider.x < path.next_x && abs(rider.y - path.next_y) < 2 && rider.y%2==1)
            rider.x += 2;
        else if (rider.x > path.next_x && abs(rider.y - path.next_y) < 2 && rider.y%2==1)
            rider.x -= 2;


        else if (rider.x < path.next_x && rider.y < path.next_y) {
            rider.x += 1;
            rider.y += 1;
        }
        else if (rider.x < path.next_x && rider.y > path.next_y)
        {
            rider.x += 1;
            rider.y -= 1;
        }
        else if (rider.x > path.next_x && rider.y < path.next_y)
        {
            rider.x -= 1;
            rider.y += 1;
        }
        else if (rider.x > path.next_x && rider.y > path.next_y)
        {
            rider.x -= 1;
            rider.y -= 1;
        }


        else if (abs(rider.x - path.next_x) < 2 && rider.y < path.next_y)
        {
            rider.y += 1;
            rider.x -= rider.x == 0? -1:1;
        }
        else if (abs(rider.x - path.next_x) < 2 && rider.y > path.next_y)
        {
            rider.y -= 1;
            rider.x -= rider.x == 0? -1:1;
        }
        else if (rider.x < path.next_x &&  abs(rider.y - path.next_y) < 2)
        {
            rider.x += 1;
            rider.y -= rider.y == 0? -1:1;
        }
        else if (rider.x > path.next_x &&  abs(rider.y - path.next_y) < 2)
        {
            rider.x -= 1;
            rider.y -= rider.y == 0? -1:1;
        }




        // 更新订单的数据
        for (auto &order:rider.orders)
        {
            --order.time;
            if(order.time < 0)
            {
                finished_orders.push_back(order);
                return 1;
            }
            if (order.status == 1)
            {
                if (abs(rider.x - order.from_x) + abs(rider.y - order.from_y) == 1)
                {
                    rider.stop_merchant_order.push_back(order);
                    order.status = 2;
                    rider.last_path_flag = 0;
                }
            }
            if (order.status == 2)
            {
                if (abs(rider.x - order.to_x) + abs(rider.y - order.to_y) == 1)
                {
                    order.status = 3;
                    rider.last_path_flag = 0;
                    rider.stop_customer_order.push_back(order);
                    finished_orders.push_back(order);
                    // fprintf(p_out, "finish size: %d order time: %d\n",finished_orders.size,  order.time);
                    if (order.time < DIE_TIME - WARN_TIME)
                    {
                        money -= LOST_MONEY;
                    }
                    else
                    {
                        money += EARN_MONEY;
                    }
                }
            }
        }

        rider.orders.erase(std::remove_if(rider.orders.begin(), rider.orders.end(), [](auto&p)             {
            return p.status == 3;
        }), rider.orders.end());
    }
    // 看看破产了没……
    if (money < 0)
    {
        return 2;
    }
    output();
    return 0;
}

void OrderDispatch::start_dispatch()
{
    while (1)
    {

        struct Order o;
        o.time = DIE_TIME;
        o.status = 1;
        int tm;
        if (fscanf(p_in, "%d%d%d%d%d%d", &o.id, &tm, &o.from_x, &o.from_y, &o.to_x, &o.to_y) != 6)  // NOLINT(cert-err34-c)
        {
            fclose(p_in);
            break;
        }
        while (time + CELL_TIME <= tm)
        {
            const int ret = run();
            if (ret == 1)
            {
                printf(AT("吊销执照"));
                return;
            }
            if (ret == 2)
            {
                printf(AT("破产"));
                return;
            }
        }
        add_order(o);
    }
    while (1)
    {

        int is_finished = 1;

        for (auto &rider:riders)
        {
            if (rider.orders.size() != 0)
            {
                is_finished = 0;
            }
        }
        if (is_finished)
        {
            printf(AT("完成所有订单"));
            break;
        }
        const int ret = run();
        if (ret == 1)
        {
            printf(AT("吊销执照"));
            return;
        }
        if(ret == 2)
        {
            printf(AT("破产"));
            return;
        }
    }
}
