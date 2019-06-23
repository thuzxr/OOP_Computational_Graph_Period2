# 实验报告

## 接口修改

对于第一阶段代码主要修改了几个方面

- `CalcNode`虚类中增加了计算和获取导数的接口（虚函数），并在各继承类中实现
- `ComGraph`类中增加了梯度优化算法函数，增加了处理Assign操作的步骤
---
## 迭代开发

### 在第一阶段基础上增加的库

- `AssertCNode`类（继承自`CalcNode`），实现用于调试的Assert运算
- `BindCNode`类（继承自`CalcNode`），实现捆绑Bind运算
- `GradCNode`类和`DerCNode`类（均继承自`CalcNode`），实现链式求导运算节点
- `AssignCNode`类（继承自`CalcNode`），实现Assign赋值运算
- `Tensor`矩阵及其运算类

### 第二阶段实现的功能

- 基础需求中的Assert, Bind, 链式求导功能，拓展需求中的Assign功能，对应可执行文件`main2`（由`period2.cpp`编译）
- 基础需求中的牛顿迭代法解方程，对应可执行文件`main3`（由`newton.cpp`编译）
- 拓展需求中的多维矩阵`Tensor`类，实现了各类操作接口，在适合的`CalcNode`的继承类中实现自动求导，还实现了神经网络中一些常用的函数；具体功能展示请运行程序`main4`（由`tensor_demo.cpp`编译）
- 拓展需求中的梯度下降求优化最小二乘法，参见下方实验分析，此功能对应可执行文件`main5`（由`nn.cpp`编译）
---
## 关于Tensor的损失函数优化器

通过优化两个Tensor之间的均方误差，我们得到了四种优化器的表现

### 梯度下降

```
PROCESS: Construction of the neural network completed.
PROCESS: Training of neural network completed. time: 1.28302
# total iter: 400, minimal loss: 6.04609 achieved at iter 400
```

### 动量梯度下降

```
PROCESS: Construction of the neural network completed.
PROCESS: Training of neural network completed. time: 1.44472
# total iter: 400, minimal loss: 4.02601e-06 achieved at iter 245
```

### `RMSProp`

```
PROCESS: Construction of the neural network completed.
PROCESS: Training of neural network completed. time: 1.81818
# total iter: 400, minimal loss: 4.69375 achieved at iter 400
```

### `Adam`

```
PROCESS: Construction of the neural network completed.
PROCESS: Training of neural network completed. time: 13.6317
# total iter: 400, minimal loss: 2.45051 achieved at iter 75
```

### 结果的分析

可以看出动量梯度下降相比于梯度下降有了明显的进步；虽然 `RMSProp` 和 `Adam` 相比于前两者有着更快的收敛速度，但是限于参数的个数，并不能将损失函数优化至动量梯度下降的范围