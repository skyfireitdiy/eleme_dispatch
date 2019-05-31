#include <orderdispatch.h>
#include <QFile>
#include <QDebug>

int main()
{
    FILE *p_in = nullptr;
    FILE *p_out = fopen("output.txt", "w");
    if(QFile("sales.txt").exists())
    {
        p_in = fopen("sales.txt", "r");
    }
    else
    {
        p_in = stdin;
    }
    OrderDispatch od(p_in, p_out);
    od.start_dispatch();
}
