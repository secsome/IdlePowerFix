# IdlePowerFix
用于解决Windows下空载时CPU风扇仍然一直转的问题。

Used to solve the problem that the CPU fan is still running all the time when the computer is idle on Windows.

## 说明/Introduction
总有神必程序会把电源管理的禁止CPU进入idle状态给打开，表现为导致CPU永远都是100%占用，系统空闲进程占用永远奇高无比。这个小工具可以把这个禁止idle关掉。

There are always some stupid program that will turn on the power management to prevent the CPU from entering the idle state, which will cause the CPU to be 100% occupied all the time, and the idle process of the system will always be extremely high. This small tool can turn off this prohibition of idle.

效果等同于以下的cmd命令:

The effect is equivalent to the following cmd command:
```
PowerCfg /SETACVALUEINDEX SCHEME_CURRENT SUB_PROCESSOR IDLEDISABLE 000
PowerCfg /SETACTIVE SCHEME_CURRENT
```