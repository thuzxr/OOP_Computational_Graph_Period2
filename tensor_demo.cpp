#include "lib/tensor.h"
#include <iostream>
#include <vector>

using namespace std;

void gap() {
    int i = 0;
    while (i++ < 20) cout << "-";
    cout << endl;
}

int main () {
    auto elem_1 = Elem(4), elem_2 = Elem(4), elem_3 = Elem(8);
    for (int i = 0; i < 4; i++) elem_1[i] = i + 1;
    for (int i = 0; i < 4; i++) elem_2[i] = 2 * i + 1;
    for (int i = 0; i < 8; i++) elem_3[i] = i;
    auto t_1 = Tensor(elem_1), t_2 = Tensor(elem_2), t_3 = Tensor(elem_3);
    
    cout << "输出其shape和整个Tensor\n";
    t_1.shape_display();
    cout << t_1;
    cout << "想reshape()为(1, 2)\n";
    try { t_1.reshape(Shape({1, 2})); }
    catch (string &ErrMsg) { cout << ErrMsg << endl; }
    gap();
    cout << "经过reshape后\n";
    t_1.reshape(Shape({2, 2}));
    t_1.shape_display();
    cout << t_1;
    gap();
    cout << "二维Tensor的转置后\n";
    t_1.transpose();
    t_1.shape_display();
    cout << t_1;
    gap();

    cout << "broadcast的加减乘除\n";
    t_1.reshape(Shape({4, 1}));
    t_2.reshape(Shape({1, 4}));
    cout << "t_1\n" << t_1 << "t_2\n" << t_2;
    gap();
    cout << "t_1 + t_2\n";
    cout << t_1 + t_2;
    gap();
    cout << "t_1 - t_2\n";
    cout << t_1 - t_2;
    gap();
    cout << "t_1 * t_2\n";
    cout << t_1.broadcast_mul(t_2);
    gap();
    cout << "t_1 / t_2\n";
    cout << t_1.broadcast_div(t_2);
    gap();
    cout << "想让(4, 1)和(2, 2)broadcast\n";
    t_2.reshape(Shape({2, 2}));
    t_1.shape_display(); t_2.shape_display();
    try { t_1 + t_2; }
    catch (string &ErrMsg) { cout << ErrMsg << endl; }
    t_2.reshape(Shape({1, 4}));

    cout << "矩阵乘法\n";
    cout << t_1 * t_2;
    gap();
    cout << "必须是二维\n";
    t_1.reshape(Shape({4}));
    t_1.shape_display(); t_2.shape_display();
    try { t_1 * t_2; }
    catch (string &ErrMsg) { cout << ErrMsg << endl; }
    gap();
    cout << "且相容\n";
    t_1.reshape(Shape({1, 4}));
    t_1.shape_display(); t_2.shape_display();
    try { t_1 * t_2; }
    catch (string &ErrMsg) { cout << ErrMsg << endl; }
    gap();
    t_1.transpose();

    cout << "其他神经网络常用函数\n";
    cout << "before relu:\n" << t_1 - t_2 << "after relu:\n" << (t_1 - t_2).relu();
    gap();
    cout << "before softmax:\n" << t_1 << "after softmax:\n" << t_1.softmax();
    gap();
    cout << "before sqrt:\n" << t_1 << "after sqrt:\n" << t_1.sqrt();
    gap();
    cout << "before argmax:\n" << t_1 << "after argmax:\n" << t_1.argmax() << endl;
    gap();
    cout << "before norm:\n" << t_1 - t_2 << "after norm:\n" << (t_1 - t_2).norm() << endl;
    gap();

    cout << "concat()\n";
    t_1.reshape(Shape({4, 1, 1}));
    t_3.reshape(Shape({4, 1, 2}));
    cout << "t_1:\n" << t_1 << "t_3:\n" << t_3 << "after concat at dim = 2\n" << t_1.concat(t_3, 2);
    gap();
    t_1.reshape(Shape({1, 4}));
    t_3.reshape(Shape({2, 4}));
    cout << "t_1:\n" << t_1 << "t_3:\n" << t_3 << "after concat at dim = 0\n" << t_1.concat(t_3, 0);
    gap();
    cout << "除去concat的维度外其余维度必须完全相同\n";
    t_1.transpose();
    t_1.shape_display(); t_3.shape_display();
    cout << "concat at dim = 0\n";
    try { t_1.concat(t_2, 0); }
    catch (string &ErrMsg) { cout << ErrMsg << endl; }
    gap();

    cout << "reduce_sum() & reduce_mul()\n";
    t_3.reshape(Shape({4, 1, 2}));
    cout << "before reduce:\n" << t_3;
    cout << "after reduce_sum at dim = 2\n" << t_3.reduce_sum(2);
    cout << "after reduce_mul at dim = 0\n" << t_3.reduce_mul(0);
    gap();

    return 0;
}