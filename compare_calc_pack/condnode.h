#ifndef COMPUTATIONAL_GRAPH_CONDNODE_H
#define COMPUTATIONAL_GRAPH_CONDNODE_H

#include "../lib/calcnode.h"

template<typename _T>
class CondNode : public CalcNode<_T>
{
private:
	static const std::string ErrMsg; //需要输出的错误信息，设定为静态
protected:
    _T Calc(); //重载Calc
    _T DerCalc(Node <_T> *operand);
public:
    using CalcNode<_T>::Result;
    using CalcNode<_T>::DerResult;
    using CalcNode<_T>::OperandNum;
    using CalcNode<_T>::Operands;
    using CalcNode<_T>::CalcNode;
};

template<typename _T>
const std::string CondNode<_T>::ErrMsg = "ERROR: Cannot derivate with COND";

template<>
double CondNode<double>::Calc()
{
    double Cond = Operands[0]->GetVal();
    double Res1 = Operands[1]->GetVal();
    double Res2 = Operands[2]->GetVal();
    //这样可以检查未被选择的操作元计算可能出现的错误
    Result = new double(Cond > 0 ? Res1 : Res2);
    return *Result;
}

template<typename _T>
_T CondNode<_T>::DerCalc(Node <_T> *operand)
{
    throw ErrMsg;
}

#endif //COMPUTATIONAL_GRAPH_CONDNODE_H
