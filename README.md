# LeakStack

> A stack with leak

一个 C++11 header-only 的特殊栈容器，支持最大容量限制、容量预分配，以及独特的「中点读取」与「中点移除」操作。位于 `leak_stack` 命名空间。

## 特性

- 最大容量限制 `MaxSize`，超限抛异常
- 手动预分配容量 `PrepareCapacity`
- `Leakage()`：返回中间位置元素
- `Take()`：移除中间位置元素，后续元素前移
- 支持移动语义，禁止拷贝

> 内部使用 `new TYPE[]` 与显式析构管理元素，适用于可移动构造、可析构的类型。

## 接口

### 构造 / 析构

| 接口 | 说明 |
| --- | --- |
| `LeakStack()` | 默认构造，空栈 |
| `LeakStack(size, val)` | 构造含 `size` 个 `val` 副本的栈 |
| `LeakStack(start, end)` | 由指针区间 `[start, end)` 构造 |
| `~LeakStack()` | 释放底层数组 |
| 拷贝构造 / 拷贝赋值 | 已删除 |
| 移动构造 / 移动赋值 | 支持，`noexcept` |

### 容量

| 接口 | 说明 |
| --- | --- |
| `Size()` / `Capacity()` / `MaxSize()` | 当前大小 / 容量 / 最大容量 |
| `Empty()` / `Full()` | 是否为空 / 是否达到 `MaxSize` |
| `SetMaxSize(maxSize)` | 设置最大容量；小于当前 `Size` 抛 `length_error` |
| `PrepareCapacity(cap)` | 预分配；越界抛 `length_error` |

### 元素操作

| 接口 | 说明 |
| --- | --- |
| `Push(const TYPE&)` / `Push(TYPE&&)` | 拷贝 / 移动入栈 |
| `Take()` | 移除中点元素，后续前移；空栈抛 `out_of_range` |
| `Leakage()` | 返回中点元素副本；空栈抛 `out_of_range` |
| `Clear()` | 析构所有元素，容量不变 |

## 异常

| 场景 | 异常 |
| --- | --- |
| `Push` 时已达 `MaxSize` | `std::length_error` |
| `SetMaxSize` / `PrepareCapacity` 参数非法 | `std::length_error` |
| 空栈调用 `Leakage` / `Take` | `std::out_of_range` |

## 复杂度

- `Push`：均摊 O(1)，扩容时 O(n)
- `Take`：O(n)（需搬移元素）
- `Leakage` / `Size` / `Empty` 等：O(1)

## 注意

1. 不可拷贝，仅支持移动。
2. `Leakage()` 按值返回，可能有一次拷贝/移动开销。

## 许可证

本项目基于 [MIT License](https://opensource.org/licenses/MIT) 开源。
