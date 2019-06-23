#include <iostream>
#include <iomanip>
#include "lib/comgraph.h"
#include "basic_calc_pack/basic_calc_pack.h"
#include "advanced_calc_pack/advanced_calc_pack.h"
#include "compare_calc_pack/compare_calc_pack.h"

using namespace std;

ostream &ErrOut = cout; //应项目要求，错误信息向cout输出
ostream &PriOut = cout; //应项目要求，PriNode信息向cout输出
ostream &AnsOut = cout; //答案输出至cout

void EstablishGraph(const int &n, ComGraph<double>& graph, double* cst){
    for(int i = 0; i <= n; i++){
        graph.BuildConNode(string("a")+to_string(i), cst[n-i]);
    }
        
    vector<string> OperandsList;
    graph.BuildPHNode("x1");

    OperandsList.push_back(string("a1"));
    OperandsList.push_back(string("x1"));
    graph.BuildCalcNode<MulCNode<double>>("m1", 2, OperandsList);
    OperandsList.clear();

    OperandsList.push_back(string("a0"));
    OperandsList.push_back(string("m1"));
    graph.BuildCalcNode<PluCNode<double>>("s1", 2, OperandsList);
    OperandsList.clear();

    

    for(int i = 2; i <= n; i++){
        OperandsList.push_back(string("x")+to_string(i-1));
        OperandsList.push_back(string("x1"));
        graph.BuildCalcNode<MulCNode<double>>(string("x")+to_string(i), 2, OperandsList);
        OperandsList.clear();

        OperandsList.push_back(string("a")+to_string(i));
        OperandsList.push_back(string("x")+to_string(i));
        graph.BuildCalcNode<MulCNode<double>>(string("m")+to_string(i), 2, OperandsList);
        OperandsList.clear();

        OperandsList.push_back(string("s")+to_string(i-1));
        OperandsList.push_back(string("m")+to_string(i));
        graph.BuildCalcNode<PluCNode<double>>(string("s")+to_string(i), 2, OperandsList);
        OperandsList.clear();
    }
    
    OperandsList.push_back(string("s")+to_string(n));
    graph.BuildCalcNode<GradCNode<double>>("g", 1, OperandsList);
    OperandsList.clear();

    OperandsList.push_back(string("g"));
    OperandsList.push_back(string("x1"));
    graph.BuildCalcNode<DerCNode<double>>(string("gf"), 2, OperandsList);
    OperandsList.clear();
}

int main(){
    ComGraph<double> graph(ErrOut, PriOut);
    int m;
    cin >> m;
    for(int i = 0; i < m; i++){
        int n;
        cin >> n;
        double *cst = new double[n+1];
        for(int j = 0; j < n+1; j++){
            cin >> cst[j];
        }
        EstablishGraph(n, graph, cst);
        double ans;
        cin >> ans;
        vector<pair<string, double>> PHList;
        for(int k = 0; k < 5; k++){
            PHList.push_back(make_pair("x1", ans));
            ans -= graph.Eval(string("s")+to_string(n), PHList) / graph.Eval("gf", PHList);
            PHList.clear();
            if(k != 4)
                cout << setprecision(4) << fixed << ans << " ";
            else
                cout << setprecision(4) << fixed << ans << "\n";
        }
        graph.clear();
    }
    return 0;
}