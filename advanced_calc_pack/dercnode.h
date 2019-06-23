#ifndef COMPUTATIONAL_GRAPH_DERCNODE_H
#define COMPUTATIONAL_GRAPH_DERCNODE_H

#include"../lib/calcnode.h"

template<typename _T>
class DerCNode : public CalcNode<_T>
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

template<typename _T>
_T DerCNode<_T>::DerCalc(Node <_T> *operand)
{
    throw "ERROR: Cannot derivate with the derivative";
}

template<>
double DerCNode<double>::Calc()
{
    double der = Operands[0]->GetDer(Operands[1]);
    Result = new double(der);
    return *Result;
}

template<>
Tensor DerCNode<Tensor>::Calc()
{
    Tensor der = Operands[0]->GetDer(Operands[1]);
    Result = new Tensor(der);
    return *Result;
}

#endif //COMPUTATIONAL_GRAPH_DERCNODE_H