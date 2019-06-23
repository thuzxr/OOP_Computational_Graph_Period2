#ifndef COMPUTATIONAL_GRAPH_COMGRAPH_H
#define COMPUTATIONAL_GRAPH_COMGRAPH_H

#include <vector>
#include <map>
#include "connode.h"
#include "phnode.h"
#include "prinode.h"
#include "varnode.h"
#include "calcnode.h"

#include "../advanced_calc_pack/dercnode.h"
#include "../advanced_calc_pack/assigncnode.h"

using std::vector;
using std::map;
using std::string;
using std::pair;

template<typename _T>
class ComGraph
{
private:
	map<string, Node<_T> *> Index;

	vector<Node<_T> *> NodeAddress;

	vector<_T> AnsHistory;

	std::ostream &ErrOut, &PriOut;

	Node<_T> *GetNode(string);    //获取指定关键字节点的节点地址，{不推荐==>不允许}在类外使用

	_T SetPHNodeVal(Node<_T> *, _T);

	void AddNode(string, Node<_T> *); //由于可能出现重名节点，因此在Index中覆盖时仍要记录被覆盖节点地址，所以新增一个vector

public:
	ComGraph() : ErrOut(std::cerr), PriOut(std::cout)
		//构造时载入错误信息和默认PriNode输出流，默认分别为cerr和cout
	{
		Index.clear();
	}

	ComGraph(std::ostream &_ErrO, std::ostream &_PrO) : ErrOut(_ErrO), PriOut(_PrO)
		//构造时载入错误信息和默认PriNode输出流，如果要自定义，就必须两个一起自定义
	{
		Index.clear();
	}

	bool FindNode(string); //检查图中是否有节点以指定名称命名

	_T SetVarVal(string, _T); //为变量节点赋值

	void ClearVarVal(string); //清除（暂时禁用）变量

	//template<typename _CN>
	//Node<_T> *BuildCalcNode(string, std::vector<Node<_T> *>); //不推荐，vector包含节点所有依赖节点地址

	template<typename _CACLN>
	Node<_T> *BuildCalcNode(string, std::vector<string>); //推荐，vector包含节点所有依赖节点名称
	template<typename _CALCN>
	Node<_T> *BuildCalcNode(string, int, std::vector<string>); //更推荐，指定操作元个数，vector包含节点所有依赖节点名称

	/*
	  这个地方出于可扩展性的考虑，没有直接把所有的feature放进来
	  想实现根据主程序的需要来建点，于是采用模板函数
	  具体用法BuildNode(节点关键字,所需建立的类型用例,包含节点所有依赖节点地址或名称的vector);
	  只能用来建立CalcNode的派生类节点
	*/

	_T Eval(string, vector<pair<string, _T> >);
	//计算答案，vector内用pair存储参数

	Node<_T> *BuildPHNode(string);

	Node<_T> *BuildConNode(string, _T);

	Node<_T> *BuildVarNode(string);

	Node<_T> *BuildVarNode(string, _T);

	Node<_T> *BuildPriNode(string, string);

	Node<_T> *BuildPriNode(string, string, std::ostream &);

	_T RecInHistory(_T);//记录某一次操作的答案

	_T ReadFromHistory(int);//读取某一次操作的答案

	void clear();

	~ComGraph()
	{
		clear();
	}

	void GradientDescend(string, string, vector<pair<string, _T> >, const double & = 1e-4); //先这么写着，没想好怎么传进去根节点的DerCNode和参数节点

	void MomGradientDescend(string, string, vector<pair<string, _T> >, _T &, const double &, const double & = 1e-4);

	void RMSprop(string, string, vector<pair<string, _T> >, _T &, const double &, const double & = 1e-4);

	void Adam(string, string, vector<pair<string, _T> >, const int &, const double & = 1e-4, const double & = 0.9, const double & = 0.999);
};
//一个安全性考虑，在类外除了节点建立时，不允许通过任何方式获取建立的节点的地址，防止其被提前删除

template<typename _T>
inline bool ComGraph<_T>::FindNode(string NodeName)
{
	return Index.count(NodeName);
}

template<typename _T>
inline Node<_T> *ComGraph<_T>::GetNode(string NodeName)
{
	if (!Index.count(NodeName)) {
		ErrOut << "NodeName " << NodeName << " not found" << std::endl;
		throw NodeName;
	} //安全检查，这里抛出的异常我没有处理，会直接杀死程序
	return Index[NodeName];
}

