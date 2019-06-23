#ifndef MSE_LOSS_FUNCTION
#define MSE_LOSS_FUNCTION
#include "../lib/calcnode.h"

template<typename _T>
class MSELoss : public CalcNode<_T>{
private:
    static const std::string DimErrMsg;

protected:
    _T Calc(); //重载Calc，在这里进行计算
	_T DerCalc(Node <Tensor> *operand);
public:
    using CalcNode<_T>::Result;
    using CalcNode<_T>::DerResult;
    using CalcNode<_T>::OperandNum;
    using CalcNode<_T>::Operands;
    using CalcNode<_T>::CalcNode;
    //输入矩阵维度为ans(dim), target(dim)，输出维度为1
};

template <typename _T>
const std::string MSELoss<_T>::DimErrMsg = "Dimension Mismatched.";

template<>
Tensor MSELoss<Tensor>::Calc(){
    Tensor ans = Operands[0]->GetVal();
    Tensor target = Operands[1]->GetVal();
    if(ans.shape_size(0) != target.shape_size(0)) {//目前只判断第一维度是否相同，默认其余分量是1
        throw DimErrMsg;
    }
    Tensor output(Shape({1}), 0.0f);
    for(int i = 0; i < ans.shape_size(0); i++){
        double a = ans.elem(i), b = target.elem(i);
        output.elem(0) += (a-b) * (a-b);
    }
    output.elem(0) /= ans.shape_size(0);
    Result = new Tensor(output);
    return *Result;
}

template<>
Tensor MSELoss<Tensor>::DerCalc(Node <Tensor> *operand){
    Tensor der;
    if(this == operand)
        der = Tensor(Shape({1,1}), 1.0);
    else{
        der = Operands[0]->GetVal();
        Tensor target = Operands[1]->GetVal();
        if(der.shape_size(0) != target.shape_size(0)) //目前只判断第一维度是否相同，默认其余分量是1
            throw DimErrMsg;
        der = der - target;
        der = der * ( 2.0 / double(der.shape_size(0)) );
        der = der.transpose() * Operands[0]->GetDer(operand); //此处是否有bug？
    }
    DerResult = new Tensor(der);
    return *DerResult;
}
#endif