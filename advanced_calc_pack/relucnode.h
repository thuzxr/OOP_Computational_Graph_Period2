#ifndef COMPUTATIONAL_GRAPH_RELUCNODE_H
#define COMPUTATIONAL_GRAPH_RELUCNODE_H

#include"../lib/calcnode.h"

template<typename _T>
class ReluCNode : public CalcNode<_T>
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
Tensor ReluCNode<Tensor>::Calc()
{
    Result = new Tensor(Operands[0]->GetVal().relu());
    return *Result;
}

template<>
Tensor ReluCNode<Tensor>::DerCalc(Node <Tensor> *operand)
{   
    auto der = Operands[0]->GetVal().der_relu() * Operands[0]->GetDer(operand);
    DerResult = new Tensor(der);
    return *DerResult;
}

#endif //COMPUTATIONAL_GRAPH_PLUCNODE_H
