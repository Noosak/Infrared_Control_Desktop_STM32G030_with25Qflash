# Programming Reference

## About GPIO

## About Timer

1. 时钟频率计算公式：**周期(ns) =  ((1+TIM_Prescaler )/主频(HZ))\*(1+TIM_Period )** 
2. 时钟频率计算公式：**频率(HZ) = ((主频(HZ) / (1 + TIM_Period) ) * (1 + TIM_Prescaler ))**
3. 定时器设置为PWM模式时，Pulse参数为PWM占空比设置（电平翻转）位置。
4. 通过CubeMX配置完成的工程默认并***不使能定时器通道与定时器计数***，故需要在配置完成后于相关位置启动相关定时器与其通道。

## About Interrupt

## About DMA