template<typename _T>
_T ComGraph<_T>::SetPHNodeVal(Node<_T> *NodetoSet, _T Val)
{
	dynamic_cast<PHNode<_T> *>(NodetoSet)->SetVal(Val); //安全类型转换，调用SetVal
	return NodetoSet->GetVal(); //返回值本身没什么作用，但是能返回代表成功了
}

template<typename _T>
_T ComGraph<_T>::SetVarVal(string NodeName, _T Val)
{
	Node<_T> *NodetoSet = GetNode(NodeName); //获取
	dynamic_cast<VarNode<_T> *>(NodetoSet)->SetVal(Val);
	return NodetoSet->GetVal(); //返回值本身没什么作用，但是能返回代表成功了
}

template<typename _T>
void ComGraph<_T>::ClearVarVal(string NodeName)
{
	Node<_T> *temp = GetNode(NodeName);
	dynamic_cast<VarNode<_T> *>(temp)->ForcedClear();
}

/*
template<typename _T>
template<typename _CN>
Node<_T> *ComGraph<_T>::BuildCalcNode(string NodeName, vector<Node<_T> *> OperandLists)
{
	Node<_T> *temp = new _CN(OperandLists); //直接建立对应的计算节点（派生类）
	AddNode(NodeName, temp);
	return temp;
}
*/

template<typename _T>
template<typename _CALCN>
Node<_T> *ComGraph<_T>::BuildCalcNode(string NodeName, vector<string> OperandNameLists)
{
	vector<Node<_T> *> OperandLists;
	for (int i = 0; i < OperandNameLists.size(); ++i) OperandLists.push_back(GetNode(OperandNameLists[i]));
	//先转换为包含操作元地址的vector
	Node<_T> *temp = new _CALCN(OperandLists);
	AddNode(NodeName, temp);
	return temp;
}

template<typename _T>
template<typename _CALCN>
Node<_T> *ComGraph<_T>::BuildCalcNode(string NodeName, int OperandNum, vector<string> OperandNameLists)
{
	vector<Node<_T> *> OperandLists;
	for (int i = 0; i < OperandNum; ++i) OperandLists.push_back(GetNode(OperandNameLists[i]));
	//先转换为包含操作元地址的vector
	Node<_T> *temp = new _CALCN(OperandNum, OperandLists);
	AddNode(NodeName, temp);
	return temp;
}

template<typename _T>
_T ComGraph<_T>::Eval(string NodeName, vector<pair<string, _T> > PHList)
{
	for (int i = 0; i < PHList.size(); ++i) SetPHNodeVal(GetNode(PHList[i].first), PHList[i].second);
	//先对所有占位节点赋值
	Node<_T> *Target = GetNode(NodeName); //获取目标节点地址
	_T Res;
	try {
		Res = Target->GetVal(); //计算
		AssignWaitingList<_T>::Clear();
	}
	catch (string &ErrMsg) {
		Target->Clear(); //清除中间结果
		for (int i = 0; i < PHList.size(); ++i) GetNode(PHList[i].first)->Clear();
		throw ErrMsg;
	}
	Target->Clear(); //清除中间结果
	for (int i = 0; i < PHList.size(); ++i) GetNode(PHList[i].first)->Clear(); //清除对占位符的赋值
	return Res; //返回答案
}

template<typename _T>
Node<_T> *ComGraph<_T>::BuildPHNode(string NodeName)
{
	Node<_T> *temp = new PHNode<_T>;
	AddNode(NodeName, temp);
	return temp;
}

template<typename _T>
Node<_T> *ComGraph<_T>::BuildConNode(string NodeName, _T ConVal)
{
	Node<_T> *temp = new ConNode<_T>(ConVal);
	AddNode(NodeName, temp);
	return temp;
}

template<typename _T>
Node<_T> *ComGraph<_T>::BuildVarNode(string NodeName)
{
	Node<_T> *temp = new VarNode<_T>;
	AddNode(NodeName, temp);
	return temp;
}

template<typename _T>
Node<_T> *ComGraph<_T>::BuildVarNode(string NodeName, _T InitVal)
{
	Node<_T> *temp = new VarNode<_T>(InitVal);
	AddNode(NodeName, temp);
	return temp;
}

template<typename _T>
Node<_T> *ComGraph<_T>::BuildPriNode(string NodeName, string WatchName)
{
	return BuildPriNode(NodeName, WatchName, PriOut);
}

template<typename _T>
Node<_T> *ComGraph<_T>::BuildPriNode(string NodeName, string WatchName, std::ostream &_OSTR)
{
	Node<_T> *temp = new PriNode<_T>(WatchName, GetNode(WatchName), _OSTR);
	AddNode(NodeName, temp);
	return temp;
}

