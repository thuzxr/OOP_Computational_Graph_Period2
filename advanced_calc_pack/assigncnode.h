#ifndef COMPUTATIONAL_GRAPH_ASSIGNCNODE_H
#define COMPUTATIONAL_GRAPH_ASSIGNCNODE_H

#include "../lib/calcnode.h"
#include "../lib/varnode.h"

using std::pair;
using std::make_pair;
//由于Assign延迟赋值机制的存在，每次eval后需要执行AssignWaitingList::Clear()操作来赋值

template <typename _T>
class AssignWaitingList{
public:
    static std::vector<pair<VarNode<_T>*, _T> > WaitingList;

    static void Clear(){
        for(auto x : WaitingList){
            x.first->SetVal(x.second);
        }
        WaitingList.clear();
    }

    static void Insert(VarNode<_T>* op1, const _T &op2){
        WaitingList.push_back(make_pair(op1, op2));
    }
};

template<typename _T>
std::vector<pair<VarNode<_T>*, _T> > AssignWaitingList<_T>::WaitingList(0);

template <typename _T>
class AssignCNode : public CalcNode<_T>
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
const std::string AssignCNode<_T>::DerErrMsg = "ERROR: Cannot derivate with ASSIGN";

template<typename _T>
_T AssignCNode<_T>::Calc()
{
    Result = new _T(Operands[1]->GetVal());
    AssignWaitingList<_T>::Insert(dynamic_cast<VarNode<_T> *>(Operands[0]), *Result);
    return *Result;
}

template<typename _T>
_T AssignCNode<_T>::DerCalc(Node<_T>* operand)
{
    throw DerErrMsg;
}

#endif
