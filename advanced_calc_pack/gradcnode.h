#ifndef COMPUTATIONAL_GRAPH_GRADCNODE_H
#define COMPUTATIONAL_GRAPH_GRADCNODE_H

#include"../lib/calcnode.h"

template<typename _T>
class GradCNode : public CalcNode<_T>
{
private:
	static const std::string ErrMsg; //需要输出的错误信息，设定为静态
protected:
    _T Calc();
    _T DerCalc(Node <_T> *operand);
public:
    using CalcNode<_T>::Result;
	using CalcNode<_T>::DerResult;
    using CalcNode<_T>::OperandNum;
    using CalcNode<_T>::Operands;
    using CalcNode<_T>::CalcNode;
    using CalcNode<_T>::DerCalc;
};

template<typename _T>
const std::string GradCNode<_T>::ErrMsg = "ERROR: Cannot derivate with GRAD";

template<typename _T>
_T GradCNode<_T>::Calc()
{
    throw ErrMsg;
}

template<>
double GradCNode<double>::DerCalc(Node <double> *operand)
{
    double der = Operands[0]->GetDer(operand);
	DerResult = new double(der);
    return *DerResult;
}

template<>
Tensor GradCNode<Tensor>::DerCalc(Node <Tensor> *operand)
{
    Tensor der = Operands[0]->GetDer(operand);
	DerResult = new Tensor(der);
    return *DerResult;
}


#endif //COMPUTATIONAL_GRAPH_GRADCNODE_H