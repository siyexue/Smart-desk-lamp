#include "OLED_Ext.h"

/**
  * @brief  清除指定行的内容
  * @param  Line: 行号 (1-4)
  * @retval 无
  */
void OLED_ClearLine(uint8_t Line)
{
    OLED_ShowString(Line, 1, "                "); // 16个空格清空一行
}

/**
  * @brief  在第3行显示坐姿提示
  * @param  str: 提示字符串
  * @retval 无
  */
void OLED_ShowPostureWarning(char *str)
{
    OLED_ClearLine(3);
    OLED_ShowString(3, 1, str);
}

/**
  * @brief  在第4行显示疲劳提示
  * @param  str: 提示字符串
  * @retval 无
  */
void OLED_ShowFatigueWarning(char *str)
{
    OLED_ClearLine(4);
    OLED_ShowString(4, 1, str);
}