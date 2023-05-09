#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#include <asm/io.h>
#include <asm/delay.h>
#include <mach/platform.h>


static int major;


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Charlie, 2015");
MODULE_DESCRIPTION("Module, aussitot insere, aussitot efface");

char buffer[20];//Creer un tableau de char
unsigned long buf_offset=0;//Pas de offset
unsigned long buf_length=20;//longeur de 20 elements


#define RS 7
#define E  27
#define D4 22
#define D5 23
#define D6 24
#define D7 25

#define GPIO_INPUT  0
#define GPIO_OUTPUT 1

#define RPI_BLOCK_SIZE  0xB4
#define RPI_GPIO_BASE   0x20200000

struct cord_xy {//Structure pour ioctl
int line;
int row;
} cord_xy;
#define IOC_MAGIC 't'
#define LCDIOCT_CLEAR _IO(IOC_MAGIC, 20)
#define LCDIOCT_SETXY _IOW(IOC_MAGIC, 21, struct cord_xy)

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

/*******************************************************************************
 * GPIO Operations 
 ******************************************************************************/

void gpio_config(int gpio, int value)
{
    int regnum = gpio / 10;
    int offset = (gpio % 10) * 3;
    gpio_regs->gpfsel[regnum] &= ~(0x7 << offset);
    gpio_regs->gpfsel[regnum] |= ((value & 0x7) << offset);
}

void gpio_write(int gpio, int value)
{
    int regnum = gpio / 32;
    int offset = gpio % 32;
    if (value&1)
        gpio_regs->gpset[regnum] = (0x1 << offset);
    else
        gpio_regs->gpclr[regnum] = (0x1 << offset);
}

/*******************************************************************************
 * LCD's Instructions ( source = doc )
 * Ces constantes sont utilisées pour former les mots de commandes
 * par exemple dans la fonction lcd_init()
 ******************************************************************************/

/* commands */
#define LCD_CLEARDISPLAY        0b00000001
#define LCD_RETURNHOME          0b00000010
#define LCD_ENTRYMODESET        0b00000100
#define LCD_DISPLAYCONTROL      0b00001000
#define LCD_CURSORSHIFT         0b00010000
#define LCD_FUNCTIONSET         0b00100000
#define LCD_SETCGRAMADDR        0b01000000
#define LCD_SETDDRAMADDR        0b10000000

/* flags for display entry mode : combine with LCD_ENTRYMODESET */
#define LCD_EM_RIGHT            0b00000000
#define LCD_EM_LEFT             0b00000010
#define LCD_EM_DISPLAYSHIFT     0b00000001
#define LCD_EM_DISPLAYNOSHIFT   0b00000000

/* flags for display on/off control : combine with LCD_DISPLAYCONTROL */
#define LCD_DC_DISPLAYON        0b00000100
#define LCD_DC_DISPLAYOFF       0b00000000
#define LCD_DC_CURSORON         0b00000010
#define LCD_DC_CURSOROFF        0b00000000
#define LCD_DC_BLINKON          0b00000001
#define LCD_DC_BLINKOFF         0b00000000

/* flags for display/cursor shift : combine with LCD_CURSORSHIFT */
#define LCD_CS_DISPLAYMOVE      0b00001000
#define LCD_CS_CURSORMOVE       0b00000000
#define LCD_CS_MOVERIGHT        0b00000100
#define LCD_CS_MOVELEFT         0b00000000

/* flags for function set : combine with LCD_FUNCTIONSET */
#define LCD_FS_8BITMODE         0b00010000
#define LCD_FS_4BITMODE         0b00000000
#define LCD_FS_2LINE            0b00001000
#define LCD_FS_1LINE            0b00000000
#define LCD_FS_5x10DOTS         0b00000100
#define LCD_FS_5x8DOTS          0b00000000

/*******************************************************************************
 * LCD's Operations
 ******************************************************************************/

/* generate E signal */
void lcd_strobe(void)
{
    gpio_write(E, 1);
    udelay(1);
    gpio_write(E, 0);
}

/* send 4bits to LCD : valable pour les commande et les data */

void lcd_write4bits(int data)
{
    /* first 4 bits */
    gpio_write(D7, data>>7);
    gpio_write(D6, data>>6);
    gpio_write(D5, data>>5);
    gpio_write(D4, data>>4);
    lcd_strobe();

    /* second 4 bits */
    gpio_write(D7, data>>3);
    gpio_write(D6, data>>2);
    gpio_write(D5, data>>1);
    gpio_write(D4, data>>0);
    lcd_strobe();
}

void lcd_command(int cmd)
{
    gpio_write(RS, 0);
    lcd_write4bits(cmd);
    udelay(2000);               // certaines commandes sont lentes 
}

void lcd_data(int character)
{
    gpio_write(RS, 1);
    lcd_write4bits(character);
    udelay(1);
}

/* initialization : pour comprendre la séquence, il faut regarder le cours */

void lcd_init(void)
{
    gpio_write(E, 0);
    lcd_command(0b00110011);    /* initialization */
    lcd_command(0b00110010);    /* initialization */
    lcd_command(LCD_FUNCTIONSET | LCD_FS_4BITMODE | LCD_FS_2LINE | LCD_FS_5x8DOTS);
    lcd_command(LCD_DISPLAYCONTROL | LCD_DC_DISPLAYON | LCD_DC_CURSORON| LCD_DC_BLINKON);
    lcd_command(LCD_ENTRYMODESET | LCD_EM_RIGHT | LCD_EM_DISPLAYNOSHIFT);
}

