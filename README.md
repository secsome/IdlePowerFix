# IdlePowerFix
���ڽ��Windows�¿���ʱCPU������Ȼһֱת�����⡣

Used to solve the problem that the CPU fan is still running all the time when the computer is idle on Windows.

## ˵��/Introduction
������س����ѵ�Դ����Ľ�ֹCPU����idle״̬���򿪣�����Ϊ����CPU��Զ����100%ռ�ã�ϵͳ���н���ռ����Զ����ޱȡ����С���߿��԰������ֹidle�ص���

There are always some stupid program that will turn on the power management to prevent the CPU from entering the idle state, which will cause the CPU to be 100% occupied all the time, and the idle process of the system will always be extremely high. This small tool can turn off this prohibition of idle.

Ч����ͬ�����µ�cmd����:

The effect is equivalent to the following cmd command:
```
PowerCfg /SETACVALUEINDEX SCHEME_CURRENT SUB_PROCESSOR IDLEDISABLE 000
PowerCfg /SETACTIVE SCHEME_CURRENT
```