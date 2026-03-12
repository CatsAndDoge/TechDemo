# TechDemo 项目说明

## 项目概览

- 引擎：UE 5.5（C++14）
- 目标：第三人称演示 + AI 组生成、行为配置、交互式逻辑切换
- 主要系统：AI 配置集、组配置与生成、AI 控制器与行为树节点、交互开关、GM命令

## 核心代码模块（Source/TechDemo）


### AI 配置与组

- `UAIConfigSet`：`PrimaryDataAsset`
	- 字段：`BehaviorTree`、`Blackboard`
	- `Perception`：视野半径 / 丢失半径 / 角度 / `MaxAge` / 敌友过滤
	- `Movement`：`Walk/Sprint/StoppingDistance`
	- 目标过滤：`AllowedTargetTags + bRequireAnyAllowedTag`
	- `ValidateConfig`：完整性校验
- `UPatrolAIConfigSet`：扩展巡逻参数 `PatrolRouteTag`、是否循环 / 随机起点、`WaitTime`
- `UNPCGroupConfigAsset`：描述组生成规则 `FNPCGroupSpawnRule`（`GroupID`、`CharacterClass`、默认/其他 `AIConfig`、数量、最小间距、重试次数）；`ValidateConfig` 负责校验

### AI 控制与行为

- `AMyAIController`
	- 组件：`Blackboard`、`AIPerception + SightConfig`、`PathFollowing`
	- 配置：`DefaultConfig` 软引用，`AIConfigMap`（`ConfigID -> ConfigSet`），`EnsureConfigLoaded` 会加载默认值，或回退到 `/Game/DemoContent/AI/Config/DA_AI_Default`
	- `ApplyAIConfig(ConfigID)`：校验 -> 设置感知/移动 -> `UseBlackboard` -> `RunBehaviorTree`；`MoveToTarget` 使用 `StoppingDistance`
	- 感知回调 `HandleTargetPerceptionUpdated`：基于 `AllowedTargetTags` 过滤，将目标写入/清空 `Blackboard.TargetActor`；若 `bUseAIPerception=false`，使用玩家作为回退目标
	- 其他：`IsValidChaseTarget`（标签过滤）、`ApplyFallbackTargetActor`
- 行为树节点
	- `UBTTask_CalcFleePoint`：根据 `TargetActorKey` 与距离阈值 `Start/Stop` 判定逃跑，计算反向逃跑点并投射导航，写入 `VectorKey`，同时更新 `IsFleeingKey`
	- `UBTService_UpdateFleeState`：周期检查距离，更新 `IsFleeingKey`（`Start/StopFleeDistance`）
	- `UBTTask_SelectNextPatrolPoint`：读取 `UPatrolAIConfigSet::PatrolRouteTag`，获取带标签路点并按名称排序，设置当前位置并推进索引（支持随机起点、循环）

### 组生成与逻辑切换

- `ANPCGroupSpawner`
	- 组件：`Root + SpawnArea Box`，可选 NavMesh 投射
	- `SpawnAllGroups`：遍历 `UNPCGroupConfigAsset::Groups`，随机点生成角色，`SpawnDefaultController`，并注册到组（记录默认/当前配置）
	- 位置规则：`Box` 随机点，最小 2D 间距检查，可 NavMesh 投射
	- 逻辑切换：`SwitchGroupLogic/SwitchGroupLogicByConfigID/RevertDefaultGroupLogic`，对组内存活的 `AMyAIController` 调用 `ApplyAIConfig`
	- 清理：`ClearupInvalidControllers` 去除失效引用

### 交互式逻辑开关

- `ALogicSwitchInteractable`
	- 组件：`MeshComponent` 根、`BoxTrigger`（Pawn Overlap）、UI 使用 `WidgetComponent + PromptWidgetClass`（替代 `TextRender`）
	- 配置：`NPCGroups` 数组（`ANPCGroupSpawner + SwitchLogicByGroup` 映射），交互冷却 `InteractCD`
	- 逻辑：玩家（标签含 `Target.Player`）进入触发区后缓存 Pawn 并显示提示；按键绑定 `E` 调用 `TryInteract -> Interact`
		- 首次切换：对每个有效组/映射调用 `SwitchGroupLogic`，成功则 `bSwitchMode=true`
		- 再次交互：调用 `RevertDefaultGroupLogic`
		- 离开触发区：隐藏提示

### GM命令

- `UTechDemoCheatManager`
	- `AISetConfig <ConfigID>`：对场景内全部 `AMyAIController` 设置配置
	- `NPCSwitchGroupLogic <GroupID> <ConfigID>`：对首个 `ANPCGroupSpawner` 切换组配置
	- `NPCRevertGroupLogic <GroupID>`：恢复组默认配置

## 运行流程简述

1. `GameMode` 设置默认 Pawn/Controller
2. `MyAIController::OnPossess` 加载并应用 AI 配置，启动 BB/BT，绑定感知或回退目标
3. 若 `NPCGroupSpawner.bAutoSpawnOnBeginPlay`，按组配置生成 NPC 并记录默认配置
4. 玩家进入 `LogicSwitchInteractable` 触发区后，按 `E` 可批量切换组逻辑；再次交互可还原
5. 行为树节点执行：巡逻取路点，逃跑节点/服务依据距离动态更新目标与逃跑点
6. GM命令可全局切换 AI 配置或指定组配置，便于调试