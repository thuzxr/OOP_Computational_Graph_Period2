#ifndef COMPUTATIONAL_GRAPH_EXPCNODE_H
#define COMPUTATIONAL_GRAPH_EXPCNODE_H

#include"../lib/calcnode.h"

template<typename _T>
class ExpCNode : public CalcNode<_T>
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
double ExpCNode<double>::Calc()
{
    Result = new double(exp(Operands[0]->GetVal()));
    return *Result;
}

template<>
double ExpCNode<double>::DerCalc(Node <double> *operand)
{
    double der = (this == operand) ? 1.0 : exp(Operands[0]->GetVal()) * Operands[0]->GetDer(operand);
    DerResult = new double(der);
    return *Result;
}

#endif //COMPUTATIONAL_GRAPH_EXPCNODE_H