void lcd_clear(void)//Fonction de vider le lcd
{
    lcd_command(LCD_CLEARDISPLAY);
    lcd_command(LCD_RETURNHOME);
}


void lcd_set_cursor(int x, int y)//Fonction de set cursor
{
    lcd_command(LCD_RETURNHOME);
    int mov;
    int i;
    if(y==0){mov=x;}
    else if(y==1){mov=x+40;}
    else if(y==2){mov=x+20;}
    else if(y==3){mov=x+60;}

    for(i=0;i<mov;i++)
    {
        lcd_command(LCD_CURSORSHIFT|LCD_CS_CURSORMOVE|LCD_CS_MOVERIGHT);
    }
}


void lcd_message(const char *txt)
{
    int a[] = { 0, 0x40, 0x14, 0x54 };
    int len = 20;
    int i=0;
    int l=0;
    lcd_clear();
    
 
    for (; (l < 4) && (i < strlen(txt)); l++) {
        lcd_command(LCD_SETDDRAMADDR + a[l]);
        for (; (i < (l + 1) * len) && (i < strlen(txt)); i++) {
        if(txt[i]=='\n')//S'il est de retour à la prochaine ligne
        {
            l++;//Mettre l, la variable de position dans la prochaine ligne
            lcd_set_cursor(0,l);//Mettre le cursor dans la prochaine ligne
        }
        else if(txt[i]=='\b')//Cas pour supprimer les datas precedent
        {
            lcd_command(LCD_CURSORSHIFT|LCD_CS_CURSORMOVE|LCD_CS_MOVELEFT);//shifter le cursor a gauche par une case
            lcd_data(' ');//Supprimer la case et remplacer avec un espace
            lcd_command(LCD_CURSORSHIFT|LCD_CS_CURSORMOVE|LCD_CS_MOVELEFT);//Shifter a gauche deux bits pour continuer a ecrire
            lcd_command(LCD_CURSORSHIFT|LCD_CS_CURSORMOVE|LCD_CS_MOVELEFT);
        }
        else//Si les datas sont normales, on écrit sur l'écran
        {
            lcd_data(txt[i]);
        }
        udelay(500);

        }
    }
}


void affiche_4lignes(const char *txt){//Fonction pour afficher dans les 4 lignes
    int ligne;
    for ( ligne=0;ligne < 4 ;ligne++){
        lcd_set_cursor(0, ligne);
        int len =20;
        int i=0;
        for (; (i < (ligne + 1) * len) && (i < strlen(txt)); i++) {
            lcd_data(txt[i]);
        }
    }
}



static int 
open_lcd_XY(struct inode *inode, struct file *file) {
    printk(KERN_DEBUG "open()\n");
    return 0;
}

static ssize_t 
read_lcd_XY(struct file *file, char *buf, size_t count, loff_t *ppos) {
    printk(KERN_DEBUG "read()\n");
    return count;
}

static ssize_t 
write_lcd_XY(struct file *file, const char *buf, size_t count, loff_t *ppos) {
    printk(KERN_DEBUG "write()\n");
    if(copy_from_user(buffer,buf+buf_offset,buf_length)){//Fonction de copy_from_user pour recuperer les valeurs. buffer est la table de char a afficher. buf+buf_offset est string a entrer.
        printk(KERN_DEBUG "ERROR");
    }
    else{
        printk(KERN_DEBUG "buf : %s\n", buffer);
        lcd_message(buffer);  
    }
    return count;
}

static int 
release_lcd_XY(struct inode *inode, struct file *file) {
    printk(KERN_DEBUG "close()\n");
    return 0;
}

static long ioctl_lcd(struct file *file, unsigned int cmd, unsigned long arg)
{
    int kline,krow;
    printk(KERN_DEBUG "Ioctl_lcd ! \n");
    struct cord_xy cord;
    if(_IOC_TYPE(cmd) != IOC_MAGIC) // Check the magic number of the device
    return -EINVAL;

    switch(cmd){
        case LCDIOCT_CLEAR://Si en case pour vider
        file->f_pos = 0;
        lcd_clear();
        break;
        case LCDIOCT_SETXY://case pour set cursor à la position x,y
        if(copy_from_user(&cord, (void*)arg, _IOC_SIZE(cmd)) != 0)
            return -EINVAL;
        kline = cord.line;
        krow = cord.row;
        break;
        default: return -EINVAL;
    }
    return 0;
}

struct file_operations fops_lcd =
{
    .owner      = THIS_MODULE,
    .open       = open_lcd_XY,
    .read       = read_lcd_XY,
    .write      = write_lcd_XY,
    .unlocked_ioctl  = ioctl_lcd,//Fonction a declarer
    .release    = release_lcd_XY

};



static int __init mon_module_init(void)
{
    int i;
    major = register_chrdev(0, "lcd_driver_RG", &fops_lcd); // 0 est le numéro majeur qu'on laisse choisir par linux
    printk(KERN_DEBUG "Hello World R&G!\n");
    gpio_config(RS, GPIO_OUTPUT);
    gpio_config(E,  GPIO_OUTPUT);
    gpio_config(D4, GPIO_OUTPUT);
    gpio_config(D5, GPIO_OUTPUT);
    gpio_config(D6, GPIO_OUTPUT);
    gpio_config(D7, GPIO_OUTPUT);
    lcd_init();
    lcd_clear();
}

static void __exit mon_module_cleanup(void)
{
    unregister_chrdev(major, "lcd_driver_RG");
   printk(KERN_DEBUG "Goodbye World <raghubar_gong> !\n");
}

module_init(mon_module_init);
module_exit(mon_module_cleanup);

