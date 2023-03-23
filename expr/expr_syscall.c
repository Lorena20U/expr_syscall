#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/uaccess.h>

int check_expr(const char *expr)
{
    int result = 1;
    int i = 0;
    char stack[256];
    stack[0] = 'N';
    int top = -1;

    while (expr[i] != '\0')
    {
        if (expr[i] == '(')
        {
            top = top + 1;
            stack[top] = '(';
        }
        else if (expr[i] == '[')
        {
            top = top + 1;
            stack[top] = '[';
        }
        else if (expr[i] == ']' || expr[i] == ')')
        {
            if (top != -1)
            {
                if (expr[i] == ']' && stack[top] == '[')
                {
                    top = top - 1;
                }
                else if (expr[i] == ')' && stack[top] == '(')
                {
                    top = top - 1;
                }
                else
                {
                    break;
                }
            }
            else
            {
                break;
            }
        }
        i = i + 1;
    }

    if (top == -1 && stack[0] != 'N')
    {
        result = 0;
    }

    return result;
}

int check_num(const char expr)
{
    if (expr >= 48 && expr <= 57)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int do_math(const char *expr, int *result)
{
    int i = 0;
    int num_stack[256];
    int num_top = -1;
    char op_stack[256];
    int op_top = -1;

    while (expr[i] != '\0')
    {
        if (check_num(expr[i]))
        {
            int num = 0;
            while (check_num(expr[i]))
            {
                num = num * 10 + (expr[i] - '0');
                i++;
            }
            num_stack[++num_top] = num;
        }
        else if (expr[i] == '+' || expr[i] == '-' || expr[i] == '*' || expr[i] == '/')
        {
            while (op_top != -1 && (op_stack[op_top] == '*' || op_stack[op_top] == '/'))
            {
                int num2 = num_stack[num_top--];
                int num1 = num_stack[num_top--];
                char op = op_stack[op_top--];
                int res;
                switch (op)
                {
                case '*':
                    res = num1 * num2;
                    break;
                case '/':
                    res = num1 / num2;
                    break;
                }
                num_stack[++num_top] = res;
            }
            op_top = op_top + 1;
            op_stack[op_top] = expr[i];
            i++;
        }
        else if (expr[i] == '(' || expr[i] == '[')
        {
            op_top = op_top + 1;
            op_stack[op_top] = expr[i];
            i++;
        }
        else if (expr[i] == ')' || expr[i] == ']')
        {
            while ((op_stack[op_top] != '(' && op_stack[op_top] != '[') && op_top != -1)
            {
                int num2 = num_stack[num_top--];
                int num1 = num_stack[num_top--];
                char op = op_stack[op_top--];
                int res;
                switch (op)
                {
                case '+':
                    res = num1 + num2;
                    num_top = num_top + 1;
                    num_stack[num_top] = res;
                    break;
                case '*':
                    res = num1 * num2;
                    num_top = num_top + 1;
                    num_stack[num_top] = res;
                    break;
                case '-':
                    res = num1 - num2;
                    num_top = num_top + 1;
                    num_stack[num_top] = res;
                    break;
                case '/':
                    res = num1 - num2;
                    num_top = num_top + 1;
                    num_stack[num_top] = res;
                    break;
                }
            }
            op_top--;
            i++;
        }
        else
        {
            i++;
        }
    }

    while (op_top != -1)
    {
        int num2 = num_stack[num_top--];
        int num1 = num_stack[num_top--];
        char op = op_stack[op_top--];
        int res;
        switch (op)
        {
        case '+':
            res = num1 + num2;
            break;
        case '-':
            res = num1 - num2;
            break;
        case '*':
            res = num1 * num2;
            break;
        case '/':
            res = num1 / num2;
            break;
        }
        num_stack[++num_top] = res;
    }

    if (num_top != 0)
    {
        return 0;
    }
    else
    {
        *result = num_stack[num_top];
        return 1;
    }
}

SYSCALL_DEFINE3(expr_syscall, const char *, expr, int, largo, int *, result)
{
    printk(KERN_INFO "expr_syscall. \n");

    char kernel_buf[256];
    long retorno;
    char *p;
    retorno = copy_from_user(kernel_buf, expr, largo);
    p = kernel_buf;

    if (retorno != 0)
    {
        printk("No se pudieron copiar los datos desde el espacio de usuario al espacio de kernel. \n");
        return 1;
    }
    else
    {

        int is_valid = check_expr(p);
        if (is_valid == 0)
        {
            int success = do_math(p, result);
            if (success)
            {
                printk("Correcto. \n");
                return 0; // hecho
            }
            else
            {
                printk("Invalido. \n");
                return -EINVAL; // invalido
            }
        }
        else
        {
            printk("Incorrecto. \n");
            return -EFAULT; // bad address
        }
    }
}
