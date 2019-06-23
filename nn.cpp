#include <iostream>
#include <sstream>
#include "lib/comgraph.h"
#include "basic_calc_pack/basic_calc_pack.h"
#include "advanced_calc_pack/advanced_calc_pack.h"
#include "compare_calc_pack/compare_calc_pack.h"
#include "lib/tensor.h"
#include <fstream>
#include <ctime>

typedef string string;
using namespace std;
ostream &ErrOut = cout; //应项目要求，错误信息向cout输出
ostream &PriOut = cout; //应项目要求，PriNode信息向cout输出
ostream &AnsOut = cout; //答案输出至cout

//默认只支持Tensor版本，double类型则需要转化为1维Tensor
//测试样例说明：
//节点x：类型Placeholder，维度为10*1的Tensor
//节点W：类型Variable，维度为5*10的Tensor
//节点b：类型Variable，维度为5*1的Tensor
//节点y：类型Placeholder，维度5*1的Tensor
//节点loss：类型为MSELoss，target为5*1的Tensor（相当于label）
//优化任务：Wx+b与target的交叉熵损失

const int iter_num = 400;
const double infinity = 1e9;
const int sample_num = 100;
const int label_num = 10;


int main() {
    
	ComGraph<Tensor> neural_network(ErrOut, PriOut); 
    auto W = get_normal_tensor(Shape({label_num, sample_num})), b = get_normal_tensor(Shape({label_num, 1}));
	//神经网络的建立-------BEGIN-------
    neural_network.BuildPHNode("x");
	neural_network.BuildVarNode("W", W);
    neural_network.BuildVarNode("b", b);
    neural_network.BuildVarNode("y");
    neural_network.BuildCalcNode<MulCNode<Tensor>>("mul", 2, vector<string>({"W", "x"}));
    neural_network.BuildCalcNode<PluCNode<Tensor>>("plu", 2, vector<string>({"mul", "b"}));
    neural_network.BuildCalcNode<MSELoss<Tensor>>("loss", 2, vector<string>({"plu", "y"}));
    neural_network.BuildCalcNode<GradCNode<Tensor>>("grad", 1, vector<string>({"loss"}));
    neural_network.BuildCalcNode<DerCNode<Tensor>>("dW", 2, vector<string>({"grad", "W"}));
    neural_network.BuildCalcNode<DerCNode<Tensor>>("db", 2, vector<string>({"grad", "b"}));
    fprintf(stderr, "PROCESS: Construction of the neural network completed.\n");
    //神经网络的建立-------END-------
    //优化过程开始--------BEGIN------
    auto elem_ = Elem({
        0.336, 0.977, 0.992, 0.992, 0.992, 0.992, 0.848, 0.961, 0.590, 0.125, 
        0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 
        0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.062, 0.699, 0.992, 0.992, 
        0.992, 0.992, 0.992, 0.992, 0.992, 0.992, 0.992, 0.902, 0.211, 0.059, 
        0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 
        0.000, 0.000, 0.000, 0.000, 0.281, 0.992, 0.992, 0.992, 0.992, 0.992, 
        0.992, 0.992, 0.992, 0.992, 0.992, 0.992, 0.992, 0.406, 0.000, 0.000, 
        0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 
        0.000, 0.238, 0.746, 0.992, 0.992, 0.992, 0.992, 0.992, 0.426, 0.324, 
        0.777, 0.992, 0.992, 0.992, 0.992, 0.949, 0.332, 0.000, 0.000, 0.000});
    auto sample_x = Tensor(Shape({sample_num, 1}), elem_);
    auto label_y = Tensor(Shape({1}), 3).onehot(label_num);
    vector<pair<string, Tensor>> PHList;
    PHList.push_back(make_pair("x", sample_x)); PHList.push_back(make_pair("y", label_y));
    auto Res = Tensor({Shape({1}), infinity});
    auto W_best = Tensor(Shape({label_num, sample_num}), 0.), b_best = Tensor(Shape({label_num, 1}), 0.);
    auto min_loss = infinity;
    clock_t start = clock();
    auto W_vel = Tensor(Shape({label_num, sample_num}), 0.), b_vel = Tensor(Shape({label_num, 1}), 0.);
    auto init_W_sqr = Tensor(Shape({label_num, sample_num}), 0.), init_b_sqr = Tensor(Shape({label_num, 1}), 0.);
    auto loss = 0.;
    auto tot_W_grad = Tensor(Shape({label_num, sample_num}), 0.), tot_b_grad = Tensor(Shape({label_num, 1}), 0.);
    for (int iter = 1; iter <= iter_num; iter++) {
        fprintf(stderr, "\r#iter %d: progress: %2.0f%%", iter, 100. * iter / iter_num);
        bool failed = false;
        try {
            Res = neural_network.Eval("loss", PHList);
        }
        catch (string &ErrMsg) {
            ErrOut << ErrMsg << endl;
            failed = true;
        }
        if (!failed) {
            loss = Res.elem(0);
            if (min_loss > loss) {
                W_best = neural_network.Eval("W", PHList);
                b_best = neural_network.Eval("b", PHList);
                min_loss = loss;
            }
        }
        try {
            /*
            //基础版本的梯度下降 (Gradient Descend)
            neural_network.GradientDescend("dW", "W", PHList);
            neural_network.GradientDescend("db", "b", PHList);
            */
                
                
            //带有动量的梯度下降 (Momentum Gradient Descend)，需要使用69行
            neural_network.MomGradientDescend("dW", "W", PHList, W_vel, 0.8);
            neural_network.MomGradientDescend("db", "b", PHList, b_vel, 0.8);
                
                
            /*
            //RMSprop（参考keras)，需要使用70行
            neural_network.RMSprop("dW", "W", PHList, init_W_sqr, 0.8);
            neural_network.RMSprop("db", "b", PHList, init_b_sqr, 0.8);
            */

           /*
           //Adam（参考keras)
           neural_network.Adam("dW", "W", PHList, 10);
           neural_network.Adam("db", "b", PHList, 10);
           */
        }
        catch (string &ErrMsg) {
            ErrOut << ErrMsg << endl;
        }
    }
    clock_t end = clock();
    AnsOut << "\rPROCESS: Training of neural network completed. time: " << 1. * (end - start) / CLOCKS_PER_SEC << endl;
    AnsOut << "# total iter: " << iter_num << ", minimal loss: " << min_loss << endl;
    AnsOut << "W =\n" << W_best << "\nb =\n" << b_best;
    //神经网络的优化------END------
    return 0;
}

