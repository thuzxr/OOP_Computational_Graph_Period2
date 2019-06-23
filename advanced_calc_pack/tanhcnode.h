#ifndef COMPUTATIONAL_GRAPH_TANHCNODE_H
#define COMPUTATIONAL_GRAPH_TANHCNODE_H

#include"../lib/calcnode.h"

template<typename _T>
class TanhCNode : public CalcNode<_T>
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
double TanhCNode<double>::Calc()
{
    Result = new double(tanh(Operands[0]->GetVal()));
    return *Result;
}

template<>
double TanhCNode<double>::DerCalc(Node <double> *operand)
{
	double der;
	if (this == operand) der = 1.0;
	else {
		double Res0 = 4 * Operands[0]->GetDer(operand);
		double Res1 = 2 + cosh(2 * Operands[0]->GetVal());
		der = (Res0 / Res1);
	}
	DerResult = new double(der);
    return *DerResult;
}

#endif //COMPUTATIONAL_GRAPH_TANHCNODE_H
