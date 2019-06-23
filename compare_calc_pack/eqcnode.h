#ifndef COMPUTATIONAL_GRAPH_EQCNODE_H
#define COMPUTATIONAL_GRAPH_EQCNODE_H

#include "../lib/calcnode.h"

template<typename _T>
class EQCNode : public CalcNode<_T> //比较运算符EQCNode
{
private:
	static const std::string ErrMsg; //需要输出的错误信息，设定为静态
protected:
    _T Calc(); //重载Calc，在这里进行计算
    _T DerCalc(Node <_T> *operand);
public:
    using CalcNode<_T>::Result;
    using CalcNode<_T>::DerResult;
    using CalcNode<_T>::OperandNum;
    using CalcNode<_T>::Operands;           //Using 基类的操作元
    using CalcNode<_T>::CalcNode;
};

template <typename _T>
const std::string EQCNode<_T>::ErrMsg = "ERROR: Cannot derivate with ==";

template<>
double EQCNode<double>::Calc()
{
    Result = new double(Operands[0]->GetVal() == Operands[1]->GetVal());
    return *Result;
}

template<typename _T>
_T EQCNode<_T>::DerCalc(Node <_T> *operand)
{
    throw ErrMsg;
}

#endif //COMPUTATIONAL_GRAPH_EQCNODE_H


