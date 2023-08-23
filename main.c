#include "LCD.h"
#include "keypad.h"

#define input_from_keypad_SIZE 16
#define conc_nums_ops_SIZE      16

// Global Variables
uint8 input_from_keypad[input_from_keypad_SIZE];
sint16 conc_nums_ops[conc_nums_ops_SIZE];
uint8 input_from_keypad_SIZE_used = 0;

void displayFromKeypad(uint8 num)
{
    switch(num)
           {
           case '0':
           case '1':
           case '2':
           case '3':
           case '4':
           case '5':
           case '6':
           case '7':
           case '8':
           case '9':
           case '*':
           case '+':
           case '-':
           case '/':
           case '=':
               LCD_wite_cmd_data(num, DATA);
               break;
           case 'R':
               input_from_keypad_SIZE_used = 0;
               LCD_wite_cmd_data(LCD_CLEAR_COMMAND,CMD);
               break;

          }
    delay_Milli(500);
}


void fillArrayFromKeypad(uint8 num)
{

    if((input_from_keypad_SIZE_used < input_from_keypad_SIZE))
    {
        input_from_keypad[input_from_keypad_SIZE_used] = num;
        input_from_keypad_SIZE_used++;
    }
    //input_from_keypad_SIZE_used will be the used size of the array
    return;
}
uint8 charToInt(uint8 ch)
{
    if( (ch>= '0')&& (ch<='9') )
    {
        ch -= '0';
    }

    return ch;
}

uint8 intToChar(uint8 int_num)
{

    return (int_num += '0');
}
void test()
{
    LCD_init();
    keypadInit();

      uint8 num, doneFlag=0;
      while (1)
      {
          uint8 keyPad_status = getNum(&num);
          if((keyPad_status == KPD_KEY_PRESSED) && ( doneFlag != 1))
          {
              if(num == '=')
              {
                  doneFlag = 1;
                  displayFromKeypad(num);
                  fillArrayFromKeypad(charToInt(num));
              }

              if(doneFlag == 0)
              {
                  displayFromKeypad(num);
                  fillArrayFromKeypad(charToInt(num));
              }
          }
          else if((num == 'R'))
          {
              doneFlag = 0;
              displayFromKeypad(num);
          }


      }
}


void displayAnswer(void)
{
    int num = conc_nums_ops[0];
    int reverse = 0,i,j;

    /* print negative sign */
    if (num < 0)
    {
        LCD_wite_cmd_data('-', DATA);
        num = -num; // Make the number positive for printing
    }

    int numLength = 1;
    int tempNum = num;

    while (tempNum /= 10) {
        numLength++;
    }

    for (i = numLength - 1; i >= 0; i--) {
        int powerOf10 = 1;
        for (j = 0; j < i; j++) {
            powerOf10 *= 10;
        }
        int digit = (num / powerOf10) % 10;
        LCD_wite_cmd_data(intToChar(digit), DATA);
    }
}
/////////////////////////



uint8 counter=0;


void conc(void)
{
    uint8 i=0,nums_flag=0,neg_flag =0;
    sint16 key_num=0,j=0;

    for(i=0;i<input_from_keypad_SIZE_used;i++)
    {
        if(input_from_keypad[i]=='+'||input_from_keypad[i]=='-'||input_from_keypad[i]=='*'||input_from_keypad[i]=='/')
        {
            if(nums_flag)
            {
                if(neg_flag)
                {
                    conc_nums_ops[j]=key_num*(-1);
                    key_num=0;
                    neg_flag=0;
                    j++;
                    counter++;
                }
                else
                {
                    conc_nums_ops[j]=key_num;
                    key_num=0;
                    j++;
                    counter++;
                }

            }
            if(input_from_keypad[i]=='-')
            {
                if(!nums_flag)
                {
                    neg_flag=1;
                }
                else
                {
                    conc_nums_ops[j]=input_from_keypad[i];
                    nums_flag=0;
                    j++;
                    counter++;
                }
            }
            else
            {
                conc_nums_ops[j]=input_from_keypad[i];
                nums_flag=0;
                j++;
                counter++;
            }
        }
        else if(input_from_keypad[i]=='=')
        {
            conc_nums_ops[j]=key_num;
            key_num=0;
            j++;
            counter++;
        }
        else
        {
            key_num=(key_num*10)+input_from_keypad[i];
            nums_flag=1;
        }
    }
}

/////////////////////////////

sint16 func(sint16 num1,sint16 op,sint16 num2)
{
    sint16 result;
      switch(op)
      {
      case '*':
          result=(num1*num2);
          break;
      case '/':
          result=(num1/num2);
          break;
      case '+':
          result=(num1+num2);
          break;
      case '-':
          result=(num1-num2);
          break;
      }
      return result;
}

void operationHandling(void)
{
    uint8 i,j;
    for(i=0;i<counter;)
    {
        if(conc_nums_ops[i]=='*'||conc_nums_ops[i]=='/')
        {
            conc_nums_ops[i-1]=func(conc_nums_ops[i-1],conc_nums_ops[i],conc_nums_ops[i+1]);
            j=i;
            while(j<counter)
            {
                conc_nums_ops[j]=conc_nums_ops[j+2];
                j++;
            }
        }
        else
        {
            i++;
        }
    }
    for(i=0;i<counter;)
    {
        if(conc_nums_ops[i]=='+'||conc_nums_ops[i]=='-')
        {
            conc_nums_ops[i-1]=func(conc_nums_ops[i-1],conc_nums_ops[i],conc_nums_ops[i+1]);
            j=i;
            while(j<counter)
            {
                conc_nums_ops[j]=conc_nums_ops[j+2];
                j++;
            }
        }
        else
        {
            i++;
        }
    }
}

int main()
{
    LCD_init();
    keypadInit();
    uint8 num, doneFlag=0;
         while (1)
         {
             uint8 keyPad_status = getNum(&num);
             if((keyPad_status == KPD_KEY_PRESSED) && ( doneFlag != 1))
             {
                 if(num == '=')
                 {
                     doneFlag = 1;
                     displayFromKeypad(num);
                     fillArrayFromKeypad(charToInt(num));
                     conc();
                     operationHandling();
                     displayAnswer();
                 }

                 if(doneFlag == 0)
                 {
                     displayFromKeypad(num);
                     fillArrayFromKeypad(charToInt(num));
                 }
             }
             else if((num == 'R'))
             {
                 doneFlag = 0;
                 displayFromKeypad(num);
             }


         }
}


