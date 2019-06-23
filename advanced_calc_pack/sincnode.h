#ifndef COMPUTATIONAL_GRAPH_SINCNODE_H
#define COMPUTATIONAL_GRAPH_SINCNODE_H

#include"../lib/calcnode.h"

template<typename _T>
class SinCNode : public CalcNode<_T>
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
double SinCNode<double>::Calc()
{
    Result = new double(sin(Operands[0]->GetVal()));
    return *Result;
}

template<>
double SinCNode<double>::DerCalc(Node <double> *operand)
{
    double der = (this == operand) ? 1.0 : Operands[0]->GetDer(operand) * cos(Operands[0]->GetVal());
    DerResult = new double(der);
    return *DerResult;
}

#endif //COMPUTATIONAL_GRAPH_SINCNODE_H
