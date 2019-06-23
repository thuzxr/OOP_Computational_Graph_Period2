#ifndef COMPUTATIONAL_GRAPH_ASSERTCNODE_H
#define COMPUTATIONAL_GRAPH_ASSERTCNODE_H

#include "../lib/calcnode.h"

template <typename _T>
class AssertCNode : public CalcNode<_T>
{
private:
    static const std::string ErrMsg; //需要输出的错误信息，设定为静态
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

    void Clear();
};

template <typename _T>
const std::string AssertCNode<_T>::ErrMsg = "ERROR: Assertion failed";

template <typename _T>
const std::string AssertCNode<_T>::DerErrMsg = "ERROR: Cannot derivate with ASSERT";

template <typename _T>
_T AssertCNode<_T>::Calc()
{
    Result = new _T(0);
    if(Operands[0]->GetVal() <= 0.)
        throw ErrMsg;
    return *Result;
}

template<typename _T>
_T AssertCNode<_T>::DerCalc(Node<_T>* operand)
{
    throw DerErrMsg;
}

template<typename _T>
void AssertCNode<_T>::Clear()
{
    for (int i = 0; i < OperandNum; ++i)
        Operands[i]->Clear();
    Node<_T>::Clear();
}

#endif