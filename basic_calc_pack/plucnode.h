#ifndef COMPUTATIONAL_GRAPH_PLUCNODE_H
#define COMPUTATIONAL_GRAPH_PLUCNODE_H

#include"../lib/calcnode.h"

template<typename _T>
class PluCNode : public CalcNode<_T>
{
protected:
    _T Calc(); //重载Calc，在这里进行计算
    _T DerCalc(Node <_T> *operand);

public:
    using CalcNode<_T>::Result;
    using CalcNode<_T>::DerResult;
    using CalcNode<_T>::OperandNum;
    using CalcNode<_T>::Operands;
    using CalcNode<_T>::CalcNode;
};

template<>
double PluCNode<double>::Calc()
{
    Result = new double(Operands[0]->GetVal() + Operands[1]->GetVal());
    return *Result;
}

template<>
Tensor PluCNode<Tensor>::Calc()
{
    Result = new Tensor(Operands[0]->GetVal() + Operands[1]->GetVal());
    return *Result;
}

template<>
double PluCNode<double>::DerCalc(Node <double> *operand)
{
    double der = (this == operand) ? 1.0 : Operands[0]->GetDer(operand) + Operands[1]->GetDer(operand);
    DerResult = new double(der);
    return *DerResult;
}

template<>
Tensor PluCNode<Tensor>::DerCalc(Node <Tensor> *operand)
{
    int row_num = GetVal().size(), col_num = operand->GetVal().size();
    Tensor der;
    if (this != operand) der = Operands[0]->GetDer(operand) + Operands[1]->GetDer(operand);
    else {
        der = Tensor(Shape({row_num, col_num}), 0.0);
        for (int i = 0; i < row_num; i++)
			der.elem(Shape({i, i})) = 1.0;
    }
    DerResult = new Tensor(der);
    return *DerResult;
}

#endif //COMPUTATIONAL_GRAPH_PLUCNODE_H
