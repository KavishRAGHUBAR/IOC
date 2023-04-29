#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <asm/io.h>
#include <mach/platform.h>

static int major;
#define NBMAX_LED 32

MODULE_LICENSE("GPL");
MODULE_AUTHOR("GONG WEIYI RAGHUBAR KAVISH");
MODULE_DESCRIPTION("Module, aussitot insere, aussitot efface");

// static int __init mon_module_init(void)
// {
//    printk(KERN_DEBUG "Hello World <raghubar_gong> !\n");
//    return 0;
// }



// static int btn;
// module_param(btn, int, 0);
// MODULE_PARM_DESC(btn, "numéro du port du bouton");

// static int __init mon_module_init(void)
// {
//     printk(KERN_DEBUG "Hello_world Raghubar_Gong !\n");
//     printk(KERN_DEBUG "btn=%d !\n", btn);
//     return 0;
// }
static const int BP=18;

struct gpio_s
{
    uint32_t gpfsel[7];
    uint32_t gpset[3];
    uint32_t gpclr[3];
    uint32_t gplev[3];
    uint32_t gpeds[3];
    uint32_t gpren[3];
    uint32_t gpfen[3];
    uint32_t gphen[3];
    uint32_t gplen[3];
    uint32_t gparen[3];
    uint32_t gpafen[3];
    uint32_t gppud[1];
    uint32_t gppudclk[3];
    uint32_t test[1];
}
volatile *gpio_regs = (struct gpio_s *)__io_address(GPIO_BASE);

static void gpio_fsel(int pin, int fun)
{
    uint32_t reg = pin / 10;
    uint32_t bit = (pin % 10) * 3;
    uint32_t mask = 0b111 << bit;
    gpio_regs->gpfsel[reg] = (gpio_regs->gpfsel[reg] & ~mask) | ((fun << bit) & mask);
}

static int gpio_read(uint32_t pin)//Le fonction de read gpio que on a fait avant
{
    //int lev;
    uint32_t reg = pin / 32;
    uint32_t bit = pin % 32;
    uint32_t mask = 1;
    return gpio_regs->gplev[reg]>>bit & mask !=0;
    //return gpio_regs_virt->gplev[reg] & mask;//lev=

}

static void gpio_write(int pin, bool val)
{
    if (val)
        gpio_regs->gpset[pin / 32] = (1 << (pin % 32));
    else
        gpio_regs->gpclr[pin / 32] = (1 << (pin % 32));
}



static int 
open_led_XY(struct inode *inode, struct file *file) {
    printk(KERN_DEBUG "open()\n");
    return 0;
}

static ssize_t 
read_led_XY(struct file *file, char *buf, size_t count, loff_t *ppos) {//Fonction pour lire les donnees par le bouton
    printk(KERN_DEBUG "read()\n");
    //int val;
    char val=(char) gpio_read(BP);//Faire une cast pour changer les valeurs en int a char
    *buf=val+'0';//convertir val a char
    return count;
}

static ssize_t 
write_led_XY(struct file *file, const char *buf, size_t count, loff_t *ppos) {
    printk(KERN_DEBUG "write()\n");
    return count;
}

static int 
release_led_XY(struct inode *inode, struct file *file) {
    printk(KERN_DEBUG "close()\n");
    return 0;
}

struct file_operations fops_led =
{
    .open       = open_led_XY,
    .read       = read_led_XY,
    .write      = write_led_XY,
    .release    = release_led_XY 
};

static int leds[NBMAX_LED];
static int nbled;
module_param_array(leds, int, &nbled, 0);
MODULE_PARM_DESC(LEDS, "tableau des numéros de port LED");

static int __init mon_module_init(void)
{
    int i;
    major = register_chrdev(0, "bp0_RG", &fops_led); // 0 est le numéro majeur qu'on laisse choisir par linux
    printk(KERN_DEBUG "Hello World R&G!\n");
    for (i=0; i < nbled; i++)
       printk(KERN_DEBUG "bp0_RG %d = %d\n", i, leds[i]);
    return 0;
}

static void __exit mon_module_cleanup(void)
{
    unregister_chrdev(major, "bp0_RG");
   printk(KERN_DEBUG "Goodbye World <raghubar_gong> !\n");
}

module_init(mon_module_init);
module_exit(mon_module_cleanup);

