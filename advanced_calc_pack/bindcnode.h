#ifndef COMPUTATIONAL_GRAPH_BINDCNODE_H
#define COMPUTATIONAL_GRAPH_BINDCNODE_H

#include "../lib/calcnode.h"

template <typename _T>
class BindCNode : public CalcNode<_T>
{
private:
    static const std::string DerErrMsg; //需要输出的错误信息，设定为静态
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
const std::string BindCNode<_T>::DerErrMsg = "ERROR: Cannot derivate with BIND";

template<typename _T>
_T BindCNode<_T>::Calc()
{
    Operands[1]->GetVal();
    Result = new _T(Operands[0]->GetVal());
    return *Result;
}

template<typename _T>
_T BindCNode<_T>::DerCalc(Node<_T>* operand)
{
    DerResult = new _T(Operands[0]->GetDer(operand));
//    Operands[1]->GetVal();
    return *DerResult;
}

#endif