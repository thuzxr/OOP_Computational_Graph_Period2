#ifndef COMPUTATIONAL_GRAPH_SIGCNODE_H
#define COMPUTATIONAL_GRAPH_SIGCNODE_H

#include"../lib/calcnode.h"

template<typename _T>
class SigCNode : public CalcNode<_T>
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
double SigCNode<double>::Calc()
{
    Result = new double(1 / (1 + exp((-1)*Operands[0]->GetVal())));
    return *Result;
}

template<>
double SigCNode<double>::DerCalc(Node <double> *operand)
{
	double der = 1.0;
	if (this == operand) DerResult = new double(der);
	else {
		double Res0 = Operands[0]->GetDer(operand);
		double Res1 = 2 + cosh(Operands[0]->GetVal());
		DerResult = new double(Res0 / Res1);
	}
    return *DerResult;
}

#endif //COMPUTATIONAL_GRAPH_SIGCNODE_H