template<typename _T>
inline _T ComGraph<_T>::RecInHistory(_T Ans)
{
	AnsHistory.push_back(Ans);
	return Ans;
}

template<typename _T>
inline _T ComGraph<_T>::ReadFromHistory(int Pos)
{
	return AnsHistory[Pos - 1]; //因为命令编号从1开始，而vector下标从0开始
}

template<typename _T>
inline void ComGraph<_T>::clear()
{
	for (int i = 0; i < NodeAddress.size(); ++i) {
		if (NodeAddress[i]) delete NodeAddress[i];
		NodeAddress[i] = nullptr;
	}
	Index.clear();
	NodeAddress.clear();
	AnsHistory.clear();
}

template<typename _T>
void ComGraph<_T>::AddNode(string NodeName, Node<_T> *Point)
{
	Index[NodeName] = Point;
	NodeAddress.push_back(Point);
}

template<>
void ComGraph<Tensor>::GradientDescend(string LossName, string VarName, vector<pair<string, Tensor> > PHList, const double &lr) {
	for (int i = 0; i < PHList.size(); ++i) SetPHNodeVal(GetNode(PHList[i].first), PHList[i].second);
	Node<Tensor> *target = GetNode(LossName), *var = GetNode(VarName);
	Tensor der = dynamic_cast<DerCNode<Tensor> *>(target)->GetVal().reshape(var->GetVal().shape()); //求导
	Tensor new_val = var->GetVal() - der * lr;
	SetVarVal(VarName, new_val);
}

template<>
void ComGraph<Tensor>::MomGradientDescend(string LossName, string VarName, vector<pair<string, Tensor> > PHList, Tensor &last_vel, const double &coeff, const double &lr) {
	for (int i = 0; i < PHList.size(); ++i) SetPHNodeVal(GetNode(PHList[i].first), PHList[i].second);
	Node<Tensor> *target = GetNode(LossName), *var = GetNode(VarName);
	Tensor der = dynamic_cast<DerCNode<Tensor> *>(target)->GetVal().reshape(var->GetVal().shape()); //求导
	Tensor new_vel = der * lr + last_vel * coeff;
	Tensor new_val = var->GetVal() - new_vel;
	SetVarVal(VarName, new_val);
	last_vel = new_vel;
}

template<>
void ComGraph<Tensor>::RMSprop(string LossName, string VarName, vector<pair<string, Tensor> > PHList, Tensor &grad_square, const double &decay, const double &lr) {
	for (int i = 0; i < PHList.size(); ++i) SetPHNodeVal(GetNode(PHList[i].first), PHList[i].second);
	Node<Tensor> *target = GetNode(LossName), *var = GetNode(VarName);
	Tensor der = dynamic_cast<DerCNode<Tensor> *>(target)->GetVal().reshape(var->GetVal().shape()); //求导
	grad_square = grad_square * decay + (der.broadcast_mul(der)) * (1 - decay);
	Tensor new_val = var->GetVal() - der.broadcast_div(grad_square.sqrt() + Tensor(grad_square.shape(), 1e-8)) * lr;
	SetVarVal(VarName, new_val);
}

template<>
void ComGraph<Tensor>::Adam(string LossName, string VarName, vector<pair<string, Tensor> > PHList, const int &iter_num, const double &lr, const double &beta_1, const double &beta_2) {
	for (int i = 0; i < PHList.size(); ++i) SetPHNodeVal(GetNode(PHList[i].first), PHList[i].second);
	int iter = 0;
	Node<Tensor> *target = GetNode(LossName), *var = GetNode(VarName);
	Tensor first_mom = Tensor(var->GetVal().shape(), 0.0), second_mom = Tensor(var->GetVal().shape(), 0.0);
	double beta_1_pow = 1., beta_2_pow = 1.;
	while (iter++ < iter_num) {
		beta_1_pow *= beta_1; beta_2_pow *= beta_2;
		Tensor der = dynamic_cast<DerCNode<Tensor> *>(target)->GetVal().reshape(var->GetVal().shape()); //求导
		first_mom = first_mom * beta_1 + der * (1 - beta_1);
		second_mom = second_mom * beta_2 + der.broadcast_mul(der) * (1 - beta_2);
		auto first_unbias = first_mom * (1. / (1. - beta_1_pow));
		auto second_unbias = second_mom * (1. / (1. - beta_2_pow));
		Tensor new_val = var->GetVal() - first_unbias.broadcast_div(second_unbias.sqrt() + Tensor(second_unbias.shape(), 1e-8)) * lr;
		SetVarVal(VarName, new_val);
	}
}

#endif //COMPUTATIONAL_GRAPH_COMGRAPH_H




